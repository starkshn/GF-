#include "pch.h"
#include "ParticleMonsterHit.h"

CParticleMonsterHit::CParticleMonsterHit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CParticle(pDevice, pContext)
{
}

CParticleMonsterHit::CParticleMonsterHit(const CParticleMonsterHit& Prototype)
    :
    CParticle(Prototype)
{
}

HRESULT CParticleMonsterHit::InitializePrototype()
{
    return S_OK;
}

HRESULT CParticleMonsterHit::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    return S_OK;
}

void CParticleMonsterHit::PriorityUpdate(_float fTimeDelta)
{

}

void CParticleMonsterHit::Update(_float fTimeDelta)
{
    if (IsActivate())
    {
        m_pVIBufferCom->SpreadParticles(fTimeDelta);

        if (false == m_pVIBufferCom->IsActivate())
            DeActivate();
    }
}

void CParticleMonsterHit::LateUpdate(_float fTimeDelta)
{
    if (IsActivate())
        m_pGameInst->AddRenderObject(CRenderer::RG_GLOW, this);
}

HRESULT CParticleMonsterHit::Render()
{
    if (IsActivate())
    {
        if (FAILED(BindShaderResources()))
            return E_FAIL;

        m_pShaderCom->Begin(3); // 패스 설정

        m_pVIBufferCom->BindBufferDesc();

        m_pVIBufferCom->Render();
    }

    return S_OK;
}


HRESULT CParticleMonsterHit::ReadyComponents()
{
    /* Com_Texture */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_MonsterHit"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPointInstance"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Particle_MonsterHit"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticleMonsterHit::BindShaderResources()
{
    if (FAILED(m_pTransformCom->BindShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    _int iRandX = rand() % 2;
    _float fStartX = iRandX == 0 ? 0.f : 0.5f;
    _int iRandY = rand() % 2;
    _float fStartY = iRandY == 0 ? 0.f : 0.5f;
    if (FAILED(m_pShaderCom->BindRawValue("g_StartX", &fStartX, sizeof _float)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->BindRawValue("g_StartY", &fStartY, sizeof _float)))
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

CParticleMonsterHit* CParticleMonsterHit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParticleMonsterHit* pInstance = new CParticleMonsterHit(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CParticleMonsterHit");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CParticleMonsterHit::Clone(void* pArg)
{
    CParticleMonsterHit* pInstance = new CParticleMonsterHit(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CParticleMonsterHit");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CParticleMonsterHit::Free()
{
    __super::Free();
}
