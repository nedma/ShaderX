/////////////////////////////////////////////////////////
// File : SXDevice.cpp
// Description : Direct3D Resource Management System
//				 implementation.
//				 (SXProbe) class implementation.
//				 (SXResource) class implementation.
// Author : Wessam Bahnassi, In|Framez
//
/////////////////////////////////////////////////////////

///////////////// #includes /////////////////
#include <d3d9.h>
#include "SXDevice.h"


///////////////// SXProbe Class Implementation /////////////////
SXProbe::SXProbe() :
	m_pD3DDevice(NULL),
	m_bNotifiedAboutLastLoss(false)
{
	// Initialize present parameters to something sensible
	m_paramsPresent.BackBufferWidth = 400;
	m_paramsPresent.BackBufferHeight = 300;
	m_paramsPresent.BackBufferCount = 0;
	m_paramsPresent.MultiSampleType = D3DMULTISAMPLE_NONE;
	m_paramsPresent.MultiSampleQuality = 0;
	m_paramsPresent.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_paramsPresent.hDeviceWindow = NULL;
	m_paramsPresent.Windowed = TRUE;
	m_paramsPresent.EnableAutoDepthStencil = FALSE;
	m_paramsPresent.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
	m_paramsPresent.Flags = 0;
	m_paramsPresent.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	m_paramsPresent.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
}

SXProbe::~SXProbe()
{
	if (m_pD3DDevice)
		DetachFromCurrentDevice();

	// Clear hold off all resources
	SXResourcesList::iterator itResources = m_lstResources.begin();

	while (itResources != m_lstResources.end())
	{
		SXResource *pResource = *itResources++;
		pResource->m_pOwnerProbe = NULL;
		pResource->m_itResource = NULL;
	}
	m_lstResources.clear();
}

HRESULT SXProbe::AttachToDevice(PDIRECT3DDEVICE9 pDevice,D3DPRESENT_PARAMETERS *pPresentParams)
{
	if (!pDevice)
		return E_POINTER;

	HRESULT hRetval;
	if (m_pD3DDevice)
	{
		hRetval = DetachFromCurrentDevice();
		if (FAILED(hRetval))
			return hRetval;
	}

	// Save present params for restoration purposes
	if (pPresentParams)
		m_paramsPresent = *pPresentParams;
	else
	{
		// Present parameters not provided, get them from the device. Note that this
		// works fine even thought the device is in lost state...
		PDIRECT3DSWAPCHAIN9 pSwapChain = NULL;
		hRetval = pDevice->GetSwapChain(0,&pSwapChain);
		if (FAILED(hRetval))
			return hRetval;
		if (!pSwapChain)
			return E_POINTER;
		hRetval = pSwapChain->GetPresentParameters(&m_paramsPresent);
		SXSAFE_RELEASE(pSwapChain);
		if (FAILED(hRetval))
			return hRetval;
	}

	// Own it
	m_pD3DDevice = pDevice;
	m_pD3DDevice->AddRef();

	// If it's not operational, then don't notify resources about it
	hRetval = m_pD3DDevice->TestCooperativeLevel();
	if (FAILED(hRetval))
		return S_OK;

	// Notify the resources about the new device
	return NotifyRestore();
}

HRESULT SXProbe::DetachFromCurrentDevice(void)
{
	if (!m_pD3DDevice)
		return D3DERR_INVALIDCALL;

	NotifyRelease();
	SXSAFE_RELEASE(m_pD3DDevice);

	return S_OK;
}

HRESULT SXProbe::ProbeDevice(void)
{
	if (!m_pD3DDevice)
		return D3DERR_INVALIDCALL;

	HRESULT hRetval = m_pD3DDevice->TestCooperativeLevel();
	if (SUCCEEDED(hRetval))
		return hRetval;

	// We can't do much if the device isn't available to us
	if (hRetval != D3DERR_DEVICENOTRESET)
		return hRetval;

	return ResetDevice(NULL);
}

HRESULT SXProbe::ResetDevice(D3DPRESENT_PARAMETERS *pPresentParams)
{
	if (!m_pD3DDevice)
		return D3DERR_INVALIDCALL;

	// We're about to reset, make sure all D3DPOOL_DEFAULT resources
	// are released first...
	NotifyLoss();

	// Attempt to restore device
	if (pPresentParams)
		m_paramsPresent = *pPresentParams;

	HRESULT hRetval = m_pD3DDevice->Reset(&m_paramsPresent);
	if (FAILED(hRetval))
		return hRetval;

	m_bNotifiedAboutLastLoss = false;

	hRetval = NotifyRestore();
	if (FAILED(hRetval))
	{
		// If we arrived here, then one (or more) resource must have failed
		// restoring its data. It's totally up to you to decide what to do
		// in such case... Here, we just fail the function with the error code
		// returned from NotifyRestore()...
		SXTRACE("SXProbe WARN: One or more resources failed to restore!\n");
		return hRetval;
	}

	SXTRACE("SXProbe INFO: Resources restored successfully...\n");
	return S_OK;
}

