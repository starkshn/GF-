#include "pch.h"
#include "UIPlayerInfoBG.h"
#include "GameInstance.h"
#include "VIBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"

CUIPlayerInfoBG::CUIPlayerInfoBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CUIPanel(pDevice, pContext)
{
}

CUIPlayerInfoBG::CUIPlayerInfoBG(const CUIPlayerInfoBG& Prototype)
	:
	CUIPanel(Prototype)
{
}

HRESULT CUIPlayerInfoBG::InitializePrototype()
{
	return S_OK;
}

HRESULT CUIPlayerInfoBG::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(ReadyComponents(nullptr)))
		return E_FAIL;

	return S_OK;
}

void CUIPlayerInfoBG::PriorityUpdate(_float fTimeDelta)
{
}

void CUIPlayerInfoBG::Update(_float fTimeDelta)
{
}

void CUIPlayerInfoBG::LateUpdate(_float fTimeDelta)
{
	if (IsActiveRender())
		m_pGameInst->AddRenderObject(CRenderer::RG_UI, this);
}

HRESULT CUIPlayerInfoBG::Render()
{
	if (FAILED(BindShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);
	m_pVIBufferCom->BindBufferDesc();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUIPlayerInfoBG::BindShaderResources()
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

HRESULT CUIPlayerInfoBG::ReadyComponents(void* pArg)
{
	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex_UI"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_PlayerInfoBG"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

CUIPlayerInfoBG* CUIPlayerInfoBG::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPlayerInfoBG* pInst = new CUIPlayerInfoBG(pDevice, pContext);

	if (FAILED(pInst->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CUIPlayerInfoBG");
		SafeRelease(pInst);
	}

	return pInst;
}

CUIPlayerInfoBG* CUIPlayerInfoBG::Clone(void* pArg)
{
	CUIPlayerInfoBG* pInst = new CUIPlayerInfoBG(*this);

	if (FAILED(pInst->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIPlayerInfoBG");
		SafeRelease(pInst);
	}

	return pInst;
}

void CUIPlayerInfoBG::Free()
{
	__super::Free();

	SafeRelease(m_pShaderCom);
	SafeRelease(m_pVIBufferCom);
	SafeRelease(m_pTextureCom);
}
