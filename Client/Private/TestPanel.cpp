#include "pch.h"
#include "TestPanel.h"
#include "GameInstance.h"
#include "VIBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"

CTestPanel::CTestPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CUIPanel(pDevice, pContext)
{
}

CTestPanel::CTestPanel(const CTestPanel& Prototype)
	:
	CUIPanel(Prototype)
{
}

HRESULT CTestPanel::InitializePrototype()
{
	return S_OK;
}

HRESULT CTestPanel::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(ReadyComponents(nullptr)))
		return E_FAIL;

	return S_OK;
}

void CTestPanel::PriorityUpdate(_float fTimeDelta)
{
}

void CTestPanel::Update(_float fTimeDelta)
{
}

void CTestPanel::LateUpdate(_float fTimeDelta)
{
	m_pGameInst->AddRenderObject(CRenderer::RG_UI, this);
}

HRESULT CTestPanel::Render()
{
	if (FAILED(BindShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->BindBufferDesc();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CTestPanel::BindShaderResources()
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

HRESULT CTestPanel::ReadyComponents(void* pArg)
{
	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_TestPanel"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

CTestPanel* CTestPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTestPanel* pInst = new CTestPanel(pDevice, pContext);

	if (FAILED(pInst->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CTestPanel");
		SafeRelease(pInst);
	}

	return pInst;
}

CTestPanel* CTestPanel::Clone(void* pArg)
{
	CTestPanel* pInst = new CTestPanel(*this);

	if (FAILED(pInst->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTestPanel");
		SafeRelease(pInst);
	}

	return pInst;
}

void CTestPanel::Free()
{
	__super::Free();

	SafeRelease(m_pShaderCom);
	SafeRelease(m_pVIBufferCom);
	SafeRelease(m_pTextureCom);
}
