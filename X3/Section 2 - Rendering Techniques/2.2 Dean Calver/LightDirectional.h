//-----------------------------------------------------------------------------
// File: LightDirectional.h
//
// Desc: header for directional light
//-----------------------------------------------------------------------------
#pragma once

class LightDirectional : public BaseLight
{
public:
	HRESULT Create();

	void Update();
	void Render();

protected:
};

