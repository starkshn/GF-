#include "ToolPlayerWeapon.h"
#include "GameInstance.h"
#include "TestModel.h"
#include "Model.h"

CToolPlayerWeapon::CToolPlayerWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CPartObject(pDevice, pContext)
{
}

CToolPlayerWeapon::CToolPlayerWeapon(const CToolPlayerWeapon& Prototype)
	:
	CPartObject(Prototype)
{
}

HRESULT CToolPlayerWeapon::InitializePrototype()
{
	return S_OK;
}

HRESULT CToolPlayerWeapon::Initialize(void* pArg)
{
	TOOL_PLAYER_WEAPON_DESC* pDesc = static_cast<TOOL_PLAYER_WEAPON_DESC*>(pArg);

	m_pState = pDesc->pState;
	m_pSocketMatrix = pDesc->pSocketMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(ReadyComponents()))
		return E_FAIL;

	return S_OK;
}

void CToolPlayerWeapon::PriorityUpdate(_float fTimeDelta)
{
}

void CToolPlayerWeapon::Update(_float fTimeDelta)
{
	_matrix     SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (_int i = 0; i < 3; ++i)
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	m_pTransformCom->Scaling(0.1f, 0.1f, 0.1f);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
	m_pTransformCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(0.6f, 0.f, 0.f, 1.f));

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->GetWorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));


	m_pColliderCom->Update(XMLoadFloat4x4(&m_WorldMatrix));
}

void CToolPlayerWeapon::LateUpdate(_float fTimeDelta)
{
	CCollider* pOtherCollider = static_cast<CCollider*>(m_pGameInst->GetComponent(LEVEL_TOOL, TEXT("Layer_Tool_Player"), TEXT("Com_Collider")));
	m_pColliderCom->Intersect(pOtherCollider);

	m_pGameInst->AddRenderObject(CRenderer::RG_NONPICKING, this);
	m_pGameInst->AddRenderObject(CRenderer::RG_SHADOW, this);

#ifdef _DEBUG
	m_pGameInst->AddDebugComponent(m_pColliderCom);
#endif
}

HRESULT CToolPlayerWeapon::Render()
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

	return S_OK;
}

HRESULT CToolPlayerWeapon::RenderShadow()
{
	if (FAILED(m_pShaderCom->BindMatrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", m_pGameInst->GetShadowTransform(CShadow::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", m_pGameInst->GetShadowTransform(CShadow::D3DTS_PROJ))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->GetNumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->BindMaterial(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;
		if (FAILED(m_pModelCom->BindBoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		m_pShaderCom->Begin(2);
		m_pModelCom->Render(i);
	}


	return S_OK;
}

HRESULT CToolPlayerWeapon::ReadyComponents()
{
	/* Com_Shader */
	if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_00_Shader_VtxMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Model_ForkLift"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom)))) 
		return E_FAIL;


	/* Com_Collider */
	CBoundingOBB::BOUND_OBB_DESC		ColliderDesc{};

	ColliderDesc.vExtents = _float3(0.6f, 1.3f, 2.f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);
	ColliderDesc.vRotation = _float3(XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f));

	if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolPlayerWeapon::BindShaderResources()
{
	if (FAILED(m_pShaderCom->BindMatrix("g_WorldMatrix", &m_WorldMatrix))) return E_FAIL;

	_float4x4 viewMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_VIEW);
	_float4x4 projMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_PROJ);

	if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &viewMatrix))) return E_FAIL;
	if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &projMatrix))) return E_FAIL;

	return S_OK;
}

CToolPlayerWeapon* CToolPlayerWeapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CToolPlayerWeapon* pInstance = new CToolPlayerWeapon(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CToolPlayerWeapon");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject* CToolPlayerWeapon::Clone(void* pArg)
{
	CToolPlayerWeapon* pInstance = new CToolPlayerWeapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CToolPlayerWeapon");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CToolPlayerWeapon::Free()
{
	__super::Free();

	SafeRelease(m_pShaderCom);
	SafeRelease(m_pModelCom);
}
