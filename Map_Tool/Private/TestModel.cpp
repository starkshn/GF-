#include "TestModel.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Transform.h"
#include "Model.h"
#include "ColBoundingBox.h"
#include "Animation.h"

#include "TestModelWeapon.h"
#include "TestModelBody.h"

CTestModel::CTestModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CContainerObject(pDevice, pContext)
{
}

CTestModel::CTestModel(const CTestModel& Prototype)
	:
	CContainerObject(Prototype)
{
}

HRESULT CTestModel::InitializePrototype()
{
	return S_OK;
}

HRESULT CTestModel::Initialize(void* pArg)
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

void CTestModel::PriorityUpdate(_float fTimeDelta)
{
	
}

void CTestModel::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	m_pBoundingBox->Update_Bouding_Box(m_pTransformCom);
	m_pColliderCom[COLL_AABB]->Update(m_pTransformCom->GetWorldMatrix());
}

void CTestModel::LateUpdate(_float fTimeDelta)
{
	__super::LateUpdate(fTimeDelta);

	CCollider* pOtherCollider = static_cast<CCollider*>(m_pGameInst->GetComponent(LEVEL_TOOL, TEXT("Layer_Tool_Player"), TEXT("Com_Collider")));
	m_pColliderCom[COLL_AABB]->Intersect(pOtherCollider);

	m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CTestModel::Render()
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

HRESULT CTestModel::ReadyComponents()
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

HRESULT CTestModel::ReadyPartObjects()
{
	CTestModelBody::TESTMODELBODY_DESC TestModelBodyDesc = {};
	TestModelBodyDesc.pParentMatrix = m_pTransformCom->GetWorldMatrixPtr();
	TestModelBodyDesc.pState = &m_iState;

	m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInst->ClonePrototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_TOOL, TEXT("Prototype_GameObject_TestModelBody"), &TestModelBodyDesc));
	if (nullptr == m_PartObjects[PART_BODY]) return E_FAIL;


	CTestModelWeapon::TESTMODELWEAPON_DESC TestModelWeaponDesc = {};
	TestModelWeaponDesc.pParentMatrix = m_pTransformCom->GetWorldMatrixPtr();
	TestModelWeaponDesc.pState = &m_iState;
	TestModelWeaponDesc.pSocketMatrix = static_cast<CTestModelBody*>(m_PartObjects[PART_BODY])->GetBoneCombiedMatrix("SWORD");

	m_PartObjects[PART_WEAPON] = static_cast<CPartObject*>(m_pGameInst->ClonePrototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_TOOL, TEXT("Prototype_GameObject_TestModelWeapon"), &TestModelWeaponDesc));
	if (nullptr == m_PartObjects[PART_WEAPON]) return E_FAIL;

	return S_OK;
}



CTestModel* CTestModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTestModel* pInst = new CTestModel(pDevice, pContext);

	if (FAILED(pInst->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CTestModel");
		SafeRelease(pInst);
	}

	return pInst;
}

CGameObject* CTestModel::Clone(void* pArg)
{
	CTestModel* pInst = new CTestModel(*this);

	if (FAILED(pInst->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTestModel");
		SafeRelease(pInst);
	}

	return pInst;
}

void CTestModel::Free()
{
	__super::Free();
}
