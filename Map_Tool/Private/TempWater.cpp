#include "TempWater.h"
#include "GameInstance.h"
#include "VIBuffer_TempWater.h"
#include "Shader.h"
#include "Transform.h"


CTempWater::CTempWater(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CGameObject(pDevice, pContext)
{
}

CTempWater::CTempWater(const CTempWater& Prototype)
	:
	CGameObject(Prototype)
{
}

HRESULT CTempWater::InitializePrototype()
{
	return S_OK;
}

HRESULT CTempWater::Initialize(void* pArg)
{
	if (__super::Initialize(pArg))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	const LIGHT_DESC* pLightDesc = m_pGameInst->GetLightDesc(1);

	if (nullptr != pLightDesc)
	{
		m_vLightDir = pLightDesc->vDirection;
		m_vLightDiffuse = pLightDesc->vDiffuse;
		m_vLightAmbient = pLightDesc->vAmbient;
		m_vLightSpecular = pLightDesc->vSpecular;
	}


	return S_OK;
}

void CTempWater::PriorityUpdate(_float fTimeDelta)
{
}

void CTempWater::Update(_float fTimeDelta)
{
	m_pVIBufferCom->UpdateYPos(fTimeDelta, 0.2f);

	m_pTransformCom->SetTransformState(CTransform::STATE_POS, _vector{ -50.f, 0.f, 0.f, 1.f });
}

void CTempWater::LateUpdate(_float fTimeDelta)
{
	m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CTempWater::Render()
{
	if(FAILED(Bind_ShaderResources()))
		return E_FAIL;

	// 0 default, 1 wireframe
	m_pShaderCom->Begin(0);

	m_pVIBufferCom->BindBufferDesc();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CTempWater::Ready_Components()
{
	// Com_Shader
	{
		if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxNorTex_TempWater"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;
	}

	// Com_VIBuffer
	{
		if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_TempWater"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
			return E_FAIL;
	}

	// Com_Texture
	{
		if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Water"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}

	return S_OK;
}

static _float fTest = 0.1f;


HRESULT CTempWater::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->BindShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	_float4x4 viewMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_VIEW);
	_float4x4 projMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_PROJ);

	if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &viewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &projMatrix)))
		return E_FAIL;

	// Y값 조절 (임시)
	{
		fTest += 7.f;

		if (FAILED(m_pShaderCom->Bind_Val("g_DT", 0.002f)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Val("g_SP", fTest)))
			return E_FAIL;
	}
	

	// 빛 방향, Diffuse, Ambient, Specular 설정, 카메라 위치 설정
	{
		if (FAILED(m_pShaderCom->BindRawValue("g_vLightDir", &m_vLightDir, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->BindRawValue("g_vLightDiffuse", &m_vLightDiffuse, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->BindRawValue("g_vLightAmbient", &m_vLightAmbient, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->BindRawValue("g_vLightSpecular", &m_vLightSpecular, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->BindRawValue("g_P", &m_fP, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->BindRawValue("g_vCamPosition", m_pGameInst->GetCameraPosition(), sizeof(_float4))))
			return E_FAIL;
	}


	if (FAILED(m_pTextureCom->BindShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	return S_OK;
}

void CTempWater::Reset_Vtx_Cnt(_int iCntX, _int iCntZ)
{
	m_pVIBufferCom->ResetVtxCnt(iCntX, iCntZ);
}

CTempWater* CTempWater::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTempWater* pInst = new CTempWater(pDevice, pContext);

	if (FAILED(pInst->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CTempWater");
		SafeRelease(pInst);
	}

	return pInst;
}

CGameObject* CTempWater::Clone(void* pArg)
{
	CTempWater* pInst = new CTempWater(*this);

	if (FAILED(pInst->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTempWater");
		SafeRelease(pInst);
	}

	return pInst;
}

void CTempWater::Free()
{
	__super::Free();

	SafeRelease(m_pShaderCom);
	SafeRelease(m_pVIBufferCom);
	SafeRelease(m_pTextureCom);
}
