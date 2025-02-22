#include "pch.h"
#include "EffectFollowLighting.h"
#include "GameInstance.h"

CEffectFollowLighting::CEffectFollowLighting(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CEffect(pDevice, pContext)
{
}

CEffectFollowLighting::CEffectFollowLighting(const CEffectFollowLighting& Prototype)
    :
    CEffect(Prototype)
{
}

HRESULT CEffectFollowLighting::InitializePrototype()
{
    return S_OK;
}

HRESULT CEffectFollowLighting::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    return S_OK;
}

void CEffectFollowLighting::PriorityUpdate(_float fTimeDelta)
{

}

void CEffectFollowLighting::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    if (IsActive())
    {
        BillBoard();

        m_pTransformCom->Scaling(3.5f, 30.f, 2.f);

        /*m_fAccScale += fTimeDelta * m_fSP;
        m_fAccScale = min(m_fMaxScale, m_fAccScale);
        m_pTransformCom->Scaling(m_fAccScale, m_fAccScale, m_fAccScale);*/
    }
    else
    {
        // m_fAccScale = 0.f;
    }

}

void CEffectFollowLighting::LateUpdate(_float fTimeDelta)
{
    if (IsActive())
        m_pGameInst->AddRenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CEffectFollowLighting::Render()
{
    if (IsActive())
    {
        if (FAILED(BindShaderResources()))
            return E_FAIL;

        m_pShaderCom->Begin(10);

        m_pVIBufferCom->BindBufferDesc();

        m_pVIBufferCom->Render();
    }


    return S_OK;
}

HRESULT CEffectFollowLighting::ReadyComponents()
{
    /* Com_Shader */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* Com_Texture */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_FollowLighting"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffectFollowLighting::BindShaderResources()
{
    if (FAILED(m_pTransformCom->BindShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    _float4x4 viewMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_VIEW);
    _float4x4 projMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_PROJ);

    if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &viewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &projMatrix)))
        return E_FAIL;


    if (FAILED(m_pTextureCom->BindShaderResource(m_pShaderCom, "g_Texture", static_cast<_uint>(m_fFrame))))
        return E_FAIL;

    if (FAILED(m_pGameInst->BindRTShaderResource(m_pShaderCom, "g_DepthTexture", TEXT("TargetDepth"))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindRawValue("g_MaxLifeTime", &m_fMaxLifeTime, sizeof _float)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindRawValue("g_CurLifeTime", &m_fAccLifeTime, sizeof _float)))
        return E_FAIL;


    return S_OK;
}

CEffectFollowLighting* CEffectFollowLighting::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEffectFollowLighting* pInstance = new CEffectFollowLighting(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CEffectFollowLighting");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CEffectFollowLighting::Clone(void* pArg)
{
    CEffectFollowLighting* pInstance = new CEffectFollowLighting(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CEffectFollowLighting");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CEffectFollowLighting::Free()
{
    __super::Free();
}
