#include "pch.h"
#include "EffectBossRockAlert.h"
#include "GameInstance.h"

CEffectBossRockAlert::CEffectBossRockAlert(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CEffect(pDevice, pContext)
{
}

CEffectBossRockAlert::CEffectBossRockAlert(const CEffectBossRockAlert& Prototype)
    :
    CEffect(Prototype)
{
}

HRESULT CEffectBossRockAlert::InitializePrototype()
{
    return S_OK;
}

HRESULT CEffectBossRockAlert::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    m_fMaxScale = 16.f;
    m_fAccScale = 0.f;
    m_fSP = 24.f;

    return S_OK;
}

void CEffectBossRockAlert::PriorityUpdate(_float fTimeDelta)
{

}

void CEffectBossRockAlert::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    if (IsActive())
    {
        m_fAccScale += fTimeDelta * m_fSP;
        m_fAccScale = min(m_fMaxScale, m_fAccScale);
        m_pTransformCom->Scaling(m_fAccScale, m_fAccScale, m_fAccScale);
    }
    else
    {
        m_fAccScale = 0.f;
    }

}

void CEffectBossRockAlert::LateUpdate(_float fTimeDelta)
{
    if (IsActive())
        m_pGameInst->AddRenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CEffectBossRockAlert::Render()
{
    if (IsActive())
    {
        if (FAILED(BindShaderResources()))
            return E_FAIL;

        m_pShaderCom->Begin(8);

        m_pVIBufferCom->BindBufferDesc();

        m_pVIBufferCom->Render();
    }


    return S_OK;
}

HRESULT CEffectBossRockAlert::ReadyComponents()
{
    /* Com_Shader */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* Com_Texture */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_BossRockAlert"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffectBossRockAlert::BindShaderResources()
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

CEffectBossRockAlert* CEffectBossRockAlert::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEffectBossRockAlert* pInstance = new CEffectBossRockAlert(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CEffectBossRockAlert");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CEffectBossRockAlert::Clone(void* pArg)
{
    CEffectBossRockAlert* pInstance = new CEffectBossRockAlert(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CEffectBossRockAlert");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CEffectBossRockAlert::Free()
{
    __super::Free();
}
