///////////////////////////////////////////////////////////////////////
// File : SXEffect.h
// Description : D3DX Effects Parameter Manipulation Class
//				 (SXEffect) class declaration.
//				 (SXEffVariant) class declaration.
//				 (SXEffParam) class declaration.
// Author : Wessam Bahnassi, In|Framez
//
///////////////////////////////////////////////////////////////////////

#ifndef _SXEFFECT_H
#define _SXEFFECT_H

///////////////// #includes /////////////////
#include <hash_map>	// Using VS.NET 2003 here, change these to suit your compiler
#include <string>


//////////////////////////////// Forward References ////////////////////////////////
class SXEffAnimation;
class SXEffVariant;
class SXEffParam;
class SXEffect;


//////////////////////////////// Type Defines ////////////////////////////////
typedef stdext::hash_map<int,SXEffVariant*> SXEffVariantElemHash;
typedef stdext::hash_map<std::string,SXEffVariant*> SXEffVariantsHash;
typedef stdext::hash_map<std::string,SXEffParam*> SXEffParamsHash;


//////////////////////////////// SXEffAnimation Class Declaration ////////////////////////////////
class SXEffAnimation
{
protected:
	friend class SXEffect;
	friend class SXEffVariant;

	SXEffAnimation(const SXEffVariant& varValue,bool bColor);
	~SXEffAnimation();
	static void LerpFloat1(SXEffAnimation& animThis,SXEffVariant& varOwner);
	static void LerpFloat2(SXEffAnimation& animThis,SXEffVariant& varOwner);
	static void LerpFloat3(SXEffAnimation& animThis,SXEffVariant& varOwner);
	static void LerpFloat4(SXEffAnimation& animThis,SXEffVariant& varOwner);
	static void LerpInteger(SXEffAnimation& animThis,SXEffVariant& varOwner);
	static void LerpColor(SXEffAnimation& animThis,SXEffVariant& varOwner);
	void Update(float fTime,SXEffVariant& varOwner);

	typedef void (*LERPFUNC)(SXEffAnimation&,SXEffVariant&);
	LERPFUNC m_pfnLerper;
	float m_fWeight;
	D3DXVECTOR4 m_vec4Key1;
	D3DXVECTOR4 m_vec4Key2;
};


//////////////////////////////// SXEffVariant Class Declaration ////////////////////////////////
class SXEffVariant
{
public:
	// Type Checking
	bool IsValid(void) const;
	bool IsA(D3DXPARAMETER_TYPE eParamType) const;
	bool IsA(D3DXPARAMETER_CLASS eParamClass) const;
	D3DXPARAMETER_TYPE GetType(void) const;
	D3DXPARAMETER_CLASS GetClass(void) const;

	// Matrix/Array Accessors
	float mat(int iCol,int iRow);
	SXEffVariant& operator[](int iIndex);	// Arrays only
	UINT GetElemsCount(void) const;	// Arrays only
	UINT GetVectorSize(void) const;	// Vectors only

	// Value Get
	operator bool(void);
	operator int(void);
	operator float(void);
	operator D3DXVECTOR2(void);
	operator D3DXVECTOR3(void);
	operator D3DXVECTOR4(void);
	operator D3DXMATRIX(void);
	operator PCSTR(void);

	operator PDIRECT3DVERTEXSHADER9 (void);
	operator PDIRECT3DPIXELSHADER9 (void);
	operator PDIRECT3DBASETEXTURE9 (void);

	// Value Set
	SXEffVariant& operator= (const SXEffVariant& varVal);
	SXEffVariant& operator= (bool bVal);
	SXEffVariant& operator= (int iVal);
	SXEffVariant& operator= (float fVal);
	SXEffVariant& operator= (const D3DXVECTOR2& vec2Val);
	SXEffVariant& operator= (const D3DXVECTOR3& vec3Val);
	SXEffVariant& operator= (const D3DXVECTOR4& vec4Val);
	SXEffVariant& operator= (const D3DXMATRIX& matVal);
	SXEffVariant& operator= (PCSTR pszVal);
	SXEffVariant& operator= (PDIRECT3DVERTEXSHADER9 pVal);
	SXEffVariant& operator= (PDIRECT3DPIXELSHADER9 pVal);
	SXEffVariant& operator= (PDIRECT3DBASETEXTURE9 pVal);

