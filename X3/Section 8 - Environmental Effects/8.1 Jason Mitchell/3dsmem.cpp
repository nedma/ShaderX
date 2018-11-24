#include "3ds.h"

#pragma pack(push)
#pragma pack(1)

typedef struct triangle_s
{
   long     mat;
   long     verts[3];
   long     index;
   long     smooth;
} triangle_t;

#pragma pack(pop)


static void  ParseMain3ds(char* pData, data3ds_t* output);
static void  ParseEdit3ds(char* pData, data3ds_t* output);
static void  ParseEditObject(char* pData, data3ds_t* output);
static void  ParseTriangleObject(char* pData, mesh3ds_t* output);
static void  ParseVertexList(char* pData, mesh3ds_t* output);
static void  ParseFaceList(char* pData, mesh3ds_t* output);
static void  ParseTextureCoordinates(char* pData, mesh3ds_t* output);
static void  ParseLocalAxis(char* pData, mesh3ds_t* output);
static void  ParseMaterialGroup(char* pData, group3ds_t* group);
static void  ParseMaterial(char* pData, material3ds_t* material);
static void  ParseTexture(char* pData, material3ds_t* material);
static void  ParseColorChunk(char* pData, float* fColor);
static int   ParseString(char* pData);
static float ParsePercentageChunk(char* pData);

void  SortTriangles(mesh3ds_t* data);
int SortByMaterial(triangle_t *a, triangle_t *b);
void RemoveDegenerates(mesh3ds_t* mesh);

#define FREE(p)   if (p) { free(p); p = NULL; }

long Read3dsFileFast(char* pFilename, data3ds_t* output)
{
   int      i;
	FILE*		pFile = NULL;
	char*		data = NULL;
   int      size = 0;
	
   memset(output, 0, sizeof(data3ds_t));

	pFile = fopen(pFilename, "rb");
	
	if (pFile)
	{
      fseek(pFile, 0, SEEK_END);

      size = ftell(pFile);

      fseek(pFile, 0, SEEK_SET);

      data = (char*)malloc(size);

      if (data != NULL)
      {
         fread(data, sizeof(char), size, pFile);
		   ParseMain3ds(data, output);
         free(data);
		}
		fclose(pFile);
	}

   for (i = 0; i < output->meshCount; i++)
   {
      RemoveDegenerates(&output->meshes[i]);
      SortTriangles(&output->meshes[i]);
   }
   
   output->vertCount = 0;
   output->triCount = 0;
   for (i = 0; i < output->meshCount; i++)
   {
      output->vertCount += output->meshes[i].vertCount;
      output->triCount += output->meshes[i].triCount;
   }

   Calculate3dsBoundingBox(output);
   Calculate3dsNormals(output);
   Calculate3dsTangentSpace(output);

	return 0;
}

void ParseMain3ds(char* pData, data3ds_t* output)
{
   int i, j, k;
   chunk3ds_t	chunk;
	long		lPos = 6;
	long        lLength = ((chunk3ds_t*)pData)->length;

	do
	{
		chunk = *(chunk3ds_t*)((long)pData + lPos);

		switch (chunk.id)
		{
		case 0x3d3d: // Edit3ds
			ParseEdit3ds(pData + lPos, output);
			break;
		default:
			break;
		}
		lPos += chunk.length;
	} while (lPos < lLength);

   for (i = 0; i < output->meshCount; i++)
   {
      for (j = 0; j < output->meshes[i].groupCount; j++)
      {
         for (k = 0; k < output->materialCount; k++)
         {
            if (!strcmp(output->meshes[i].groups[j].name, output->materials[k].name)) 
            {
               output->meshes[i].groups[j].mat = k;
               break;
            }
         }
      }
   }
}

