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

	if (nullptr != pArg)
	{
		EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);
		m_fMaxLifeTime = pDesc->fMaxLifeTime;
		m_bUseFrameEffect = pDesc->bUseFrameEffect;
	}
	
	return S_OK;
}

void CEffect::PriorityUpdate(_float fTimeDelta)
{
	
}

void CEffect::Update(_float fTimeDelta)
{
	if (m_bUseFrameEffect)
	{
		m_fFrame += 90.f * fTimeDelta;

		if (m_fFrame >= 90.f)
			m_fFrame = 0.f;

		ComputeViewZ();
	}
	else if (0.f != m_fMaxLifeTime)
	{
		if (IsActive())
		{
			m_fAccLifeTime += fTimeDelta;
			if (m_fAccLifeTime >= m_fMaxLifeTime)
			{
				m_fAccLifeTime = 0.f;
				DeActive();
			}
		}
	}
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

void CEffect::ForceDeActive()
{
	DeActive();
	m_fAccLifeTime = 0.f;
}

void CEffect::BillBoard()
{
	_float4* pCamPos = const_cast<_float4*>(m_pGameInst->GetCameraPosition());
	_vector vCamPos = XMLoadFloat4(pCamPos);

	_vector vLook = XMVector3Normalize(vCamPos - m_pTransformCom->GetTransformState(CTransform::STATE_POS));
	_vector vRight = XMVector3Normalize(XMVector3Cross(_vector{ 0.f, 1.f, 0.f }, vLook));
	_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

	m_pTransformCom->SetTransformState(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->SetTransformState(CTransform::STATE_UP, vUp);
	m_pTransformCom->SetTransformState(CTransform::STATE_LOOK, vLook);
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

void CEffect::Free()
{
	__super::Free();


	SafeRelease(m_pShaderCom);
	SafeRelease(m_pTextureCom);
	SafeRelease(m_pVIBufferCom);
}
