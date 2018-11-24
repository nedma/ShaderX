#include "3ds.h"
#include <index_t.h>

//========//
// macros //
//========//
#define VEC_Add(a, b, c)			((c)[0] = (a)[0] + (b)[0], (c)[1] = (a)[1] + (b)[1], (c)[2] = (a)[2] + (b)[2])
#define VEC_AddTo(a, b)				((a)[0] += (b)[0], (a)[1] += (b)[1], (a)[2] += (b)[2])					
#define VEC_AddScalar(a, b, c)	((c)[0] = (a)[0] + (b), (c)[1] = (a)[1] + (b), (c)[2] = (a)[2] + (b))
#define VEC_AddScalarTo(a, b)		((a)[0] += (b), (a)[1] += (b), (a)[2] += (b))					
#define VEC_Copy(a, b)				((b)[0] = (a)[0], (b)[1] += (a)[1], (b)[2] += (a)[2])					
#define VEC_Cross(a, b, c)			((c)[0] = (a)[1] * (b)[2] - (a)[2] * (b)[1], (c)[1] = (a)[2] * (b)[0] - (a)[0] * (b)[2], (c)[2] = (a)[0] * (b)[1] - (a)[1] * (b)[0])
#define VEC_Divide(a, b, c)		((c)[0] = (a)[0] / (b)[0], (c)[1] = (a)[1] / (b)[1], (c)[2] = (a)[2] / (b)[2])
#define VEC_DivideBy(a, b)			((a)[0] /= (b)[0], (a)[1] /= (b)[1], (a)[2] /= (b)[2])
#define VEC_DivideByScalar(v, s)	((v)[0] /= s, (v)[1] /= s, (v)[2] /= s)	
#define VEC_DivideScalar(v, s, o)	((o)[0] = (v)[0] / s, (o)[1] = (v)[1] / s, (o)[2] = v[2] / s)				
#define VEC_DotProduct(a, b)		((a)[0] * (b)[0] + (a)[1] * (b)[1] + (a)[2] * (b)[2])
#define VEC_Dot4(a, b)		      ((a)[0] * (b)[0] + (a)[1] * (b)[1] + (a)[2] * (b)[2] + (a)[3] * (b)[3])
#define VEC_MidPoint(a, b, c)		((c)[0] = (b)[0] + ((a)[0] - (b)[0]) * 0.5f, (c)[1] = (b)[1] + ((a)[1] - (b)[1]) * 0.5f, (c)[2] = (b)[2] + ((a)[2] - (b)[2]) * 0.5f)
#define VEC_Mult(a, b, c)			((c)[0] = (a)[0] * (b)[0], (c)[1] = (a)[1] * (b)[1], (c)[2] = (a)[2] * (b)[2])
#define VEC_MultBy(a, b)			((a)[0] *= (b)[0], (a)[1] *= (b)[1], (a)[2] *= (b)[2])
#define VEC_MultByScalar(v, s)	((v)[0] *= s, (v)[1] *= s, (v)[2] *= s)	
#define VEC_MultScalar(v, s, o)	((o)[0] = (v)[0] * s, (o)[1] = (v)[1] * s, (o)[2] = v[2] * s)				
#define VEC_Negate(a, b)			((b)[0] = -(a)[0], (b)[1] = -(a)[1], (b)[2] = -(a)[2])
#define VEC_Scale(v, s, o)			((o)[0] = (v)[0] * s, (o)[1] = (v)[1] * s, (o)[2] = v[2] * s)				
#define VEC_ScaleBy(v, s)			((v)[0] *= s, (v)[1] *= s, (v)[2] *= s)	
#define VEC_Set(v, x, y, z)		((v)[0] = x, (v)[1] = y, (v)[2] = z)
#define VEC_Subtract(a, b, c)		((c)[0] = (a)[0] - (b)[0], (c)[1] = (a)[1] - (b)[1], (c)[2] = (a)[2] - (b)[2])
#define VEC_SubtractFrom(a, b)	((a)[0] -= (b)[0], (a)[1] -= (b)[1], (a)[2] -= (b)[2])
#define VEC_Magnitude(v)			(sqrt((v)[0] * (v)[0] + (v)[1] * (v)[1] + (v)[2] * (v)[2]))
#define VEC_Normalize(v)         { double __mag = 1.0 / VEC_Magnitude(v); \
                                   (v)[0] *= __mag; (v)[1] *= __mag; (v)[2] *= __mag; }
#define VEC_Normalizef(v)        { float __mag = 1.0f / (float)VEC_Magnitude(v); \
                                   (v)[0] *= __mag; (v)[1] *= __mag; (v)[2] *= __mag; }

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

typedef void (*cb)(chunk3ds_t*, void*);

long Read3dsFile(char* pFilename, data3ds_t* output);
void Free3dsData(data3ds_t* data);
void ReadString(char* string);
void ReadChunkArray(long length, void (*callback)(chunk3ds_t*, void*), void* data);
void InspectChunkArray(long length, void (*callback)(chunk3ds_t*, void*), void* data);
void ReadMain3ds(chunk3ds_t* chunk, data3ds_t* output);
void InspectEdit3ds(chunk3ds_t* chunk, data3ds_t* output);
void InspectEditObject(chunk3ds_t* chunk, data3ds_t* output);
void ReadEdit3ds(chunk3ds_t* chunk, data3ds_t* output);
void ReadEditObject(chunk3ds_t* chunk, data3ds_t* output);
void ReadTriangleObject(chunk3ds_t* chunk, mesh3ds_t* output);
void ReadVertexList(mesh3ds_t* output);
void ReadFaceList(mesh3ds_t* output, long length);
void InspectFaceSubs(chunk3ds_t* chunk, mesh3ds_t* output);
void ReadFaceSubs(chunk3ds_t* chunk, mesh3ds_t* output);
void ReadTextureCoordinates(mesh3ds_t* output);
void ReadLocalAxis(mesh3ds_t* output);
void ReadMaterialGroup(group3ds_t* group);
void ReadMaterial(chunk3ds_t* chunk, material3ds_t* material);
void ReadTexture(chunk3ds_t* chunk, material3ds_t* material);
void ReadColorChunk(chunk3ds_t* chunk, float* fColor);
void ReadPercentageChunk(chunk3ds_t* chunk, float* value);
short ReadChunkId( );
void SkipChunk( );

