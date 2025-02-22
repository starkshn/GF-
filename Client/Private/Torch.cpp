#include "pch.h"
#include "Torch.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Transform.h"
#include "Model.h"
#include "Navigation.h"

CTorch::CTorch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CGameObject(pDevice, pContext)
{
}

CTorch::CTorch(const CTorch& Prototype)
	:
	CGameObject(Prototype)
{
}

HRESULT CTorch::InitializePrototype()
{
	return S_OK;
}

HRESULT CTorch::Initialize(void* pArg)
{
	if (__super::Initialize(pArg))
		return E_FAIL;

	if (FAILED(ReadyComponents()))
		return E_FAIL;

	_vector vCurPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);
	vCurPos.m128_f32[1] -= 1.25f;
	m_pTransformCom->SetTransformState(CTransform::STATE_POS, vCurPos);
	m_pTransformCom->Scaling(0.1f, 0.1f, 0.1f);

	return S_OK;
}

void CTorch::PriorityUpdate(_float fTimeDelta)
{
	// m_pNavMesh->Update();
}

void CTorch::Update(_float fTimeDelta)
{
}

void CTorch::LateUpdate(_float fTimeDelta)
{
	m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CTorch::Render()
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

HRESULT CTorch::ReadyComponents()
{
	// Com_Shader
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;
	}

	// Com_Model
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Model_Torch"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CTorch::BindShaderResources()
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

CTorch* CTorch::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTorch* pInst = new CTorch(pDevice, pContext);

	if (FAILED(pInst->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CTorch");
		SafeRelease(pInst);
	}

	return pInst;
}

CGameObject* CTorch::Clone(void* pArg)
{
	CTorch* pInst = new CTorch(*this);

	if (FAILED(pInst->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTorch");
		SafeRelease(pInst);
	}

	return pInst;
}

void CTorch::Free()
{
	__super::Free();

	SafeRelease(m_pShaderCom);
	SafeRelease(m_pModelCom);
}
