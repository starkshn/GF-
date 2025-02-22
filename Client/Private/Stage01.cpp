#include "pch.h"
#include "Stage01.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Transform.h"
#include "Model.h"
#include "Level_Loading.h"
#include "LevelBoss.h"

CStage01::CStage01(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CGameObject(pDevice, pContext)
{
}

CStage01::CStage01(const CStage01& Prototype)
    :
    CGameObject(Prototype)
{
}

HRESULT CStage01::InitializePrototype()
{
    return S_OK;
}

HRESULT CStage01::Initialize(void* pArg)
{
	if (__super::Initialize(pArg))
		return E_FAIL;

	if (FAILED(ReadyComponents()))
		return E_FAIL;

	return S_OK;
}

void CStage01::PriorityUpdate(_float fTimeDelta)
{
	
}

void CStage01::Update(_float fTimeDelta)
{
}

void CStage01::LateUpdate(_float fTimeDelta)
{
    m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CStage01::Render()
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

	return S_OK;
}

HRESULT CStage01::ReadyComponents()
{
	// Com_Shader
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;
	}

	// Com_Model
	{
		if (FAILED(AddComponent(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Stage01"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CStage01::BindShaderResources()
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

CStage01* CStage01::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStage01* pInst = new CStage01(pDevice, pContext);

	if (FAILED(pInst->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CStage01");
		SafeRelease(pInst);
	}

	return pInst;
}

CGameObject* CStage01::Clone(void* pArg)
{
	CStage01* pInst = new CStage01(*this);

	if (FAILED(pInst->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CStage01");
		SafeRelease(pInst);
	}

	return pInst;
}

void CStage01::Free()
{
	__super::Free();

	SafeRelease(m_pShaderCom);
	SafeRelease(m_pModelCom);
}