HRESULT SXProbe::AddResource(SXResource *pResource)
{
	if (!pResource)
	{
		SXTRACE("SXProbe ERROR: Invalid resource pointer passed to AddResource()\n");
		return E_POINTER;
	}

	if (pResource->m_pOwnerProbe)
	{
		if (pResource->m_pOwnerProbe != this)
		{
			SXTRACE("SXProbe ERROR: Attempting to add a resource managed by another SXProbe's control\n");
			return E_INVALIDARG;	// Baaad boy, trying to host a resource owned by another probe...
		}
		return S_OK;	// Already added to our list
	}

	pResource->m_pOwnerProbe = this;
	m_lstResources.push_back(pResource);
	pResource->m_itResource = --m_lstResources.end();
	return S_OK;
}

HRESULT SXProbe::RemoveResource(SXResource *pResource)
{
	if (!pResource)
	{
		SXTRACE("SXProbe ERROR: Invalid resource pointer passed to RemoveResource()\n");
		return E_POINTER;
	}

	if (pResource->m_pOwnerProbe != this)
	{
		SXTRACE("SXProbe ERROR: Attempting to remove a resource not managed by this SXProbe\n");
		return E_INVALIDARG;	// We don't own that resource baby...
	}

	m_lstResources.erase(pResource->m_itResource);
	pResource->m_pOwnerProbe = NULL;
	pResource->m_itResource = NULL;
	return S_OK;
}

HRESULT SXProbe::GetD3DDevice(PDIRECT3DDEVICE9 *ppOutD3DDevice)
{
	if (!ppOutD3DDevice)
		return E_POINTER;
	if (!m_pD3DDevice)
		return D3DERR_INVALIDCALL;
	*ppOutD3DDevice = m_pD3DDevice;
	m_pD3DDevice->AddRef();
	return S_OK;
}

HRESULT SXProbe::NotifyLoss(void)
{
	if (m_bNotifiedAboutLastLoss)
		return S_OK;	// Skip if resources are already notified...

	SXTRACE("SXProbe INFO: Notifying resources about device loss\n");

	// Iterate registered resources
	HRESULT hRetval = S_OK;
	SXResourcesList::iterator itResources = m_lstResources.begin();
	while (itResources != m_lstResources.end())
	{
		SXResource *pResource = *itResources;
		hRetval = pResource->OnDeviceLoss();

		// You can handle notification failure here if you're interested...
		//if (FAILED(hRetval))
		//{
			// A warning sent to the debug output maybe?
		//}
		++itResources;
	}

	return hRetval;
}

HRESULT SXProbe::NotifyRelease(void)
{
	SXTRACE("SXProbe INFO: Notifying resources about device release\n");

	// Iterate registered resources
	HRESULT hRetval = S_OK;
	SXResourcesList::iterator itResources = m_lstResources.begin();
	while (itResources != m_lstResources.end())
	{
		SXResource *pResource = *itResources;
		hRetval = pResource->OnDeviceRelease();

		// You can handle notification failure here if you're interested...
		//if (FAILED(hRetval))
		//{
			// A warning sent to the debug output maybe?
		//}
		++itResources;
	}

	return hRetval;
}

HRESULT SXProbe::NotifyRestore(void)
{
	SXTRACE("SXProbe INFO: Notifying resources about device restore\n");

	// Iterate registered resources
	HRESULT hRetval = S_OK;
	SXResourcesList::iterator itResources = m_lstResources.begin();
	while (itResources != m_lstResources.end())
	{
		SXResource *pResource = *itResources;
		hRetval = pResource->OnDeviceRestore();

		// You can handle notification failure here if you're interested...
		//if (FAILED(hRetval))
		//{
			// A warning sent to the debug output maybe?
		//}
		++itResources;
	}

	return hRetval;
}


///////////////// SXResource Class Implementation /////////////////
SXResource::SXResource() :
	m_pOwnerProbe(NULL),
	m_itResource(NULL)
{
}

SXResource::~SXResource()
{
	if (m_pOwnerProbe)
		m_pOwnerProbe->RemoveResource(this);
}

HRESULT SXResource::OnDeviceLoss(void)
{
	// This method is called when the resource
	// is required to release all of its internal
	// objects and data that are residing at video
	// memory (i.e. D3DPOOL_DEFAULT).
	// The resource must keep internal data required
	// to restore it back again upon the next
	// call to OnDeviceRestore() member function.
	return S_OK;
}

HRESULT SXResource::OnDeviceRelease(void)
{
	// This method is called when the resource
	// is required to release all of its internal
	// Direct3D objects that are either residing
	// at video memory or ultimately allocated
	// through the IDirect3DDevice interface (pretty
	// covers all Direct3D objects but IDirect3D and
	// IDirect3DDevice).
	// The resource must keep internal data required
	// to restore it back again upon the next
	// call to OnDeviceRestore().
	return S_OK;
}

HRESULT SXResource::OnDeviceRestore(void)
{
	// This method is called when the resource
	// needs to be restored to its suitable memory pool..
	// When the resource is requested to be removed
	// from video memory or even release all of the
	// its internal objects that where allocated through
	// IDirect3DDevice, when that is the case,
	// a call to this function must restore the resource
	// back to fully operational state.
	return S_OK;
}

///////////////// End of File : SXDevice.cpp /////////////////