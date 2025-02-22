#include "Tool_Stage_01.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Transform.h"
#include "Model.h"

CTool_Stage_01::CTool_Stage_01(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CGameObject(pDevice, pContext)
{
}

CTool_Stage_01::CTool_Stage_01(const CTool_Stage_01& Prototype)
    :
    CGameObject(Prototype)
{
}

HRESULT CTool_Stage_01::InitializePrototype()
{
    return S_OK;
}

HRESULT CTool_Stage_01::Initialize(void* pArg)
{
	if (__super::Initialize(pArg))
		return E_FAIL;

	if (FAILED(ReadyComponents()))
		return E_FAIL;


	return S_OK;
}

void CTool_Stage_01::PriorityUpdate(_float fTimeDelta)
{
	// m_pTransformCom->Rotation(_vector{ 0.f, 1.f, 0.f, 0.f }, XM_PI / 2.f);
	// m_pTransformCom->Rotation(_vector{ 1.f, 0.f, 0.f, 0.f }, XM_PI / 2.f);
	// +XM_PI / 20.f
}

void CTool_Stage_01::Update(_float fTimeDelta)
{
}

void CTool_Stage_01::LateUpdate(_float fTimeDelta)
{
    m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CTool_Stage_01::Render()
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

HRESULT CTool_Stage_01::ReadyComponents()
{
	// Com_Shader
	{
		if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_00_Shader_VtxMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;
	}

	// Com_Model
	{
		if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Stage01"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CTool_Stage_01::BindShaderResources()
{
	if (FAILED(m_pTransformCom->BindShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	_float4x4 viewMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_VIEW);
	_float4x4 projMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_PROJ);

	if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &viewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &projMatrix)))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = m_pGameInst->GetLightDesc(0);

	if (FAILED(m_pShaderCom->BindRawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->BindRawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->BindRawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->BindRawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->BindRawValue("g_vCamPosition", m_pGameInst->GetCameraPosition(), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CTool_Stage_01* CTool_Stage_01::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTool_Stage_01* pInst = new CTool_Stage_01(pDevice, pContext);

	if (FAILED(pInst->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CTool_Stage_01");
		SafeRelease(pInst);
	}

	return pInst;
}

CGameObject* CTool_Stage_01::Clone(void* pArg)
{
	CTool_Stage_01* pInst = new CTool_Stage_01(*this);

	if (FAILED(pInst->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTool_Stage_01");
		SafeRelease(pInst);
	}

	return pInst;
}

void CTool_Stage_01::Free()
{
	__super::Free();

	SafeRelease(m_pShaderCom);
	SafeRelease(m_pModelCom);
}
