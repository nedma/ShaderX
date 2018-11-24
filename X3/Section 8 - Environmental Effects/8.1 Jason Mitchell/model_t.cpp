//==================================//
// mesh.cpp                         //
//                                  //
// Drew Card                        //
// ATI Research                     //
// Software Engineer                //
// 3D Application Research Group    //
// dcard@ati.com                    //
//==================================//

//==========//
// includes //
//==========//
#include "model_t.h"
#include <math.h>
#include <stdio.h>
#include <index_t.h>

//================//
// local typedefs //
//================//
typedef struct fatVertex_s
{
   float       v[3];
   float       n[3];
   float       tc[6][3];
   tanSpace_t  tanSpace;
} fatVertex_t;

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

#define FREE(p)                  if (p) { free(p); p = NULL; }
#define PI                       3.14159265358979f

//=================//
// local variables //
//=================//
static float   g_defaultAmb[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
static float   g_defaultDiff[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
static float   g_defaultSpec[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
static float   g_defaultEmis[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
static float   g_defaultShine = 90.0f;

//=============================================================================
// vtCompareInt
//=============================================================================
inline int CompareInt(int* a, int* b)
{
   if (*a > *b)
   {
      return 1;
   }
   else if (*a < *b)
   {
      return -1;
   }

   return 0;
}

//=============================================================================
// CompareVertex_t
//=============================================================================
inline int CompareVertex_t(vertex_t* v1, vertex_t* v2)
{
   static int    i, j;

   for (i = 0; i < 3; i++)
   {
      if (v1->v[i] > v2->v[i])
      {
         return 1;
      }
      else if (v1->v[i] < v2->v[i])
      {
         return -1;
      }
   }

   for (i = 0; i < 3; i++)
   {
      if (v1->n[i] > v2->n[i])
      {
         return 1;
      }
      else if (v1->n[i] < v2->n[i])
      {
         return -1;
      }
   }

   for (i = 0; i < 2; i++)
   {
      if (v1->tc[i] > v2->tc[i])
      {
         return 1;
      }
      else if (v1->tc[i] < v2->tc[i])
      {
         return -1;
      }
   }

   return 0;
}

//=============================================================================
// CompareFatVertex_t
//=============================================================================
inline int CompareFatVertex_t(fatVertex_t* v1, fatVertex_t* v2)
{
   static int    i, j;

   for (i = 0; i < 3; i++)
   {
      if (v1->v[i] > v2->v[i])
      {
         return 1;
      }
      else if (v1->v[i] < v2->v[i])
      {
         return -1;
      }
   }

   for (i = 0; i < 3; i++)
   {
      if (v1->n[i] > v2->n[i])
      {
         return 1;
      }
      else if (v1->n[i] < v2->n[i])
      {
         return -1;
      }
   }

   for (i = 0; i < 6; i++)
   {
      for (j = 0; j < 3; j++)
      {
         if (v1->tc[i][j] > v2->tc[i][j])
         {
            return 1;
         }
         else if (v1->tc[i][j] < v2->tc[i][j])
         {
            return -1;
         }
      }
   }

   for (i = 0; i < 9; i++)
   {
      if (v1->tanSpace.m[i] > v2->tanSpace.m[i])
      {
         return 1;
      }
      if (v1->tanSpace.m[i] < v2->tanSpace.m[i])
      {
         return -1;
      }
   }

   return 0;
}

void BastardizeNormal(float* norm)
{
   norm[0] = ((int)(norm[0] * 100.0f)) / 100.0f;
   norm[1] = ((int)(norm[1] * 100.0f)) / 100.0f;
   norm[2] = ((int)(norm[2] * 100.0f)) / 100.0f;
   VEC_Normalizef(norm);
}

void CompactNormals(mesh_t* mesh, mesh3ds_t* mesh3ds)
{
	int	      i, j, k;
   vertex_t    *triVerts;
   index_t     index;

   triVerts = (vertex_t*)malloc(sizeof(vertex_t) * mesh->numTris * 3);
   if (triVerts == NULL)
   {
      return;
   }
   memset(triVerts, 0, sizeof(vertex_t) * mesh->numTris * 3);		

   
   for (i = 0; i < mesh3ds->triCount; i++)
   {
      for (j = 0; j < 3; j++)
      {
         //=========================//
         // copy in the vertex data //
         //=========================//
         memcpy(&triVerts[i * 3 + j], &mesh->verts[mesh->tris[i][j]], sizeof(vertex_t)); 
         
         BastardizeNormal(mesh3ds->norms[i * 3 + j]);
         memcpy(triVerts[i * 3 + j].n, mesh3ds->norms[i * 3 + j], sizeof(float) * 3); 
      }
   }

   indexArray(&index, (char*)triVerts, sizeof(vertex_t), mesh->numTris * 3, (sortFunc_t)CompareVertex_t);
   int result;
   for (i = 0; i < mesh->numTris; i++)
   {
      for (j = 0; j < 3; j++)
      {
         k = indexFind(&index, &triVerts[i * 3 + j], &result);
         mesh->tris[i][j] = k;
      }
   }

   free(mesh->verts);
   mesh->verts = (vertex_t*)malloc(sizeof(vertex_t) * index.count);
   mesh->numVerts = index.count;
   
   for (i = 0; i < mesh->numVerts; i++)
   {
      memcpy(&mesh->verts[i], &triVerts[index.indices[i]], sizeof(vertex_t)); 
   }

   indexFree(&index);

   if (triVerts)
   {
      free(triVerts);
   }
}

void CalculateFaceNormals(mesh_t* mesh)
{
	int	      i;
	float		   v1[3];
	float		   v2[3];
   
   FREE(mesh->faceNormals);

   //========================================//
   // allocate a buffer for the flat normals //
   //========================================//
   mesh->faceNormals = (float(*)[4])malloc(sizeof(float) * 4 * mesh->numTris);
	if (mesh->faceNormals != NULL)
	{
      memset(mesh->faceNormals, 0, sizeof(float) * 3 * mesh->numTris);

      //===============================================//
		// Got through every triangle to find its normal //
      //===============================================//
		for (i = 0; i < mesh->numTris; i++)
		{
			VEC_Subtract(mesh->verts[mesh->tris[i][1]].v, mesh->verts[mesh->tris[i][0]].v, v1);
			VEC_Subtract(mesh->verts[mesh->tris[i][2]].v, mesh->verts[mesh->tris[i][0]].v, v2);
			VEC_Cross(v1, v2, mesh->faceNormals[i]);
			VEC_Normalizef(mesh->faceNormals[i]);
         mesh->faceNormals[i][3] = mesh->faceNormals[i][0] * mesh->verts[mesh->tris[i][0]].v[0] + 
                                   mesh->faceNormals[i][1] * mesh->verts[mesh->tris[i][0]].v[1] + 
                                   mesh->faceNormals[i][2] * mesh->verts[mesh->tris[i][0]].v[2];
		}
   }
}

void CalculateNormals(mesh_t* mesh)
{
	int	      i, j, k;
   int         normCount;
   long        (*triRefs)[50];
   float       (*faceNorms)[3];
	float		   v1[3];
	float		   v2[3];
   vertex_t    *triVerts;
   index_t     index;
   float       dot;

   //============================================================================//
   // build a table that links each vertex to all triangles that use said vertex // 
   //============================================================================//
   triRefs = (long(*)[50])malloc(sizeof(long) * 50 * mesh->numVerts);
   if (triRefs == NULL)
   {
      return;
   }
   memset(triRefs, 0, sizeof(unsigned long) * 50 * mesh->numVerts);		

   triVerts = (vertex_t*)malloc(sizeof(vertex_t) * mesh->numTris * 3);
   if (triVerts == NULL)
   {
      return;
   }
   memset(triVerts, 0, sizeof(vertex_t) * mesh->numTris * 3);		

   for (i = 0; i < mesh->numTris; i++)
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
   faceNorms = (float(*)[3])malloc(sizeof(float) * 3 * mesh->numTris);
	if ((faceNorms != NULL) && (triRefs != NULL))
	{
      memset(faceNorms, 0, sizeof(float) * 3 * mesh->numTris);

      //===============================================//
		// Got through every triangle to find its normal //
      //===============================================//
		for (i = 0; i < mesh->numTris; i++)
		{
			VEC_Subtract(mesh->verts[mesh->tris[i][1]].v, mesh->verts[mesh->tris[i][0]].v, v1);
			VEC_Subtract(mesh->verts[mesh->tris[i][2]].v, mesh->verts[mesh->tris[i][0]].v, v2);
			VEC_Cross(v1, v2, faceNorms[i]);
			VEC_Normalizef(faceNorms[i]);
		}

		for (i = 0; i < mesh->numTris; i++)
		{
         for (k = 0; k < 3; k++)
         {
            memcpy(&triVerts[i * 3 + k], &mesh->verts[mesh->tris[i][k]], sizeof(vertex_t));
            //memcpy(triVerts[i * 3 + k].n, mesh->faceNorms[i], sizeof(float) * 3);

            normCount = 0;
			   for (j = 1; j <= triRefs[mesh->tris[i][k]][0]; j++)
            {
					dot = VEC_DotProduct(faceNorms[i], faceNorms[triRefs[mesh->tris[i][k]][j]]);
					if (dot >= 0.5f)
					{
						VEC_AddTo(triVerts[i * 3 + k].n, faceNorms[triRefs[mesh->tris[i][k]][j]]);
						normCount++;
					}
			   }

  			   VEC_DivideByScalar(triVerts[i * 3 + k].n, (float)normCount);
			   VEC_Normalizef(triVerts[i * 3 + k].n);

            BastardizeNormal(triVerts[i * 3 + k].n);
         }
		}

      /*
      //===============================================================//
      // for each vertex average all face normals that use this vertex //
      //===============================================================//
      for (i = 0; i < mesh->numVerts; i++)
		{
         for (j = 1; j <= triRefs[i][0]; j++)
         {
			   VEC_AddTo(mesh->verts[i].n, faceNorms[triRefs[i][j] - 1]);
			}
			VEC_DivideByScalar(mesh->verts[i].n, (float)triRefs[i][0]);
			VEC_Normalizef(mesh->verts[i].n);
		}
      */
   }

   indexArray(&index, (char*)triVerts, sizeof(vertex_t), mesh->numTris * 3, (sortFunc_t)CompareVertex_t);
   int result;
   for (i = 0; i < mesh->numTris; i++)
   {
      for (j = 0; j < 3; j++)
      {
         k = indexFind(&index, &triVerts[i * 3 + j], &result);
         mesh->tris[i][j] = k;
      }
   }

   free(mesh->verts);
   mesh->verts = (vertex_t*)malloc(sizeof(vertex_t) * index.count);
   mesh->numVerts = index.count;
   
   for (i = 0; i < mesh->numVerts; i++)
   {
      memcpy(&mesh->verts[i], &triVerts[index.indices[i]], sizeof(vertex_t)); 
   }

   indexFree(&index);

   //==========================================//
   // free up the local buffers that were used //
   //==========================================//
   if (triRefs)
   {
      free(triRefs);
   }

   if (triVerts)
   {
      free(triVerts);
   }
}

void CalculateNormalsBySmoothGroups(mesh_t* mesh, int* smooth)
{
	int	      i, j, k;
   int         normCount;
   long        (*triRefs)[50];
   float       (*faceNorms)[3];
	float		   v1[3];
	float		   v2[3];
   vertex_t    *triVerts;
   index_t     index;

   //============================================================================//
   // build a table that links each vertex to all triangles that use said vertex // 
   //============================================================================//
   triRefs = (long(*)[50])malloc(sizeof(long) * 50 * mesh->numVerts);
   if (triRefs == NULL)
   {
      return;
   }
   memset(triRefs, 0, sizeof(unsigned long) * 50 * mesh->numVerts);		

   triVerts = (vertex_t*)malloc(sizeof(vertex_t) * mesh->numTris * 3);
   if (triVerts == NULL)
   {
      return;
   }
   memset(triVerts, 0, sizeof(vertex_t) * mesh->numTris * 3);		

   for (i = 0; i < mesh->numTris; i++)
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
   faceNorms = (float(*)[3])malloc(sizeof(float) * 3 * mesh->numTris);
	if ((faceNorms != NULL) && (triRefs != NULL))
	{
      memset(faceNorms, 0, sizeof(float) * 3 * mesh->numTris);

      //===============================================//
		// Got through every triangle to find its normal //
      //===============================================//
		for (i = 0; i < mesh->numTris; i++)
		{
			VEC_Subtract(mesh->verts[mesh->tris[i][1]].v, mesh->verts[mesh->tris[i][0]].v, v1);
			VEC_Subtract(mesh->verts[mesh->tris[i][2]].v, mesh->verts[mesh->tris[i][0]].v, v2);
			VEC_Cross(v1, v2, faceNorms[i]);
			VEC_Normalizef(faceNorms[i]);
		}

		for (i = 0; i < mesh->numTris; i++)
		{
         for (k = 0; k < 3; k++)
         {
            memcpy(&triVerts[i * 3 + k], &mesh->verts[mesh->tris[i][k]], sizeof(vertex_t));

            normCount = 0;
			   for (j = 1; j <= triRefs[mesh->tris[i][k]][0]; j++)
            {
               if (smooth[i] == smooth[triRefs[mesh->tris[i][k]][j]])
					{
						VEC_AddTo(triVerts[i * 3 + k].n, faceNorms[triRefs[mesh->tris[i][k]][j]]);
						normCount++;
					}
			   }

  			   VEC_DivideByScalar(triVerts[i * 3 + k].n, (float)normCount);
			   VEC_Normalizef(triVerts[i * 3 + k].n);

            BastardizeNormal(triVerts[i * 3 + k].n);
         }
		}
   }

   indexArray(&index, (char*)triVerts, sizeof(vertex_t), mesh->numTris * 3, (sortFunc_t)CompareVertex_t);
   int result;
   for (i = 0; i < mesh->numTris; i++)
   {
      for (j = 0; j < 3; j++)
      {
         k = indexFind(&index, &triVerts[i * 3 + j], &result);
         mesh->tris[i][j] = k;
      }
   }

   free(mesh->verts);
   mesh->verts = (vertex_t*)malloc(sizeof(vertex_t) * index.count);
   mesh->numVerts = index.count;
   
   for (i = 0; i < mesh->numVerts; i++)
   {
      memcpy(&mesh->verts[i], &triVerts[index.indices[i]], sizeof(vertex_t)); 
   }

   indexFree(&index);

   //==========================================//
   // free up the local buffers that were used //
   //==========================================//
   if (triRefs)
   {
      free(triRefs);
   }

   if (triVerts)
   {
      free(triVerts);
   }
}

void ComputeTangentSpace(float* vert1, float* vert2, float* vert3, 
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

void CalculateTangentSpace(mesh_t* mesh)
{
   int   i, j;
   int   *vertFlags;
   
   if (mesh->tanSpace != NULL)
   {
      free(mesh->tanSpace);
      mesh->tanSpace = NULL;
   }

   //========================//
   // allocate tangent space //
   //========================//
   mesh->tanSpace = (tanSpace_t*)malloc(sizeof(tanSpace_t) * mesh->numVerts);
   if (mesh->tanSpace == NULL)
   {
      return;
   }
   memset(mesh->tanSpace, 0, sizeof(tanSpace_t) * mesh->numVerts);		

   //============================//
   // allocate some vertex flags //
   //============================//
   vertFlags = (int*)malloc(sizeof(int) * mesh->numVerts);
   memset(vertFlags, 0, sizeof(int) * mesh->numVerts);		

   for (i = 0; i < mesh->numTris; i++)
   {
      for (j = 0; j < 3; j++)
      {
         if (vertFlags[mesh->tris[i][j]] == 0)
         {
            ComputeTangentSpace(
               mesh->verts[mesh->tris[i][0]].v, mesh->verts[mesh->tris[i][1]].v, mesh->verts[mesh->tris[i][2]].v,
               mesh->verts[mesh->tris[i][0]].tc, mesh->verts[mesh->tris[i][1]].tc, mesh->verts[mesh->tris[i][2]].tc,
               mesh->verts[mesh->tris[i][j]].n, mesh->tanSpace[mesh->tris[i][j]].m);
            vertFlags[mesh->tris[i][j]] = 1;
         }
      }
   }

   free(vertFlags);
}

///*
int Load3ds(char* filename, model_t* model)
{
   int            i, j, k, l, m;
   data3ds_t      data;
   group3ds_t     *group3ds;
   mesh3ds_t      *mesh3ds;
   mesh_t         *mesh;
   int            result;
   unsigned long  (*rawTris)[3];
   index_t        vertIndex;
   char           path[32];
   char           *slash;
   unsigned long  pathLength = 0;
   fatVertex_t*   fatVerts = NULL;

   //==================//
   // read in 3ds file //
   //==================//
   Read3dsFileFast(filename, &data);


   strcpy(path, filename);

   //===============================================//
   // get pointer to the last slash in the filename //
   //===============================================//
   slash = strrchr(path, '\\');
   if (slash != NULL)
   {
      slash[1] = '\0';
   }
   else
   {
      slash = strrchr(path, '/');
      if (slash != NULL)
      {
         slash[1] = '\0';
      }
      else
      {
         path[0] = '\0';
      }
   }

   pathLength = strlen(path);

   if (model)
   {
      memset(model, 0, sizeof(model_t)); 

      strcpy(model->path, path);
      model->magic = MODEL_MAGIC;
      for (i = 0; i < data.meshCount; i++)
      {
         model->numMeshes += data.meshes[i].groupCount;
      }
      model->meshes = (mesh_t**)malloc(sizeof(mesh_t*) * model->numMeshes);
      model->numMaterials = data.materialCount;

      if (model->numMaterials != 0)
      {
         model->materials = (material_t*)malloc(sizeof(material_t) * model->numMaterials);
         if ((model->materials == NULL) || (model->meshes == NULL))
         {
            FreeModel(model);
            Free3dsData(&data);
            return MODEL_MEMORY_ERROR;
         }
         memset(model->materials, 0, sizeof(material_t) * model->numMaterials);
      }

      for (i = 0; i < data.materialCount; i++)
      {
         memcpy(model->materials[i].ambient, data.materials[i].ambient, sizeof(float) * 17);
         strcpy(model->materials[i].texName, data.materials[i].texture);
      }

      model->numMeshes = 0;
      for (i = 0; i < data.meshCount; i++)
      {
         mesh3ds = &data.meshes[i];

         for (j = 0; j < mesh3ds->groupCount; j++)
         {
            group3ds = &mesh3ds->groups[j];
            
            //==================================================//
            // set the rawTris array to the start of this group //
            //==================================================//
            rawTris = (unsigned long(*)[3])&mesh3ds->tris[group3ds->start];

            //===================//
            // allocate the mesh //
            //===================//
            model->meshes[model->numMeshes] = (mesh_t*)malloc(sizeof(mesh_t));
            if (model->meshes[model->numMeshes])
            {
               mesh = model->meshes[model->numMeshes];

               memset(model->meshes[model->numMeshes], 0, sizeof(mesh_t));
               strcpy(model->meshes[model->numMeshes]->name, mesh3ds->name);
               model->meshes[model->numMeshes]->magic = MESH_MAGIC;
               model->meshes[model->numMeshes]->numVerts = group3ds->size * 3;
               model->meshes[model->numMeshes]->numTris = group3ds->size;
               if ((group3ds->mat >= 0) && (group3ds->mat < model->numMaterials))
               {
                  model->meshes[model->numMeshes]->material = &model->materials[group3ds->mat];
               }
               else
               {
                  model->meshes[model->numMeshes]->material = NULL;
               }

               fatVerts = (fatVertex_t*)malloc(sizeof(fatVertex_t) * group3ds->size * 3);
               if (fatVerts == NULL)
               {
                  return MODEL_MEMORY_ERROR;
               }
               memset(fatVerts, 0, sizeof(fatVertex_t) * group3ds->size * 3);

               //=============================//
               // now copy in the vertex data //
               //=============================//
               for (k = 0; k < group3ds->size; k++)
               {
                  for (l = 0; l < 3; l++)
                  {
                     if (mesh3ds->verts)
                     {
                        memcpy(fatVerts[k * 3 + l].v, mesh3ds->verts[rawTris[k][l]], sizeof(float) * 3);
                     }
                     if (mesh3ds->norms)
                     {
                        memcpy(fatVerts[k * 3 + l].n, mesh3ds->norms[(group3ds->start + k) * 3 + l], sizeof(float) * 3);
                        BastardizeNormal(fatVerts[k * 3 + l].n);
                     }
                     if (mesh3ds->texCoords)
                     {
                        memcpy(fatVerts[k * 3 + l].tc[0], mesh3ds->texCoords[rawTris[k][l]], sizeof(float) * 2);
                     }
                     if (mesh3ds->tangentSpace)
                     {
                        memcpy(fatVerts[k * 3 + l].tanSpace.m, mesh3ds->tangentSpace[(group3ds->start + k) * 3 + l], sizeof(float) * 9);
                        BastardizeNormal(fatVerts[k * 3 + l].tanSpace.tan);
                        BastardizeNormal(fatVerts[k * 3 + l].tanSpace.binormal);
                        BastardizeNormal(fatVerts[k * 3 + l].tanSpace.normal);
                     }
                  }
               }
               
               //=======================================//
               // index the vertex data we just created //
               //=======================================//
               indexArray(&vertIndex, (char*)fatVerts, sizeof(fatVertex_t), group3ds->size * 3, (sortFunc_t)CompareFatVertex_t);
               
               //==========================================================//
               // set the vertex count to the number of verts in the index //
               //==========================================================//
               mesh->numVerts = vertIndex.count;

               //===================//
               // allocate the mesh //
               //===================//
               AllocateMesh(mesh);

               m = 0;
               for (k = 0; k < mesh->numTris; k++)
               {
                  for (l = 0; l < 3; l++)
                  {
                     mesh->tris[k][l] = m++;
                  }
               }

               if (vertIndex.count > 0)
               {
                  //========================================================//
                  // copy from the original array into the new vertex array //
                  //========================================================//
                  for (k = 0; k < vertIndex.count; k++)
                  {
                     memcpy(mesh->verts[k].v, fatVerts[vertIndex.indices[k]].v, sizeof(float) * 3);
                     memcpy(mesh->verts[k].n, fatVerts[vertIndex.indices[k]].n, sizeof(float) * 3);
                     memcpy(mesh->verts[k].tc, fatVerts[vertIndex.indices[k]].tc[0], sizeof(float) * 2);
                     memcpy(mesh->tanSpace[k].m, fatVerts[vertIndex.indices[k]].tanSpace.m, sizeof(float) * 9);
                  }

                  //==================================================================================//
                  // use the vertex index to find which vertex each triangle is supposed to reference //
                  //==================================================================================//
                  for (k = 0; k < mesh->numTris; k++)
                  {
                     for (l = 0; l < 3; l++)
                     {
                        mesh->tris[k][l] = indexFind(&vertIndex, &fatVerts[mesh->tris[k][l]], &result);
                     }
                  }

                  FREE(fatVerts);
                  indexFree(&vertIndex);
               }

               CalculateFaceNormals(model->meshes[model->numMeshes]);

               model->numMeshes++;
            }
            else
            {
               FreeModel(model);
               Free3dsData(&data);
               return MODEL_MEMORY_ERROR;
            }
         }
      }

      model->numTris = 0;
      model->numVerts = 0;
      for (i = 0; i < model->numMeshes; i++)
      {
         model->numTris += model->meshes[i]->numTris;
         model->numVerts += model->meshes[i]->numVerts;
      }
   }
   else
   {
      Free3dsData(&data);
      return MODEL_ERROR;
   }


   /*
   printf("tris\n");
   for (i = 0; i < model->meshes[0]->numTris; i++)
   {
      printf("(%.2f %.2f %.2f) (%.2f %.2f %.2f) (%.2f %.2f %.2f)\n", 
         model->meshes[0]->verts[model->meshes[0]->tris[i][0]].v[0], model->meshes[0]->verts[model->meshes[0]->tris[i][0]].v[1], model->meshes[0]->verts[model->meshes[0]->tris[i][0]].v[2],
         model->meshes[0]->verts[model->meshes[0]->tris[i][1]].v[0], model->meshes[0]->verts[model->meshes[0]->tris[i][1]].v[1], model->meshes[0]->verts[model->meshes[0]->tris[i][1]].v[2],
         model->meshes[0]->verts[model->meshes[0]->tris[i][2]].v[0], model->meshes[0]->verts[model->meshes[0]->tris[i][2]].v[1], model->meshes[0]->verts[model->meshes[0]->tris[i][2]].v[2]);
      printf("(%.2f %.2f %.2f) (%.2f %.2f %.2f) (%.2f %.2f %.2f)\n\n", 
         data.meshes[0].verts[data.meshes[0].tris[i][0]][0], data.meshes[0].verts[data.meshes[0].tris[i][0]][1], data.meshes[0].verts[data.meshes[0].tris[i][0]][2],
         data.meshes[0].verts[data.meshes[0].tris[i][1]][0], data.meshes[0].verts[data.meshes[0].tris[i][1]][1], data.meshes[0].verts[data.meshes[0].tris[i][1]][2],
         data.meshes[0].verts[data.meshes[0].tris[i][2]][0], data.meshes[0].verts[data.meshes[0].tris[i][2]][1], data.meshes[0].verts[data.meshes[0].tris[i][2]][2]);
   }
   */


   Free3dsData(&data);
   return MODEL_OK;
}
//*/

/*
int Load3ds(char* filename, model_t* model)
{
   int            i, j, k, l, m;
   data3ds_t      data;
   group3ds_t     *group3ds;
   mesh3ds_t      *mesh3ds;
   mesh_t         *mesh;
   int            result;
   unsigned long  (*rawTris)[3];
   index_t        vertIndex;
   char           path[32];
   char           *slash;
   unsigned long  pathLength = 0;
   vertex_t*      verts;

   //==================//
   // read in 3ds file //
   //==================//
   Read3dsFileFast(filename, &data);
   
   strcpy(path, filename);

   //===============================================//
   // get pointer to the last slash in the filename //
   //===============================================//
   slash = strrchr(path, '\\');
   if (slash != NULL)
   {
      slash[1] = '\0';
   }
   else
   {
      slash = strrchr(path, '/');
      if (slash != NULL)
      {
         slash[1] = '\0';
      }
      else
      {
         path[0] = '\0';
      }
   }

   pathLength = strlen(path);

   if (model)
   {
      memset(model, 0, sizeof(model_t)); 

      strcpy(model->path, path);
      model->magic = MODEL_MAGIC;
      for (i = 0; i < data.meshCount; i++)
      {
         model->numMeshes += data.meshes[i].groupCount;
      }
      model->meshes = (mesh_t**)malloc(sizeof(mesh_t*) * model->numMeshes);
      model->numMaterials = data.materialCount;

      if (model->numMaterials != 0)
      {
         model->materials = (material_t*)malloc(sizeof(material_t) * model->numMaterials);
         if ((model->materials == NULL) || (model->meshes == NULL))
         {
            FreeModel(model);
            Free3dsData(&data);
            return MODEL_MEMORY_ERROR;
         }
         memset(model->materials, 0, sizeof(material_t) * model->numMaterials);
      }

      for (i = 0; i < data.materialCount; i++)
      {
         memcpy(model->materials[i].ambient, data.materials[i].ambient, sizeof(float) * 17);
         strcpy(model->materials[i].texName, data.materials[i].texture);
      }

      model->numMeshes = 0;
      for (i = 0; i < data.meshCount; i++)
      {
         mesh3ds = &data.meshes[i];

         for (j = 0; j < mesh3ds->groupCount; j++)
         {
            group3ds = &mesh3ds->groups[j];
            
            //==================================================//
            // set the rawTris array to the start of this group //
            //==================================================//
            rawTris = (unsigned long(*)[3])&mesh3ds->tris[group3ds->start];

            //===================//
            // allocate the mesh //
            //===================//
            model->meshes[model->numMeshes] = (mesh_t*)malloc(sizeof(mesh_t));
            if (model->meshes[model->numMeshes])
            {
               mesh = model->meshes[model->numMeshes];

               memset(model->meshes[model->numMeshes], 0, sizeof(mesh_t));
               strcpy(model->meshes[model->numMeshes]->name, mesh3ds->name);
               model->meshes[model->numMeshes]->magic = MESH_MAGIC;
               model->meshes[model->numMeshes]->numVerts = group3ds->size * 3;
               model->meshes[model->numMeshes]->numTris = group3ds->size;
               if ((group3ds->mat >= 0) && (group3ds->mat < model->numMaterials))
               {
                  model->meshes[model->numMeshes]->material = &model->materials[group3ds->mat];
               }
               else
               {
                  model->meshes[model->numMeshes]->material = NULL;
               }


               AllocateMesh(model->meshes[model->numMeshes]);

               //=============================//
               // now copy in the vertex data //
               //=============================//
               for (k = 0; k < model->meshes[model->numMeshes]->numTris; k++)
               {
                  for (l = 0; l < 3; l++)
                  {
                      if (mesh3ds->verts)
                     {
                        memcpy(model->meshes[model->numMeshes]->verts[k * 3 + l].v, mesh3ds->verts[rawTris[k][l]], sizeof(float) * 3);
                     }
                     if (mesh3ds->norms)
                     {
                        memcpy(model->meshes[model->numMeshes]->verts[k * 3 + l].n, mesh3ds->norms[(group3ds->start + k) * 3 + l], sizeof(float) * 3);
                        BastardizeNormal(model->meshes[model->numMeshes]->verts[k * 3 + l].n);
                     }
                     if (mesh3ds->texCoords)
                     {
                        memcpy(model->meshes[model->numMeshes]->verts[k * 3 + l].tc, mesh3ds->texCoords[rawTris[k][l]], sizeof(float) * 2);
                     }
                  }
               }

               m = 0;
               for (k = 0; k < model->meshes[model->numMeshes]->numTris; k++)
               {
                  for (l = 0; l < 3; l++)
                  {
                     model->meshes[model->numMeshes]->tris[k][l] = m++;
                  }
               }

               //=======================================//
               // index the vertex data we just created //
               //=======================================//
               indexArray(&vertIndex, (char*)mesh->verts, sizeof(vertex_t), mesh->numVerts, (sortFunc_t)CompareVertex_t);
                           
               if ((vertIndex.count != mesh->numVerts) && (vertIndex.count > 0))
               {
                  verts = (vertex_t*)malloc(sizeof(vertex_t) * vertIndex.count);
                  if (verts != NULL)
                  {
                     //========================================================//
                     // copy from the original array into the new vertex array //
                     //========================================================//
                     for (k = 0; k < vertIndex.count; k++)
                     {
                        memcpy(&verts[k], &mesh->verts[vertIndex.indices[k]], sizeof(vertex_t));
                     }

                     //==================================================================================//
                     // use the vertex index to find which vertex each triangle is supposed to reference //
                     //==================================================================================//
                     for (k = 0; k < mesh->numTris; k++)
                     {
                        for (l = 0; l < 3; l++)
                        {
                           mesh->tris[k][l] = indexFind(&vertIndex, &mesh->verts[mesh->tris[k][l]], &result);
                        }
                     }

                     FREE(mesh->verts);
                     mesh->numVerts = vertIndex.count;
                     mesh->verts = verts;
                     indexFree(&vertIndex);
                  }
               }

               model->numMeshes++;
            }
            else
            {
               FreeModel(model);
               Free3dsData(&data);
               return MODEL_MEMORY_ERROR;
            }
      
         }
      }

      model->numTris = 0;
      model->numVerts = 0;
      for (i = 0; i < model->numMeshes; i++)
      {
         model->numTris += model->meshes[i]->numTris;
         model->numVerts += model->meshes[i]->numVerts;
      }
   }
   else
   {
      Free3dsData(&data);
      return MODEL_ERROR;
   }

   Free3dsData(&data);
   return MODEL_OK;
}
*/

int Load3dsNoTex(char* filename, model_t* model)
{
   int            i, j, k, l, m;
   data3ds_t      data;
   group3ds_t     *group3ds;
   mesh3ds_t      *mesh3ds;
   mesh_t         *mesh;
   int            result;
   unsigned long  (*rawTris)[3];
   index_t        vertIndex;
   char           path[32];
   char           *slash;
   unsigned long  pathLength = 0;
   vertex_t*      verts;

   //==================//
   // read in 3ds file //
   //==================//
   Read3dsFileFast(filename, &data);
   
   strcpy(path, filename);

   //===============================================//
   // get pointer to the last slash in the filename //
   //===============================================//
   slash = strrchr(path, '\\');
   if (slash != NULL)
   {
      slash[1] = '\0';
   }
   else
   {
      slash = strrchr(path, '/');
      if (slash != NULL)
      {
         slash[1] = '\0';
      }
      else
      {
         path[0] = '\0';
      }
   }

   pathLength = strlen(path);

   if (model)
   {
      memset(model, 0, sizeof(model_t)); 

      strcpy(model->path, path);
      model->magic = MODEL_MAGIC;
      for (i = 0; i < data.meshCount; i++)
      {
         model->numMeshes += data.meshes[i].groupCount;
      }
      model->meshes = (mesh_t**)malloc(sizeof(mesh_t*) * model->numMeshes);
      model->numMaterials = data.materialCount;

      if (model->numMaterials != 0)
      {
         model->materials = (material_t*)malloc(sizeof(material_t) * model->numMaterials);
         if ((model->materials == NULL) || (model->meshes == NULL))
         {
            FreeModel(model);
            Free3dsData(&data);
            return MODEL_MEMORY_ERROR;
         }
         memset(model->materials, 0, sizeof(material_t) * model->numMaterials);
      }

      for (i = 0; i < data.materialCount; i++)
      {
         memcpy(model->materials[i].ambient, data.materials[i].ambient, sizeof(float) * 17);
         strcpy(model->materials[i].texName, data.materials[i].texture);
      }

      model->numMeshes = 0;
      for (i = 0; i < data.meshCount; i++)
      {
         mesh3ds = &data.meshes[i];

         for (j = 0; j < mesh3ds->groupCount; j++)
         {
            group3ds = &mesh3ds->groups[j];
            
            //==================================================//
            // set the rawTris array to the start of this group //
            //==================================================//
            rawTris = (unsigned long(*)[3])&mesh3ds->tris[group3ds->start];

            //===================//
            // allocate the mesh //
            //===================//
            model->meshes[model->numMeshes] = (mesh_t*)malloc(sizeof(mesh_t));
            if (model->meshes[model->numMeshes])
            {
               mesh = model->meshes[model->numMeshes];

               memset(model->meshes[model->numMeshes], 0, sizeof(mesh_t));
               strcpy(model->meshes[model->numMeshes]->name, mesh3ds->name);
               model->meshes[model->numMeshes]->magic = MESH_MAGIC;
               model->meshes[model->numMeshes]->numVerts = group3ds->size * 3;
               model->meshes[model->numMeshes]->numTris = group3ds->size;
               if ((group3ds->mat >= 0) && (group3ds->mat < model->numMaterials))
               {
                  model->meshes[model->numMeshes]->material = &model->materials[group3ds->mat];
               }
               else
               {
                  model->meshes[model->numMeshes]->material = NULL;
               }


               AllocateMesh(model->meshes[model->numMeshes]);

               //=============================//
               // now copy in the vertex data //
               //=============================//
               for (k = 0; k < model->meshes[model->numMeshes]->numTris; k++)
               {
                  for (l = 0; l < 3; l++)
                  {
                      if (mesh3ds->verts)
                     {
                        memcpy(model->meshes[model->numMeshes]->verts[k * 3 + l].v, mesh3ds->verts[rawTris[k][l]], sizeof(float) * 3);
                     }
                     if (mesh3ds->norms)
                     {
                        memcpy(model->meshes[model->numMeshes]->verts[k * 3 + l].n, mesh3ds->norms[(group3ds->start + k) * 3 + l], sizeof(float) * 3);
                        BastardizeNormal(model->meshes[model->numMeshes]->verts[k * 3 + l].n);
                     }
                     if (mesh3ds->texCoords)
                     {
                        memset(model->meshes[model->numMeshes]->verts[k * 3 + l].tc, 0, sizeof(float) * 2);
                     }
                  }
               }

               m = 0;
               for (k = 0; k < model->meshes[model->numMeshes]->numTris; k++)
               {
                  for (l = 0; l < 3; l++)
                  {
                     model->meshes[model->numMeshes]->tris[k][l] = m++;
                  }
               }

               //=======================================//
               // index the vertex data we just created //
               //=======================================//
               indexArray(&vertIndex, (char*)mesh->verts, sizeof(vertex_t), mesh->numVerts, (sortFunc_t)CompareVertex_t);
                           
               if ((vertIndex.count != mesh->numVerts) && (vertIndex.count > 0))
               {
                  verts = (vertex_t*)malloc(sizeof(vertex_t) * vertIndex.count);
                  if (verts != NULL)
                  {
                     //========================================================//
                     // copy from the original array into the new vertex array //
                     //========================================================//
                     for (k = 0; k < vertIndex.count; k++)
                     {
                        memcpy(&verts[k], &mesh->verts[vertIndex.indices[k]], sizeof(vertex_t));
                     }

                     //==================================================================================//
                     // use the vertex index to find which vertex each triangle is supposed to reference //
                     //==================================================================================//
                     for (k = 0; k < mesh->numTris; k++)
                     {
                        for (l = 0; l < 3; l++)
                        {
                           mesh->tris[k][l] = indexFind(&vertIndex, &mesh->verts[mesh->tris[k][l]], &result);
                        }
                     }

                     FREE(mesh->verts);
                     mesh->numVerts = vertIndex.count;
                     mesh->verts = verts;
                     indexFree(&vertIndex);
                  }
               }

               model->numMeshes++;
            }
            else
            {
               FreeModel(model);
               Free3dsData(&data);
               return MODEL_MEMORY_ERROR;
            }
      
         }
      }

      model->numTris = 0;
      model->numVerts = 0;
      for (i = 0; i < model->numMeshes; i++)
      {
         model->numTris += model->meshes[i]->numTris;
         model->numVerts += model->meshes[i]->numVerts;
      }
   }
   else
   {
      Free3dsData(&data);
      return MODEL_ERROR;
   }

   Free3dsData(&data);
   return MODEL_OK;
}


//====================================================//
// FreeModel                                          //
// frees all resources associated with this model     //
//====================================================//
void FreeModel(model_t* model)
{
   int i;

   if (model->info == MODEL_BIG_CHUNK)
   {
      FREE(model->meshes);
      free(model);
   }
   else
   {
      for (i = 0; i < model->numMeshes; i++)
      {
         FreeMesh(model->meshes[i]);
         FREE(model->meshes[i]);
      }
      FREE(model->meshes);
      FREE(model->materials);
   }
}

//====================================================//
// AllocateMesh                                       //
// allocates all arrays for this mesh this mesh       //
//====================================================//
void AllocateMesh(mesh_t* mesh)
{
   mesh->verts = (vertex_t*)malloc(sizeof(vertex_t) * mesh->numVerts);
   if (mesh->verts)
   {
      memset(mesh->verts, 0, sizeof(vertex_t) * mesh->numVerts);
   }

   mesh->tanSpace = (tanSpace_t*)malloc(sizeof(tanSpace_t) * mesh->numVerts);
   if (mesh->tanSpace)
   {
      memset(mesh->tanSpace, 0, sizeof(tanSpace_t) * mesh->numVerts);
   }

   mesh->tris = (unsigned long(*)[3])malloc(sizeof(unsigned long) * 3 * mesh->numTris);
   if (mesh->tris)
   {
      memset(mesh->tris, 0, sizeof(unsigned long) * 3 * mesh->numTris);
   }
}

//====================================================//
// FreeMesh                                           //
// frees all resources associated with this mesh      //
//====================================================//
void FreeMesh(mesh_t* mesh)
{
   FREE(mesh->verts);
   FREE(mesh->tris);
   FREE(mesh->tanSpace);
   FREE(mesh->faceNormals);
}

//===================================================================//
// SaveModel                                                         //
// saves the model                                                   //
//===================================================================//
int SaveModel(char* filename, model_t* model, unsigned int flags = 0)
{
   int            i;
   int            pos;
   FILE           *file;
   model_t        header;
   material_t     material;
   mesh_t         mesh;
   
   if (model)
   {
      file = fopen(filename, "wb");

      if (file)
      {
         memcpy(&header, model, sizeof(model_t));
         if (model->materials)
         {
            header.materials = (material_t*)sizeof(model_t);
         }
         else
         {
            header.materials = 0;
         }

         if (model->meshes)
         {
            header.meshes = (mesh_t**)(sizeof(model_t) + header.numMaterials * sizeof(material_t));
         }
         else
         {
            header.meshes = 0;
         }

         memset(header.path, 0, 32);
         
         fwrite(&header, sizeof(model_t), 1, file);

         for (i = 0; i < model->numMaterials; i++)
         {
            //===================//
            // copy the material //
            //===================//
            memcpy(&material, &model->materials[i], sizeof(material_t));

            //=========================//
            // clear the runtime stuff //
            //=========================//
            material.bumpHandle = 0;
            material.texHandle = 0;
            material.dl = 0;

            //========================//
            // write out the material //
            //========================//
            fwrite(&material, sizeof(material_t), 1, file); 
         }

         for (i = 0; i < model->numMeshes; i++)
         {
            //======================//
            // copy the mesh header //
            //======================//
            memcpy(&mesh, model->meshes[i], sizeof(mesh_t));

            //==============================//
            // compute the size of the mesh //
            //==============================//
            mesh.size = sizeof(mesh_t) + mesh.numVerts * sizeof(vertex_t) + mesh.numTris * sizeof(unsigned long) * 3;
            if (model->meshes[i]->tanSpace && (flags & WRITE_TANGENT_SPACE))
            {
               mesh.size += mesh.numVerts * sizeof(tanSpace_t);
            }
            if (model->meshes[i]->faceNormals && (flags & WRITE_FACE_NORMALS))
            {
               mesh.size += mesh.numTris * sizeof(float) * 4;
            }

            mesh.dl = 0;
            mesh.vertexBuffer = 0;
            mesh.indexBuffer = 0;

            //=============================//
            // compute the pointer offsets //
            //=============================//
            pos = sizeof(mesh_t);
            mesh.verts = (vertex_t*)sizeof(mesh_t);
            pos += mesh.numVerts * sizeof(vertex_t);
            mesh.tris = (unsigned long(*)[3])((unsigned long)mesh.verts + mesh.numVerts * sizeof(vertex_t));
            pos += mesh.numTris * sizeof(unsigned long) * 3;
            if (model->meshes[i]->tanSpace && (flags & WRITE_TANGENT_SPACE))
            {
               mesh.tanSpace = (tanSpace_t*)pos;
               pos += sizeof(tanSpace_t) * model->meshes[i]->numVerts;
            }
            else
            {
               mesh.tanSpace = NULL;
            }
            if (model->meshes[i]->faceNormals && (flags & WRITE_FACE_NORMALS))
            {
               mesh.faceNormals = (float(*)[4])pos;
               pos += sizeof(float) * 4 * model->meshes[i]->numTris;
            }
            else
            {
               mesh.faceNormals = NULL;
            }

            if (mesh.material)
            {
               mesh.material = (material_t*)(((unsigned long)mesh.material - (unsigned long)model->materials) / sizeof(material_t) + 1);
            }
            else
            {
               mesh.material = NULL;
            }

            //======================//
            // write out the header //
            //======================//
            fwrite(&mesh, sizeof(mesh_t), 1, file);
         
            //========================//
            // write out the vertices //
            //========================//
            fwrite(model->meshes[i]->verts, sizeof(vertex_t), model->meshes[i]->numVerts, file);
         
            //=========================//
            // write out the triangles //
            //=========================//
            fwrite(model->meshes[i]->tris, sizeof(unsigned long) * 3, model->meshes[i]->numTris, file);


            if (model->meshes[i]->tanSpace && (flags & WRITE_TANGENT_SPACE))
            {
               //=============================//
               // write out the tangent space //
               //=============================//
               fwrite(model->meshes[i]->tanSpace, sizeof(float) * 9, model->meshes[i]->numVerts, file);
            }

            if (model->meshes[i]->faceNormals && (flags & WRITE_FACE_NORMALS))
            {
               //===========================//
               // write out the faceNormals //
               //===========================//
               fwrite(model->meshes[i]->faceNormals, sizeof(float) * 4, model->meshes[i]->numTris, file);
            }
         }

         fclose(file);
      }
      else
      {
         return MODEL_FILE_ERROR;
      }
   }
   else
   {
      return MODEL_ERROR;
   }

   return MODEL_OK;
}

//===================================================================//
// LoadModel                                                         //
// loads the model                                                   //
//===================================================================//
int LoadModel(char* filename, model_t* model)
{
   int         i;
   unsigned long        start;
   unsigned long        matOffset;
   unsigned long        meshOffset;
   unsigned long        triOffset;
   unsigned long        vertOffset;
   FILE        *file;
   char        path[32];
   char        *slash;
   unsigned long        pathLength = 0;

   strcpy(path, filename);

   //===============================================//
   // get pointer to the last slash in the filename //
   //===============================================//
	slash = strrchr(path, '\\');
   if (slash != NULL)
   {
      slash[1] = '\0';
   }
   else
   {
      slash = strrchr(path, '/');
      if (slash != NULL)
      {
         slash[1] = '\0';
      }
      else
      {
         path[0] = '\0';
      }
   }

   pathLength = strlen(path);

   

   file = fopen(filename, "rb");

   if (file)
   {
      if (model)
      {
         //==========================//
         // read in the model header //
         //==========================//
         fread(model, sizeof(model_t), 1, file);

         strcpy(model->path, path);

         if (model->numMaterials)
         {
            matOffset = (unsigned long)model->materials;
            
            //========================//
            // allocate the materials //
            //========================//
            model->materials = (material_t*)malloc(sizeof(material_t) * model->numMaterials);
            if (model->materials)
            {
               //========================================//
               // seek to the start of the material list //
               //========================================//
               fseek(file, matOffset, SEEK_SET);

               //=======================//
               // read in the materials //
               //=======================//
               fread(model->materials, sizeof(material_t), model->numMaterials, file); 
            }
            else
            {
               fclose(file);
               return MODEL_MEMORY_ERROR;
            }
         }
         
         if (model->numMeshes)
         {
            meshOffset = (unsigned long)model->meshes;
            //==============================//
            // allocate the mesh structures //
            //==============================//
            model->meshes = (mesh_t**)malloc(sizeof(mesh_t*) * model->numMeshes);
            
            if (model->meshes)
            {
               //=================================//
               // seek to the start of the meshes //
               //=================================//
               fseek(file, meshOffset, SEEK_SET);

               for (i = 0; i < model->numMeshes; i++)
               {
                  //==========================================//
                  // record the starting position of the mesh //
                  //==========================================//
                  start = ftell(file);


                  model->meshes[i] = (mesh_t*)malloc(sizeof(mesh_t));
                  if (model->meshes[i])
                  {
                     fread(model->meshes[i], sizeof(mesh_t), 1, file);

                     if ((long)model->meshes[i]->material != 0)
                     {
                        model->meshes[i]->material = &model->materials[(unsigned long)model->meshes[i]->material - 1];
                     }

                     //====================//
                     // record the offsets //
                     //====================//
                     vertOffset = (unsigned long)model->meshes[i]->verts;
                     triOffset = (unsigned long)model->meshes[i]->tris;

                     //===============================//
                     // allocate all of the mesh data //
                     //===============================//
                     AllocateMesh(model->meshes[i]);
      
                     //======================================//
                     // seek to the start of the vertex data //
                     //======================================//
                     fseek(file, start + vertOffset, SEEK_SET);
                     //=========================//
                     // read in the vertex data //
                     //=========================//
                     fread(model->meshes[i]->verts, sizeof(vertex_t), model->meshes[i]->numVerts, file);

                     //========================================//
                     // seek to the start of the triangle data //
                     //========================================//
                     fseek(file, start + triOffset, SEEK_SET);
                     //===========================//
                     // read in the triangle data //
                     //===========================//
                     fread(model->meshes[i]->tris, sizeof(unsigned long) * 3, model->meshes[i]->numTris, file);

                     //=======================//
                     // seek to the next mesh //
                     //=======================//
                     fseek(file, start + model->meshes[i]->size, SEEK_SET);
                  }
                  else
                  {
                     fclose(file);
                     return MODEL_MEMORY_ERROR;
                  }
               }
            }
            else
            {
               fclose(file);
               return MODEL_MEMORY_ERROR;
            }
         }
      }
      else
      {
         fclose(file);
         return MODEL_ERROR;
      }

      fclose(file);
   }
   else
   {
      return MODEL_FILE_ERROR;
   }

   return MODEL_OK;
}

//===================================================================//
// LoadModelFast                                                     //
// loads the model                                                   //
//===================================================================//
int LoadModelFast(char* filename, model_t** model)
{
   int         i;
   unsigned long        modelSize;
   mesh_t*     firstMesh;
   FILE        *file;
   char        path[32];
   char        *slash;
   unsigned long        pathLength = 0;

   strcpy(path, filename);

   //===============================================//
   // get pointer to the last slash in the filename //
   //===============================================//
	slash = strrchr(path, '\\');
   if (slash != NULL)
   {
      slash[1] = '\0';
   }
   else
   {
      slash = strrchr(path, '/');
      if (slash != NULL)
      {
         slash[1] = '\0';
      }
      else
      {
         path[0] = '\0';
      }
   }

   pathLength = strlen(path);

   file = fopen(filename, "rb");

   if (file)
   {
      //=============================//
      // seek to the end of the file //
      //=============================//
      fseek(file, 0, SEEK_END);

      //====================//
      // tell the file size //
      //====================//
      modelSize = ftell(file);

      //==================================//
      // seek to the begining of the file //
      //==================================//
      fseek(file, 0, SEEK_SET);

      //====================//
      // allocate the model //
      //====================//
      *model = (model_t*)malloc(modelSize);
      if (*model)
      {
         //===================//
         // read in the model //
         //===================//
         fread(*model, modelSize, 1, file);

         strcpy((*model)->path, path);

         //========================================================//
         // flag this guy as a big chunk so we know how to free it //
         //========================================================//
         (*model)->info = MODEL_BIG_CHUNK;

         //========================================//
         // convert material offset into a pointer //
         //========================================//
         (*model)->materials = (material_t*)((unsigned long)(*model)->materials + (unsigned long)(*model));

         if ((*model)->numMeshes != 0)
         {
            //=================================//
            // get a pointer to the first mesh //
            //=================================//
            firstMesh = (mesh_t*)((unsigned long)(*model)->meshes + (unsigned long)(*model));
            
            //====================================//
            // allocate an array of mesh pointers //
            //====================================//
            (*model)->meshes = (mesh_t**)malloc(sizeof(mesh_t*) * (*model)->numMeshes);
         
            if ((*model)->meshes != NULL)
            {
               //==============================================================//
               // set the zeroth mesh pointer value to point to the first mesh //
               //==============================================================//
               (*model)->meshes[0] = firstMesh;

               //==========================================================================================//
               // go through the rest of the mesh table and compute values for all the other mesh pointers //
               //==========================================================================================//
               for (i = 0; i < (*model)->numMeshes; i++)
               {
                  if (i != 0)
                  {
                     //====================================================================//
                     // set this mesh pointer to the address of the previous plus its size //
                     //====================================================================//
                     (*model)->meshes[i] = (mesh_t*)((unsigned long)((*model)->meshes[i - 1]) + (*model)->meshes[i - 1]->size);
                  }
                  
                  //==========================================================================//
                  // set the material pointer to point to a material in models material array //
                  //==========================================================================//
                  if ((long)(*model)->meshes[i]->material != 0)
                  {
                     (*model)->meshes[i]->material = &(*model)->materials[(unsigned long)(*model)->meshes[i]->material - 1];
                  }

                  //==============================================================================//
                  // compute the vertex pointer value using the start of the mesh plus the offset //
                  //==============================================================================//
                  (*model)->meshes[i]->verts = (vertex_t*)((unsigned long)((*model)->meshes[i]) + (unsigned long)(*model)->meshes[i]->verts);

                  //================================================================================//
                  // compute the triangle pointer value using the start of the mesh plus the offset //
                  //================================================================================//
                  (*model)->meshes[i]->tris = (unsigned long(*)[3])((unsigned long)((*model)->meshes[i]) + (unsigned long)(*model)->meshes[i]->tris);
               }
            }
         }
      }
      else
      {
         fclose(file);
         *model = NULL;
         return MODEL_MEMORY_ERROR;
      }

      fclose(file);
   }
   else
   {
      *model = NULL;
      return MODEL_FILE_ERROR;
   }

   /*
   for (i = 0; i < (*model)->numMeshes; i++)
   {
      if ((*model)->meshes[i]->numTris >= 65536)
      {
         printf("we have issues\n");
      }
   }
   */
    
   return MODEL_OK;
}


