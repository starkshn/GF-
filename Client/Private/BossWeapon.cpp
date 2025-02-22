#include "pch.h"
#include "BossWeapon.h"
#include "GameInstance.h"
#include "Bounding.h"
#include "Camera_Free.h"

CBossWeapon::CBossWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CMonsterWeaponBase(pDevice, pContext)
{

}

CBossWeapon::CBossWeapon(const CBossWeapon& Prototype)
	:
	CMonsterWeaponBase(Prototype)
{

}

HRESULT CBossWeapon::InitializePrototype()
{
	return S_OK;
}

HRESULT CBossWeapon::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(ReadyComponents()))
		return E_FAIL;

	return S_OK;
}

void CBossWeapon::PriorityUpdate(_float fTimeDelta)
{

}

void CBossWeapon::Update(_float fTimeDelta)
{
	_matrix     SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (_int i = 0; i < 3; ++i)
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	m_pTransformCom->Scaling(0.3f, 0.3f, 0.3f);
	m_pTransformCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(0.0f, 0.f, 0.f, 1.f));
	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->GetWorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));
	m_pColliderCom->Update(XMLoadFloat4x4(&m_WorldMatrix));
	
	// Update Boss Rock World Pos
	m_pTransformCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(1.5f, 3.5f, 0.f, 1.f));
	XMStoreFloat4x4(&m_BossHandMatrix, m_pTransformCom->GetWorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

	// Update Boss Smash World Pos
	m_pTransformCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(1.5f, 3.5f, 0.f, 1.f));
	XMStoreFloat4x4(&m_BossSmashMatrix, m_pTransformCom->GetWorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));
}

void CBossWeapon::LateUpdate(_float fTimeDelta)
{
	static _float fAcc = 0.f;
	static _bool bCool = true;

	auto pObj = m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_BOSS, TEXT("A"));
	if (pObj)
	{
		auto pCom = static_cast<CCollider*>(pObj->GetComponent(TEXT("Com_Collider")));

		if (m_pColliderCom->Intersect(pCom))
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(pCom->GetComOwner());
			if (pPlayer)
			{
				pPlayer->SetDamage(20);

				if (bCool)
				{
					auto pCam = m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_BOSS, TEXT("B"));
					if (pCam)
						static_cast<CCamera_Free*>(pCam)->SetCameraEffectType(CCamera_Free::EFFECT_SHAKE);

					bCool = false;
				}
			}
		}
	}

	m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);

#ifdef _DEBUG
	// m_pGameInst->AddDebugComponent(m_pColliderCom);
#endif


	if (!bCool)
	{
		fAcc += fTimeDelta;
		if (fAcc > 3.f)
		{
			fAcc = 0.f;
			bCool = true;
		}
	}
}

HRESULT CBossWeapon::Render()
{
	// m_pColliderCom->Render();

	return S_OK;
}

HRESULT CBossWeapon::ReadyComponents()
{
	/* Com_Collider */
	CBoundingSphere::BOUND_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColliderDesc.fRadius = 30.f;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBossWeapon::BindShaderResources()
{
	return S_OK;
}

CBossWeapon* CBossWeapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBossWeapon* pInstance = new CBossWeapon(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CBossWeapon");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject* CBossWeapon::Clone(void* pArg)
{
	CBossWeapon* pInstance = new CBossWeapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBossWeapon");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CBossWeapon::Free()
{
	__super::Free();
}
