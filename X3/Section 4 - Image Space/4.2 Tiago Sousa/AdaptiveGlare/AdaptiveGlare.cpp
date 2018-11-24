///////////////////////////////////////////////////////////////////////////////////////////////////
//  Proj : ShaderX 3
//  File : AdaptiveGlare.cpp
//  Desc : Adaptive glare effect demo
//  Code : Tiago Sousa
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "D3dApp.h"
#include "Common.h"
#include "Win32App.h"
#include "Control.h"
#include "Camera.h"
#include "Shader.h"
#include "Mesh.h"
#include "Material.h"
#include "RenderTarget.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

class CMyApp:public CD3DApp 
{
public:  
  CMyApp()
  {        
    m_iFrameCounter=0;    
    m_pCGContext=0;
    m_pSkybox=0;
    m_pScene=0;    
    m_plD3DBackbufferSurf=0;
    m_plD3DDepthStencilSurfAA=0;
    m_plD3DDepthStencilSurf=0;
    m_pRTBackground=0;
    m_pRTGlareAmount=0;
    m_pRTGlare=0;    
    m_pRTBackground2x2=0;
    m_pRTResize=0;
    m_bGlareOn=1;
  }

  ~CMyApp()
  {
    Release();
  }

  // Initialize/shutdown application
  int InitializeApp(void);
  int ShutDownApp(void);

  // Update/render frame
  int Update(float fTimeSpan);
  int Render(float fTimeSpan);

  // Render a screen aligned quad. Assumes textures/shaders are already set.
  void RenderScreenAlignedQuad(float fOffU=0, float fOffV=0);
  // Load scene shaders
  int ReloadShaders(void);
  // Blur texture
  void BlurTexture(CRenderTarget *pTexture, int iBlurAmount);  
  // Copy one texture into other
  void CopyTexture(CRenderTarget *pSrc, CRenderTarget *pDst);
  // Set current viewport
  void SetViewport(int iWidth, int iHeight);

  // Render current scene
  void RenderScene(void);
  // Compute glare amount
  void GlareAmount(void);
  // Apply brightpass filter
  void BrightPass(void);
  // Final image composition
  void FinalComposition(void);

private:
  bool m_bGlareOn;
  int m_iFrameCounter;

  int m_iWidth, m_iHeight;
  
  CCamera m_pCamera; 
  CCameraControl m_pCameraControl;    

  CBaseMesh *m_pSkybox, *m_pScene;
  CShader m_pSHSkyBox, m_pSHScene, m_pSHBlur, m_pSHGlareMap, m_pSHGlareFinal, m_pSHGlareAmount;  

  IDirect3DSurface9 *m_plD3DBackbufferSurf,
                    *m_plD3DDepthStencilSurfAA, 
                    *m_plD3DDepthStencilSurf;

  CRenderTarget *m_pRTBackground, *m_pRTBackground2x2, *m_pRTGlareAmount, *m_pRTGlare, *m_pRTResize;
};

