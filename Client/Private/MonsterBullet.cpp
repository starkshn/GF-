#include "pch.h"
#include "MonsterBullet.h"
#include "GameInstance.h"


CMonsterBullet::CMonsterBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CBullet(pDevice, pContext)
{

}

CMonsterBullet::CMonsterBullet(const CMonsterBullet& Prototype)
	:
	CBullet(Prototype)
{

}

HRESULT CMonsterBullet::InitializePrototype()
{
	return S_OK;
}

HRESULT CMonsterBullet::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(ReadyComponents()))
		return E_FAIL;

	return S_OK;
}

void CMonsterBullet::PriorityUpdate(_float fTimeDelta)
{

}

void CMonsterBullet::Update(_float fTimeDelta)
{
	if (m_bFire)
	{
		if (MoveProjectile(fTimeDelta))
		{
			
		}
	}

	m_pColliderCom->Update(m_pTransformCom->GetWorldMatrix());
}

void CMonsterBullet::LateUpdate(_float fTimeDelta)
{
	m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);
	CheckColWall();
	CheckColPlayer();
}

HRESULT CMonsterBullet::Render()
{
#ifdef _DEBUG
	m_pColliderCom->Render();
#endif
	return S_OK;
}

HRESULT CMonsterBullet::ReadyComponents()
{
	/* Com_Collider */
	CBoundingSphere::BOUND_SPHERE_DESC		ColliderDesc{};
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColliderDesc.fRadius = 0.15f;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterBullet::BindShaderResources()
{
	return S_OK;
}

CMonsterBullet* CMonsterBullet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonsterBullet* pInstance = new CMonsterBullet(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CMonsterBullet");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject* CMonsterBullet::Clone(void* pArg)
{
	CMonsterBullet* pInstance = new CMonsterBullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonsterBullet");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CMonsterBullet::Free()
{
	__super::Free();
}
