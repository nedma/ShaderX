// ENG-RenderSettings.cpp: implementation of the CRenderSettings class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ENG-RenderSettings.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRenderSettings::CRenderSettings() {
    RenderID = 0;
    RenderPass = 0;
    CameraSource.x = 0.0;
    CameraSource.y = 0.0;
    CameraSource.z = -150;
    CameraTarget.x = 0;
    CameraTarget.y = 0;
    CameraTarget.z = 0;
    CameraUpVector.x = 0;
    CameraUpVector.y = 1;
    CameraUpVector.z = 0;
    CameraFieldOfView = D3DX_PI / 2.0f;
    CameraAspectRatio = 4.0f / 3.0f;
    CameraZNear = (float)0.001;
    CameraZFar = 1000;
    GeometryDetail = 1.0;
    TextureDetail = 1.0;
    RenderTime = 0.0;
    PrevRenderTime = 0.0;
    TimesDelta = 0.0;
    Flags = 0;
}

CRenderSettings::~CRenderSettings() {
}

void CRenderSettings::ComputeViewMatrix() {
    D3DXMatrixLookAtLH(&ViewMatrix, &CameraSource, &CameraTarget, &CameraUpVector);
}

void CRenderSettings::ComputeProjectionMatrix() {
    D3DXMatrixPerspectiveFovLH(&ProjectionMatrix, CameraFieldOfView, CameraAspectRatio, CameraZNear, CameraZFar);
}

void CRenderSettings::ComputeCameraMatrix() {
    ComputeViewMatrix();
    ComputeProjectionMatrix();
    D3DXMatrixMultiply(&CameraTransformMatrix, &ViewMatrix, &ProjectionMatrix);
}

void CRenderSettings::Clone(CRenderSettings &Original) {
    memcpy(this,&Original, sizeof(CRenderSettings));
}