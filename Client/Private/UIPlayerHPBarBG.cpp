#include "pch.h"
#include "UIPlayerHPBarBG.h"
#include "GameInstance.h"
#include "VIBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"

CUIPlayerHPBarBG::CUIPlayerHPBarBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CUIElement(pDevice, pContext)
{

}

CUIPlayerHPBarBG::CUIPlayerHPBarBG(const CUIPlayerHPBarBG& Prototype)
	:
	CUIElement(Prototype)
{

}

HRESULT CUIPlayerHPBarBG::InitializePrototype()
{
	return S_OK;
}

HRESULT CUIPlayerHPBarBG::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(ReadyComponents(nullptr)))
		return E_FAIL;

	return S_OK;
}

void CUIPlayerHPBarBG::PriorityUpdate(_float fTimeDelta)
{
}

void CUIPlayerHPBarBG::Update(_float fTimeDelta)
{
}

void CUIPlayerHPBarBG::LateUpdate(_float fTimeDelta)
{
	if (IsActiveRender())
		m_pGameInst->AddRenderObject(CRenderer::RG_UI, this);
}

HRESULT CUIPlayerHPBarBG::Render()
{
	if (FAILED(BindShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(2);
	m_pVIBufferCom->BindBufferDesc();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUIPlayerHPBarBG::BindShaderResources()
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

HRESULT CUIPlayerHPBarBG::ReadyComponents(void* pArg)
{
	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex_UI"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_PlayerHPBarBG"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

CUIPlayerHPBarBG* CUIPlayerHPBarBG::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPlayerHPBarBG* pInst = new CUIPlayerHPBarBG(pDevice, pContext);

	if (FAILED(pInst->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CUIPlayerHPBarBG");
		SafeRelease(pInst);
	}

	return pInst;
}

CUIPlayerHPBarBG* CUIPlayerHPBarBG::Clone(void* pArg)
{
	CUIPlayerHPBarBG* pInst = new CUIPlayerHPBarBG(*this);

	if (FAILED(pInst->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIPlayerHPBarBG");
		SafeRelease(pInst);
	}

	return pInst;
}

void CUIPlayerHPBarBG::Free()
{
	__super::Free();

	SafeRelease(m_pShaderCom);
	SafeRelease(m_pVIBufferCom);
	SafeRelease(m_pTextureCom);
}
