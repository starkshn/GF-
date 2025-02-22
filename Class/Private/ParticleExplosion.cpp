#include "ParticleExplosion.h"
#include "GameInstance.h"
#include "Shader.h"
#include "VIBufferParticleRect.h"

CParticleExplosion::CParticleExplosion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CGameObject(pDevice, pContext)
{
}

CParticleExplosion::CParticleExplosion(const CParticleExplosion& Prototype)
    :
    CGameObject(Prototype)
{
}

HRESULT CParticleExplosion::InitializePrototype()
{
    return S_OK;
}

HRESULT CParticleExplosion::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    m_pVIBufferCom->Activate();

    return S_OK;
}

void CParticleExplosion::PriorityUpdate(_float fTimeDelta)
{
}

void CParticleExplosion::Update(_float fTimeDelta)
{
    m_pVIBufferCom->SpreadParticles(fTimeDelta);
}

void CParticleExplosion::LateUpdate(_float fTimeDelta)
{
    m_pGameInst->AddRenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CParticleExplosion::Render()
{
    if (FAILED(BindShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(0); // 패스 설정

    m_pVIBufferCom->BindBufferDesc();

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CParticleExplosion::ReadyComponents()
{
    /* Com_Shader */
    if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxPointInstance"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Particle_Explosion"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* Com_Texture */
    if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Explosion"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticleExplosion::BindShaderResources()
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

    
    if (FAILED(m_pShaderCom->BindRawValue("g_vCamPosition", m_pGameInst->GetCameraPosition(), sizeof _float4)))
        return E_FAIL;

    return S_OK;
}

CParticleExplosion* CParticleExplosion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParticleExplosion* pInstance = new CParticleExplosion(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CParticleExplosion");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CParticleExplosion::Clone(void* pArg)
{
    CParticleExplosion* pInstance = new CParticleExplosion(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CParticleExplosion");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CParticleExplosion::Free()
{
    __super::Free();

    SafeRelease(m_pShaderCom);
    SafeRelease(m_pTextureCom);
    SafeRelease(m_pVIBufferCom);
}
