#include "pch.h"
#include "MonsterWeapon06.h"
#include "GameInstance.h"
#include "GameObject.h"

CMonsterWeapon06::CMonsterWeapon06(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CMonsterWeaponBase(pDevice, pContext)
{

}

CMonsterWeapon06::CMonsterWeapon06(const CMonsterWeapon06& Prototype)
	:
	CMonsterWeaponBase(Prototype)
{

}

HRESULT CMonsterWeapon06::InitializePrototype()
{
	return S_OK;
}

HRESULT CMonsterWeapon06::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(ReadyComponents()))
		return E_FAIL;

	return S_OK;
}

void CMonsterWeapon06::PriorityUpdate(_float fTimeDelta)
{

}

void CMonsterWeapon06::Update(_float fTimeDelta)
{
	_matrix     SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (_int i = 0; i < 3; ++i)
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	m_pTransformCom->Scaling(0.1f, 0.1f, 0.1f);
	// m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
	m_pTransformCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(0.0f, -0.15f, -2.0f, 1.f));

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->GetWorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));


	m_pColliderCom->Update(XMLoadFloat4x4(&m_WorldMatrix));
}

void CMonsterWeapon06::LateUpdate(_float fTimeDelta)
{
	if (m_pColliderCom->Intersect(m_pOtherCol))
	{
		auto pOwner = m_pOtherCol->GetComOwner();
		if (pOwner)
		{
			if ("Player" == pOwner->GetObjectTag())
			{
				auto pPlayer = static_cast<CPlayer*>(pOwner);
				pPlayer->SetDamage(10);
			}
		}
	}

	m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);

#ifdef _DEBUG
	//m_pGameInst->AddDebugComponent(m_pColliderCom);
#endif
}

HRESULT CMonsterWeapon06::Render()
{
	//m_pColliderCom->Render();

	return S_OK;
}

HRESULT CMonsterWeapon06::ReadyComponents()
{
	/* Com_Collider */
	CBoundingOBB::BOUND_OBB_DESC		ColliderDesc{};

	ColliderDesc.vExtents = _float3(0.6f, 1.3f, 8.f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);
	ColliderDesc.vRotation = _float3(XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f));

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterWeapon06::BindShaderResources()
{
	return S_OK;
}

CMonsterWeapon06* CMonsterWeapon06::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonsterWeapon06* pInstance = new CMonsterWeapon06(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CMonsterWeapon06");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject* CMonsterWeapon06::Clone(void* pArg)
{
	CMonsterWeapon06* pInstance = new CMonsterWeapon06(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonsterWeapon06");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CMonsterWeapon06::Free()
{
	__super::Free();
}