void ParseEdit3ds(char* pData, data3ds_t* output)
{
   int      i;
	chunk3ds_t	chunk;
	chunk3ds_t	triChunk;
	long		lPos = 6;
   long     temp;
	long		lLength = ((chunk3ds_t*)pData)->length;

	do
	{
		chunk = *(chunk3ds_t*)((long)pData + lPos);
		
		switch (chunk.id)
		{
      case 0xAFFF: // EDIT_MATERIAL
         output->materialCount++;
			break;
  		case 0x4000: // EDIT_OBJECT
         temp = strlen(pData + lPos + 6) + 1;
         triChunk = *(chunk3ds_t*)((long)pData + lPos + 6 + temp);
         if (triChunk.id == 0x4100)
         {
            output->meshCount++;
         }
			break;
		default:
			break;
		}
		lPos += chunk.length;
	} while (lPos < lLength);

   if (output->materialCount)
   {
      output->materials = (material3ds_t*)malloc(sizeof(material3ds_t) * output->materialCount);
      memset(output->materials, 0, sizeof(material3ds_t) * output->materialCount);
   }

   if (output->meshCount)
   {
      output->meshes = (mesh3ds_t*)malloc(sizeof(mesh3ds_t) * output->meshCount);
      memset(output->meshes, 0, sizeof(mesh3ds_t) * output->meshCount);
   }

   lPos = 6;
   i = 0;
   output->meshCount = 0;
	do
	{
		chunk = *(chunk3ds_t*)((long)pData + lPos);
		
		switch (chunk.id)
		{
		case 0x4000: // EDIT_OBJECT
			ParseEditObject((char*)(pData + lPos), output);
			break;
      case 0xAFFF: // EDIT_MATERIAL
         if (output->materials)
         {
			   ParseMaterial((char*)(pData + lPos), &output->materials[i++]);
         }
         break;
		default:
			break;
		}
		lPos += chunk.length;
	} while (lPos < lLength);
}


void ParseEditObject(char* pData, data3ds_t* output)
{
   chunk3ds_t	chunk;
	long		   lPos = 6;
	long        lLength = ((chunk3ds_t*)pData)->length;

	lPos += strlen(pData + lPos) + 1;
	do
	{
		chunk = *(chunk3ds_t*)((long)pData + lPos);

		switch (chunk.id)
		{
		case 0x4100: // TRIANGLE_OBJECT
         strcpy(output->meshes[output->meshCount].name, pData + 6);
			ParseTriangleObject((char*)(pData + lPos), &output->meshes[output->meshCount++]);
			break;
		default:
			break;
		}
		lPos += chunk.length;
	} while (lPos < lLength);
}

void ParseTriangleObject(char* pData, mesh3ds_t* output)
{
	chunk3ds_t	chunk;
	long		   lPos = 6;
	long        lLength = ((chunk3ds_t*)pData)->length;
	long		   lMeshSize = 0;

	do
	{
		chunk = *(chunk3ds_t*)((long)pData + lPos);

		switch (chunk.id)
		{
		case 0x4110: // VERTEX_LIST
			ParseVertexList((char*)(pData + lPos + 6), output);
			break;
		case 0x4120: // FACE_LIST
			ParseFaceList((char*)(pData + lPos), output);
			break;
		case 0x4140: // MAPPING_COORDS
			ParseTextureCoordinates((char*)(pData + lPos + 6), output);
			break;
      case 0x4160: // LOCAL_AXIS
         ParseLocalAxis((char*)(pData + lPos + 6), output);
         break;
		case 0x4170: // TEXTURE_INFO
			//ParseTextureInfo((char*)(pData + lPos + 6), output);
			break;
		default:
			break;
		}
		lPos += chunk.length;
	} while (lPos < lLength);

   /*
   for (int i = 0; i < output->vertCount; i++)
   {
      for (int j = 0; j < 3; j++)
      {
         output->verts[i][j] += output->position[j];
      }
   }
   */
}

void ParseVertexList(char* pData, mesh3ds_t* output)
{
	unsigned short	shNumCoords = *(unsigned short*)(pData);
	pData += sizeof(unsigned short);

	output->vertCount = shNumCoords;
	output->verts = (float(*)[3])malloc(sizeof(float) * 3 * output->vertCount);
	memcpy(output->verts, pData, sizeof(float) * 3 * output->vertCount);
}

