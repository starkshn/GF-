#include "pch.h"
#include "EffectRoarNoise.h"
#include "GameInstance.h"


CEffectRoarNoise::CEffectRoarNoise(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CEffect(pDevice, pContext)
{
}

CEffectRoarNoise::CEffectRoarNoise(const CEffectRoarNoise& Prototype)
    :
    CEffect(Prototype)
{
}

HRESULT CEffectRoarNoise::InitializePrototype()
{
    return S_OK;
}

HRESULT CEffectRoarNoise::Initialize(void* pArg)
{
    if (!pArg)
        return E_FAIL;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    return S_OK;
}

void CEffectRoarNoise::PriorityUpdate(_float fTimeDelta)
{
}

void CEffectRoarNoise::Update(_float fTimeDelta)
{
    if (IsActive())
    {
        __super::Update(fTimeDelta);

        m_pTransformCom->Scaling(5.f, 5.f, 5.f);
    }
}

void CEffectRoarNoise::LateUpdate(_float fTimeDelta)
{
    if (IsActive())
    {
        m_pGameInst->AddRenderObject(CRenderer::RG_DISTORTION, this);
    }
}

HRESULT CEffectRoarNoise::Render()
{
    if (IsActive())
    {
        if (FAILED(BindShaderResources()))
            return E_FAIL;

        m_pShaderCom->Begin(0);
        m_pVIBufferCom->BindBufferDesc();
        m_pVIBufferCom->Render();
    }

    return S_OK;
}


HRESULT CEffectRoarNoise::ReadyComponents()
{
    /* Com_Shader */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_RoarNoise"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffectRoarNoise::BindShaderResources()
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

CEffectRoarNoise* CEffectRoarNoise::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEffectRoarNoise* pInstance = new CEffectRoarNoise(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CEffectRoarNoise");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CEffectRoarNoise::Clone(void* pArg)
{
    CEffectRoarNoise* pInstance = new CEffectRoarNoise(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CEffectRoarNoise");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CEffectRoarNoise::Free()
{
    __super::Free();

}