static FILE* g_pFile = NULL;
static char  g_szString[64];
#define FREE(p)   if (p) { free(p); p = NULL; }

int ComparePosition(float *a, float *b)
{
   int i;

   for (i = 0; i < 3; i++)
   {
      if (a[i] > b[i])
      {
         return 1;
      }
      if (a[i] < b[i])
      {
         return -1;
      }
   }

   return 0;
}

int SortByMaterial(triangle_t *a, triangle_t *b)
{
   if (a->mat > b->mat)
   {
      return 1;
   }
   if (a->mat < b->mat)
   {
      return -1;
   }

   if (a->smooth > b->smooth)
   {
      return 1;
   }
   if (a->smooth < b->smooth)
   {
      return -1;
   }

   return 0;
}

void  CalculateMesh3dsBoundingBox(mesh3ds_t* mesh)
{
   int i, j;

   for (i = 0; i < mesh->vertCount; i++)
   {
      if (i == 0)
      {
         memcpy(mesh->max, mesh->verts[i], sizeof(float) * 3);
         memcpy(mesh->min, mesh->verts[i], sizeof(float) * 3);
      }
      else
      {
         for (j = 0; j < 3; j++)
         {
            if (mesh->verts[i][j] > mesh->max[j])
            {
               mesh->max[j] = mesh->verts[i][j];
            }
            if (mesh->verts[i][j] < mesh->min[j])
            {
               mesh->min[j] = mesh->verts[i][j];
            }
         }
      }
   }

   mesh->center[0] = mesh->min[0] + (mesh->max[0] - mesh->min[0]) * 0.5f;
   mesh->center[1] = mesh->min[1] + (mesh->max[1] - mesh->min[1]) * 0.5f;
   mesh->center[2] = mesh->min[2] + (mesh->max[2] - mesh->min[2]) * 0.5f;
}

void  Calculate3dsBoundingBox(data3ds_t* model)
{
   int i, j;

   for (i = 0; i < model->meshCount; i++)
   {
      CalculateMesh3dsBoundingBox(&model->meshes[i]);

      if (i == 0)
      {
         memcpy(model->max, model->meshes[i].max, sizeof(float) * 3);
         memcpy(model->min, model->meshes[i].min, sizeof(float) * 3);
      }
      else
      {
         for (j = 0; j < 3; j++)
         {
            if (model->meshes[i].max[j] > model->max[j])
            {
               model->max[j] = model->meshes[i].max[j];
            }
            if (model->meshes[i].min[j] < model->min[j])
            {
               model->min[j] = model->meshes[i].min[j];
            }
         }
      }
   }

   model->center[0] = model->min[0] + (model->max[0] - model->min[0]) * 0.5f;
   model->center[1] = model->min[1] + (model->max[1] - model->min[1]) * 0.5f;
   model->center[2] = model->min[2] + (model->max[2] - model->min[2]) * 0.5f;
}