void ParseFaceList(char* pData, mesh3ds_t* output)
{
   int               i;
	long		         lPos = 6;
   long              lLength = ((chunk3ds_t*)pData)->length;
	long		         lStartGroups;
   chunk3ds_t        chunk;
	unsigned short		shNumFaces = *(unsigned short*)(pData + 6);
	
	lPos += 2; 
	output->triCount = shNumFaces;
	output->tris = (long(*)[3])malloc(sizeof(long) * 3 * output->triCount);

	unsigned short (*pVerts)[4] = (unsigned short(*)[4])(pData + lPos);
	for (i = 0; i < shNumFaces; i++)
	{
		for (int j = 0; j < 3; j++)
      {
         output->tris[i][j] = (long)pVerts[i][j];
	   }
   }

   lPos += sizeof(unsigned short) * 4 * shNumFaces;
   lStartGroups = lPos;

   do
	{
		chunk = *(chunk3ds_t*)((long)pData + lPos);

		switch (chunk.id)
		{
		case 0x4130: // MATERIAL_GROUP
         output->groupCount++;
			break;
      case 0x4150: // SMOOTH_GROUP
         output->smooth = (long*)malloc(sizeof(long) * output->triCount);
         if (output->smooth)
         {
            memcpy(output->smooth, pData + lPos + 6, sizeof(long) * output->triCount);
         }
         break;
		}
		lPos += chunk.length;
	} while (lPos < lLength);


   if (output->groupCount)
   {
      output->groups = (group3ds_t*)malloc(sizeof(group3ds_t) * output->groupCount);   
      memset(output->groups, 0, sizeof(group3ds_t) * output->groupCount);

      if (output->groups)
      {
         lPos = lStartGroups;

         i = 0;
  	      do
	      {
		      chunk = *(chunk3ds_t*)((long)pData + lPos);

		      switch (chunk.id)
		      {
			   case 0x4130: // MATERIAL_GROUP
				   ParseMaterialGroup(pData + lPos + 6, &output->groups[i++]);
				   break;
			   default:
				   break;
		      }
		      lPos += chunk.length;
	      } while (lPos < lLength);
      }
   }
}

void ParseTextureCoordinates(char* pData, mesh3ds_t* output)
{
	unsigned short	shNumCoords = *(unsigned short*)(pData);
	pData += sizeof(unsigned short);

	output->texCoordCount = shNumCoords;
   
   output->texCoords = (float(*)[2])malloc(sizeof(float) * 2 * output->texCoordCount);
	memcpy(output->texCoords, pData, sizeof(float) * 2 * output->texCoordCount);
}

void ParseLocalAxis(char* pData, mesh3ds_t* output)
{
   memcpy(output->axis, pData, sizeof(float) * 9);

   pData += sizeof(float) * 9;

   memcpy(output->position, pData, sizeof(float) * 3);
}

void ParseMaterialGroup(char* pData, group3ds_t* group)
{
	char*          pName;
	int	         nCount;
	unsigned short	shNumFaces;
   unsigned short* pFaces;
	long	         lGroupSize = 0;

   pName = pData;
	nCount = strlen(pData) + 1;
	
	pData += nCount;
	shNumFaces = *(unsigned short*)(pData);
	pData += sizeof(unsigned short);

	group->tris = (long*)malloc(shNumFaces * sizeof(long));
	
	if (group->tris)
	{
		memset(group->tris, 0, shNumFaces * sizeof(long));


		group->size = shNumFaces;
		group->mat = 0;
		strcpy(group->name, pName);

		pFaces = (unsigned short*)pData;
		for (int i = 0; i < shNumFaces; i++)
		{
			group->tris[i] = pFaces[i];
		}
	}
}

