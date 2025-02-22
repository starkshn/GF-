#include "Tool_Player.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Transform.h"
#include "Model.h"
#include "ColBoundingBox.h"
#include "Animation.h"
#include "ToolPlayerBody.h"
#include "ToolPlayerWeapon.h"

CTool_Player::CTool_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CContainerObject(pDevice, pContext)
{
}

CTool_Player::CTool_Player(const CTool_Player& Prototype)
	:
	CContainerObject(Prototype)
{
}

HRESULT CTool_Player::InitializePrototype()
{
	return S_OK;
}

HRESULT CTool_Player::Initialize(void* pArg)
{
	CONTAINER_OBJECT_DESC Desc = {};
	Desc.iNumPartObjects = PART_END;
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotSpeedPerSec = XMConvertToRadians(90.f);

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(ReadyComponents()))
		return E_FAIL;

	if (FAILED(ReadyPartObjects()))
		return E_FAIL;


	m_pTransformCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(m_pGameInst->ComputeRandom(0.f, 10.f), 0.f, m_pGameInst->ComputeRandom(0.f, 10.f), 1.f));

	return S_OK;
}

void CTool_Player::PriorityUpdate(_float fTimeDelta)
{

}

void CTool_Player::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_pGameInst->MouseDown(MOUSE_KEY::LB))
	{
		_float4 vPickingPos;
		if (m_pGameInst->IsPicked(&vPickingPos))
		{
			m_pTransformCom->SetTransformState(CTransform::STATE_POS, XMLoadFloat4(&vPickingPos));
		}
	}

	if (m_pGameInst->KeyPressing(KEY::W))
	{
		m_pTransformCom->GoFoward(fTimeDelta);
	}

	if (m_pGameInst->KeyPressing(KEY::S))
	{
		m_pTransformCom->GoBackward(fTimeDelta);
	}

	if (m_pGameInst->KeyPressing(KEY::A))
	{
		m_pTransformCom->GoLeft(fTimeDelta);
	}

	if (m_pGameInst->KeyPressing(KEY::D))
	{
		m_pTransformCom->GoRight(fTimeDelta);
	}

	


	m_pBoundingBox->Update_Bouding_Box(m_pTransformCom);
	m_pColliderCom[COLL_AABB]->Update(m_pTransformCom->GetWorldMatrix());
}

void CTool_Player::LateUpdate(_float fTimeDelta)
{
	__super::LateUpdate(fTimeDelta);

	CCollider* pOtherCollider = static_cast<CCollider*>(m_pGameInst->GetComponent(LEVEL_TOOL, TEXT("Layer_Tool_Player"), TEXT("Com_Collider")));
	m_pColliderCom[COLL_AABB]->Intersect(pOtherCollider);

	m_pGameInst->AddRenderObject(CRenderer::RG_NONPICKING, this);
	// m_pGameInst->AddRenderObject(CRenderer::RG_NONPICKING, this);

#ifdef _DEBUG
	m_pGameInst->AddDebugComponent(m_pColliderCom[COLL_AABB]);
#endif

	// 후처리 지형 타기
	/*_vector		vWorldPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);
	m_pTransformCom->SetTransformState(CTransform::STATE_POS, XMVectorSetY(vWorldPos, m_pGameInst->ComputeHeight(vWorldPos)));*/
}

HRESULT CTool_Player::Render()
{
	// 바운딩 박스 렌더
	{
		_float4x4 viewMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_VIEW);
		_float4x4 projMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_PROJ);

		if (FAILED(m_pBoundingBox->Render_Bounding_Box(m_pTransformCom, XMLoadFloat4x4(&viewMatrix), XMLoadFloat4x4(&projMatrix))))
			return E_FAIL;
	}

	m_pColliderCom[COLL_AABB]->Render();

	return S_OK;
}

HRESULT CTool_Player::ReadyComponents()
{
	// Com_BoudingBox
	{
		if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_BoundingBox"), TEXT("Com_BoundingBox"), reinterpret_cast<CComponent**>(&m_pBoundingBox))))
			return E_FAIL;
	}

	{
		/* Com_Collider */
		CBoundingAABB::BOUND_AABB_DESC		ColliderDesc{};
		ColliderDesc.vExtents = _float3(0.3f, 0.7f, 0.3f);
		ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

		if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Collider_AABB"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom[COLL_AABB]), &ColliderDesc)))
			return E_FAIL;

		return S_OK;
	}

	return S_OK;
}

HRESULT CTool_Player::ReadyPartObjects()
{
	CToolPlayerBody::TOOL_PLAYER_BODY_DESC BodyDesc = {};
	BodyDesc.pParentMatrix = m_pTransformCom->GetWorldMatrixPtr();
	BodyDesc.pState = &m_iState;

	m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInst->ClonePrototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_TOOL, TEXT("Prototype_GameObject_ToolPlayerBody"), &BodyDesc));
	if (nullptr == m_PartObjects[PART_BODY]) return E_FAIL;


	CToolPlayerWeapon::TOOL_PLAYER_WEAPON_DESC	WeaponDesc = {};
	WeaponDesc.pParentMatrix = m_pTransformCom->GetWorldMatrixPtr();
	WeaponDesc.pState = &m_iState;
	WeaponDesc.pSocketMatrix = static_cast<CToolPlayerBody*>(m_PartObjects[PART_BODY])->GetBoneCombiedMatrix("SWORD");

	m_PartObjects[PART_WEAPON] = static_cast<CPartObject*>(m_pGameInst->ClonePrototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_TOOL, TEXT("Prototype_GameObject_ToolPlayerWeapon"), &WeaponDesc));
	if (nullptr == m_PartObjects[PART_WEAPON]) return E_FAIL;

	return S_OK;
}


CTool_Player* CTool_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTool_Player* pInst = new CTool_Player(pDevice, pContext);

	if (FAILED(pInst->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CTool_Player");
		SafeRelease(pInst);
	}

	return pInst;
}

CGameObject* CTool_Player::Clone(void* pArg)
{
	CTool_Player* pInst = new CTool_Player(*this);

	if (FAILED(pInst->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTool_Player");
		SafeRelease(pInst);
	}

	return pInst;
}

void CTool_Player::Free()
{
	__super::Free();
}
