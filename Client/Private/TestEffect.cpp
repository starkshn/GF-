#include "pch.h"
#include "TestEffect.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Transform.h"
#include "Model.h"
#include "Texture.h"

CTestEffect::CTestEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CGameObject(pDevice, pContext)
{
}

CTestEffect::CTestEffect(const CTestEffect& Prototype)
	:
	CGameObject(Prototype)
{
}

HRESULT CTestEffect::InitializePrototype()
{
	return S_OK;
}

HRESULT CTestEffect::Initialize(void* pArg)
{
	if (__super::Initialize(pArg))
		return E_FAIL;

	if (FAILED(ReadyComponents()))
		return E_FAIL;


	return S_OK;
}

void CTestEffect::PriorityUpdate(_float fTimeDelta)
{

}

void CTestEffect::Update(_float fTimeDelta)
{
	m_pTransformCom->SetTransformState(CTransform::STATE_POS, _vector{ 0.f, 2.f, 2.f, 1.f });
	m_pTransformCom->Scaling(3.f, 3.f, 3.f);


	m_fAccLifeTime += fTimeDelta;
	if (m_fAccLifeTime >= m_fMaxLifeTime)
	{
		m_fAccLifeTime = 0.f;
	}
}

void CTestEffect::LateUpdate(_float fTimeDelta)
{
	m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CTestEffect::Render()
{
	if (FAILED(BindShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->GetNumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		// i 번째에 해당하는 텍스쳐를 Bind 해준다.
		// if (FAILED(m_pModelCom->BindMaterial(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
		// 	return E_FAIL;

		// 0 default, 1 wireframe
		m_pShaderCom->Begin(1);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CTestEffect::ReadyComponents()
{
	// Com_Shader
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;
	}

	// Com_Model
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Model_EffectSphere"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;
	}


	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_Test"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_TestEffectColor"), TEXT("Com_Texture_Color"), reinterpret_cast<CComponent**>(&m_pTextureColorCom))))
		return E_FAIL;
	


	return S_OK;
}

HRESULT CTestEffect::BindShaderResources()
{
	if (FAILED(m_pTransformCom->BindShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	_float4x4 viewMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_VIEW);
	_float4x4 projMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_PROJ);

	if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &viewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &projMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->BindShaderResource(m_pShaderCom, "g_ColorTexture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->BindRawValue("g_MaxLifeTime", &m_fMaxLifeTime, sizeof _float)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->BindRawValue("g_CurLifeTime", &m_fAccLifeTime, sizeof _float)))
		return E_FAIL;

	if (FAILED(m_pTextureColorCom->BindShaderResource(m_pShaderCom, "g_ColorTestTexture")))
		return E_FAIL;

	return S_OK;
}

CTestEffect* CTestEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTestEffect* pInst = new CTestEffect(pDevice, pContext);

	if (FAILED(pInst->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CTestEffect");
		SafeRelease(pInst);
	}

	return pInst;
}

CGameObject* CTestEffect::Clone(void* pArg)
{
	CTestEffect* pInst = new CTestEffect(*this);

	if (FAILED(pInst->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTestEffect");
		SafeRelease(pInst);
	}

	return pInst;
}

void CTestEffect::Free()
{
	__super::Free();

	SafeRelease(m_pShaderCom);
	SafeRelease(m_pModelCom);
}
