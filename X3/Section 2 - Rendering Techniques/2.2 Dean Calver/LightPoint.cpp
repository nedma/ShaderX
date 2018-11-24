#include "dxstdafx.h"


#include "GBuffer.h"
#include "GlobalObjects.h"
#include "GlobalMatrices.h"
#include "LightModelTexture.h"

#include "BaseLight.h"
#include "LightPoint.h"

HRESULT LightPoint::Create()
{
	HRESULT hr;
	// a pixel shader that does directional lighting on the fat framebuffer
	if( FAILED( hr = D3DXCreateEffectFromFile(	g_Objs.m_pD3DDevice, 
												TEXT("LightPoint.fx"),
												0, // MACROs
												0, // Include
												0, // flags
												g_Objs.m_LightEffectPool, // Effect Pool
												&m_Shader,
												0 ) ) )
		return DXTRACE_ERR( L"D3DXCreateEffectFromFile LightPoint.fx", hr );

	// a sphere mesh of radius 0.5f, it well tessleated (we are not vertex limited at this stage)
	if( FAILED( hr = D3DXCreateSphere( g_Objs.m_pD3DDevice, 0.5f, 20, 10, &m_Mesh, NULL ) ) )
		return DXTRACE_ERR( L"D3DXCreateSphere", hr );

	m_ShowVolume = false;

	return S_OK;
}

void LightPoint::Update()
{
	if( m_Shader == 0 )
		return;
	// call our super update
	BaseLight::Update();

	D3DXVECTOR4 vec;
	const D3DXVECTOR3 pt(0,0,0);
	// extract the position
	D3DXVec3Transform( &vec, &pt, &m_WorldViewMatrix );
	m_Shader->SetVector( "f3LightPosition", &vec );

	// update colour
	m_Shader->SetVector( "f3LightColour", &m_ActualColour );

	// update distanace
	m_Shader->SetFloat( "fRcpRadius", 1.f / m_Radius );

	// update attenuation function
	m_Shader->SetFloat( "fAttenuationFunction", AttenuationIndexToV(AF_SHAPE1) );

}

//-----------------------------------------------------------------------------
// Name: CullObject()
// Desc: Determine the cullstate for an object bounding box (OBB).  
//       The algorithm is:
//       1) If any OBB corner pt is inside the frustum, return CS_INSIDE
//       2) Else if all OBB corner pts are outside a single frustum plane, 
//          return CS_OUTSIDE
//       3) Else if any frustum edge penetrates a face of the OBB, return 
//          CS_INSIDE_SLOW
//       4) Else if any OBB edge penetrates a face of the frustum, return
//          CS_INSIDE_SLOW
//       5) Else if any point in the frustum is outside any plane of the 
//          OBB, return CS_OUTSIDE_SLOW
//       6) Else return CS_INSIDE_SLOW
//-----------------------------------------------------------------------------
CULLSTATE LightPoint::CullLight( CULLINFO* pCullInfo )
{
    ZeroMemory( &m_bOutside, sizeof(m_bOutside) );

	D3DXVECTOR3 LocalVecBounds[8];
	D3DXVECTOR4 WorldVecBounds[8];

	LocalVecBounds[0] = D3DXVECTOR3(-1, 1, 1);
	LocalVecBounds[1] = D3DXVECTOR3( 1, 1, 1);
	LocalVecBounds[2] = D3DXVECTOR3(-1,-1, 1);
	LocalVecBounds[3] = D3DXVECTOR3( 1,-1, 1);
	LocalVecBounds[4] = D3DXVECTOR3(-1, 1,-1);
	LocalVecBounds[5] = D3DXVECTOR3( 1, 1,-1);
	LocalVecBounds[6] = D3DXVECTOR3(-1,-1,-1);
	LocalVecBounds[7] = D3DXVECTOR3( 1,-1,-1);

	D3DXVec3TransformArray( WorldVecBounds, sizeof( D3DXVECTOR4), LocalVecBounds, sizeof(D3DXVECTOR3), &m_WorldMatrix, 8 );

	D3DXVECTOR4* pVecBounds = WorldVecBounds;

    // Check boundary vertices against all 6 frustum planes, 
    // and store result (1 if outside) in a bitfield
    for( int iPoint = 0; iPoint < 8; iPoint++ )
    {
        for( int iPlane = 0; iPlane < 6; iPlane++ )
        {
            if( pCullInfo->planeFrustum[iPlane].a * pVecBounds[iPoint].x +
                pCullInfo->planeFrustum[iPlane].b * pVecBounds[iPoint].y +
                pCullInfo->planeFrustum[iPlane].c * pVecBounds[iPoint].z +
                pCullInfo->planeFrustum[iPlane].d < 0)
            {
                m_bOutside[iPoint] |= (1 << iPlane);
            }
        }
    }

	m_bOut =	m_bOutside[0] & m_bOutside[1] & m_bOutside[2] & m_bOutside[3] & 
				m_bOutside[4] & m_bOutside[5] & m_bOutside[6] & m_bOutside[7];
	m_bIn =		m_bOutside[0] | m_bOutside[1] | m_bOutside[2] | m_bOutside[3]  | 
		        m_bOutside[4] | m_bOutside[5] | m_bOutside[6] | m_bOutside[7];

    // If all points are outside any single frustum plane, the object is
    // outside the frustum
    if( m_bOut != 0 )
    {
        return CS_OUTSIDE;
    } else if( m_bIn == 0 )
    {
        return CS_INSIDE;
    } else
		return CS_CROSSING;
}

bool LightPoint::CrossesNearPlane()
{
	// behind near plane
    if( (m_bOut & (1 << CP_NEAR)) != 0 )
    {
        return false;
    }
	// in front of near plane
    if( (m_bIn & (1 << CP_NEAR)) == 0 )
    {
        return false;
    }

	return true;
}

void LightPoint::Render()
{
	if( m_Shader == 0 )
		return;

	CULLSTATE CullState = CullLight( &g_Objs.m_CullInfo );
	if( CullState == CS_OUTSIDE )
		return;

	// debugger or proper code
	if ( m_ShowVolume )
	{
		if( CrossesNearPlane() == true )
		{
			m_Shader->SetTechnique( "Debug0");
		} else
		{
			m_Shader->SetTechnique( "Debug1");
		}
	} else
	{
		// normal if the sphere cross the near plane use the more expensive version
		if( CrossesNearPlane() == true )
		{
			m_Shader->SetTechnique( "T0");
		} else
		{
			m_Shader->SetTechnique( "T1");
		}
	}

	const D3DXMATRIXA16& matViewProjection = g_Matrices.m_ViewProj;
	D3DXMATRIXA16 matWorldViewProjection;

	D3DXMatrixMultiply( &matWorldViewProjection, &m_WorldMatrix, &matViewProjection );
	m_Shader->SetMatrix( "matWorldView", &m_WorldViewMatrix );
	m_Shader->SetMatrix( "matWorldViewProjection", &matWorldViewProjection );

	unsigned int numPasses;
	// directional lights don't use the mesh object they render to the entire screen
	m_Shader->Begin( &numPasses, D3DXFX_DONOTSAVESTATE );
	for(unsigned int i=0;i < numPasses;i++)
	{
		m_Shader->BeginPass(i);
		m_Mesh->DrawSubset(0);
		m_Shader->EndPass();
	}

	m_Shader->End();
}
