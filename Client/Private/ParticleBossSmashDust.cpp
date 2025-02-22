#include "pch.h"
#include "ParticleBossSmashDust.h"

CParticleBossSmashDust::CParticleBossSmashDust(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CParticle(pDevice, pContext)
{
}

CParticleBossSmashDust::CParticleBossSmashDust(const CParticleBossSmashDust& Prototype)
    :
    CParticle(Prototype)
{
}

HRESULT CParticleBossSmashDust::InitializePrototype()
{
    return S_OK;
}

HRESULT CParticleBossSmashDust::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    return S_OK;
}

void CParticleBossSmashDust::PriorityUpdate(_float fTimeDelta)
{
}

void CParticleBossSmashDust::Update(_float fTimeDelta)
{
    if (IsActivate())
    {
        m_pVIBufferCom->SpreadParticles(fTimeDelta);

        if (false == m_pVIBufferCom->IsActivate())
            DeActivate();
    }
}

void CParticleBossSmashDust::LateUpdate(_float fTimeDelta)
{
    if (IsActivate())
        m_pGameInst->AddRenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CParticleBossSmashDust::Render()
{
    if (IsActivate())
    {
        if (FAILED(BindShaderResources()))
            return E_FAIL;

        m_pShaderCom->Begin(2); // 패스 설정

        m_pVIBufferCom->BindBufferDesc();

        m_pVIBufferCom->Render();
    }

    return S_OK;
}

HRESULT CParticleBossSmashDust::ReadyComponents()
{
    /* Com_Texture */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_BossSmashDust"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPointInstance"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Particle_BossSmashDust"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticleBossSmashDust::BindShaderResources()
{
    if (FAILED(m_pTransformCom->BindShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    _float4x4 viewMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_VIEW);
    _float4x4 projMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_PROJ);

    if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &viewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &projMatrix)))
        return E_FAIL;

    _int iRandX = rand() % 2;
    _float fStartX = iRandX == 0 ? 0.f : 0.5f;
    _int iRandY = rand() % 2;
    _float fStartY = iRandY == 0 ? 0.f : 0.5f;
    if (FAILED(m_pShaderCom->BindRawValue("g_StartX", &fStartX, sizeof _float)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->BindRawValue("g_StartY", &fStartY, sizeof _float)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &projMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->BindShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindRawValue("g_vCamPosition", m_pGameInst->GetCameraPosition(), sizeof _float4)))
        return E_FAIL;

    return S_OK;
}

CParticleBossSmashDust* CParticleBossSmashDust::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParticleBossSmashDust* pInstance = new CParticleBossSmashDust(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CParticleBossSmashDust");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CParticleBossSmashDust::Clone(void* pArg)
{
    CParticleBossSmashDust* pInstance = new CParticleBossSmashDust(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CParticleBossSmashDust");
        SafeRelease(pInstance);
    }

    return pInstance; return nullptr;
}

void CParticleBossSmashDust::Free()
{
    __super::Free();
}