//====================================================================//
// compute normals for this mesh taking smoothing groups into account //
//====================================================================//
static void CalculateMeshNormals(mesh3ds_t* mesh)
{
	int	      i, j, k;
   int         normCount;
   long        (*triRefs)[50];
   float       (*faceNorms)[3];
	float		   v1[3];
	float		   v2[3];

   //======================//
   // allocate the normals //
   //======================//
   mesh->norms = (float(*)[3])malloc(sizeof(float) * 3 * mesh->triCount * 3);
   if (mesh->norms == NULL)
   {
      return;
   }
   memset(mesh->norms, 0, sizeof(float) * 3 * mesh->triCount * 3);

   if (mesh->smooth == NULL)
   {
      for (i = 0; i < mesh->triCount; i++)
		{
			VEC_Subtract(mesh->verts[mesh->tris[i][1]], mesh->verts[mesh->tris[i][0]], v1);
			VEC_Subtract(mesh->verts[mesh->tris[i][2]], mesh->verts[mesh->tris[i][0]], v2);
			VEC_Cross(v1, v2, mesh->norms[i * 3]);
			VEC_Normalizef(mesh->norms[i * 3]);
         memcpy(mesh->norms[i * 3 + 1], mesh->norms[i * 3], sizeof(float) * 3);
         memcpy(mesh->norms[i * 3 + 2], mesh->norms[i * 3], sizeof(float) * 3);
		}
      return;
   }


   index_t  vertIndex;
   int      vert = 0;
   int      result = 0;

   //============================================//
   // index the vertices based on position alone //
   //============================================//
   indexArray(&vertIndex, (char*)mesh->verts, sizeof(float) * 3, mesh->vertCount, (sortFunc_t)ComparePosition);

   //============================================================================//
   // build a table that links each vertex to all triangles that use said vertex // 
   //============================================================================//
   triRefs = (long(*)[50])malloc(sizeof(long) * 50 * vertIndex.count);
   if (triRefs == NULL)
   {
      return;
   }
   memset(triRefs, 0, sizeof(unsigned long) * 50 * vertIndex.count);		

   for (i = 0; i < mesh->triCount; i++)
   {
      for (j = 0; j < 3; j++)
      {
         vert = indexFind(&vertIndex, mesh->verts[mesh->tris[i][j]], &result);
         if (triRefs[vert][0] < 48)
         {
            triRefs[vert][0]++;
            triRefs[vert][triRefs[vert][0]] = i;
         }
      }
   }

   //========================================//
   // allocate a buffer for the flat normals //
   //========================================//
   faceNorms = (float(*)[3])malloc(sizeof(float) * 3 * mesh->triCount);
	if ((faceNorms != NULL) && (triRefs != NULL))
	{
      memset(faceNorms, 0, sizeof(float) * 3 * mesh->triCount);

      //==============================================//
		// go through every triangle to find its normal //
      //==============================================//
		for (i = 0; i < mesh->triCount; i++)
		{
			VEC_Subtract(mesh->verts[mesh->tris[i][1]], mesh->verts[mesh->tris[i][0]], v1);
			VEC_Subtract(mesh->verts[mesh->tris[i][2]], mesh->verts[mesh->tris[i][0]], v2);
			VEC_Cross(v1, v2, faceNorms[i]);
			VEC_Normalizef(faceNorms[i]);
		}

		for (i = 0; i < mesh->triCount; i++)
		{
         for (j = 0; j < 3; j++)
         {
            vert = indexFind(&vertIndex, mesh->verts[mesh->tris[i][j]], &result);
            
            normCount = 0;
			   for (k = 1; k <= triRefs[vert][0]; k++)
            {
               if (mesh->smooth[i] == mesh->smooth[triRefs[vert][k]])
					{
						VEC_AddTo(mesh->norms[i * 3 + j], faceNorms[triRefs[vert][k]]);
						normCount++;
					}
			   }

  			   VEC_DivideByScalar(mesh->norms[i * 3 + j], (float)normCount);
			   VEC_Normalizef(mesh->norms[i * 3 + j]);
         }
		}
   }

   indexFree(&vertIndex);

   /*
   //============================================================================//
   // build a table that links each vertex to all triangles that use said vertex // 
   //============================================================================//
   triRefs = (long(*)[50])malloc(sizeof(long) * 50 * mesh->vertCount);
   if (triRefs == NULL)
   {
      return;
   }
   memset(triRefs, 0, sizeof(unsigned long) * 50 * mesh->vertCount);		

   for (i = 0; i < mesh->triCount; i++)
   {
      for (j = 0; j < 3; j++)
      {
         if (triRefs[mesh->tris[i][j]][0] < 48)
         {
            triRefs[mesh->tris[i][j]][0]++;
            triRefs[mesh->tris[i][j]][triRefs[mesh->tris[i][j]][0]] = i;
         }
      }
   }

   //========================================//
   // allocate a buffer for the flat normals //
   //========================================//
   faceNorms = (float(*)[3])malloc(sizeof(float) * 3 * mesh->triCount);
	if ((faceNorms != NULL) && (triRefs != NULL))
	{
      memset(faceNorms, 0, sizeof(float) * 3 * mesh->triCount);

      //==============================================//
		// go through every triangle to find its normal //
      //==============================================//
		for (i = 0; i < mesh->triCount; i++)
		{
			VEC_Subtract(mesh->verts[mesh->tris[i][1]], mesh->verts[mesh->tris[i][0]], v1);
			VEC_Subtract(mesh->verts[mesh->tris[i][2]], mesh->verts[mesh->tris[i][0]], v2);
			VEC_Cross(v1, v2, faceNorms[i]);
			VEC_Normalizef(faceNorms[i]);
		}

		for (i = 0; i < mesh->triCount; i++)
		{
         for (j = 0; j < 3; j++)
         {
            normCount = 0;
			   for (k = 1; k <= triRefs[mesh->tris[i][j]][0]; k++)
            {
               if (mesh->smooth[i] == mesh->smooth[triRefs[mesh->tris[i][j]][k]])
					{
						VEC_AddTo(mesh->norms[i * 3 + j], faceNorms[triRefs[mesh->tris[i][j]][k]]);
						normCount++;
					}
			   }

  			   VEC_DivideByScalar(mesh->norms[i * 3 + j], (float)normCount);
			   VEC_Normalizef(mesh->norms[i * 3 + j]);
         }
		}
   }
   */

   //==========================================//
   // free up the local buffers that were used //
   //==========================================//
   if (triRefs)
   {
      free(triRefs);
   }

   if (faceNorms)
   {
      free(faceNorms);
   }
}

//============================//
// compute mesh tangent space //
//============================//
static void SmoothMeshTangentSpace(mesh3ds_t* mesh)
{
   int	      i, j, k;
   int         tri, vert;
   int         tanCount;
   int         (*tanRefs)[50];
   float       tanSpace[9];
   float       tempBinorm[3];
   float       dotTangent;
   float       dotBinormal;
   float       dotNormal;

   float       (*newTanSpace)[9];

   newTanSpace = (float(*)[9])malloc(sizeof(float) * 9 * mesh->triCount * 3);
   if (newTanSpace == NULL)
   {
      return;
   }

   //===========================================================//
   // build a table that links verts that share a tangent space // 
   //===========================================================//
   tanRefs = (int(*)[50])malloc(sizeof(int) * 50 * mesh->vertCount);
   if (tanRefs == NULL)
   {
      return;
   }
   memset(tanRefs, 0, sizeof(int) * 50 * mesh->vertCount);		

   //===========================//
   // go through every triangle //
   //===========================//
   for (i = 0; i < mesh->triCount; i++)
   {
      //=========================================//
      // go through each vertex of this triangle //
      //=========================================//
      for (j = 0; j < 3; j++)
      {
         if (tanRefs[mesh->tris[i][j]][0] < 48)
         {
            tanRefs[mesh->tris[i][j]][0]++;
            tanRefs[mesh->tris[i][j]][tanRefs[mesh->tris[i][j]][0]] = i * 3 + j;
         }
      }
   }

	for (i = 0; i < mesh->triCount; i++)
	{
      for (j = 0; j < 3; j++)
      {
         memset(tanSpace, 0, sizeof(float) * 9);
         tanCount = 0;
			for (k = 1; k <= tanRefs[mesh->tris[i][j]][0]; k++)
         {
            tri = tanRefs[mesh->tris[i][j]][k] / 3;
            vert = tanRefs[mesh->tris[i][j]][k] % 3;

            dotTangent = VEC_DotProduct(&mesh->tangentSpace[i * 3 + j][0], &mesh->tangentSpace[tri * 3 + vert][0]); 
            dotBinormal = VEC_DotProduct(&mesh->tangentSpace[i * 3 + j][3], &mesh->tangentSpace[tri * 3 +vert][3]); 
            dotNormal = VEC_DotProduct(&mesh->tangentSpace[i * 3 + j][6], &mesh->tangentSpace[tri * 3 +vert][6]); 
                           
            if ((dotTangent > 0.85) && (dotBinormal > 0.85) && (dotNormal > 0.85))
            {
					VEC_AddTo(&tanSpace[0], &mesh->tangentSpace[tri * 3 + vert][0]);
					VEC_AddTo(&tanSpace[3], &mesh->tangentSpace[tri * 3 + vert][3]);
					VEC_AddTo(&tanSpace[6], &mesh->tangentSpace[tri * 3 + vert][6]);
               tanCount++;
				}
			}

  			VEC_DivideByScalar(&tanSpace[0], (float)tanCount);
  			VEC_DivideByScalar(&tanSpace[3], (float)tanCount);
  			VEC_DivideByScalar(&tanSpace[6], (float)tanCount);
			VEC_Normalizef(&tanSpace[0]);
			VEC_Normalizef(&tanSpace[3]);
			VEC_Normalizef(&tanSpace[6]);

         VEC_Cross(&tanSpace[0], &tanSpace[6], tempBinorm);
         VEC_Normalizef(tempBinorm);
         dotBinormal = VEC_DotProduct(&tanSpace[3], tempBinorm);
         if (dotBinormal < 0.0f)
         {
            tempBinorm[0] = -tempBinorm[0];
            tempBinorm[1] = -tempBinorm[1];
            tempBinorm[2] = -tempBinorm[2];
         }

         memcpy(newTanSpace[i * 3 + j], tanSpace, sizeof(float) * 9);
         memcpy(&newTanSpace[i * 3 + j][3], tempBinorm, sizeof(float) * 3);
         //memcpy(mesh->tangentSpace[i * 3 + j], tanSpace, sizeof(float) * 9);
      }
	}

   FREE(mesh->tangentSpace);
   mesh->tangentSpace = newTanSpace;

   //==========================================//
   // free up the local buffers that were used //
   //==========================================//
   if (tanRefs)
   {
      free(tanRefs);
   }
}


