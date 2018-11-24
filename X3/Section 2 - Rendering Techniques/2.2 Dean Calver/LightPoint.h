#pragma once

class LightPoint : public BaseLight
{
public:
	HRESULT Create();

	void Update();
	void Render();

	// radius should be half the world scale of the object else special things will happen
	float	m_Radius;

	// each light should have a unique ID for the stencil buffer
	// you could either allocate them every frame(and if nessecary clear the stencil buffer 
	// n times to support > 255 lights)
	// or as the demos do just allocate a static ID.
	// used to guarentee each light effects each pixel once and once only
	unsigned char m_LightID;

	// use debugger pixel shader or the real thing (true for debugger)
	bool m_ShowVolume;
protected:

	CULLSTATE CullLight( CULLINFO* pCullInfo );
	bool CrossesNearPlane();

	BYTE m_bOutside[8];
	BYTE m_bOut;
	BYTE m_bIn;

};