	// Animation
	void CancelAnimation(void);
	void Animate(float fKey1,float fKey2);
	void Animate(const D3DXVECTOR2& vec2Key1,const D3DXVECTOR2& vec2Key2);
	void Animate(const D3DXVECTOR3& vec3Key1,const D3DXVECTOR3& vec3Key2);
	void Animate(const D3DXVECTOR4& vec4Key1,const D3DXVECTOR4& vec4Key2);
	void Animate(int iKey1,int iKey2);
	void AnimateAsColor(D3DCOLOR clrKey1,D3DCOLOR clrKey2);

protected:
	friend class SXEffect;
	friend class SXEffParam;

	// Construction/Destruction
	SXEffVariant(SXEffect *pOwnerEffect,D3DXHANDLE hParam);
	~SXEffVariant();

	// Internal Properties
	SXEffect *m_pOwnerEffect;
	D3DXHANDLE m_hParam;
	SXEffVariantElemHash m_hashElems;	// Array elements
	SXEffAnimation *m_pAnim;
};


//////////////////////////////// SXEffParam Class Declaration ////////////////////////////////
class SXEffParam : public SXEffVariant
{
public:
	// Operations
	SXEffParam& operator () (PCSTR pszParamName);
	SXEffVariant& GetPassAnnotation(PCSTR pszPassName,PCSTR pszAnnName);
	SXEffVariant& Annotation(PCSTR pszAnnName);

	// Value Set
	SXEffVariant& operator= (const SXEffVariant& varVal);
	SXEffVariant& operator= (bool bVal);
	SXEffVariant& operator= (int iVal);
	SXEffVariant& operator= (float fVal);
	SXEffVariant& operator= (const D3DXVECTOR2& vec2Val);
	SXEffVariant& operator= (const D3DXVECTOR3& vec3Val);
	SXEffVariant& operator= (const D3DXVECTOR4& vec4Val);
	SXEffVariant& operator= (const D3DXMATRIX& matVal);
	SXEffVariant& operator= (PCSTR pszVal);
	SXEffVariant& operator= (PDIRECT3DVERTEXSHADER9 pVal);
	SXEffVariant& operator= (PDIRECT3DPIXELSHADER9 pVal);
	SXEffVariant& operator= (PDIRECT3DBASETEXTURE9 pVal);

protected:
	friend class SXEffect;

	// Construction/Destruction
	SXEffParam(SXEffect *pOwnerEffect,D3DXHANDLE hParam);
	~SXEffParam();

	// Internal Properties
	SXEffVariantsHash m_hashAnnotations;
};


//////////////////////////////// SXEffect Class Declaration ////////////////////////////////
class SXEffect
{
public:
	// Construction/Destruction
	SXEffect();
	~SXEffect();

	// Initialization
	HRESULT CreateFromFile(PDIRECT3DDEVICE9 pDevice,PCTSTR pszFileName);
	HRESULT AttachToExisting(LPD3DXEFFECT pEffect);
	void Clear(void);

	// Operations
	SXEffParam& operator() (PCSTR pszParamName,D3DXHANDLE hParent = NULL);
	SXEffParam& GetTechnique(PCSTR pszTechniqueName);
	void SetAnimWeight(float fTime);

	// Notifications
	HRESULT OnDeviceLoss();
	HRESULT OnDeviceRestore();

	// Access
	LPD3DXEFFECT GetD3DXEffect(void) const;

protected:
	friend SXEffParam;
	friend SXEffVariant;

	// Internal Properties
	LPD3DXEFFECT m_pEffect;
	SXEffParamsHash m_hashParams;
	SXEffParam m_paramInvalid;	// Error Code
};

#include "SXEffect.inl"

#endif	// _SXEFFECT_H

////////////////// End of File : SXEffect.h //////////////////