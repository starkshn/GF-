#include "pch.h"
#include "EffectCircle1004.h"
#include "GameInstance.h"

CEffectCircle1004::CEffectCircle1004(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CEffect(pDevice, pContext)
{
}

CEffectCircle1004::CEffectCircle1004(const CEffectCircle1004& Prototype)
    :
    CEffect(Prototype)
{
}

HRESULT CEffectCircle1004::InitializePrototype()
{
    return S_OK;
}

HRESULT CEffectCircle1004::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    m_fMaxScale = 3.f;
    m_fSP = m_fMaxScale/ m_fMaxLifeTime;
    m_fAccScale = 0.f;

    return S_OK;
}

void CEffectCircle1004::PriorityUpdate(_float fTimeDelta)
{

}

void CEffectCircle1004::Update(_float fTimeDelta)
{
    if (IsActive())
    {
        __super::Update(fTimeDelta);
        // BillBoard();

        m_fAccScale += fTimeDelta * m_fSP;
        m_fAccScale = min(m_fMaxScale, m_fAccScale);
        m_pTransformCom->Scaling(m_fAccScale, m_fAccScale, m_fAccScale);
    }
    else
    {
        m_fAccScale = 0.f;
    }
}

void CEffectCircle1004::LateUpdate(_float fTimeDelta)
{
    if (IsActive())
        m_pGameInst->AddRenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CEffectCircle1004::Render()
{
    if (IsActive())
    {
        if (FAILED(BindShaderResources()))
            return E_FAIL;

        m_pShaderCom->Begin(11);

        m_pVIBufferCom->BindBufferDesc();

        m_pVIBufferCom->Render();
    }


    return S_OK;
}

HRESULT CEffectCircle1004::ReadyComponents()
{
    /* Com_Shader */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* Com_Texture */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_Circle1004"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    ///* Com_Texture */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_Circle1004_Noise"), TEXT("Com_Texture_Noies"), reinterpret_cast<CComponent**>(&m_pNoiseTex))))
        return E_FAIL;

    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_Circle1004_Color"), TEXT("Com_Texture_Color"), reinterpret_cast<CComponent**>(&m_pColorTex))))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffectCircle1004::BindShaderResources()
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

    if (FAILED(m_pNoiseTex->BindShaderResource(m_pShaderCom, "g_TextureNoise")))
        return E_FAIL;

    if (FAILED(m_pColorTex->BindShaderResource(m_pShaderCom, "g_TextureColor")))
        return E_FAIL;

    if (FAILED(m_pGameInst->BindRTShaderResource(m_pShaderCom, "g_DepthTexture", TEXT("TargetDepth"))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindRawValue("g_MaxLifeTime", &m_fMaxLifeTime, sizeof _float)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindRawValue("g_CurLifeTime", &m_fAccLifeTime, sizeof _float)))
        return E_FAIL;


    return S_OK;
}

CEffectCircle1004* CEffectCircle1004::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEffectCircle1004* pInstance = new CEffectCircle1004(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CEffectCircle1004");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CEffectCircle1004::Clone(void* pArg)
{
    CEffectCircle1004* pInstance = new CEffectCircle1004(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CEffectCircle1004");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CEffectCircle1004::Free()
{
    __super::Free();
}
