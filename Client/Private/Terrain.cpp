#include "pch.h"
#include "Terrain.h"
#include "GameInstance.h"
#include "Navigation.h"

CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CGameObject{ pDevice, pContext }
{
}

CTerrain::CTerrain(const CTerrain& Prototype)
	:
	CGameObject(Prototype)
{
}

HRESULT CTerrain::InitializePrototype()
{
	return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CTerrain::PriorityUpdate(_float fTimeDelta)
{
	m_pNavigationCom->Update(m_pTransformCom->GetWorldMatrix());
	m_pNavigationComS2->Update(m_pTransformCom->GetWorldMatrix());
	m_pNavigationComS3->Update(m_pTransformCom->GetWorldMatrix());
}

void CTerrain::Update(_float fTimeDelta)
{

}

void CTerrain::LateUpdate(_float fTimeDelta)
{
	m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);

#ifdef _DEBUG
	// m_pGameInst->AddDebugComponent(m_pNavigationCom);
	// m_pGameInst->AddDebugComponent(m_pNavigationComS2);
	// m_pGameInst->AddDebugComponent(m_pNavigationComS3);
#endif
}

HRESULT CTerrain::Render()
{

#ifdef _DEBUG
	// m_pNavigationCom->Render();
	// m_pNavigationComS2->Render();
	// m_pNavigationComS3->Render();
#endif


	//if (FAILED(Bind_ShaderResources()))
	//	return E_FAIL;

	//m_pShaderCom->Begin(0);

	//m_pVIBufferCom->BindBufferDesc();

	//m_pVIBufferCom->Render();



	return S_OK;
}

HRESULT CTerrain::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxNorTex_Terrain_Brush"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(AddComponent(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(AddComponent(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	// Com_Texture_Mask
	if (FAILED(AddComponent(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain_Mask"), TEXT("Com_Texture_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom[TYPE_MASK])))) 
		return E_FAIL;

	// Com_Texture_Brush
	if (FAILED(AddComponent(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain_Brush"), TEXT("Com_Texture_Brush"), reinterpret_cast<CComponent**>(&m_pTextureCom[TYPE_BRUSH])))) 
		return E_FAIL;

	/* Com_Navigation */
	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
		return E_FAIL;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Navigation_S2"),
		TEXT("Com_Navigation_S2"), reinterpret_cast<CComponent**>(&m_pNavigationComS2))))
		return E_FAIL;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Navigation_S3"),
		TEXT("Com_Navigation_S3"), reinterpret_cast<CComponent**>(&m_pNavigationComS3))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->BindShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	_float4x4 viewMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_VIEW);
	_float4x4 projMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_PROJ);

	if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &viewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &projMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->BindRawValue("g_vCamPosition", m_pGameInst->GetCameraPosition(), sizeof(_float4)))) return E_FAIL;

	if (FAILED(m_pTextureCom[TYPE_DIFFUSE]->BindShaderResources(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pTextureCom[TYPE_MASK]->BindShaderResource(m_pShaderCom, "g_MaskTexture")))
		return E_FAIL;

	if (FAILED(m_pTextureCom[TYPE_BRUSH]->BindShaderResource(m_pShaderCom, "g_BrushTexture")))
		return E_FAIL;

	return S_OK;
}

CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTerrain* pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CTerrain");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject* CTerrain::Clone(void* pArg)
{
	CTerrain* pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTerrain");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	SafeRelease(m_pShaderCom);

	for (auto& pTextureCom : m_pTextureCom)
		if (nullptr != pTextureCom)
			SafeRelease(pTextureCom);

	SafeRelease(m_pVIBufferCom);
}
