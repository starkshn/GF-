#include "pch.h"
#include "Particle.h"

CParticle::CParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CGameObject(pDevice, pContext)
{
}

CParticle::CParticle(const CParticle& Prototype)
	:
	CGameObject(Prototype)
{
}

HRESULT CParticle::InitializePrototype()
{
	return S_OK;
}

HRESULT CParticle::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CParticle::PriorityUpdate(_float fTimeDelta)
{

}

void CParticle::Update(_float fTimeDelta)
{

}

void CParticle::LateUpdate(_float fTimeDelta)
{

}

HRESULT CParticle::Render()
{
	return S_OK;
}

void CParticle::Activate()
{
	if (nullptr != m_pVIBufferCom)
	{
		if (false == IsActivate() && false == m_pVIBufferCom->IsActivate())
		{
			m_bActivate = true;
			m_pVIBufferCom->Activate();
		}
	}
}

void CParticle::ResetParticles()
{
	if (nullptr != m_pVIBufferCom)
		m_pVIBufferCom->ResetParticles();
}

void CParticle::UpdateParticleWorldMatrix(_fmatrix WorldMatrix)
{
	m_pTransformCom->SetTransformState(CTransform::STATE_RIGHT, XMVector3Normalize((WorldMatrix).r[0]));
	m_pTransformCom->SetTransformState(CTransform::STATE_UP, XMVector3Normalize((WorldMatrix).r[1]));
	m_pTransformCom->SetTransformState(CTransform::STATE_LOOK, -XMVector3Normalize((WorldMatrix).r[2]));
	m_pTransformCom->SetTransformState(CTransform::STATE_POS, (WorldMatrix).r[3]);
}

void CParticle::UpdateParticleWorldPos(_fvector vWorldPos)
{
	m_pTransformCom->SetTransformState(CTransform::STATE_POS, vWorldPos);
}

void CParticle::SetPivot(_fvector vPivot)
{
	if (nullptr != m_pVIBufferCom)
		m_pVIBufferCom->SetPivot(vPivot);
}

void CParticle::Free()
{
	__super::Free();

	if (nullptr != m_pTextureCom)
		SafeRelease(m_pTextureCom);

	if (nullptr != m_pShaderCom)
		SafeRelease(m_pShaderCom);

	if (nullptr != m_pVIBufferCom)
		SafeRelease(m_pVIBufferCom);

}
