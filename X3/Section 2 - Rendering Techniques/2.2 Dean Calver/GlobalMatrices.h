//-----------------------------------------------------------------------------
// File: GlobalMatrices.h
//
// Desc: Header file for matrices that are shared all over the place
//-----------------------------------------------------------------------------
#pragma once

class GlobalMatrices
{
public:

	D3DXMATRIXA16 m_Proj;

	D3DXMATRIXA16 m_View;
	D3DXMATRIXA16 m_ViewProj;

	D3DXMATRIXA16 m_ShadowView;
	D3DXMATRIXA16 m_ShadowViewProj;
};

extern GlobalMatrices g_Matrices;