int CompareLong(long* a, long* b)
{
   if (*a > *b)
   {
      return 1;
   }
   if (*a < *b)
   {
      return -1;
   }

   return 0;  
}

void RemoveDegenerates(mesh3ds_t* mesh)
{
   int i;
   int j;
   int k;
   int l;
   long* found;

   for (i = 0; i < mesh->triCount; i++)
   {
      if ((mesh->tris[i][0] >= mesh->vertCount) ||
          (mesh->tris[i][1] >= mesh->vertCount) ||
          (mesh->tris[i][2] >= mesh->vertCount))
      {
         i = i;
      }
   }

   for (i = 0;i < mesh->groupCount; i++)
   {
      qsort(mesh->groups[i].tris, mesh->groups[i].size, sizeof(long), (sortFunc_t)CompareLong);
   }

   for (i = 0; i < mesh->triCount; i++)
   {
      if (((mesh->tris[i][0] == mesh->tris[i][1]) ||
           (mesh->tris[i][0] == mesh->tris[i][2]) ||
           (mesh->tris[i][1] == mesh->tris[i][2])) ||

          ((mesh->verts[mesh->tris[i][0]][0] == mesh->verts[mesh->tris[i][1]][0]) &&
           (mesh->verts[mesh->tris[i][0]][1] == mesh->verts[mesh->tris[i][1]][1]) &&
           (mesh->verts[mesh->tris[i][0]][2] == mesh->verts[mesh->tris[i][1]][2])) ||

          ((mesh->verts[mesh->tris[i][0]][0] == mesh->verts[mesh->tris[i][2]][0]) &&
           (mesh->verts[mesh->tris[i][0]][1] == mesh->verts[mesh->tris[i][2]][1]) &&
           (mesh->verts[mesh->tris[i][0]][2] == mesh->verts[mesh->tris[i][2]][2])) ||

          ((mesh->verts[mesh->tris[i][1]][0] == mesh->verts[mesh->tris[i][2]][0]) &&
           (mesh->verts[mesh->tris[i][1]][1] == mesh->verts[mesh->tris[i][2]][1]) &&
           (mesh->verts[mesh->tris[i][1]][2] == mesh->verts[mesh->tris[i][2]][2])))
      {
         if (i != (mesh->triCount - 1))
         {
            memmove(&mesh->tris[i], &mesh->tris[i+1], sizeof(long) * 3 * (mesh->triCount - i - 1));
         }

         for (j = 0; j < mesh->groupCount; j++)
         {
            found = (long*)bsearch(&i, mesh->groups[j].tris, mesh->groups[j].size, sizeof(long), (sortFunc_t)CompareLong);
         
            if (found != NULL)
            {
               k = ((int)found - (int)mesh->groups[j].tris) / sizeof(long);

               if (k < mesh->groups[j].size - 1)
               {
                  memmove(&mesh->groups[j].tris[k], &mesh->groups[j].tris[k+1], sizeof(long) * (mesh->groups[j].size - k - 1));
               }
               mesh->groups[j].size--;

               for (l = k; l < mesh->groups[j].size; l++)
               {
                  mesh->groups[j].tris[l]--;
               }
            }
         }
         mesh->triCount--;
         i--;
      }
   }
}
/*
void RemoveDegenerates(mesh3ds_t* mesh)
{
   int i;
   int j;
   int k;

   for (i = 0; i < mesh->triCount; i++)
   {
      if (((mesh->tris[i][0] == mesh->tris[i][1]) ||
           (mesh->tris[i][0] == mesh->tris[i][2]) ||
           (mesh->tris[i][1] == mesh->tris[i][2])) ||

          ((mesh->verts[mesh->tris[i][0]][0] == mesh->verts[mesh->tris[i][1]][0]) &&
           (mesh->verts[mesh->tris[i][0]][1] == mesh->verts[mesh->tris[i][1]][1]) &&
           (mesh->verts[mesh->tris[i][0]][2] == mesh->verts[mesh->tris[i][1]][2])) ||

          ((mesh->verts[mesh->tris[i][0]][0] == mesh->verts[mesh->tris[i][2]][0]) &&
           (mesh->verts[mesh->tris[i][0]][1] == mesh->verts[mesh->tris[i][2]][1]) &&
           (mesh->verts[mesh->tris[i][0]][2] == mesh->verts[mesh->tris[i][2]][2])) ||

          ((mesh->verts[mesh->tris[i][1]][0] == mesh->verts[mesh->tris[i][2]][0]) &&
           (mesh->verts[mesh->tris[i][1]][1] == mesh->verts[mesh->tris[i][2]][1]) &&
           (mesh->verts[mesh->tris[i][1]][2] == mesh->verts[mesh->tris[i][2]][2])))
      {
         if (i != (mesh->triCount - 1))
         {
            memmove(&mesh->tris[i], &mesh->tris[i+1], sizeof(long) * 3 * (mesh->triCount - i - 1));
         }

         for (j = 0; j < mesh->groupCount; j++)
         {
            for (k = 0; k < mesh->groups[j].size; k++)
            {
               if (mesh->groups[j].tris[k] == i)
               {
                  if (k < mesh->groups[j].size - 1)
                  {
                     memmove(&mesh->groups[j].tris[k], &mesh->groups[j].tris[k+1], sizeof(long) * (mesh->groups[j].size - k - 1));
                  }
                  mesh->groups[j].size--;
                  k--;
               }
               else if (mesh->groups[j].tris[k] > i)
               {
                  mesh->groups[j].tris[k]--;
               }
            }
         }


         mesh->triCount--;
         i--;
      }
   }
}
*/

