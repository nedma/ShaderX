///////////////////////////////////////////////////////////////////////
// File : SXEffect.cpp
// Description : D3DX Effects Parameter Manipulation Class
//				 (SXEffect) class implementation.
//				 (SXEffVariant) class implementation.
//				 (SXEffParam) class implementation.
// Author : Wessam Bahnassi, In|Framez
//
///////////////////////////////////////////////////////////////////////

///////////////// #includes /////////////////
#include <d3dx9.h>
#include "SXEffect.h"


///////////////// Macros /////////////////
#define SXSAFE_RELEASE(ptr) {if(ptr){(ptr)->Release();(ptr)=NULL;}}
#define SXSAFE_DELETE(ptr) {if(ptr){delete(ptr);(ptr)=NULL;}}
#define SXSAFE_DELETE_ARRAY(ptr) {if(ptr){delete[](ptr);(ptr)=NULL;}}
#ifdef _DEBUG
#define SXTRACE(text) OutputDebugString(text)
#else
#define SXTRACE(text) __noop
#endif // _DEBUG

#define IMPLEMENT_ANIM(bColor) SXSAFE_DELETE(m_pAnim);	\
	m_pAnim = new SXEffAnimation(*this,bColor);	\
	if (!m_pAnim) return;	\
	if (!m_pAnim->m_pfnLerper) {SXSAFE_DELETE(m_pAnim);return;}


//////////////////////////////// SXEffAnimation Class Declaration ////////////////////////////////
SXEffAnimation::SXEffAnimation(const SXEffVariant& varValue,bool bColor) :
	m_pfnLerper(NULL),
	m_fWeight(0.0f),
	m_vec4Key1(0,0,0,0),
	m_vec4Key2(1,1,1,1)
{
	if (bColor)
	{
		if (!varValue.IsA(D3DXPT_INT))
			return;
		m_pfnLerper = LerpColor;
		return;
	}

	// Assign LERPer
	switch (varValue.GetClass())
	{
	case D3DXPC_SCALAR:
		{
			D3DXPARAMETER_TYPE paramType = varValue.GetType();
			if (paramType == D3DXPT_FLOAT)
				m_pfnLerper = LerpFloat1;
			else if (paramType == D3DXPT_INT)
				m_pfnLerper = LerpInteger;
		}
		return;

	case D3DXPC_VECTOR:
		if (!varValue.IsA(D3DXPT_FLOAT))
			return;
		else
		{
			static LERPFUNC aFuncs[] = {NULL,LerpFloat1,LerpFloat2,LerpFloat3,LerpFloat4};
			UINT uSize = varValue.GetVectorSize();
			if (uSize >= sizeof(aFuncs)/sizeof(aFuncs[0]))
				return;
			m_pfnLerper = aFuncs[uSize];
		}
	}
}

SXEffAnimation::~SXEffAnimation()
{
}

void SXEffAnimation::LerpFloat1(SXEffAnimation& animThis,SXEffVariant& varOwner)
{
	float fVal = animThis.m_vec4Key1.x + animThis.m_fWeight * (animThis.m_vec4Key2.x-animThis.m_vec4Key1.x);
	varOwner = fVal;
}

void SXEffAnimation::LerpFloat2(SXEffAnimation& animThis,SXEffVariant& varOwner)
{
	D3DXVECTOR2 vec2Val;
	varOwner = *D3DXVec2Lerp(&vec2Val,(D3DXVECTOR2*)&animThis.m_vec4Key1,(D3DXVECTOR2*)&animThis.m_vec4Key2,animThis.m_fWeight);
}

void SXEffAnimation::LerpFloat3(SXEffAnimation& animThis,SXEffVariant& varOwner)
{
	D3DXVECTOR3 vec3Val;
	varOwner = *D3DXVec3Lerp(&vec3Val,(D3DXVECTOR3*)&animThis.m_vec4Key1,(D3DXVECTOR3*)&animThis.m_vec4Key2,animThis.m_fWeight);
}

void SXEffAnimation::LerpFloat4(SXEffAnimation& animThis,SXEffVariant& varOwner)
{
	D3DXVECTOR4 vec4Val;
	varOwner = *D3DXVec4Lerp(&vec4Val,&animThis.m_vec4Key1,&animThis.m_vec4Key2,animThis.m_fWeight);
}

