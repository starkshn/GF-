#include "pch.h"
#include "UIHPVertical.h"
#include "GameInstance.h"
#include "VIBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"
#include "GlobalFuncs.h"

CUIHPVertical::CUIHPVertical(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CUIElement(pDevice, pContext)
{

}

CUIHPVertical::CUIHPVertical(const CUIHPVertical& Prototype)
	:
	CUIElement(Prototype)
{

}

HRESULT CUIHPVertical::InitializePrototype()
{
	return S_OK;
}

HRESULT CUIHPVertical::Initialize(void* pArg)
{
	if (!pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(ReadyComponents(nullptr)))
		return E_FAIL;

	return S_OK;
}

void CUIHPVertical::PriorityUpdate(_float fTimeDelta)
{
}

void CUIHPVertical::Update(_float fTimeDelta)
{
	if (IsActiveRender())
	{
		if (m_bOnce)
		{
			m_fAcc += fTimeDelta;
			if (m_fAcc > m_fScalingTimeMax / 2.f)
			{
				m_bOnce = 0;
			}

			m_fAccReal = m_fAcc * 10.f;
		}
		else
		{
			m_fAcc -= fTimeDelta;
			if (m_fAcc < 0.f)
			{
				m_bOnce = 1;
				m_fAcc = 0.f;
				DeActiveRender();
			}

			m_fAccReal = m_fAcc * 10.f;
		}

		m_pTransformCom->Scaling(5.f + m_fAcc * 100.f, 20.f + m_fAcc * 200.f, 1.f);
		cout << "ºñÀ² ¤Ó " << m_fAccReal << endl;
	}
}

void CUIHPVertical::LateUpdate(_float fTimeDelta)
{
	if (IsActiveRender())
		m_pGameInst->AddRenderObject(CRenderer::RG_UI, this);
}

HRESULT CUIHPVertical::Render()
{
	if (IsActiveRender())
	{
		if (FAILED(BindShaderResources()))
			return E_FAIL;

		m_pShaderCom->Begin(10);
		m_pVIBufferCom->BindBufferDesc();
		m_pVIBufferCom->Render();
	}

	return S_OK;
}

HRESULT CUIHPVertical::BindShaderResources()
{
	if (FAILED(m_pTransformCom->BindShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->BindShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIHPVertical::ReadyComponents(void* pArg)
{
	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex_UI"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_VerticalHP"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->BindRawValue("g_fAlphaAccVertical", &m_fAccReal, sizeof _float)))
		return E_FAIL;
	// g_AlphaAccVertical

	return S_OK;
}

CUIHPVertical* CUIHPVertical::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIHPVertical* pInst = new CUIHPVertical(pDevice, pContext);

	if (FAILED(pInst->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CUIHPVertical");
		SafeRelease(pInst);
	}

	return pInst;
}

CUIHPVertical* CUIHPVertical::Clone(void* pArg)
{
	CUIHPVertical* pInst = new CUIHPVertical(*this);

	if (FAILED(pInst->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIHPVertical");
		SafeRelease(pInst);
	}

	return pInst;
}

void CUIHPVertical::Free()
{
	__super::Free();

	SafeRelease(m_pShaderCom);
	SafeRelease(m_pVIBufferCom);
	SafeRelease(m_pTextureCom);
}