int CMyApp::InitializeApp(void) 
{     
  _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

  //////////////////////////////////////////////////////////////////////////  
  // Create camera and camera controler
  //////////////////////////////////////////////////////////////////////////  

  int iWidth=0, iHeight=0, iBps=0;
  m_pApp->GetApp()->GetScreenInfo(iWidth, iHeight, iBps);

  m_pCamera.Create(60, iWidth, iHeight, 2.0f, 2048.0f);  
  m_pCamera.SetPosition(CVector3f(18,0, 70));
  m_pCamera.SetHeading(45);

  m_pCameraControl.SetCamera(m_pCamera);
  m_pCameraControl.SetInput(m_pApp->GetApp()->m_pInput);

  //////////////////////////////////////////////////////////////////////////  
  // Load scene shaders
  //////////////////////////////////////////////////////////////////////////  

  if(FAILED(ReloadShaders()))
  {
    OutputMsg("Error", "Loading shaders");
    return APP_ERR_INITFAIL;
  }

  //////////////////////////////////////////////////////////////////////////  
  // Load scene geometry
  //////////////////////////////////////////////////////////////////////////  

  m_pSkybox=new CBaseMesh;
  if(FAILED(m_pSkybox->Create("skybox.tds")))
  {
    return APP_ERR_INITFAIL;
  }

  m_pScene=new CBaseMesh;
  if(FAILED(m_pScene->Create("scene.tds")))  
  {
    return APP_ERR_INITFAIL;
  }

  //////////////////////////////////////////////////////////////////////////  
  // Create render targets
  //////////////////////////////////////////////////////////////////////////  

  m_pRTBackground=new CRenderTarget;
  if(FAILED(m_pRTBackground->Create(iWidth, iHeight, D3DFMT_A8R8G8B8)))
  {
    return APP_ERR_INITFAIL;
  }

  m_pRTGlare=new CRenderTarget;
  if(FAILED(m_pRTGlare->Create(iWidth/8, iHeight/8, D3DFMT_A8R8G8B8)))
  {
    return APP_ERR_INITFAIL;
  }

  m_pRTGlareAmount=new CRenderTarget;
  if(FAILED(m_pRTGlareAmount->Create(1, 1, D3DFMT_A8R8G8B8)))
  {
    return APP_ERR_INITFAIL;
  }

  m_pRTBackground2x2=new CRenderTarget;
  if(FAILED(m_pRTBackground2x2->Create(2, 2, D3DFMT_A8R8G8B8)))
  {
    return APP_ERR_INITFAIL;
  }

  m_pRTResize=new CRenderTarget;
  if(FAILED(m_pRTResize->Create(128, 128, D3DFMT_A8R8G8B8, 1)))
  {
    return APP_ERR_INITFAIL;
  }

  // Get backbuffer
  if(FAILED(m_plD3DDevice->GetRenderTarget(0, &m_plD3DBackbufferSurf)))
  {
    OutputMsg("Error", "Getting current backbuffer surface");
    return APP_ERR_INITFAIL;
  }

  // Get depthstencil 
  if(FAILED(m_plD3DDevice->GetDepthStencilSurface(&m_plD3DDepthStencilSurfAA)))
  {
    OutputMsg("Error", "Getting current depth stencil surface");
    return APP_ERR_INITFAIL;
  }

  // Create depthstencil withouth multisampling
  if(FAILED(m_plD3DDevice->CreateDepthStencilSurface(iWidth, iHeight, D3DFMT_D24X8, (D3DMULTISAMPLE_TYPE)0, 0, 0, &m_plD3DDepthStencilSurf, 0)))
  {
    OutputMsg("Error", "Getting current depth stencil surface");
    return APP_ERR_INITFAIL;
  }
    
  return APP_OK;
}

int CMyApp::ShutDownApp(void)
{  
  SAFE_DELETE(m_pSkybox)
  SAFE_DELETE(m_pScene)  
  SAFE_RELEASE(m_plD3DBackbufferSurf)
  SAFE_RELEASE(m_plD3DDepthStencilSurfAA)  
  SAFE_RELEASE(m_plD3DDepthStencilSurf)    
  SAFE_DELETE(m_pRTBackground)
  SAFE_DELETE(m_pRTGlareAmount)
  SAFE_DELETE(m_pRTGlare)
  SAFE_DELETE(m_pRTBackground2x2)
  SAFE_DELETE(m_pRTResize)

  return APP_OK;
}