void SXEffAnimation::LerpInteger(SXEffAnimation& animThis,SXEffVariant& varOwner)
{
	int iVal = (int)(animThis.m_vec4Key1.x + animThis.m_fWeight * (animThis.m_vec4Key2.x-animThis.m_vec4Key1.x));
	varOwner = iVal;
}

void SXEffAnimation::LerpColor(SXEffAnimation& animThis,SXEffVariant& varOwner)
{
	D3DXCOLOR clrVal;
	D3DXColorLerp(&clrVal,(D3DXCOLOR*)&animThis.m_vec4Key1,(D3DXCOLOR*)&animThis.m_vec4Key2,animThis.m_fWeight);
	varOwner = (int)(D3DCOLOR)clrVal;
}


//////////////////////////////// SXEffVariant Class Implementation ////////////////////////////////
SXEffVariant::SXEffVariant(SXEffect *pOwnerEffect,D3DXHANDLE hParam) :
	m_pOwnerEffect(pOwnerEffect),
	m_hParam(hParam),
	m_pAnim(NULL)
{
}

SXEffVariant::~SXEffVariant()
{
	// Clear animation
	SXSAFE_DELETE(m_pAnim);

	// Clear off any variants allocated through this array variant
	SXEffVariantElemHash::const_iterator itElem = m_hashElems.begin();
	while (itElem != m_hashElems.end())
	{
		SXEffVariant *pElem = (itElem++)->second;
		SXSAFE_DELETE(pElem);
	}
	m_hashElems.clear();
}

D3DXPARAMETER_TYPE SXEffVariant::GetType(void) const
{
	D3DXPARAMETER_DESC descParam;
	HRESULT hRetval = m_pOwnerEffect->GetD3DXEffect()->GetParameterDesc(m_hParam,&descParam);
	return SUCCEEDED(hRetval)?descParam.Type:(D3DXPARAMETER_TYPE)D3DXEDT_FORCEDWORD;
}

D3DXPARAMETER_CLASS SXEffVariant::GetClass(void) const
{
	D3DXPARAMETER_DESC descParam;
	HRESULT hRetval = m_pOwnerEffect->GetD3DXEffect()->GetParameterDesc(m_hParam,&descParam);
	return SUCCEEDED(hRetval)?descParam.Class:(D3DXPARAMETER_CLASS)D3DXEDT_FORCEDWORD;
}

SXEffVariant& SXEffVariant::operator[] (int iIndex)
{
	SXEffVariantElemHash::const_iterator itElem = m_hashElems.find(iIndex);
	if (itElem != m_hashElems.end())
		return *(itElem->second);

	D3DXHANDLE hParam = m_pOwnerEffect->GetD3DXEffect()->GetParameterElement(m_hParam,iIndex);
	if (!hParam)
		return m_pOwnerEffect->m_paramInvalid;

	SXEffVariant *pRetval = new SXEffVariant(m_pOwnerEffect,hParam);
	if (!pRetval)
		return m_pOwnerEffect->m_paramInvalid;

	m_hashElems[iIndex] = pRetval;
	return *pRetval;
}

UINT SXEffVariant::GetElemsCount(void) const
{
	D3DXPARAMETER_DESC descParam;
	HRESULT hRetval = m_pOwnerEffect->GetD3DXEffect()->GetParameterDesc(m_hParam,&descParam);
	return SUCCEEDED(hRetval)?descParam.Elements:0;
}

UINT SXEffVariant::GetVectorSize(void) const
{
	D3DXPARAMETER_DESC descParam;
	HRESULT hRetval = m_pOwnerEffect->GetD3DXEffect()->GetParameterDesc(m_hParam,&descParam);
	return SUCCEEDED(hRetval)?descParam.Columns:0;
}

SXEffVariant::operator bool (void)
{
	BOOL bValue = FALSE;
	m_pOwnerEffect->GetD3DXEffect()->GetBool(m_hParam,&bValue);
	return (bValue == TRUE);
}

