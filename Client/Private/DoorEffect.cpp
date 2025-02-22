#include "pch.h"
#include "DoorEffect.h"
#include "GameInstance.h"


CDoorEffect::CDoorEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CEffect(pDevice, pContext)
{
}

CDoorEffect::CDoorEffect(const CDoorEffect& Prototype)
    :
    CEffect(Prototype)
{
}

HRESULT CDoorEffect::InitializePrototype()
{
    return S_OK;
}

HRESULT CDoorEffect::Initialize(void* pArg)
{
    if (!pArg)
        return E_FAIL;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    return S_OK;
}

void CDoorEffect::PriorityUpdate(_float fTimeDelta)
{
}

static _bool bRe = false;

void CDoorEffect::Update(_float fTimeDelta)
{
    if (IsActive())
    {
        // __super::Update(fTimeDelta);
        // UpdateScaling(fTimeDelta);
        UpdateLoop(fTimeDelta);
    }
}

void CDoorEffect::LateUpdate(_float fTimeDelta)
{

    //if (IsActive() && !bRe)
    //{
    //    m_pGameInst->AddRenderObject(CRenderer::RG_GLOW, this);
    //}


    if (IsActive())
    {
        m_pGameInst->AddRenderObject(CRenderer::RG_GLOW, this);
    }
}

HRESULT CDoorEffect::Render()
{
    if (IsActive())
    {
        if (FAILED(BindShaderResources()))
            return E_FAIL;

        m_pShaderCom->Begin(20);
        m_pVIBufferCom->BindBufferDesc();
        m_pVIBufferCom->Render();
    }

    return S_OK;
}

void CDoorEffect::UpdateScaling(_float fTimeDelta)
{
    static _float fLoopTime = 0.f;
    static _float fAcc = 0.f;

    if (bRe)
    {
        fLoopTime += fTimeDelta;
        if (fLoopTime >= 1.f)
        {
            fLoopTime = 0.f;
            bRe = false;
        }

    }
    else
    {
        m_fAccLifeTime += fTimeDelta;
        fAcc += fTimeDelta * 7.f;

        if (m_fAccLifeTime >= m_fMaxLifeTime)
        {
            m_fAccLifeTime = 0.f;
            fAcc = 0.f;
            bRe = true;
        }

        // BillBoard();
        m_pTransformCom->Scaling(2.f + fAcc, 2.f + fAcc, 2.f + fAcc);
    }
}

void CDoorEffect::UpdateLoop(_float fTimeDelta)
{
    static _float fLoopTime = 0.f;
    static _float fAcc = 0.f;

    if (bRe)
    {
        m_fAccLifeTime += fTimeDelta;
        fAcc -= fTimeDelta * 0.5f;

        if (m_fAccLifeTime >= m_fMaxLifeTime)
        {
            m_fAccLifeTime = 0.f;
            bRe = false;
        }
    }
    else
    {
        m_fAccLifeTime += fTimeDelta;
        fAcc += fTimeDelta * 0.5f;
        
        if (m_fAccLifeTime >= m_fMaxLifeTime)
        {
            m_fAccLifeTime = 0.f;
            fAcc = 0.f;
            // bRe = true;
        }
    }

    m_pTransformCom->Scaling(3.f + fAcc, 3.f + fAcc, 3.f + fAcc);

}

HRESULT CDoorEffect::ReadyComponents()
{
    /* Com_Shader */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_EffectDoor"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CDoorEffect::BindShaderResources()
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

    // 애니매이션 시간
    if (FAILED(m_pShaderCom->BindRawValue("g_MaxLifeTime", &m_fMaxLifeTime, sizeof _float)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindRawValue("g_CurLifeTime", &m_fAccLifeTime, sizeof _float)))
        return E_FAIL;

    return S_OK;
}

CDoorEffect* CDoorEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDoorEffect* pInstance = new CDoorEffect(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CDoorEffect");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CDoorEffect::Clone(void* pArg)
{
    CDoorEffect* pInstance = new CDoorEffect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CDoorEffect");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CDoorEffect::Free()
{
    __super::Free();

}