int CMyApp::ReloadShaders(void)
{
  // Make sure to release all data before reloading
  m_pSHScene.Release();
  m_pSHSkyBox.Release();
  m_pSHGlareMap.Release();
  m_pSHBlur.Release();
  m_pSHGlareFinal.Release();
  m_pSHGlareAmount.Release();

  //////////////////////////////////////////////////////////////////////////  
  // Load shaders
  //////////////////////////////////////////////////////////////////////////  

  // Create shared shader element declaration (Position, texture coordinates and normal)
  D3DVERTEXELEMENT9 plD3dSharedDecl[] = 
  {    
    { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0},
    { 0,  3*sizeof(float), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
    { 0,  3*sizeof(float)+2*sizeof(float), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},    
    D3DDECL_END()
  };

  m_pSHScene.CreateVertexShader(m_plD3DDevice, m_pCGContext, "simpleVP.cg", plD3dSharedDecl, CG_PROFILE_VS_1_1);
  m_pSHScene.CreatePixelShader(m_plD3DDevice, m_pCGContext, "simpleFP.cg", CG_PROFILE_PS_1_1);

  m_pSHSkyBox.CreateVertexShader(m_plD3DDevice, m_pCGContext, "skyboxVP.cg", plD3dSharedDecl, CG_PROFILE_VS_1_1);
  m_pSHSkyBox.CreatePixelShader(m_plD3DDevice, m_pCGContext, "skyboxFP.cg", CG_PROFILE_PS_1_1);

  m_pSHGlareMap.CreateVertexShader(m_plD3DDevice, m_pCGContext, "sharedQuadVP.cg", plD3dSharedDecl, CG_PROFILE_VS_1_1);
  m_pSHGlareMap.CreatePixelShader(m_plD3DDevice, m_pCGContext, "glareFP.cg", CG_PROFILE_PS_1_1);
  
  m_pSHBlur.CreateVertexShader(m_plD3DDevice, m_pCGContext, "sharedQuadVP.cg", plD3dSharedDecl, CG_PROFILE_VS_1_1);
  m_pSHBlur.CreatePixelShader(m_plD3DDevice, m_pCGContext, "blurFP.cg", CG_PROFILE_PS_1_1);

  m_pSHGlareFinal.CreateVertexShader(m_plD3DDevice, m_pCGContext, "sharedQuadVP.cg", plD3dSharedDecl, CG_PROFILE_VS_1_1);
  m_pSHGlareFinal.CreatePixelShader(m_plD3DDevice, m_pCGContext, "glareFinalFP.cg", CG_PROFILE_PS_1_1);

  m_pSHGlareAmount.CreateVertexShader(m_plD3DDevice, m_pCGContext, "sharedQuadVP.cg", plD3dSharedDecl, CG_PROFILE_VS_1_1);
  m_pSHGlareAmount.CreatePixelShader(m_plD3DDevice, m_pCGContext, "glareAmountFP.cg", CG_PROFILE_PS_1_1);

  return APP_OK;
}

void CMyApp::RenderScreenAlignedQuad(float fOffU, float fOffV)
{
  float pVertexBuffer[]=
  {
    // pos | uv  
    -fOffU, -fOffV, 0, 0, 0, 
    -fOffU, 1-fOffV, 0, 0, 1,
    1-fOffU, 0-fOffV, 0, 1, 0,
    1-fOffU, 1-fOffV, 0, 1, 1,
  };

  // Setup orthographic projection
  CMatrix44f pOrthoProj;
  pOrthoProj.Identity(); 
  D3DXMatrixOrthoOffCenterRH((D3DXMATRIX *)&pOrthoProj, 0, 1, 1, 0, 0.0f, 1.0f);
  pOrthoProj.Transpose();

  // Set ViewProj Matrix vsh constant (assuming WorldViewProjection is at register 0)
  m_plD3DDevice->SetVertexShaderConstantF(0, &pOrthoProj.m_f11, 4);

  m_plD3DDevice->SetRenderState(D3DRS_ZENABLE, 0);
  m_plD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertexBuffer, 5*sizeof(float));   
  m_plD3DDevice->SetRenderState(D3DRS_ZENABLE, 1);
}

void CMyApp::SetViewport(int iWidth, int iHeight)
{
  D3DVIEWPORT9 pViewport;
  pViewport.X=0; pViewport.Y=0;
  pViewport.Width=iWidth; pViewport.Height=iHeight;     
  pViewport.MinZ=0.0f; pViewport.MaxZ=1.0f; 
  m_plD3DDevice->SetViewport(&pViewport);
}

