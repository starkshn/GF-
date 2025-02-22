#include "pch.h"
#include "UIPlayerTex.h"
#include "GameInstance.h"
#include "VIBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"

CUIPlayerTex::CUIPlayerTex(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CUIElement(pDevice, pContext)
{
}

CUIPlayerTex::CUIPlayerTex(const CUIPlayerTex& Prototype)
	:
	CUIElement(Prototype)
{
}

HRESULT CUIPlayerTex::InitializePrototype()
{
	return S_OK;
}

HRESULT CUIPlayerTex::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(ReadyComponents(nullptr)))
		return E_FAIL;

	return S_OK;
}

void CUIPlayerTex::PriorityUpdate(_float fTimeDelta)
{
}

void CUIPlayerTex::Update(_float fTimeDelta)
{
}

void CUIPlayerTex::LateUpdate(_float fTimeDelta)
{
	if (IsActiveRender())
		m_pGameInst->AddRenderObject(CRenderer::RG_UI, this);
}

HRESULT CUIPlayerTex::Render()
{
	if (FAILED(BindShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->BindBufferDesc();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUIPlayerTex::BindShaderResources()
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

HRESULT CUIPlayerTex::ReadyComponents(void* pArg)
{
	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex_UI"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_PlayerTex"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

CUIPlayerTex* CUIPlayerTex::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPlayerTex* pInst = new CUIPlayerTex(pDevice, pContext);

	if (FAILED(pInst->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CUIPlayerTex");
		SafeRelease(pInst);
	}

	return pInst;
}

CUIPlayerTex* CUIPlayerTex::Clone(void* pArg)
{
	CUIPlayerTex* pInst = new CUIPlayerTex(*this);

	if (FAILED(pInst->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIPlayerTex");
		SafeRelease(pInst);
	}

	return pInst;
}

void CUIPlayerTex::Free()
{
	__super::Free();

	SafeRelease(m_pShaderCom);
	SafeRelease(m_pVIBufferCom);
	SafeRelease(m_pTextureCom);
}