//======================================================//
// compute normals taking smoothing groups into account //
//======================================================//
void Calculate3dsNormals(data3ds_t* model)
{
   int i;

   for (i = 0; i < model->meshCount; i++)
   {
      CalculateMeshNormals(&model->meshes[i]);
   }
}

void TangentSpace(float* vert1, float* vert2, float* vert3, 
                         float* t1, float* t2, float* t3, 
                         float* normal, float *tangentSpace)
{
   float    a, b;
   float    u1, u2;
   float    v1, v2;
   float    edge1[3];
   float    edge2[3];
   float    dot;
   float    dvTmp[3];
   float    duTmp[3];


   if ((t1[0] == t2[0]) || 
       (t1[1] == t2[1]) || 
       (t1[0] == t3[0]) || 
       (t1[1] == t3[1]))
   {
      tangentSpace[0] = 1.0f;
      tangentSpace[1] = 0.0f;
      tangentSpace[2] = 0.0f;

      tangentSpace[3] = 0.0f;
      tangentSpace[4] = 1.0f;
      tangentSpace[5] = 0.0f;

      tangentSpace[6] = 0.0f;
      tangentSpace[7] = 0.0f;
      tangentSpace[8] = 1.0f;
      return;
   }

   //===================================//
   // compute vertex space edge vectors //
   //===================================//
   edge1[0] = vert2[0] - vert1[0];
   edge1[1] = vert2[1] - vert1[1];
   edge1[2] = vert2[2] - vert1[2];
   edge2[0] = vert3[0] - vert1[0];
   edge2[1] = vert3[1] - vert1[1];
   edge2[2] = vert3[2] - vert1[2];
   
   //====================================//
   // compute texture space edge vectors //
   //====================================//
   u1 = t2[0] - t1[0];
   u2 = t3[0] - t1[0];
   v1 = t2[1] - t1[1];
   v2 = t3[1] - t1[1];

   //=========================================================//
   // compute releation between how edges change as u changes //
   //=========================================================//
   a = (u1 - v1 * u2 / v2);
   if (a != 0.0f)
   {
      a = 1.0f / a;
   }
   b = (u2 - v2 * u1 / v1);
   if (b != 0.0f)
   {
      b = 1.0f / b;
   }
   duTmp[0] = a * edge1[0] + b * edge2[0];
   duTmp[1] = a * edge1[1] + b * edge2[1];
   duTmp[2] = a * edge1[2] + b * edge2[2];
   VEC_Normalizef(duTmp);

   //=========================================================//
   // compute releation between how edges change as v changes //
   //=========================================================//
   a = (v1 - u1 * v2 / u2);
   if (a != 0.0f)
   {
      a = 1.0f / a;
   }
   b = (v2 - u2 * v1 / u1);
   if (b != 0.0f)
   {
      b = 1.0f / b;
   }
   dvTmp[0] = a * edge1[0] + b * edge2[0];
   dvTmp[1] = a * edge1[1] + b * edge2[1];
   dvTmp[2] = a * edge1[2] + b * edge2[2];
   VEC_Normalizef(dvTmp);

   //=====================================//
   // normal portion of the tangent space //
   //=====================================//
   tangentSpace[6] = normal[0];
   tangentSpace[7] = normal[1];
   tangentSpace[8] = normal[2];

   dot = VEC_DotProduct(duTmp, &tangentSpace[6]);

   //======================================//
   // tangent portion of the tangent space //
   //======================================//
   tangentSpace[0] = duTmp[0] - (dot * tangentSpace[6]);
   tangentSpace[1] = duTmp[1] - (dot * tangentSpace[7]);
   tangentSpace[2] = duTmp[2] - (dot * tangentSpace[8]);
   VEC_Normalizef(tangentSpace);

   dot = VEC_DotProduct(dvTmp, &tangentSpace[6]);

   //=======================================//
   // binormal portion of the tangent space //
   //=======================================//
   tangentSpace[3] = dvTmp[0] - (dot * tangentSpace[6]);
   tangentSpace[4] = dvTmp[1] - (dot * tangentSpace[7]);
   tangentSpace[5] = dvTmp[2] - (dot * tangentSpace[8]);
   VEC_Normalizef(&tangentSpace[3]);
}

static void CalculateMeshTangentSpace(mesh3ds_t* mesh)
{
   int   i, j;
   
   if (mesh->norms == NULL)
   {
      CalculateMeshNormals(mesh);
   }

   if ((mesh->norms == NULL) || (mesh->texCoords == NULL))
   {
      return;
   }

   if (mesh->tangentSpace != NULL)
   {
      free(mesh->tangentSpace);
      mesh->tangentSpace = NULL;
   }

   //========================//
   // allocate tangent space //
   //========================//
   mesh->tangentSpace = (float(*)[9])malloc(sizeof(float) * 9 * mesh->triCount * 3);
   if (mesh->tangentSpace == NULL)
   {
      return;
   }
   memset(mesh->tangentSpace, 0, sizeof(float) * 9 * mesh->triCount * 3);		

   for (i = 0; i < mesh->triCount; i++)
   {
      for (j = 0; j < 3; j++)
      {
         TangentSpace(
            mesh->verts[mesh->tris[i][0]], mesh->verts[mesh->tris[i][1]], mesh->verts[mesh->tris[i][2]],
            mesh->texCoords[mesh->tris[i][0]], mesh->texCoords[mesh->tris[i][1]], mesh->texCoords[mesh->tris[i][2]],
            mesh->norms[i * 3 + j], mesh->tangentSpace[i * 3 + j]);
      }
   }
}

