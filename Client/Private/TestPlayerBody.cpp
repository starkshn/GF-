#include "pch.h"
#include "TestPlayerBody.h"

#include "GameInstance.h"
#include "TestPlayer.h"
#include "Model.h"


CTestPlayerBody::CTestPlayerBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CPartObject(pDevice, pContext)
{
}

CTestPlayerBody::CTestPlayerBody(const CTestPlayerBody& Prototype)
    :
    CPartObject(Prototype)
{
}

HRESULT CTestPlayerBody::InitializePrototype()
{
    return S_OK;
}

HRESULT CTestPlayerBody::Initialize(void* pArg)
{
    TESTPLAYERBODY_DESC* pDesc = static_cast<TESTPLAYERBODY_DESC*>(pArg);

    m_pState = pDesc->pState;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;


    m_pModelCom->SetupAnimation(3, CAnimation::LOOP);


    return S_OK;
}

void CTestPlayerBody::PriorityUpdate(_float fTimeDelta)
{
}

void CTestPlayerBody::Update(_float fTimeDelta)
{
    m_pModelCom->PlayAnimation(fTimeDelta);

    // 현재 CTestPlayerBody의 월드 행렬은 나의 월드 변환행렬(다른 말로 로컬 변환행렬) 곱하기 부모의 월드 변환행렬이 된다.
    XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->GetWorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));
}

void CTestPlayerBody::LateUpdate(_float fTimeDelta)
{
    if (*m_pState & CTestPlayer::STATE_IDLE)
        m_pModelCom->SetupAnimation(3, CAnimation::LOOP);
    else if (*m_pState & CTestPlayer::STATE_WALK)
        m_pModelCom->SetupAnimation(4, CAnimation::LOOP);
    
    m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CTestPlayerBody::Render()
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

    return S_OK;
}

HRESULT CTestPlayerBody::ReadyComponents()
{
    /* Com_Shader */
    if (FAILED(AddComponent(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Model */
    if (FAILED(AddComponent(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CTestPlayerBody::BindShaderResources()
{
    if (FAILED(m_pShaderCom->BindMatrix("g_WorldMatrix", &m_WorldMatrix))) return E_FAIL;

    _float4x4 viewMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_VIEW);
    _float4x4 projMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_PROJ);

    if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &viewMatrix))) return E_FAIL;
    if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &projMatrix))) return E_FAIL;

    return S_OK;
}

CTestPlayerBody* CTestPlayerBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTestPlayerBody* pInstance = new CTestPlayerBody(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CTestPlayerBody");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CTestPlayerBody::Clone(void* pArg)
{
    CTestPlayerBody* pInstance = new CTestPlayerBody(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CTestPlayerBody");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CTestPlayerBody::Free()
{
    __super::Free();

    SafeRelease(m_pShaderCom);
    SafeRelease(m_pModelCom);
}