void CMyApp::BlurTexture(CRenderTarget *pTexture, int iBlurAmount)
{
  if(!pTexture || !iBlurAmount)
  {
    return;
  }

  // Setup texture offsets, for texture neighbours sampling
  int iWidth, iHeight, iFormat;  
  pTexture->GetProperties(iWidth, iHeight, iFormat);
  RECT pRect={0, 0, iWidth, iHeight };   

  float s1=1.0f/(float) iWidth;
  float t1=1.0f/(float) iHeight;
  float s_off=s1*0.5f; 
  float t_off=t1*0.5f; 

  m_pSHBlur.SetShader();  

  // Set texture stages/samples state
  m_plD3DDevice->SetTexture(0,pTexture->GetTexture());
  m_plD3DDevice->SetTexture(1,pTexture->GetTexture());
  m_plD3DDevice->SetTexture(2,pTexture->GetTexture());
  m_plD3DDevice->SetTexture(3,pTexture->GetTexture());

  m_plD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
  m_plD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
  m_plD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
  m_plD3DDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
  m_plD3DDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
  m_plD3DDevice->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
  m_plD3DDevice->SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
  m_plD3DDevice->SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
  m_plD3DDevice->SetSamplerState(2, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
  m_plD3DDevice->SetSamplerState(3, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
  m_plD3DDevice->SetSamplerState(3, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
  m_plD3DDevice->SetSamplerState(3, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

  for(int b=0; b<iBlurAmount; b++) 
  {      
    float fPasses=(float)b;
    float pfOffset0[]={ s1*0.5f*fPasses+s_off, t1*fPasses+t_off, -s1*fPasses-s_off, t1*0.5f*fPasses+t_off};
    float pfOffset1[]={ -s1*0.5f*fPasses-s_off, -t1*fPasses-t_off, s1*fPasses+s_off, -t1*0.5f*fPasses-t_off};
    m_pSHBlur.SetVertexParam("vOffset01", pfOffset0, 1);
    m_pSHBlur.SetVertexParam("vOffset02", pfOffset1, 1);

    RenderScreenAlignedQuad(s_off, t_off);    
    m_plD3DDevice->StretchRect(m_plD3DBackbufferSurf, &pRect, pTexture->GetSurface(), &pRect, D3DTEXF_NONE);
  }
}

int CMyApp::Update(float fTimeSpan)
{  
  // Some hardcoded keys. 
  // Escape quits aplication, F5 reloads shaders, F1/F2 - enable disable glare

  if(m_pApp->m_pInput.GetKeyPressed(VK_ESCAPE))
  {
    return APP_ERR_UNKNOWN;
  }
 
  if(m_pApp->m_pInput.GetKeyPressed(VK_F5))
  {
    if(FAILED(ReloadShaders()))
    {
      OutputMsg("Error", "Loading shaders");     
    }
  }

  if(m_pApp->m_pInput.GetKeyPressed(VK_F1))
  {
    m_bGlareOn=1;
  }
  if(m_pApp->m_pInput.GetKeyPressed(VK_F2))  
  {
    m_bGlareOn=0;
  }  

  m_pCameraControl.Update(0.1f);    
  
  return APP_OK;
}

void CMyApp::CopyTexture(CRenderTarget *pSrc, CRenderTarget *pDst)
{  
  // Get current render target
  IDirect3DSurface9 *m_plD3DCurrSurf;
  m_plD3DDevice->GetRenderTarget(0, &m_plD3DCurrSurf);
  D3DVIEWPORT9 pViewport;
  m_plD3DDevice->GetViewport(&pViewport);

  m_plD3DDevice->SetRenderTarget(0, pDst->GetSurface());  
  m_plD3DDevice->SetDepthStencilSurface(m_plD3DDepthStencilSurf);
  SetViewport(pDst->GetWidth(), pDst->GetHeight());

  m_plD3DDevice->SetVertexDeclaration(m_pSHBlur.GetVertexDeclaration());
  m_plD3DDevice->SetVertexShader(m_pSHBlur.GetVertexShader());  
  float pfOffset0[]={ 0, 0, 0, 0};  
  m_pSHBlur.SetVertexParam("vOffset01", pfOffset0, 1);
  
  m_plD3DDevice->SetPixelShader(0);  

  m_plD3DDevice->SetTexture(0, pSrc->GetTexture()); 
  m_plD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
  m_plD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
 
  // Need to explicitly define this texture stages states for Nvidia cards.
  m_plD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
  m_plD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
  m_plD3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

  RenderScreenAlignedQuad(0.5f/(float)pSrc->GetWidth(), 0.5f/(float)pSrc->GetHeight());

  // restore data
  m_plD3DDevice->SetRenderTarget(0, m_plD3DCurrSurf);
  m_plD3DDevice->SetDepthStencilSurface(m_plD3DDepthStencilSurfAA);
  m_plD3DDevice->SetViewport(&pViewport);  
  SAFE_RELEASE(m_plD3DCurrSurf)
}

void CMyApp::RenderScene(void)
{
  // Setup viewport
  SetViewport(m_iWidth, m_iHeight);

  // WorldViewProjection matrix is shared among all vertex shaders
  CMatrix44f pProj=m_pCamera.GetProjectionMatrix(), pView=m_pCamera.GetViewMatrix();
  CMatrix44f pWorldViewProj=pView*pProj;
  pWorldViewProj.Transpose();

  //////////////////////////////////////////////////////////////////////////    
  // Render skybox/background
  //////////////////////////////////////////////////////////////////////////

  m_pSHSkyBox.SetShader();
  m_pSHSkyBox.SetVertexParam("ModelViewProj", &pWorldViewProj.m_f11, 4);
  float pCamPos[]={ m_pCamera.GetPosition().m_fX, m_pCamera.GetPosition().m_fY, m_pCamera.GetPosition().m_fZ, 0.0f};    
  m_pSHSkyBox.SetVertexParam("vCameraPosition", pCamPos, 1);

  m_plD3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
  m_plD3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
  m_plD3DDevice->SetRenderState(D3DRS_ZENABLE, 0);

  // Get vertex buffer and material lists
  CVertexBuffer *pVB=m_pSkybox->GetVB();
  const CMaterial *pMaterialList=m_pSkybox->GetMaterialList();

  pVB->Enable();
  CSubMesh *pSubMeshList=m_pSkybox->GetSubMeshes();    
  for(int s=0; s<m_pSkybox->GetSubMeshCount(); s++)
  {
    // Set index buffer      
    CIndexBuffer *pIB=pSubMeshList[s].GetIndexBuffer();
    pIB->Enable();

    // Set decal texture
    const CTexture *pDecal=pMaterialList[pSubMeshList[s].GetMaterialID()].GetDecalTex();
    m_plD3DDevice->SetTexture(0,(IDirect3DTexture9*)pDecal->GetTexture());

    int iVertexCount=pSubMeshList[s].GetFaceCount()*3;
    int iIndicesCount=pSubMeshList[s].GetIndexBuffer()->GetCount();      
    m_plD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, pVB->GetCount(), 0, pSubMeshList[s].GetFaceCount());               
  }

  m_plD3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
  m_plD3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
  m_plD3DDevice->SetRenderState(D3DRS_ZENABLE, 1);

  //////////////////////////////////////////////////////////////////////////    
  // Render scene geometry
  //////////////////////////////////////////////////////////////////////////

  m_pSHScene.SetShader();
  m_pSHScene.SetVertexParam("ModelViewProj", &pWorldViewProj.m_f11, 4);

  pVB=m_pScene->GetVB();
  pVB->Enable();

  pSubMeshList=m_pScene->GetSubMeshes();
  pMaterialList=m_pScene->GetMaterialList();    

  for(int s=0; s<m_pScene->GetSubMeshCount(); s++)
  {
    // Set index buffer      
    CIndexBuffer *pIB=pSubMeshList[s].GetIndexBuffer();
    pIB->Enable();

    // Set decal texture
    const CTexture *pDecal=pMaterialList[pSubMeshList[s].GetMaterialID()].GetDecalTex();
    m_plD3DDevice->SetTexture(0,(IDirect3DTexture9*)pDecal->GetTexture());
    m_plD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, pVB->GetCount(), 0, pSubMeshList[s].GetFaceCount());         
  }

  if(m_bGlareOn)
  {            
    // Get backbuffer into background texture            
    m_plD3DDevice->StretchRect(m_plD3DBackbufferSurf, 0, m_pRTBackground->GetSurface(), 0, D3DTEXF_NONE);      

    // Copy into small texture, and generate mipmaps
    CopyTexture( m_pRTBackground, m_pRTResize);      
    m_pRTResize->GenerateMipMaps();
  }
}

void CMyApp::GlareAmount(void)
{
  //////////////////////////////////////////////////////////////////////////
  // Compute glare amount
  //////////////////////////////////////////////////////////////////////////
  
  // 1st: Resize screen into a 2x2 texture
  CopyTexture(m_pRTResize, m_pRTBackground2x2);

  // 2nd: sample the 2x2 texture, to get an average luminosity

  // Setup viewport
  SetViewport(1, 1);
  m_plD3DDevice->SetRenderTarget(0, m_pRTGlareAmount->GetSurface());
  m_plD3DDevice->SetDepthStencilSurface(m_plD3DDepthStencilSurf);

  // Setup shader
  m_pSHGlareAmount.SetShader();    

  float pfOffset0[]={ -0.5, -0.5, -0.5, 0.5};
  float pfOffset1[]={ 0.5, -0.5, 0.5, 0.5};

  // Set texel offsets, for correct texel to pixel mapping
  m_pSHGlareAmount.SetVertexParam("vOffset01", pfOffset0, 1);
  m_pSHGlareAmount.SetVertexParam("vOffset02", pfOffset1, 1);

  m_plD3DDevice->SetTexture(0,m_pRTBackground2x2->GetTexture()); 
  m_plD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
  m_plD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
  m_plD3DDevice->SetTexture(1,m_pRTBackground2x2->GetTexture()); 
  m_plD3DDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
  m_plD3DDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
  m_plD3DDevice->SetTexture(2,m_pRTBackground2x2->GetTexture()); 
  m_plD3DDevice->SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_POINT);
  m_plD3DDevice->SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
  m_plD3DDevice->SetTexture(3,m_pRTBackground2x2->GetTexture()); 
  m_plD3DDevice->SetSamplerState(3, D3DSAMP_MINFILTER, D3DTEXF_POINT);
  m_plD3DDevice->SetSamplerState(3, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

  // Use alpha blending to interpolate between previous result
  m_plD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
  m_plD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
  m_plD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

  RenderScreenAlignedQuad(0.0, 0.0);

  m_plD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
  m_plD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
  m_plD3DDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
  m_plD3DDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
  m_plD3DDevice->SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
  m_plD3DDevice->SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
  m_plD3DDevice->SetSamplerState(3, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
  m_plD3DDevice->SetSamplerState(3, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

  m_plD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, 0);

  // Restore framebuffer
  m_plD3DDevice->SetRenderTarget(0, m_plD3DBackbufferSurf);
  m_plD3DDevice->SetDepthStencilSurface(m_plD3DDepthStencilSurfAA);
}

void CMyApp::BrightPass(void)
{
  //////////////////////////////////////////////////////////////////////////
  // Get brightest pixels
  //////////////////////////////////////////////////////////////////////////

  // Setup viewport
  m_plD3DDevice->SetRenderTarget(0, m_pRTGlare->GetSurface());  
  m_plD3DDevice->SetDepthStencilSurface(m_plD3DDepthStencilSurf);
  SetViewport(m_pRTGlare->GetWidth(), m_pRTGlare->GetHeight());

  // Setup shader
  m_pSHGlareMap.SetShader();    

  float s1=0.5f/(float) m_pRTBackground->GetWidth();        
  float t1=0.5f/(float) m_pRTBackground->GetHeight();         
  float pfOffset0[]={ s1, t1, s1, t1};          

  // Set texel offsets, for correct texel to pixel mapping
  m_pSHGlareMap.SetVertexParam("vOffset01", pfOffset0, 1);
  m_pSHGlareMap.SetVertexParam("vOffset02", pfOffset0, 1);

  float pGlareParams[]= { 0.4f, 0.4f, 0.4f, 1.0f };  
  m_pSHGlareMap.SetFragmentParam("vParams",pGlareParams, 1); 

  m_plD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
  m_plD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
  m_plD3DDevice->SetTexture(0,m_pRTBackground->GetTexture()); 
  m_plD3DDevice->SetTexture(1,m_pRTGlareAmount->GetTexture()); 

  RenderScreenAlignedQuad();

  m_plD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
  m_plD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);  

  // restore backbuffer
  m_plD3DDevice->SetRenderTarget(0, m_plD3DBackbufferSurf);  
  m_plD3DDevice->SetDepthStencilSurface(m_plD3DDepthStencilSurfAA);
  SetViewport(m_pRTGlare->GetWidth(), m_pRTGlare->GetHeight());

  BlurTexture(m_pRTGlare, 5);                     
}

void CMyApp::FinalComposition(void)
{
  //////////////////////////////////////////////////////////////////////////    
  // Final composition
  //////////////////////////////////////////////////////////////////////////
               
  // Restore viewport
  SetViewport(m_iWidth, m_iHeight);

  // Set texel offsets, for correct texel to pixel mapping
  float s1=0.5f/(float) m_pRTBackground->GetWidth();     
  float t1=0.5f/(float) m_pRTBackground->GetHeight();         
  float s2=0.5f/(float) m_pRTGlare->GetWidth();     
  float t2=0.5f/(float) m_pRTGlare->GetHeight();         
  float pfOffset0[]={ s1, t1, s2, t2};          
  float pfOffset1[]={ 0, 0, 0, 0};          

  m_pSHGlareFinal.SetShader();   

  // Set texel offsets, for correct texel to pixel mapping
  m_pSHGlareFinal.SetVertexParam("vOffset01", pfOffset0, 1);
  m_pSHGlareFinal.SetVertexParam("vOffset02", pfOffset1, 1);

  m_plD3DDevice->SetTexture(0, m_pRTBackground->GetTexture());
  m_plD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
  m_plD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

  m_plD3DDevice->SetTexture(1, m_pRTGlare->GetTexture());                             

  RenderScreenAlignedQuad();

  m_plD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
  m_plD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);  
}

int CMyApp::Render(float fTimeSpan) 
{    
  m_plD3DDevice->Clear(0, 0, D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(255, 0, 0, 128), 1.0f, 0);
  if(SUCCEEDED(m_plD3DDevice->BeginScene()))
  {        
    int iBps=0;
    m_pApp->GetApp()->GetScreenInfo(m_iWidth, m_iHeight, iBps);

    RenderScene();
    
    if(m_bGlareOn)
    {            
      m_plD3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
      m_plD3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
      m_plD3DDevice->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
      m_plD3DDevice->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
      m_plD3DDevice->SetSamplerState( 2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
      m_plD3DDevice->SetSamplerState( 2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
      m_plD3DDevice->SetSamplerState( 3, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
      m_plD3DDevice->SetSamplerState( 3, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
      m_plD3DDevice->SetRenderState(D3DRS_ZENABLE, 0);

      GlareAmount();
      BrightPass();
      FinalComposition();

      m_plD3DDevice->SetRenderState(D3DRS_ZENABLE, 1);
      m_plD3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
      m_plD3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
      m_plD3DDevice->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
      m_plD3DDevice->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
      m_plD3DDevice->SetSamplerState( 2, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
      m_plD3DDevice->SetSamplerState( 2, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
      m_plD3DDevice->SetSamplerState( 3, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
      m_plD3DDevice->SetSamplerState( 3, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);      
    }
                      
    m_plD3DDevice->EndScene();
    m_plD3DDevice->Present(NULL, NULL, NULL, NULL);    
  }    
    
  m_iFrameCounter++;
  // Update each second
  if(m_pTimer->GetCurrTime()>=1.0f) 
  {    
    m_pApp->SetCaption("%s (FPS=%d)" , m_pApp->GetAppName(), m_iFrameCounter);
    m_pTimer->Reset();    
    m_iFrameCounter=0;
  }

  return APP_OK;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) 
{  
  CMyApp pMyApp;
  if(APP_FAILED(pMyApp.Create(hInstance, "Adaptive glare", 1024, 768, 32, 0, 0, 0, 1)))
  {
    return APP_ERR_INITFAIL;
  }

  pMyApp.Run();

  pMyApp.Release();
  return APP_OK;
}