//=======================//
// compute tangent space //
//=======================//
void Calculate3dsTangentSpace(data3ds_t* model)
{
   int i;

   for (i = 0; i < model->meshCount; i++)
   {
      CalculateMeshTangentSpace(&model->meshes[i]);
      if (model->meshes[i].tangentSpace)
      {
         SmoothMeshTangentSpace(&model->meshes[i]);
      }
   }
}

//======================================================//
// sorts triangles by group and then by smoothing group //
//======================================================//
void SortTriangles(mesh3ds_t* data)
{
   int            result = 0;
   int            next = 0;
   int            nextNext = 0;
   int            i, j;
   triangle_t*    tris;
   triangle_t*    temp;
   long*          oldSmooth = NULL;
   int            groupCount;

   if (data->groupCount == 0)
   {
      data->groups = (group3ds_t*)malloc(sizeof(group3ds_t));
      if (data->groups)
      {
         data->groupCount = 1;
         memset(&data->groups[0], 0, sizeof(group3ds_t));
         data->groups[0].mat = -1;
         data->groups[0].start = 0;
         data->groups[0].size = data->triCount;
         data->groups[0].tris = (long*)malloc(sizeof(long) * data->triCount);

         for (i = 0; i < data->groups[0].size; i++)
         {
            data->groups[0].tris[i] = i;
         }
      }
      if (data->smooth == NULL)
      {
         return;
      }
   }
   
   tris = (triangle_t*)malloc(data->triCount * sizeof(triangle_t));
   temp = (triangle_t*)malloc(data->triCount * sizeof(triangle_t));
   
   for (i = 0; i < data->triCount; i++)
   {
      memcpy(tris[i].verts, data->tris[i], sizeof(long) * 3);
      tris[i].mat = -1;
      tris[i].index = i;

      if (data->smooth)
      {
         tris[i].smooth = data->smooth[i];
      }
      else
      {
         tris[i].smooth = 0;
      }
   }

   for (i = 0; i < data->groupCount; i++)
   {
      for (j = 0; j < data->groups[i].size; j++)
      {
         tris[data->groups[i].tris[j]].mat = data->groups[i].mat;
      }
   }

   //====================================//
   // now sort the triangles by material //
   //====================================//
   qsort(tris, data->triCount, sizeof(triangle_t), (int (__cdecl *)(const void *,const void *))SortByMaterial);
   /*i = 0;
   while (i < data->triCount)
   {
      result = 0;
      next = i + 1;
      while ((next < data->triCount) && (result == 0))
      {
         result = SortByMaterial(&tris[i], &tris[next]);
         next++;
      }
      if (next == data->triCount)
      {
         break;
      }
      next--;

      if (result > 0)
      {
         result = 0;
         nextNext = next + 1;
         while ((nextNext < data->triCount) && (result == 0))
         {
            result = SortByMaterial(&tris[next], &tris[nextNext]);
            nextNext++;
         }
         if (result != 0)
         {
            nextNext--;
         }

         //==========================================//
         // copy the less than portion into a buffer //
         //==========================================//
         memcpy(temp, &tris[next], (nextNext - next) * sizeof(triangle_t));

         //=====================================//
         // move the greater than portion ahead //
         //=====================================//
         memmove(&tris[(nextNext - next) + i], &tris[i], (next - i) * sizeof(triangle_t));
         
         //====================================================//
         // copy the less than portion back in from the buffer //
         //====================================================//
         memcpy(&tris[i], temp, (nextNext - next) * sizeof(triangle_t));

         //===================================//
         // start at the begining of the list //
         //===================================//
         i = 0;
      }
      else if (result < 0)
      {
         i = next;
      }
      else
      {
         break;
      }
   }*/

   groupCount = 1;
   data->groups[0].mat = tris[0].mat;
   data->groups[0].start = 0;
   data->groups[0].size = 0;
   for (i = 0; i < data->triCount; i++)
   {
      memcpy(data->tris[i], tris[i].verts, sizeof(long) * 3);
      if (data->smooth)
      {
         data->smooth[i] = tris[i].smooth;
      }

      if (data->groups[groupCount - 1].mat != tris[i].mat)
      {
         data->groups[groupCount].mat = tris[i].mat;
         data->groups[groupCount].start = i;
         data->groups[groupCount].size = 0;
         groupCount++;
      }

      data->groups[groupCount-1].size++;
   }

   if (tris)
   {
      free(tris);
   }
   if (temp)
   {
      free(temp);
   }
}

