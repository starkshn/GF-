#include "pch.h"
#include "EffectProjectileWallHit.h"
#include "GameInstance.h"

CEffectProjectileWallHit::CEffectProjectileWallHit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CEffect(pDevice, pContext)
{
}

CEffectProjectileWallHit::CEffectProjectileWallHit(const CEffectProjectileWallHit& Prototype)
    :
    CEffect(Prototype)
{
}

HRESULT CEffectProjectileWallHit::InitializePrototype()
{
    return S_OK;
}

HRESULT CEffectProjectileWallHit::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    return S_OK;
}

void CEffectProjectileWallHit::PriorityUpdate(_float fTimeDelta)
{
       
}

void CEffectProjectileWallHit::Update(_float fTimeDelta)
{
    if (IsActive())
        __super::Update(fTimeDelta);
}

void CEffectProjectileWallHit::LateUpdate(_float fTimeDelta)
{
    if (IsActive())
        m_pGameInst->AddRenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CEffectProjectileWallHit::Render()
{
    if (IsActive())
    {
        if (FAILED(BindShaderResources()))
            return E_FAIL;

        m_pShaderCom->Begin(2);

        m_pVIBufferCom->BindBufferDesc();

        m_pVIBufferCom->Render();
    }


    return S_OK;
}

HRESULT CEffectProjectileWallHit::ReadyComponents()
{
    /* Com_Shader */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* Com_Texture */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_ProjectileWallHit"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffectProjectileWallHit::BindShaderResources()
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

CEffectProjectileWallHit* CEffectProjectileWallHit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEffectProjectileWallHit* pInstance = new CEffectProjectileWallHit(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CEffectProjectileWallHit");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CEffectProjectileWallHit::Clone(void* pArg)
{
    CEffectProjectileWallHit* pInstance = new CEffectProjectileWallHit(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CEffectProjectileWallHit");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CEffectProjectileWallHit::Free()
{
    __super::Free();
}
