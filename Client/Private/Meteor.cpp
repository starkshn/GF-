#include "pch.h"
#include "Meteor.h"
#include "GameInstance.h"
#include "Model.h"
#include "Collider.h"
#include "EffectManager.h"
#include "ParticleManager.h"

CMeteor::CMeteor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CProjectile(pDevice, pContext)
{
}

CMeteor::CMeteor(const CMeteor& Prototype)
	:
	CProjectile(Prototype)
{
}

HRESULT CMeteor::InitializePrototype()
{
	return S_OK;
}

HRESULT CMeteor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(ReadyComponents()))
		return E_FAIL;

	m_pTransformCom->SetTransformState(CTransform::STATE_RIGHT, _vector{ 1.f, 0.f, 0.f, 1.f });
	m_pTransformCom->SetTransformState(CTransform::STATE_UP, _vector{ 0.f, 1.f, 0.f, 1.f });
	m_pTransformCom->SetTransformState(CTransform::STATE_LOOK, _vector{ 0.f, 1.f, 0.f, 1.f });
	m_pTransformCom->SetTransformState(CTransform::STATE_POS, _vector{ 0.f, 0.f, 0.f, 1.f });

	m_pColliderCom->DeActiveCol();
	return S_OK;
}

void CMeteor::PriorityUpdate(_float fTimeDelta)
{

}

void CMeteor::Update(_float fTimeDelta)
{
	_vector vPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);

	if (m_bFire)
	{
		m_pColliderCom->ActiveCol();
		m_fAccTime += fTimeDelta;
		if (m_fMaxMoveTime > m_fAccTime)
		{
			vPos = vPos + XMLoadFloat3(&m_vDir) * fTimeDelta * m_pTransformCom->GetSpeed();
			m_pTransformCom->SetTransformState(CTransform::STATE_POS, vPos);
		}
		else
		{
			m_bFire = false;
			m_fAccTime = 0.f;
			DeActvie();
			m_pGameInst->DeActivePointLight(m_strLightName);
		}

		ActiveFollowLighting(fTimeDelta);
	}

	m_pGameInst->UpdatePointLightPos(m_strLightName, vPos);
	m_pColliderCom->Update(m_pTransformCom->GetWorldMatrix());
}

void CMeteor::LateUpdate(_float fTimeDelta)
{
	if (m_bActive)
	{
		if (false == m_pColliderCom->IsEndCollision())
			m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);
	}
}

HRESULT CMeteor::Render()
{
	if (m_bActive)
	{
		if (FAILED(BindShaderResources()))
			return E_FAIL;

		_uint		iNumMeshes = m_pModelCom->GetNumMeshes();
		for (_uint i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom->BindMaterial(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0))) return E_FAIL;
			m_pShaderCom->Begin(0);
			m_pModelCom->Render(i);
		}

		m_pColliderCom->Render();

	}

	return S_OK;
}

void CMeteor::Active()
{
	m_bActive = true;

	if (m_pColliderCom->IsEndCollision())
		m_pColliderCom->ResetCol();


}

void CMeteor::DeActvie()
{
	m_bActive = false;
	m_pColliderCom->DeActiveCol(); // 충돌 안하겠다.
}

void CMeteor::ActiveFollowLighting(_float fTimeDelta)
{
	static _float fAcc = 0.f;
	if (m_bFire && !m_pColliderCom->IsEndCollision())
	{
		fAcc += fTimeDelta;
		if (fAcc >= m_fActiveCycle)
		{
			fAcc = 0.f;
			_vector vPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);

			HIT_RESULT Hit = {};
			XMStoreFloat3(&Hit.vHitPos, vPos);

			Hit.vHitPos.y += 5.f;
			CEffectManager::GetInstance()->ActiveEffect(CEffectManager::FOLLOW_LIGHTING, Hit);

			Hit.vHitPos.y = 0.2f;
			CParticleManager::GetInstance()->ActiveParticle(CParticleManager::FOLLOW_LIGHTING_DUST, Hit);
		}
	}
}

void CMeteor::Fire()
{
	__super::Fire();
	m_pColliderCom->ActiveCol();
}

HRESULT CMeteor::ReadyComponents()
{
	// Com_Shader
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;
	}

	// Com_Model
	{
		if (FAILED(AddComponent(LEVEL_BOSS, TEXT("Prototype_Component_Model_BossRock"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;
	}

	// Collider
	{
		CBoundingSphere::BOUND_SPHERE_DESC		ColliderDesc{};
		ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
		ColliderDesc.fRadius = 1.5f;

		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMeteor::BindShaderResources()
{

	if (FAILED(m_pShaderCom->BindMatrix("g_WorldMatrix", m_pTransformCom->GetWorldMatrixPtr()))) return E_FAIL;

	_float4x4 viewMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_VIEW);
	_float4x4 projMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_PROJ);

	if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &viewMatrix))) return E_FAIL;
	if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &projMatrix))) return E_FAIL;

	return S_OK;
}

CMeteor* CMeteor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMeteor* pInstance = new CMeteor(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CMeteor");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject* CMeteor::Clone(void* pArg)
{
	CMeteor* pInstance = new CMeteor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMeteor");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CMeteor::Free()
{
	__super::Free();
}
