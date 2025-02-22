#include "pch.h"
#include "ParticleWall.h"

CParticleWall::CParticleWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CParticle(pDevice, pContext)
{
}

CParticleWall::CParticleWall(const CParticleWall& Prototype)
    :
    CParticle(Prototype)
{
}

HRESULT CParticleWall::InitializePrototype()
{
    return S_OK;
}

HRESULT CParticleWall::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    return S_OK;
}

void CParticleWall::PriorityUpdate(_float fTimeDelta)
{

}

void CParticleWall::Update(_float fTimeDelta)
{
    if (IsActivate())
    {
        m_pVIBufferCom->SpreadParticles(fTimeDelta);

        if (false == m_pVIBufferCom->IsActivate())
            DeActivate();
    }
}

void CParticleWall::LateUpdate(_float fTimeDelta)
{
    if (IsActivate())
        m_pGameInst->AddRenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CParticleWall::Render()
{
    if (FAILED(BindShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(0); // 패스 설정

    m_pVIBufferCom->BindBufferDesc();

    m_pVIBufferCom->Render();

    return S_OK;
}


HRESULT CParticleWall::ReadyComponents()
{
    /* Com_Texture */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_WallHit"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPointInstance_WallHit"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Particle_WallHit"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;



    return S_OK;
}

HRESULT CParticleWall::BindShaderResources()
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

CParticleWall* CParticleWall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParticleWall* pInstance = new CParticleWall(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CParticleWall");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CParticleWall::Clone(void* pArg)
{
    CParticleWall* pInstance = new CParticleWall(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CParticleWall");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CParticleWall::Free()
{
    __super::Free();

    SafeRelease(m_pShaderCom);
    SafeRelease(m_pTextureCom);
    SafeRelease(m_pVIBufferCom);
}
