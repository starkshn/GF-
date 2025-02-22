#include "Monster09.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Transform.h"
#include "Model.h"
#include "ColBoundingBox.h"

CMonster09::CMonster09(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CGameObject(pDevice, pContext)
{
}

CMonster09::CMonster09(const CMonster09& Prototype)
	:
	CGameObject(Prototype)
{
}

HRESULT CMonster09::InitializePrototype()
{
	return S_OK;
}

HRESULT CMonster09::Initialize(void* pArg)
{
	if (__super::Initialize(pArg))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->SetSpeed(1.f);

	for (_int i = 0; i < m_pModelCom->GetNumAnimations(); ++i)
		m_pModelCom->SetupAnimation(i, CAnimation::PLAY_TYPE::LOOP);

	m_pModelCom->SetCurrentAnimationIndex(0);


	return S_OK;
}

void CMonster09::PriorityUpdate(_float fTimeDelta)
{

}

void CMonster09::Update(_float fTimeDelta)
{
	m_pModelCom->PlayAnimation(fTimeDelta);
	m_pBoundingBox->Update_Bouding_Box(m_pTransformCom);
}

void CMonster09::LateUpdate(_float fTimeDelta)
{
	m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CMonster09::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	// 현재 모델이 사용하고 있는 전체 메쉬수를 가지고 온다.
	_uint		iNumMeshes = m_pModelCom->GetNumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		// i 번째에 해당하는 텍스쳐를 Bind 해준다.
		if (FAILED(m_pModelCom->BindMaterial(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->BindBoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		// 0 default, 1 wireframe
		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	// 바운딩 박스 렌더
	{
		_float4x4 viewMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_VIEW);
		_float4x4 projMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_PROJ);

		if (FAILED(m_pBoundingBox->Render_Bounding_Box(m_pTransformCom, XMLoadFloat4x4(&viewMatrix), XMLoadFloat4x4(&projMatrix))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonster09::Ready_Components()
{
	// Com_Shader
	{
		if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;
	}

	// Com_Model
	{
		if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Model_Monster09"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;
	}

	// Com_BoudingBox
	{
		if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_BoundingBox"), TEXT("Com_BoundingBox"), reinterpret_cast<CComponent**>(&m_pBoundingBox))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonster09::Bind_ShaderResources()
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

CMonster09* CMonster09::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster09* pInst = new CMonster09(pDevice, pContext);

	if (FAILED(pInst->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CMonster09");
		SafeRelease(pInst);
	}

	return pInst;
}

CGameObject* CMonster09::Clone(void* pArg)
{
	CMonster09* pInst = new CMonster09(*this);

	if (FAILED(pInst->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonster09");
		SafeRelease(pInst);
	}

	return pInst;
}

void CMonster09::Free()
{
	__super::Free();

	SafeRelease(m_pShaderCom);
	SafeRelease(m_pModelCom);
	SafeRelease(m_pBoundingBox);
}
