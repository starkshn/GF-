#include "pch.h"
#include "EffectHit1004.h"
#include "GameInstance.h"

CEffectHit1004::CEffectHit1004(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CEffect(pDevice, pContext)
{
}

CEffectHit1004::CEffectHit1004(const CEffectHit1004& Prototype)
    :
    CEffect(Prototype)
{
}

HRESULT CEffectHit1004::InitializePrototype()
{
    return S_OK;
}

HRESULT CEffectHit1004::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    return S_OK;
}

void CEffectHit1004::PriorityUpdate(_float fTimeDelta)
{

}

void CEffectHit1004::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    if (IsActive())
    {
        BillBoard();
        m_pTransformCom->Scaling(3.f, 3.f, 3.f);
    }
}

void CEffectHit1004::LateUpdate(_float fTimeDelta)
{
    if (IsActive())
        m_pGameInst->AddRenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CEffectHit1004::Render()
{
    if (IsActive())
    {
        if (FAILED(BindShaderResources()))
            return E_FAIL;

        m_pShaderCom->Begin(12);

        m_pVIBufferCom->BindBufferDesc();

        m_pVIBufferCom->Render();
    }


    return S_OK;
}

HRESULT CEffectHit1004::ReadyComponents()
{
    /* Com_Shader */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* Com_Texture */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_Hit1004Fire"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_Hit1004Diffuse"), TEXT("Com_Texture_Diffuse"), reinterpret_cast<CComponent**>(&m_pTextureDiffuseCom))))
        return E_FAIL;

    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_Hit1004Noise"), TEXT("Com_Texture_Noise"), reinterpret_cast<CComponent**>(&m_pTextureNoiseCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffectHit1004::BindShaderResources()
{
    if (FAILED(m_pTransformCom->BindShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    _float4x4 viewMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_VIEW);
    _float4x4 projMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_PROJ);

    if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &viewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &projMatrix)))
        return E_FAIL;


    if (FAILED(m_pTextureCom->BindShaderResource(m_pShaderCom, "g_Texture")))
        return E_FAIL;

    if (FAILED(m_pTextureDiffuseCom->BindShaderResource(m_pShaderCom, "g_DiffuseTexture")))
        return E_FAIL;

    if (FAILED(m_pTextureNoiseCom->BindShaderResource(m_pShaderCom, "g_NoiseTexture")))
        return E_FAIL;


    if (FAILED(m_pGameInst->BindRTShaderResource(m_pShaderCom, "g_DepthTexture", TEXT("TargetDepth"))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindRawValue("g_MaxLifeTime", &m_fMaxLifeTime, sizeof _float)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindRawValue("g_CurLifeTime", &m_fAccLifeTime, sizeof _float)))
        return E_FAIL;


    return S_OK;
}

CEffectHit1004* CEffectHit1004::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEffectHit1004* pInstance = new CEffectHit1004(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CEffectHit1004");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CEffectHit1004::Clone(void* pArg)
{
    CEffectHit1004* pInstance = new CEffectHit1004(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CEffectHit1004");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CEffectHit1004::Free()
{
    __super::Free();
}
