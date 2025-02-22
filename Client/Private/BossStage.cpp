#include "pch.h"
#include "BossStage.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Transform.h"
#include "Model.h"
#include "Navigation.h"

CBossStage::CBossStage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CGameObject(pDevice, pContext)
{
}

CBossStage::CBossStage(const CBossStage& Prototype)
	:
	CGameObject(Prototype)
{
}

HRESULT CBossStage::InitializePrototype()
{
	return S_OK;
}

HRESULT CBossStage::Initialize(void* pArg)
{
	if (__super::Initialize(pArg))
		return E_FAIL;

	if (FAILED(ReadyComponents()))
		return E_FAIL;

	m_pTransformCom->SetTransformState(CTransform::STATE_POS, _fvector{ 7.f, 0.f, 28.f, 1.f });


	return S_OK;
}

void CBossStage::PriorityUpdate(_float fTimeDelta)
{
	// m_pNavMesh->Update();
}

void CBossStage::Update(_float fTimeDelta)
{
}

void CBossStage::LateUpdate(_float fTimeDelta)
{
	m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CBossStage::Render()
{
	if (FAILED(BindShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->GetNumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		// i 번째에 해당하는 텍스쳐를 Bind 해준다.
		if (FAILED(m_pModelCom->BindMaterial(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		// 0 default, 1 wireframe
		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	// m_pNavMesh->Render();

	return S_OK;
}

HRESULT CBossStage::ReadyComponents()
{
	// Com_Shader
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;
	}

	// Com_Model
	{
		if (FAILED(AddComponent(LEVEL_BOSS, TEXT("Prototype_Component_Model_BossStage"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;
	}

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Navigation_Boss"), TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavMesh), nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBossStage::BindShaderResources()
{
	if (FAILED(m_pTransformCom->BindShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	_float4x4 viewMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_VIEW);
	_float4x4 projMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_PROJ);

	if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &viewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &projMatrix)))
		return E_FAIL;

	return S_OK;
}

CBossStage* CBossStage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBossStage* pInst = new CBossStage(pDevice, pContext);

	if (FAILED(pInst->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CBossStage");
		SafeRelease(pInst);
	}

	return pInst;
}

CGameObject* CBossStage::Clone(void* pArg)
{
	CBossStage* pInst = new CBossStage(*this);

	if (FAILED(pInst->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBossStage");
		SafeRelease(pInst);
	}

	return pInst;
}

void CBossStage::Free()
{
	__super::Free();

	SafeRelease(m_pShaderCom);
	SafeRelease(m_pModelCom);
}
