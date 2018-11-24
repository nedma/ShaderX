#pragma once

//-----------------------------------------------------------------------------
// Name: BaseLight
// Desc: Simple base class for light objects
//-----------------------------------------------------------------------------
class BaseLight
{
public:

	BaseLight();
	virtual ~BaseLight();

	// handle after a reset
	virtual void Reset();

	// do any update code nessecary
	virtual void Update();
	
	// render the light geometry reading parameters from the G-Buffer
	virtual void Render() = 0;

	D3DXMATRIX		m_WorldMatrix;		// world position/rotation/scale 

	ID3DXEffect*	m_Shader;			// Light shader
    ID3DXMesh*		m_Mesh;				// D3DX that hits every pixel used by this light

	D3DXVECTOR4		m_LightColour;		// colour of the light
	float			m_LightIntensity;	// intensity of the light

protected:
	D3DXMATRIX		m_WorldViewMatrix;		// after update has factoring in the view space
	D3DXVECTOR4		m_ActualColour;			// colour * intensity (calculated in update)

	// runs the pixel shader to the entire screen
	void ApplyFullScreenPixelShader( const unsigned int width, const unsigned int height );
};