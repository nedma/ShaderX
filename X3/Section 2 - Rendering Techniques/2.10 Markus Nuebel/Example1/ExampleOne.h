//-----------------------------------------------------------------------------
// File: ExampleOne.h
//
// Desc: Header file for ExampleOne sample app
//-----------------------------------------------------------------------------
#pragma once

#include "ExampleBase.h"



//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class (CD3DApplication) provides the
//       generic functionality needed in all Direct3D samples. CMyD3DApplication
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CBaseD3DApplication
{
public:
    CMyD3DApplication();
    virtual ~CMyD3DApplication();

protected:

	D3DXVECTOR4	m_vLight1;
	D3DXVECTOR4	m_vLight2;
	D3DXVECTOR4	m_vVSParameters;
	D3DXVECTOR4	m_vPSParameters;

   HRESULT RenderText();

	virtual HRESULT CreateShaders();
	virtual HRESULT InitializeLights();
   virtual HRESULT SetShaderConstants();
   virtual HRESULT PreRender();
   virtual HRESULT PostRender();
   virtual HRESULT FrameMove();

};