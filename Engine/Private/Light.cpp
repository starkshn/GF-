#include "Light.h"
#include "Shader.h"
#include "VIBufferRect.h"

CLight::CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: 
	m_pDevice{ pDevice },
	m_pContext{ pContext }
{
	SafeAddRef(m_pDevice);
	SafeAddRef(m_pContext);
}

HRESULT CLight::Initialize(const LIGHT_DESC& LightDesc)
{
	m_LightDesc = LightDesc;

	return S_OK;
}

HRESULT CLight::Render(CShader* pShader, CVIBufferRect* pVIBuffer)
{
	_uint iPassIndex = 1;

	if (LIGHT_DESC::TYPE_DIRECTIONAL == m_LightDesc.eType)
	{
		if (FAILED(pShader->BindRawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof _float4)))
			return E_FAIL;

		iPassIndex = 1;
	}
	else if (LIGHT_DESC::TYPE_POINT == m_LightDesc.eType)
	{
		if (FAILED(pShader->BindRawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof _float4)))
			return E_FAIL;
		
		if (FAILED(pShader->BindRawValue("g_fLightRange", &m_LightDesc.fRange, sizeof _float)))
			return E_FAIL;

		iPassIndex = 2;
	}

	if (FAILED(pShader->BindRawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof _float4)))
		return E_FAIL;
	
	if (FAILED(pShader->BindRawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof _float4)))
		return E_FAIL;

	if (FAILED(pShader->BindRawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof _float4)))
		return E_FAIL;

	if (FAILED(pShader->Begin(iPassIndex)))
		return E_FAIL;

	return pVIBuffer->Render();;
}

CLight* CLight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHT_DESC& LightDesc)
{
	CLight* pInstance = new CLight(pDevice, pContext);

	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		MSG_BOX("Failed to Created : CLight");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
	__super::Free();

	SafeRelease(m_pDevice);
	SafeRelease(m_pContext);
}