long Read3dsFile(char* pFilename, data3ds_t* output)
{
   int         i;
   chunk3ds_t  chunk;
	
   memset(output, 0, sizeof(data3ds_t));

	g_pFile = fopen(pFilename, "rb");
	
	if (g_pFile)
	{
      fseek(g_pFile, 0, SEEK_SET);
      fread(&chunk, sizeof(chunk3ds_t), 1, g_pFile);
      if (chunk.id == 0x4d4d)
      {
         ReadChunkArray(chunk.length - 6, (cb)ReadMain3ds, output);
      }
      fclose(g_pFile);
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

void Free3dsData(data3ds_t* data)
{
   int i, j;

   if (data->meshes)
   {
      for (i = 0; i < data->meshCount; i++)
      {
         FREE(data->meshes[i].verts);
         FREE(data->meshes[i].tris);
         FREE(data->meshes[i].norms);
         FREE(data->meshes[i].tangentSpace);
         FREE(data->meshes[i].texCoords);
         FREE(data->meshes[i].smooth);

         if (data->meshes[i].groups)
         {
            for (j = 0; j < data->meshes[i].groupCount; j++)
            {
               FREE(data->meshes[i].groups[j].tris);
            }
            FREE(data->meshes[i].groups);
         }
      }
      free(data->meshes);
   }

   FREE(data->materials);

   memset(data, 0, sizeof(data3ds_t));
}

void ReadString(char* string)
{
   int i = 0;
   do
   {
      fread(&string[i], sizeof(char), 1, g_pFile);
   } while (string[i++] != '\0');
}

void ReadChunkArray(long length, void (*callback)(chunk3ds_t*, void*), void* data)
{
   chunk3ds_t	chunk;
   long        start; 
   long        pos = 0;

   do
	{
      //======================================//
      // store the position of the this chunk //
      //======================================//
      start = ftell(g_pFile);

      //=======================//
      // read in the sub chunk //
      //=======================//
      fread(&chunk, sizeof(chunk3ds_t), 1, g_pFile);

      //===========================//
      // callback with subchunk id //
      //===========================//
      callback(&chunk, data);

      //=====================================//
      // set the position the next sub chunk //
      //=====================================//
      fseek(g_pFile, start + chunk.length, SEEK_SET);

      //========================================//
      // account for this chunk in the position //
      //========================================//
      pos += chunk.length;
	} while (pos < length);
}

void InspectChunkArray(long length, void (*callback)(chunk3ds_t*, void*), void* data)
{
   chunk3ds_t	chunk;
   long        start; 
   long        chunkStart;
   long        pos = 0;

   //==================================//
   // record the position of the chunk //
   //==================================//
   chunkStart = ftell(g_pFile);

   do
	{
      //==========================================//
      // store the position of the this sub chunk //
      //==========================================//
      start = ftell(g_pFile);

      //=======================//
      // read in the sub chunk //
      //=======================//
      fread(&chunk, sizeof(chunk3ds_t), 1, g_pFile);
   
      //===========================//
      // callback with subchunk id //
      //===========================//
      callback(&chunk, data);

      //=====================================//
      // set the position the next sub chunk //
      //=====================================//
      fseek(g_pFile, start + chunk.length, SEEK_SET);

      //========================================//
      // account for this chunk in the position //
      //========================================//
      pos += chunk.length;
	} while (pos < length);

   //=============================================//
   // set the position to the start of this chunk //
   //=============================================//
   fseek(g_pFile, chunkStart, SEEK_SET);
}


void ReadMain3ds(chunk3ds_t* chunk, data3ds_t* output)
{
   int i, j, k;
	
   switch (chunk->id)
	{
	case 0x3d3d: // Edit3ds
		InspectChunkArray(chunk->length - 6, (cb)InspectEdit3ds, output);
		
      if (output->meshCount)
      {
         output->meshes = (mesh3ds_t*)malloc(sizeof(mesh3ds_t) * output->meshCount);
         memset(output->meshes, 0, sizeof(mesh3ds_t) * output->meshCount);
      }

      if (output->materialCount)
      {
         output->materials = (material3ds_t*)malloc(sizeof(material3ds_t) * output->materialCount);
         memset(output->materials, 0, sizeof(material3ds_t) * output->materialCount);
      }

      output->meshCount = 0;
      output->materialCount = 0;

      ReadChunkArray(chunk->length - 6, (cb)ReadEdit3ds, output);
      
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
      break;
	default:
		break;
	}
}

void InspectEdit3ds(chunk3ds_t* chunk, data3ds_t* output)
{
   switch (chunk->id)
   {
   case 0xAFFF: // EDIT_MATERIAL
      output->materialCount++;
	   break;
   case 0x4000: // EDIT_OBJECT
      ReadString(g_szString);
      InspectChunkArray(chunk->length - 6 + strlen(g_szString), (cb)InspectEditObject, output);
	   break;
   };
}

void InspectEditObject(chunk3ds_t* chunk, data3ds_t* output)
{
   switch (chunk->id)
   {
   case 0x4100: // EDIT_OBJECT
      output->meshCount++;
      break;
   };
}

void ReadEdit3ds(chunk3ds_t* chunk, data3ds_t* output)
{
	switch (chunk->id)
	{
	case 0x4000: // EDIT_OBJECT
      ReadString(g_szString);
      ReadChunkArray(chunk->length - 6 + strlen(g_szString), (cb)ReadEditObject, output);
		break;
   case 0xAFFF: // EDIT_MATERIAL
		ReadChunkArray(chunk->length - 6, (cb)ReadMaterial, &output->materials[output->materialCount++]);
      break;
	default:
		break;
	};
}

void ReadEditObject(chunk3ds_t* chunk, data3ds_t* output)
{
	switch (chunk->id)
	{
	case 0x4100: // TRIANGLE_OBJECT
      strcpy(output->meshes[output->meshCount].name, g_szString);
      ReadChunkArray(chunk->length - 6, (cb)ReadTriangleObject, &output->meshes[output->meshCount++]);
		break;
	};
}

void ReadTriangleObject(chunk3ds_t* chunk, mesh3ds_t* output)
{
	switch (chunk->id)
	{
	case 0x4110: // VERTEX_LIST
		ReadVertexList(output);
		break;
	case 0x4120: // FACE_LIST
		ReadFaceList(output, chunk->length - 6);
		break;
	case 0x4140: // MAPPING_COORDS
		ReadTextureCoordinates(output);
		break;
   case 0x4160: // LOCAL_AXIS
      ReadLocalAxis(output);
      break;
	case 0x4170: // TEXTURE_INFO
		//ReadTextureInfo((char*)(pData + lPos + 6), output);
		break;
	default:
		break;
	};
}

void ReadVertexList(mesh3ds_t* output)
{
   unsigned short	shNumCoords;
   fread(&shNumCoords, sizeof(unsigned short), 1, g_pFile);

	output->vertCount = shNumCoords;
	output->verts = (float(*)[3])malloc(sizeof(float) * 3 * output->vertCount);

   fread(output->verts, sizeof(float), 3 * output->vertCount, g_pFile);
}

void ReadFaceList(mesh3ds_t* output, long length)
{
   long pos = 6;
   int               i;
	unsigned short		shNumFaces;

   fread(&shNumFaces, sizeof(unsigned short), 1, g_pFile);
   pos += sizeof(unsigned short);

	output->triCount = shNumFaces;
	output->tris = (long(*)[3])malloc(sizeof(long) * 3 * output->triCount);

   unsigned short verts[4];

	for (i = 0; i < shNumFaces; i++)
	{
      fread(verts, sizeof(short), 4, g_pFile);
		for (int j = 0; j < 3; j++)
      {
         output->tris[i][j] = (long)verts[j];
	   }
   }

   pos += sizeof(unsigned short) * 4 * shNumFaces;

   if (pos < length)
   {
      InspectChunkArray(pos - 6, (cb)InspectFaceSubs, output);
   
      if (output->groupCount)
      {
         output->groups = (group3ds_t*)malloc(sizeof(group3ds_t) * output->groupCount);
         memset(output->groups, 0, sizeof(group3ds_t) * output->groupCount);
         if (output->groups)
         {
            output->groupCount = 0;
            ReadChunkArray(pos - 6, (cb)ReadFaceSubs, output);
         }
      }
   }
}

void InspectFaceSubs(chunk3ds_t* chunk, mesh3ds_t* output)
{
	switch (chunk->id)
	{
	case 0x4130: // MATERIAL_GROUP
      output->groupCount++;
		break;
	};
}

void ReadFaceSubs(chunk3ds_t* chunk, mesh3ds_t* output)
{
   switch (chunk->id)
   {
   case 0x4130: // MATERIAL_GROUP
      ReadMaterialGroup(&output->groups[output->groupCount++]);
      break;
   case 0x4150: // SMOOTH_GROUP
      output->smooth = (long*)malloc(sizeof(long) * output->triCount);
      if (output->smooth)
      {
         fread(output->smooth, sizeof(long), output->triCount, g_pFile);
      }
      break;
   };
}

void ReadTextureCoordinates(mesh3ds_t* output)
{
   unsigned short	shNumCoords;

	fread(&shNumCoords, sizeof(unsigned short), 1, g_pFile);
   output->texCoordCount = shNumCoords;
   
   output->texCoords = (float(*)[2])malloc(sizeof(float) * 2 * output->texCoordCount);
	fread(output->texCoords, sizeof(float), 2 * output->texCoordCount, g_pFile);
}

void ReadLocalAxis(mesh3ds_t* output)
{
   fread(output->axis, sizeof(float), 9, g_pFile);
   fread(output->position, sizeof(float), 3, g_pFile);
}

void ReadMaterialGroup(group3ds_t* group)
{
   unsigned short	numFaces;
   unsigned short face;
	
   ReadString(group->name);

   fread(&numFaces, sizeof(unsigned short), 1, g_pFile);

   group->tris = (long*)malloc(numFaces * sizeof(long));
	
	if (group->tris)
	{
		memset(group->tris, 0, numFaces * sizeof(long));

		group->size = numFaces;
		group->mat = 0;

      for (int i = 0; i < numFaces; i++)
		{
         fread(&face, sizeof(unsigned short), 1, g_pFile);
			group->tris[i] = face;
		}
	}
}

void ReadMaterial(chunk3ds_t* chunk, material3ds_t* material)
{
   switch (chunk->id)
   {
	   case 0xa000: // MAT_NAME
         ReadString(material->name);
		   break;
	   case 0xa010: // MAT_AMBIENT
		   ReadChunkArray(chunk->length - 6, (cb)ReadColorChunk, material->ambient);
		   material->ambient[3] = 1.0f;
		   break;
	   case 0xa020: // MAT_DIFFUSE
		   ReadChunkArray(chunk->length - 6, (cb)ReadColorChunk, material->diffuse);
		   material->diffuse[3] = 1.0f;
		   break;
	   case 0xa030: // MAT_SPECUAR
		   ReadChunkArray(chunk->length - 6, (cb)ReadColorChunk, material->specular);
		   material->specular[3] = 1.0f;
		   break;
	   case 0xa040: // MAT_SHININESS
		   ReadChunkArray(chunk->length - 6, (cb)ReadPercentageChunk, &material->shininess);
		   material->shininess *= 140.0f;
         break;
	   case 0xa041: // MAT_SHIN2PCT
		   break;
	   case 0xa042: // MAT_SHIN3PCT
		   break;
	   case 0xa050: // MAT_TRANSPANENCY
		   break;
	   case 0xa080: // MAT_EMISSION
		   ReadChunkArray(chunk->length - 6, (cb)ReadColorChunk, material->emission);
		   material->emission[3] = 1.0f;
		   break;
	   case 0xa200: // MAT_TEXMAP
		   ReadChunkArray(chunk->length - 6, (cb)ReadTexture, material);
		   break;
	   default:
		   break;
   };
}

void ReadTexture(chunk3ds_t* chunk, material3ds_t* material)
{
   switch (chunk->id)
   {
	   case 0xa300: // MAT_MAP_NAME
         ReadString(material->texture);
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
   };
}

void ReadColorChunk(chunk3ds_t* chunk, float* fColor)
{
   unsigned char chRGB[3];
	switch (chunk->id)
	{
	case 0x0010: // COLOR_F
		fread(fColor, sizeof(float), 3, g_pFile);
		break;
	case 0x0011: // COLOR_24
		fread(chRGB, sizeof(char), 3, g_pFile);
		
		fColor[0] = float(chRGB[0]) / 256.0f;
		fColor[1] = float(chRGB[1]) / 256.0f;
		fColor[2] = float(chRGB[2]) / 256.0f;
		break;
	};
}

void ReadPercentageChunk(chunk3ds_t* chunk, float* value)
{
   short shPercent;

   switch (chunk->id)
	{
	case 0x0030: // INT_PERCENTAGE (short)
		fread(&shPercent, sizeof(short), 1, g_pFile);
		*value = (float)shPercent;
		break;
	case 0x0031: // FLOAT_PERCENTAGE
		fread(value, sizeof(float), 1, g_pFile);
		break;
	default:
		break;
	};

	*value /= 100.0f;
}