void ParseMaterial(char* pData, material3ds_t* material)
{
	chunk3ds_t	chunk;
	long		lPos = 6;
	long        lLength = ((chunk3ds_t*)pData)->length;

	do
	{
		chunk = *(chunk3ds_t*)((long)pData + lPos);

		switch (chunk.id)
		{
			case 0xa000: // MAT_NAME
				strcpy(material->name, pData + lPos + 6);
				break;
			case 0xa010: // MAT_AMBIENT
				ParseColorChunk(pData + lPos, material->ambient);
				material->ambient[3] = 1.0f;
				break;
			case 0xa020: // MAT_DIFFUSE
				ParseColorChunk(pData + lPos, material->diffuse);
				material->diffuse[3] = 1.0f;
				break;
			case 0xa030: // MAT_SPECUAR
				ParseColorChunk(pData + lPos, material->specular);
				material->specular[3] = 1.0f;
				break;
			case 0xa040: // MAT_SHININESS
				material->shininess = ParsePercentageChunk(pData + lPos) * 140.0f;
				break;
			case 0xa041: // MAT_SHIN2PCT
				break;
			case 0xa042: // MAT_SHIN3PCT
				break;
			case 0xa050: // MAT_TRANSPANENCY
				break;
			case 0xa080: // MAT_EMISSION
				ParseColorChunk(pData + lPos, material->emission);
				material->emission[3] = 1.0f;
				break;
			case 0xa200: // MAT_TEXMAP
				ParseTexture((char*)(pData + lPos), material);
				break;
			default:
				break;
		}
		lPos += chunk.length;
	} while (lPos < lLength);
}

/*
void ParseTextureInfo(char* pData)
{
	float fRepeatX;
	float fRepeatY;
	float fScale;

	fRepeatX = (float)(*((float*)&pData[2]));
	fRepeatY = (float)(*((float*)&pData[6]));
	fScale = (float)(*((float*)&pData[70]));
}
*/

void ParseTexture(char* pData, material3ds_t* material)
{
	chunk3ds_t	chunk;
	long		lPos = 6;
	long        lLength = ((chunk3ds_t*)pData)->length;

	do
	{
		chunk = *(chunk3ds_t*)((long)pData + lPos);

		switch (chunk.id)
		{
			case 0xa300: // MAT_MAP_NAME
            strcpy(material->texture, pData + lPos + 6);
				break;
			case 0xa351: // MAT_MAP_TILING
				break;
			case 0xa354: // MAT_MAP_U_SCALE
				break;
			case 0xa356: // MAT_MAP_V_SCALE
				break;
			case 0xa358: // MAT_MAP_U_OFFSET
				break;
			case 0xa35a: // MAT_MAP_V_OFFSET
				break;
			case 0xa35c: // MAT_MAP_V_ANG
				break;
			default:
				break;
		}
		lPos += chunk.length;
	} while (lPos < lLength);
}

void ParseColorChunk(char* pData, float* fColor)
{
	chunk3ds_t	chunk;
	long		lPos = 6;
	
	chunk = *(chunk3ds_t*)((long)pData + lPos);

	unsigned char chRGB[3];
	switch (chunk.id)
	{
	case 0x0010: // COLOR_F
		memcpy(fColor, pData + lPos + 6, sizeof(float) * 3);
		break;
	case 0x0011: // COLOR_24
		memcpy(chRGB, pData + lPos + 6, sizeof(char) * 3);
		
		fColor[0] = float(chRGB[0]) / 256.0f;
		fColor[1] = float(chRGB[1]) / 256.0f;
		fColor[2] = float(chRGB[2]) / 256.0f;
		break;
	default:
		break;
	}
}

int ParseString(char* pData)
{
	int nCount = 0;
	while (pData[nCount] != '\0')
	{
		nCount++;
	} 
	return nCount;
}

float ParsePercentageChunk(char* pData)
{
	chunk3ds_t		chunk;
	long			lPos = 6;
	float			fPercentage = 0.0f;
	short		shPercent;
	
	chunk = *(chunk3ds_t*)((long)pData + lPos);

	switch (chunk.id)
	{
	case 0x0030: // INT_PERCENTAGE (short)
		memcpy(&shPercent, pData + lPos + 6, sizeof(short));
		fPercentage = (float)shPercent;
		break;
	case 0x0031: // FLOAT_PERCENTAGE
		memcpy(&fPercentage, pData + lPos + 6, sizeof(float));
		break;
	default:
		break;
	}

	fPercentage /= 100.0f;
	return fPercentage;
}
