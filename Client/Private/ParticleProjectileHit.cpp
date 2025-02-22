#include "pch.h"
#include "ParticleProjectileHit.h"

CParticleProjectileHit::CParticleProjectileHit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CParticle(pDevice, pContext)
{
}

CParticleProjectileHit::CParticleProjectileHit(const CParticleProjectileHit& Prototype)
    :
    CParticle(Prototype)
{
}

HRESULT CParticleProjectileHit::InitializePrototype()
{
    return S_OK;
}

HRESULT CParticleProjectileHit::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    return S_OK;
}

void CParticleProjectileHit::PriorityUpdate(_float fTimeDelta)
{

}

void CParticleProjectileHit::Update(_float fTimeDelta)
{
    if (IsActivate())
    {
        m_pVIBufferCom->SpreadParticles(fTimeDelta);

        if (false == m_pVIBufferCom->IsActivate())
            DeActivate();
    }
}

void CParticleProjectileHit::LateUpdate(_float fTimeDelta)
{
    if (IsActivate())
        m_pGameInst->AddRenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CParticleProjectileHit::Render()
{
    if (FAILED(BindShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(0); // 패스 설정

    m_pVIBufferCom->BindBufferDesc();

    m_pVIBufferCom->Render();
    
    return S_OK;
}


HRESULT CParticleProjectileHit::ReadyComponents()
{
    /* Com_Texture */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_ProjectileHit"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPointInstance_ProjectileHit"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Particle_ProjectileHit"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticleProjectileHit::BindShaderResources()
{
    if (FAILED(m_pTransformCom->BindShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    _float4x4 viewMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_VIEW);
    _float4x4 projMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_PROJ);

    if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &viewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &projMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->BindShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    return S_OK;
}

CParticleProjectileHit* CParticleProjectileHit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParticleProjectileHit* pInstance = new CParticleProjectileHit(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CParticleProjectileHit");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CParticleProjectileHit::Clone(void* pArg)
{
    CParticleProjectileHit* pInstance = new CParticleProjectileHit(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CParticleProjectileHit");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CParticleProjectileHit::Free()
{
    __super::Free();

    SafeRelease(m_pShaderCom);
    SafeRelease(m_pTextureCom);
    SafeRelease(m_pVIBufferCom);
}
