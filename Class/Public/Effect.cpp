#include "pch.h"
#include "Effect.h"
#include "GameInstance.h"


CEffect::CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: 
	CAlphaObject(pDevice, pContext)
{
}

CEffect::CEffect(const CEffect& Prototype)
	:
	CAlphaObject(Prototype)
{
}

HRESULT CEffect::InitializePrototype()
{
	return S_OK;
}

HRESULT CEffect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(ReadyComponents()))
		return E_FAIL;

	m_pTransformCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(m_pGameInst->ComputeRandom(0.f, 20.f), 2.f, m_pGameInst->ComputeRandom(0.f, 20.f), 1.f));
	

	return S_OK;
}

void CEffect::PriorityUpdate(_float fTimeDelta)
{
	
}

void CEffect::Update(_float fTimeDelta)
{
	m_fFrame += 90.f * fTimeDelta;
	
	if (m_fFrame >= 90.f)
		m_fFrame = 0.f;

	ComputeViewZ();
}

void CEffect::LateUpdate(_float fTimeDelta)
{
	m_pGameInst->AddRenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CEffect::Render()
{
	if (FAILED(BindShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->BindBufferDesc();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CEffect::ReadyComponents()
{
	/* Com_Shader */
	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(AddComponent(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect::BindShaderResources()
{
	if (FAILED(m_pTransformCom->BindShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	_float4x4 viewMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_VIEW);
	_float4x4 projMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_PROJ);

	if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &viewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &projMatrix)))
		return E_FAIL;


	if (FAILED(m_pTextureCom->BindShaderResource(m_pShaderCom, "g_Texture", static_cast<_uint>(m_fFrame))))
		return E_FAIL;

	if (FAILED(m_pGameInst->BindRTShaderResource(m_pShaderCom, "g_DepthTexture", TEXT("TargetDepth"))))
		return E_FAIL;

	return S_OK;
}

CEffect* CEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect* pInstance = new CEffect(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CEffect");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect::Clone(void* pArg)
{
	CEffect* pInstance = new CEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffect");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CEffect::Free()
{
	__super::Free();


	SafeRelease(m_pShaderCom);
	SafeRelease(m_pTextureCom);
	SafeRelease(m_pVIBufferCom);
}
