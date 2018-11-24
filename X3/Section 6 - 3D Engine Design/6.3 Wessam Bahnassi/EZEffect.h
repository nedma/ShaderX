//-----------------------------------------------------------------------------
// File: EZEffect.h
//
// Desc: Header file EZEffect sample app
//-----------------------------------------------------------------------------
#pragma once




//-----------------------------------------------------------------------------
// Defines, and constants
//-----------------------------------------------------------------------------
// TODO: change "DirectX AppWizard Apps" to your name or the company name
#define DXAPP_KEY        TEXT("Software\\DirectX AppWizard Apps\\EZEffect")

// Struct to store the current input state
struct UserInput
{
    // TODO: change as needed
    BOOL bRotateUp;
    BOOL bRotateDown;
    BOOL bRotateLeft;
    BOOL bRotateRight;
};




//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class (CD3DApplication) provides the 
//       generic functionality needed in all Direct3D samples. CMyD3DApplication 
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
    BOOL                    m_bLoadingApp;          // TRUE, if the app is loading
    CD3DFont*               m_pFont;                // Font for drawing text
    UserInput               m_UserInput;            // Struct for storing user input

	// Happy face scene stuff
	SXEffect				m_Effect;
	struct MESH
	{
		MESH():
			pVB(NULL),
			pIB(NULL),
			uStride(0),
			dwFVF(0),
			dwVertsCount(0),
			dwTrisCount(0)
		{
		};

		~MESH()
		{
			SAFE_RELEASE(pVB);
			SAFE_RELEASE(pIB);
		};
		PDIRECT3DVERTEXBUFFER9	pVB;
		PDIRECT3DINDEXBUFFER9	pIB;
		UINT uStride;
		DWORD dwFVF;
		DWORD dwVertsCount;
		DWORD dwTrisCount;
	};

	MESH m_mshFace;
	MESH m_mshLHand;
	MESH m_mshRHand;
	float m_fTimeWeight;

	D3DXMATRIX m_matViewProj;
	D3DXVECTOR3 m_vec3Light;

	PDIRECT3DTEXTURE9		m_pTexFace;

    FLOAT                   m_fWorldRotX;           // World rotation state X-axis
    FLOAT                   m_fWorldRotY;           // World rotation state Y-axis

protected:
    virtual HRESULT OneTimeSceneInit();
    virtual HRESULT InitDeviceObjects();
    virtual HRESULT RestoreDeviceObjects();
    virtual HRESULT InvalidateDeviceObjects();
    virtual HRESULT DeleteDeviceObjects();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();
    virtual HRESULT FinalCleanup();
    virtual HRESULT ConfirmDevice( D3DCAPS9*, DWORD, D3DFORMAT );
	HRESULT CreateGeometry(PCTSTR pszVBFile,PCTSTR pszIBFile,MESH& mshObject,bool bTextured);

	HRESULT RenderHappyScene();
    HRESULT RenderText();

    void    UpdateInput( UserInput* pUserInput );
public:
    LRESULT MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    CMyD3DApplication();
    virtual ~CMyD3DApplication();
};

