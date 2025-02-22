#include "pch.h"
#include "ParticleBossWalk.h"
#include "GameInstance.h"

CParticleBossWalk::CParticleBossWalk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CParticle(pDevice, pContext)
{
}

CParticleBossWalk::CParticleBossWalk(const CParticleBossWalk& Prototype)
    :
    CParticle(Prototype)
{

}

HRESULT CParticleBossWalk::InitializePrototype()
{
    return S_OK;
}

HRESULT CParticleBossWalk::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    return S_OK;
}

void CParticleBossWalk::PriorityUpdate(_float fTimeDelta)
{
}

void CParticleBossWalk::Update(_float fTimeDelta)
{
    if (IsActivate())
    {
        _vector vPos = static_cast<CTransform*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_BOSS, LEVEL_BOSS, TEXT("Layer_Boss"))->GetComponent(TEXT("Com_Transform")))->GetTransformState(CTransform::STATE_POS);
        m_pTransformCom->SetTransformState(CTransform::STATE_POS, vPos);

        m_pVIBufferCom->SpreadParticles(fTimeDelta);

        if (false == m_pVIBufferCom->IsActivate())
            DeActivate();
    }
}

void CParticleBossWalk::LateUpdate(_float fTimeDelta)
{
    if (IsActivate())
        m_pGameInst->AddRenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CParticleBossWalk::Render()
{
    if (IsActivate())
    {
        if (FAILED(BindShaderResources()))
            return E_FAIL;

        m_pShaderCom->Begin(4); // 패스 설정

        m_pVIBufferCom->BindBufferDesc();

        m_pVIBufferCom->Render();
    }

    return S_OK;
}

HRESULT CParticleBossWalk::ReadyComponents()
{
    /* Com_Texture */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_BossWalk"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPointInstance"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Particle_BossWalk"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticleBossWalk::BindShaderResources()
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

CParticleBossWalk* CParticleBossWalk::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParticleBossWalk* pInstance = new CParticleBossWalk(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CParticleBossWalk");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CParticleBossWalk::Clone(void* pArg)
{
    CParticleBossWalk* pInstance = new CParticleBossWalk(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CParticleBossWalk");
        SafeRelease(pInstance);
    }

    return pInstance; return nullptr;
}

void CParticleBossWalk::Free()
{
    __super::Free();
}
