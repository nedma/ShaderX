// ENG-RenderSettings.h: interface for the CRenderSettings class.
//
//////////////////////////////////////////////////////////////////////

//TODO: vyhodit nepotrebne flagy

#pragma once

#include "RootClass.h"
#include "nommgr.h"
#include "d3dx9.h"
#include "mmgr.h"

//############################################################################
//Forward declarations
class CEngine;

//flags of render settings
const UINT D3RS_TIMEOVERLEAPPED    =1;  //if RenderTime reached maximum and is starting from zero.
const UINT D3RS_LIGHTISENABLED     =2;  //are lights enabled?
const UINT D3RS_SHADOWSENABLED     =4;  //are shadows enabled?
const UINT D3RS_TEXTURESENABLED    =8;  //are textures enabled?
const UINT D3RS_FORCEOBJECTCOLOR   =16; //object is forced to change it's color
const UINT D3RS_SHADOWSENABLED2    =D3RS_SHADOWSENABLED | 32; //shadows2 enabled? - Shadows2 do nothing more than storing pixel's position in world space in the map.
const UINT D3RS_NOSHADOWPOSTPROCESS=64; //Skip shadow map postprocessing

class CRenderSettings : public CRootClass {
public:
    //Engine we are using
    CEngine             *Engine;
    //device, we use for rendering.
    IDirect3DDevice9    *Direct3DDevice;
    
    //Render ID. This ID is unique within every frame and objects should check 
    //it if they don't want to be rendered multiple times (typical example is 
    //when object lies in two portal cells, both are visible and so both calls
    //render on object, but object is rendered only once).
    UINT        RenderID;

    //Pass number
    UINT        RenderPass;      //Index of render pass within this frame.

    //scene camera
    D3DXVECTOR3 CameraSource;           //Camera source position point
    D3DXVECTOR3 CameraTarget;           //Camera target position point
    D3DXVECTOR3 CameraUpVector;         //Up vector for camera

    //    D3DXVECTOR3 CameraSource;           //Camera source position point
    //    D3DXVECTOR3 CameraTarget;           //Camera target position point
    //    D3DXVECTOR3 CameraUpVector;         //Up vector for camera
    float       CameraFieldOfView;      //Field of View
    float       CameraAspectRatio;      //Aspect ratio
    float       CameraZNear;            //Z Near of projection
    float       CameraZFar;             //Z far of projection
    D3DXMATRIX  CameraTransformMatrix; //transformation matrix for this frame (view * projection matrix)
    D3DXMATRIX  ViewMatrix;             //View (world to camera space) matrix
    D3DXMATRIX  ProjectionMatrix;       //projection matrix.

    //detail levels (1< simpler mode; =1 normal, unmodified mode; >1 more detail)
    float       GeometryDetail;  //Detail of geometry
    float       TextureDetail;   //Detail of textures

    //engine timing
    double      RenderTime;      //Current time point to render
    double      PrevRenderTime;  //Last time point , that was rendered
    double      TimesDelta;      //Diference between time points

    //Flags
    UINT        Flags;

    //Type of render
    UINT        RenderType;

    CRenderSettings();
    virtual ~CRenderSettings();

    void ComputeViewMatrix();
    void ComputeProjectionMatrix();
    void ComputeCameraMatrix();

    void Clone(CRenderSettings &Original);
};
