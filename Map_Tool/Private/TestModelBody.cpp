#include "TestModelBody.h"
#include "GameInstance.h"
#include "TestModel.h"
#include "Model.h"

CTestModelBody::CTestModelBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CPartObject(pDevice, pContext)
{
}

CTestModelBody::CTestModelBody(const CTestModelBody& Prototype)
    :
    CPartObject(Prototype)
{
}

HRESULT CTestModelBody::InitializePrototype()
{
    return S_OK;
}

HRESULT CTestModelBody::Initialize(void* pArg)
{
    TESTMODELBODY_DESC* pDesc = static_cast<TESTMODELBODY_DESC*>(pArg);

    m_pState = pDesc->pState;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    m_pModelCom->SetupAnimation(rand() % 20, CAnimation::LOOP);

    return S_OK;
}

void CTestModelBody::PriorityUpdate(_float fTimeDelta)
{

}

void CTestModelBody::Update(_float fTimeDelta)
{
    m_pModelCom->PlayAnimation(fTimeDelta);

    // 현재 CTestPlayerBody의 월드 행렬은 나의 월드 변환행렬(다른 말로 로컬 변환행렬) 곱하기 부모의 월드 변환행렬이 된다.
    XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->GetWorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));

    m_pColliderCom->Update(XMLoadFloat4x4(&m_WorldMatrix));
}

void CTestModelBody::LateUpdate(_float fTimeDelta)
{
    if (*m_pState & CTestModel::STATE_IDLE)
        m_pModelCom->SetupAnimation(3, CAnimation::LOOP);
    else if (*m_pState & CTestModel::STATE_WALK)
        m_pModelCom->SetupAnimation(4, CAnimation::LOOP);

    CCollider* pOtherCollider = static_cast<CCollider*>(m_pGameInst->GetComponent(LEVEL_TOOL, TEXT("Layer_Tool_Player"), TEXT("Com_Collider")));
    m_pColliderCom->Intersect(pOtherCollider);

    m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CTestModelBody::Render()
{
    if (FAILED(BindShaderResources()))
        return E_FAIL;

    _uint   iNumMeshes = m_pModelCom->GetNumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pModelCom->BindMaterial(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE))) return E_FAIL;
        if (FAILED(m_pModelCom->BindBoneMatrices(m_pShaderCom, "g_BoneMatrices", i))) return E_FAIL;

        m_pShaderCom->Begin(0);
        m_pModelCom->Render(i);
    }


    m_pColliderCom->Render();

    return S_OK;
}

HRESULT CTestModelBody::ReadyComponents()
{
    /* Com_Shader */
    if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Model */
    if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Model_Fiona"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;



    /* Com_Collider */
    CBoundingSphere::BOUND_SPHERE_DESC		ColliderDesc = {};
    ColliderDesc.fRadius = 0.7f;
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.fRadius, 0.f);

    if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CTestModelBody::BindShaderResources()
{
    if (FAILED(m_pShaderCom->BindMatrix("g_WorldMatrix", &m_WorldMatrix))) return E_FAIL;

    _float4x4 viewMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_VIEW);
    _float4x4 projMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_PROJ);

    if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &viewMatrix))) return E_FAIL;
    if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &projMatrix))) return E_FAIL;

    const LIGHT_DESC* pLightDesc = m_pGameInst->GetLightDesc(0);
    if (FAILED(m_pShaderCom->BindRawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4)))) return E_FAIL;
    if (FAILED(m_pShaderCom->BindRawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4)))) return E_FAIL;
    if (FAILED(m_pShaderCom->BindRawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4)))) return E_FAIL;
    if (FAILED(m_pShaderCom->BindRawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4)))) return E_FAIL;
    if (FAILED(m_pShaderCom->BindRawValue("g_vCamPosition", m_pGameInst->GetCameraPosition(), sizeof(_float4)))) return E_FAIL;

    return S_OK;
}

CTestModelBody* CTestModelBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTestModelBody* pInstance = new CTestModelBody(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CTestModelBody");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CTestModelBody::Clone(void* pArg)
{
    CTestModelBody* pInstance = new CTestModelBody(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CTestModelBody");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CTestModelBody::Free()
{
    __super::Free();

    SafeRelease(m_pShaderCom);
    SafeRelease(m_pModelCom);
}
