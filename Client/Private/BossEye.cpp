#include "pch.h"
#include "BossEye.h"
#include "GameInstance.h"
#include "Bounding.h"

CBossEye::CBossEye(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CMonsterWeaponBase(pDevice, pContext)
{

}

CBossEye::CBossEye(const CBossEye& Prototype)
	:
	CMonsterWeaponBase(Prototype)
{

}

HRESULT CBossEye::InitializePrototype()
{
	return S_OK;
}

HRESULT CBossEye::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(ReadyComponents()))
		return E_FAIL;

	return S_OK;
}

void CBossEye::PriorityUpdate(_float fTimeDelta)
{

}

void CBossEye::Update(_float fTimeDelta)
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

void CBossEye::LateUpdate(_float fTimeDelta)
{
	m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);

#ifdef _DEBUG
	// m_pGameInst->AddDebugComponent(m_pColliderCom);
#endif
}

HRESULT CBossEye::Render()
{
	// m_pColliderCom->Render();

	return S_OK;
}

HRESULT CBossEye::ReadyComponents()
{
	/* Com_Collider */
	CBoundingSphere::BOUND_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColliderDesc.fRadius = 5.f;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBossEye::BindShaderResources()
{
	return S_OK;
}

CBossEye* CBossEye::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBossEye* pInstance = new CBossEye(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CBossEye");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject* CBossEye::Clone(void* pArg)
{
	CBossEye* pInstance = new CBossEye(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBossEye");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CBossEye::Free()
{
	__super::Free();
}
