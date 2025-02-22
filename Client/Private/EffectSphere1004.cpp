#include "pch.h"
#include "EffectSphere1004.h"
#include "GameInstance.h"
#include "Model.h"

CEffectSphere1004::CEffectSphere1004(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CEffect(pDevice, pContext)
{
}

CEffectSphere1004::CEffectSphere1004(const CEffectSphere1004& Prototype)
    :
    CEffect(Prototype)
{
}

HRESULT CEffectSphere1004::InitializePrototype()
{
    return S_OK;
}

HRESULT CEffectSphere1004::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    m_fMaxScale = 7.f;
    m_fSP = 50.f;
    m_fAccScale = 1.f;

    return S_OK;
}

void CEffectSphere1004::PriorityUpdate(_float fTimeDelta)
{

}

void CEffectSphere1004::Update(_float fTimeDelta)
{
    if (IsActive())
    {
        __super::Update(fTimeDelta);
        BillBoard();

        m_fAccScale += fTimeDelta * m_fSP;
        m_fAccScale = min(m_fMaxScale, m_fAccScale);
        m_pTransformCom->Scaling(m_fAccScale, m_fAccScale, m_fAccScale);
        // m_pTransformCom->Scaling(4.f, 4.f, 4.f);

        // m_pTransformCom->SetRotationSpeed(30.f);
        // m_pTransformCom->Turn(m_pTransformCom->GetTransformState(CTransform::STATE_UP), fTimeDelta);
    }
    else
    {
        m_fAccScale = 1.f;
        m_pTransformCom->Scaling(1.f, 1.f, 1.f);
    }
}

void CEffectSphere1004::LateUpdate(_float fTimeDelta)
{
    if (IsActive())
        m_pGameInst->AddRenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CEffectSphere1004::Render()
{
    if (IsActive())
    {
        if (FAILED(BindShaderResources()))
            return E_FAIL;

        _uint		iNumMeshes = m_pModelCom->GetNumMeshes();

        for (_uint i = 0; i < iNumMeshes; ++i)
        {
            m_pShaderCom->Begin(2);
            m_pModelCom->Render(i);
        }
    }

    return S_OK;
}

HRESULT CEffectSphere1004::ReadyComponents()
{
    /* Com_Shader */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    // Model
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Model_EffectSphere"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    // Diffuse
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Diffuse_EffectSphere1004"), TEXT("Com_Texture_Diffuse"), reinterpret_cast<CComponent**>(&m_pDiffuseTex))))
        return E_FAIL;

    // Noise
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Noise_EffectSphere1004"), TEXT("Com_Texture_Noise"), reinterpret_cast<CComponent**>(&m_pNoiseTex))))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffectSphere1004::BindShaderResources()
{
    if (FAILED(m_pTransformCom->BindShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    _float4x4 viewMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_VIEW);
    _float4x4 projMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_PROJ);

    if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &viewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &projMatrix)))
        return E_FAIL;

    //if (FAILED(m_pTextureCom->BindShaderResource(m_pShaderCom, "g_DiffuseTexture")))
    //    return E_FAIL;

    // Diffuse, Noise
    {
        if (FAILED(m_pDiffuseTex->BindShaderResource(m_pShaderCom, "g_DiffuseCustomTexture")))
            return E_FAIL;

        if (FAILED(m_pNoiseTex->BindShaderResource(m_pShaderCom, "g_NoiseTexture")))
            return E_FAIL;
    }


    //if (FAILED(m_pGameInst->BindRTShaderResource(m_pShaderCom, "g_DepthTexture", TEXT("TargetDepth"))))
    //    return E_FAIL;

    if (FAILED(m_pShaderCom->BindRawValue("g_MaxLifeTime", &m_fMaxLifeTime, sizeof _float)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindRawValue("g_CurLifeTime", &m_fAccLifeTime, sizeof _float)))
        return E_FAIL;


    return S_OK;
}

CEffectSphere1004* CEffectSphere1004::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEffectSphere1004* pInstance = new CEffectSphere1004(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CEffectSphere1004");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CEffectSphere1004::Clone(void* pArg)
{
    CEffectSphere1004* pInstance = new CEffectSphere1004(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CEffectSphere1004");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CEffectSphere1004::Free()
{
    __super::Free();
}
