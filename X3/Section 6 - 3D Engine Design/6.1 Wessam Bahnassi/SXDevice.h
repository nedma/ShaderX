/////////////////////////////////////////////////////////
// File : SXDevice.h
// Description : Direct3D Resource Management System
//				 interface declaration.
//				 (SXProbe) class declaration.
//				 (SXResource) class declaration.
// Author : Wessam Bahnassi, In|Framez
//
/////////////////////////////////////////////////////////

///////////////// #includes /////////////////
#include <list>	// Using VS.NET 2003 here. Change this to suit your compiler


///////////////// #defines /////////////////
#define SXSAFE_RELEASE(ptr) {if(ptr){(ptr)->Release();(ptr)=NULL;}}
#define SXSAFE_DELETE(ptr) {if(ptr){delete(ptr);(ptr)=NULL;}}
#define SXSAFE_DELETE_ARRAY(ptr) {if(ptr){delete[](ptr);(ptr)=NULL;}}
//#ifdef _DEBUG
	#define SXTRACE(text) OutputDebugString(text)
//#else
//	#define SXTRACE(text) __noop
//#endif // _DEBUG


///////////////// SXProbe Class Declaration /////////////////
class SXResource;	// Forward Reference
typedef std::list<SXResource*> SXResourcesList;

class SXProbe
{
public:

	// Construction/Destruction
	SXProbe();
	~SXProbe();

	// Device Management
	HRESULT AttachToDevice(PDIRECT3DDEVICE9 pDevice,D3DPRESENT_PARAMETERS *pPresentParams = NULL);
	HRESULT DetachFromCurrentDevice(void);
	HRESULT ProbeDevice(void);
	HRESULT ResetDevice(D3DPRESENT_PARAMETERS *pPresentParams = NULL);

	// Resource Management
	HRESULT AddResource(SXResource *pResource);
	HRESULT RemoveResource(SXResource *pResource);

	// Access
	PDIRECT3DDEVICE9 GetD3DDevice(void);	// Fast Access
	HRESULT GetD3DDevice(PDIRECT3DDEVICE9 *ppOutD3DDevice);
	D3DPRESENT_PARAMETERS& GetPresentParams(void);

private:
	// Notification Functions
	HRESULT NotifyLoss(void);
	HRESULT NotifyRelease(void);
	HRESULT NotifyRestore(void);

	// Internal Members
	PDIRECT3DDEVICE9 m_pD3DDevice;	// Reference Counted
	SXResourcesList m_lstResources;	// Registered Resources List
	D3DPRESENT_PARAMETERS m_paramsPresent;	// For restoration purposes
	bool m_bNotifiedAboutLastLoss;	// Did we notify our resources about device loss already?
};


///////////////// SXProbe Inline Functions Implementation /////////////////
inline PDIRECT3DDEVICE9 SXProbe::GetD3DDevice(void)
{
	return m_pD3DDevice;
}

inline D3DPRESENT_PARAMETERS& SXProbe::GetPresentParams(void)
{
	return m_paramsPresent;
}


///////////////// SXResource Class Declaration /////////////////
class SXResource
{
public:
	SXResource();
	virtual ~SXResource();

protected:
	friend class SXProbe;	// Open access for the probe

	// Overrides
	virtual HRESULT OnDeviceLoss(void);
	virtual HRESULT OnDeviceRelease(void);
	virtual HRESULT OnDeviceRestore(void);

	// Internal Members
	SXProbe *m_pOwnerProbe;

private:
	SXResourcesList::iterator m_itResource;	// For fast list access
};

///////////////// End of File : SXDevice.h /////////////////