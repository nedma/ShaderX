//-----------------------------------------------------------------------------
// File: LightHemishere.cpp
//
// Desc: Implementation of hemisphere light code 
//-----------------------------------------------------------------------------
#pragma once

class LightHemisphere : public BaseLight
{
public:
	HRESULT Create();

	void Update();
	void Render();

	D3DXVECTOR4 m_SkyColour;
protected:
};

