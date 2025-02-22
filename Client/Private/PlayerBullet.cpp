#include "pch.h"
#include "PlayerBullet.h"
#include "GameInstance.h"


CPlayerBullet::CPlayerBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CBullet(pDevice, pContext)
{

}

CPlayerBullet::CPlayerBullet(const CPlayerBullet& Prototype)
	:
	CBullet(Prototype)
{

}

HRESULT CPlayerBullet::InitializePrototype()
{
	return S_OK;
}

HRESULT CPlayerBullet::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(ReadyComponents()))
		return E_FAIL;

	return S_OK;
}

void CPlayerBullet::PriorityUpdate(_float fTimeDelta)
{

}

void CPlayerBullet::Update(_float fTimeDelta)
{
	if (m_bFire)
	{
		if (MoveProjectile(fTimeDelta))
		{

		}
	}
	
	m_pColliderCom->Update(m_pTransformCom->GetWorldMatrix());
}

void CPlayerBullet::LateUpdate(_float fTimeDelta)
{
	m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);
	CheckColWall();
	CheckColMonster();
}

HRESULT CPlayerBullet::Render()
{
	// m_pColliderCom->Render();
	return S_OK;
}

HRESULT CPlayerBullet::ReadyComponents()
{
	/* Com_Collider */
	CBoundingSphere::BOUND_SPHERE_DESC		ColliderDesc{};
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColliderDesc.fRadius = 0.15f;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerBullet::BindShaderResources()
{
	return S_OK;
}

CPlayerBullet* CPlayerBullet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayerBullet* pInstance = new CPlayerBullet(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CPlayerBullet");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayerBullet::Clone(void* pArg)
{
	CPlayerBullet* pInstance = new CPlayerBullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))	
	{
		MSG_BOX("Failed to Cloned : CPlayerBullet");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CPlayerBullet::Free()
{
	__super::Free();
}
