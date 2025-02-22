#include "pch.h"
#include "ParticleBossRockColDustLighting.h"

CParticleBossRockColDustLighting::CParticleBossRockColDustLighting(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CParticle(pDevice, pContext)
{
}

CParticleBossRockColDustLighting::CParticleBossRockColDustLighting(const CParticleBossRockColDustLighting& Prototype)
    :
    CParticle(Prototype)
{
}

HRESULT CParticleBossRockColDustLighting::InitializePrototype()
{
    return S_OK;
}

HRESULT CParticleBossRockColDustLighting::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    return S_OK;
}

void CParticleBossRockColDustLighting::PriorityUpdate(_float fTimeDelta)
{
}

void CParticleBossRockColDustLighting::Update(_float fTimeDelta)
{
    if (IsActivate())
    {
        m_pVIBufferCom->SpreadParticles(fTimeDelta);

        if (false == m_pVIBufferCom->IsActivate())
            DeActivate();
    }
}

void CParticleBossRockColDustLighting::LateUpdate(_float fTimeDelta)
{
    if (IsActivate())
        m_pGameInst->AddRenderObject(CRenderer::RG_GLOW, this);
}

HRESULT CParticleBossRockColDustLighting::Render()
{
    if (IsActivate())
    {
        if (FAILED(BindShaderResources()))
            return E_FAIL;

        m_pShaderCom->Begin(7); // 패스 설정

        m_pVIBufferCom->BindBufferDesc();

        m_pVIBufferCom->Render();
    }

    return S_OK;
}

HRESULT CParticleBossRockColDustLighting::ReadyComponents()
{
    /* Com_Texture */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_BossRockColDustLighting"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPointInstance"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Particle_BossRockColDustLighting"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticleBossRockColDustLighting::BindShaderResources()
{
    if (FAILED(m_pTransformCom->BindShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    _float4x4 viewMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_VIEW);
    _float4x4 projMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_PROJ);

    if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &viewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &projMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &projMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->BindShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindRawValue("g_vCamPosition", m_pGameInst->GetCameraPosition(), sizeof _float4)))
        return E_FAIL;

    return S_OK;
}

CParticleBossRockColDustLighting* CParticleBossRockColDustLighting::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParticleBossRockColDustLighting* pInstance = new CParticleBossRockColDustLighting(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CParticleBossRockColDustLighting");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CParticleBossRockColDustLighting::Clone(void* pArg)
{
    CParticleBossRockColDustLighting* pInstance = new CParticleBossRockColDustLighting(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CParticleBossRockColDustLighting");
        SafeRelease(pInstance);
    }

    return pInstance; return nullptr;
}

void CParticleBossRockColDustLighting::Free()
{
    __super::Free();
}