SXEffVariant::operator int (void)
{
	int iValue = 0;
	m_pOwnerEffect->GetD3DXEffect()->GetInt(m_hParam,&iValue);
	return iValue;
}

SXEffVariant::operator float (void)
{
	float fValue = 0.0f;
	m_pOwnerEffect->GetD3DXEffect()->GetFloat(m_hParam,&fValue);
	return fValue;
}

SXEffVariant::operator D3DXVECTOR2 (void)
{
	D3DXVECTOR2 vec2Value(0,0);
	m_pOwnerEffect->GetD3DXEffect()->GetFloatArray(m_hParam,vec2Value,2);
	return vec2Value;
}

SXEffVariant::operator D3DXVECTOR3 (void)
{
	D3DXVECTOR3 vec3Value(0,0,0);
	m_pOwnerEffect->GetD3DXEffect()->GetFloatArray(m_hParam,vec3Value,3);
	return vec3Value;
}

SXEffVariant::operator D3DXVECTOR4 (void)
{
	D3DXVECTOR4 vec4Value(0,0,0,1);
	m_pOwnerEffect->GetD3DXEffect()->GetVector(m_hParam,&vec4Value);
	return vec4Value;
}

SXEffVariant::operator D3DXMATRIX (void)
{
	D3DXMATRIX matValue(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
	m_pOwnerEffect->GetD3DXEffect()->GetMatrix(m_hParam,&matValue);
	return matValue;
}

SXEffVariant::operator PCSTR (void)
{
	PCSTR pszString = NULL;
	m_pOwnerEffect->GetD3DXEffect()->GetString(m_hParam,&pszString);
	return pszString;
}

SXEffVariant::operator PDIRECT3DVERTEXSHADER9 (void)
{
	PDIRECT3DVERTEXSHADER9 pVertexShader = NULL;
	m_pOwnerEffect->GetD3DXEffect()->GetVertexShader(m_hParam,&pVertexShader);
	if (pVertexShader)
		pVertexShader->Release();	// Decrease reference count
	return pVertexShader;
}

SXEffVariant::operator PDIRECT3DPIXELSHADER9 (void)
{
	PDIRECT3DPIXELSHADER9 pPixelShader = NULL;
	m_pOwnerEffect->GetD3DXEffect()->GetPixelShader(m_hParam,&pPixelShader);
	if (pPixelShader)
		pPixelShader->Release();	// Decrease reference count
	return pPixelShader;
}

SXEffVariant::operator PDIRECT3DBASETEXTURE9 (void)
{
	PDIRECT3DBASETEXTURE9 pTexture = NULL;
	m_pOwnerEffect->GetD3DXEffect()->GetTexture(m_hParam,&pTexture);
	if (pTexture)
		pTexture->Release();	// Decrease reference count
	return pTexture;
}

SXEffVariant& SXEffVariant::operator= (const SXEffVariant& varVal)
{
	// Copy value blindly
	LPD3DXEFFECT pEffect = m_pOwnerEffect->GetD3DXEffect();
	D3DXPARAMETER_DESC descThisParam,descThatParam;
	HRESULT hRetval = pEffect->GetParameterDesc(m_hParam,&descThisParam);
	hRetval |= pEffect->GetParameterDesc(varVal.m_hParam,&descThatParam);
	if (FAILED(hRetval))
	{
		SXTRACE("SXEffect ERROR: Failed to get value for copy\n");
		return *this;
	}

	BYTE *pData = new BYTE[descThatParam.Bytes];
	if (!pData)
		return *this;	// Out of memory

	pEffect->GetValue(varVal.m_hParam,pData,descThatParam.Bytes);
	pEffect->SetValue(m_hParam,pData,descThatParam.Bytes);
	SXSAFE_DELETE_ARRAY(pData);
	return *this;
}

void SXEffVariant::Animate(float fKey1,float fKey2)
{
	IMPLEMENT_ANIM(false);
	m_pAnim->m_vec4Key1.x = fKey1;
	m_pAnim->m_vec4Key2.x = fKey2;
}

void SXEffVariant::Animate(const D3DXVECTOR2& vec2Key1,const D3DXVECTOR2& vec2Key2)
{
	IMPLEMENT_ANIM(false);
	*((D3DXVECTOR2*)&m_pAnim->m_vec4Key1) = vec2Key1;
	*((D3DXVECTOR2*)&m_pAnim->m_vec4Key2) = vec2Key2;
}

void SXEffVariant::Animate(const D3DXVECTOR3& vec3Key1,const D3DXVECTOR3& vec3Key2)
{
	IMPLEMENT_ANIM(false);
	*((D3DXVECTOR3*)&m_pAnim->m_vec4Key1) = vec3Key1;
	*((D3DXVECTOR3*)&m_pAnim->m_vec4Key2) = vec3Key2;
}

void SXEffVariant::Animate(const D3DXVECTOR4& vec4Key1,const D3DXVECTOR4& vec4Key2)
{
	IMPLEMENT_ANIM(false);
	m_pAnim->m_vec4Key1 = vec4Key1;
	m_pAnim->m_vec4Key2 = vec4Key2;
}

void SXEffVariant::Animate(int iKey1,int iKey2)
{
	IMPLEMENT_ANIM(false);
	m_pAnim->m_vec4Key1.x = (float)iKey1;
	m_pAnim->m_vec4Key2.x = (float)iKey2;
}

void SXEffVariant::AnimateAsColor(D3DCOLOR clrKey1,D3DCOLOR clrKey2)
{
	IMPLEMENT_ANIM(true);
	*((D3DXCOLOR*)&m_pAnim->m_vec4Key1) = D3DXCOLOR(clrKey1);
	*((D3DXCOLOR*)&m_pAnim->m_vec4Key2) = D3DXCOLOR(clrKey2);
}


//////////////////////////////// SXEffParam Class Implementation ////////////////////////////////
SXEffParam::SXEffParam(SXEffect *pOwnerEffect,D3DXHANDLE hParam) : SXEffVariant(pOwnerEffect,hParam)
{
}

SXEffParam::~SXEffParam()
{
	// Clear off any annotations allocated through this parameter
	SXEffVariantsHash::const_iterator itVars = m_hashAnnotations.begin();
	while (itVars != m_hashAnnotations.end())
	{
		SXEffVariant *pVar = (itVars++)->second;
		SXSAFE_DELETE(pVar);
	}
	m_hashAnnotations.clear();
}

SXEffVariant& SXEffParam::GetPassAnnotation(PCSTR pszPassName,PCSTR pszAnnName)
{
	// Was it requested before? If so, then it's stored in our hash table
	SXEffVariantsHash::const_iterator itAnn = m_hashAnnotations.find(pszAnnName);
	if (itAnn != m_hashAnnotations.end())
		return *(itAnn->second);

	// Nope, try to get it from ID3DXEffect
	D3DXHANDLE hPass = m_pOwnerEffect->GetD3DXEffect()->GetPassByName(m_hParam,pszPassName);
	if (!hPass)
		return m_pOwnerEffect->m_paramInvalid;

	D3DXHANDLE hAnn = m_pOwnerEffect->GetD3DXEffect()->GetAnnotationByName(hPass,pszAnnName);
	if (!hAnn)
		return m_pOwnerEffect->m_paramInvalid;

	SXEffVariant *pAnn = new SXEffVariant(m_pOwnerEffect,hAnn);
	if (!pAnn)
		return m_pOwnerEffect->m_paramInvalid;

	// Push it into the hash table for faster access next time
	m_hashAnnotations[pszAnnName] = pAnn;
	return *pAnn;
}

SXEffVariant& SXEffParam::Annotation(PCSTR pszAnnName)
{
	SXEffVariantsHash::const_iterator itVariant = m_hashAnnotations.find(pszAnnName);
	if (itVariant != m_hashAnnotations.end())
		return *(itVariant->second);

	D3DXHANDLE hParam = m_pOwnerEffect->GetD3DXEffect()->GetAnnotationByName(m_hParam,pszAnnName);
	if (!hParam)
		return m_pOwnerEffect->m_paramInvalid;

	SXEffVariant *pRetval = new SXEffVariant(m_pOwnerEffect,hParam);
	if (!pRetval)
		return m_pOwnerEffect->m_paramInvalid;

	m_hashAnnotations[pszAnnName] = pRetval;
	return *pRetval;
}


//////////////////////////////// SXEffect Class Implementation ////////////////////////////////
SXEffect::SXEffect() :
	m_pEffect(NULL),
	m_paramInvalid(NULL,NULL)
{
	m_paramInvalid.m_pOwnerEffect = this;
}

SXEffect::~SXEffect()
{
	Clear();
}

HRESULT SXEffect::CreateFromFile(PDIRECT3DDEVICE9 pDevice,PCTSTR pszFileName)
{
	Clear();
	LPD3DXBUFFER pCompileErrors = NULL;
	HRESULT hRetval = D3DXCreateEffectFromFile(pDevice,
		pszFileName,	// File name
		NULL,	// Macro definitions
		NULL,	// Includes
		D3DXSHADER_DEBUG,	// Flags
		NULL,	// Effect Pool Object
		&m_pEffect,
		&pCompileErrors);

	if (pCompileErrors)
	{
		SXTRACE("SXEffect WARN: D3DX Effect Compiler said:\n");
		SXTRACE((PCTSTR)pCompileErrors->GetBufferPointer());
	}
	SXSAFE_RELEASE(pCompileErrors);
	return hRetval;
}

HRESULT SXEffect::AttachToExisting(LPD3DXEFFECT pEffect)
{
	if (!pEffect)
		return E_POINTER;
	pEffect->AddRef();
	Clear();
	m_pEffect = pEffect;
	return S_OK;
}

void SXEffect::Clear(void)
{
	// Free all cached parameters
	SXEffParamsHash::const_iterator itParams = m_hashParams.begin();
	while (itParams != m_hashParams.end())
	{
		SXEffParam *pParam = (itParams++)->second;
		SXSAFE_DELETE(pParam);
	}
	m_hashParams.clear();
	SXSAFE_RELEASE(m_pEffect);
}

SXEffParam& SXEffect::operator () (PCSTR pszParamName,D3DXHANDLE hParent)
{
	SXEffParamsHash::const_iterator itParam = m_hashParams.find(pszParamName);
	if (itParam != m_hashParams.end())
		return *(itParam->second);

	D3DXHANDLE hParam = m_pEffect->GetParameterByName(hParent,pszParamName);
	if (!hParam)
		return m_paramInvalid;

	SXEffParam *pRetval = new SXEffParam(this,hParam);
	if (!pRetval)
		return m_paramInvalid;

	m_hashParams[pszParamName] = pRetval;
	return *pRetval;
}

SXEffParam& SXEffect::GetTechnique(PCSTR pszTechniqueName)
{
	// Was it requested before? If so, then it's stored in our hash table
	SXEffParamsHash::const_iterator itTec = m_hashParams.find(pszTechniqueName);
	if (itTec != m_hashParams.end())
		return *(itTec->second);

	// Nope, try to get it from ID3DXEffect
	D3DXHANDLE hTechnique = m_pEffect->GetTechniqueByName(pszTechniqueName);
	if (!hTechnique)
		return m_paramInvalid;

	SXEffParam *pParam = new SXEffParam(this,hTechnique);
	if (!pParam)
		return m_paramInvalid;

	m_hashParams[pszTechniqueName] = pParam;
	return *pParam;
}

void SXEffect::SetAnimWeight(float fTime)
{
	// Loop on all parameters, asking them to set their time
	// to the new value which should be in the range [0,1]
	SXEffParamsHash::const_iterator itParam = m_hashParams.begin();
	while (itParam != m_hashParams.end())
	{
		SXEffParam *pParam = (itParam++)->second;
		if (pParam->m_pAnim)
			pParam->m_pAnim->Update(fTime,*pParam);
	}
}

HRESULT SXEffect::OnDeviceLoss()
{
	if (!m_pEffect)
		return S_OK;
	return m_pEffect->OnLostDevice();
}

HRESULT SXEffect::OnDeviceRestore()
{
	if (!m_pEffect)
		return S_OK;
	return m_pEffect->OnResetDevice();
}

////////////////// End of File : SXEffect.cpp //////////////////