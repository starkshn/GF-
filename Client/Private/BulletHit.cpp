#include "pch.h"
#include "BulletHit.h"
#include "GameInstance.h"

CBulletHit::CBulletHit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CEffect(pDevice, pContext)
{
}

CBulletHit::CBulletHit(const CBulletHit& Prototype)
    :
    CEffect(Prototype)
{
}

HRESULT CBulletHit::InitializePrototype()
{
    return S_OK;
}

HRESULT CBulletHit::Initialize(void* pArg)
{
    if (!pArg)
        return E_FAIL;

    m_eHitType = static_cast<BULLET_HIT_DESC*>(pArg)->eHitType;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    return S_OK;
}

void CBulletHit::PriorityUpdate(_float fTimeDelta)
{

}

void CBulletHit::Update(_float fTimeDelta)
{
    if (IsActive())
    {
        __super::Update(fTimeDelta);
        BillBoard();

        switch (m_eHitType)
        {
        case Client::BULLET_HIT_TYPE::PLAYER01:
            break;
        case Client::BULLET_HIT_TYPE::PLAYER02:
            break;
        case Client::BULLET_HIT_TYPE::PLAYER03:
            m_pTransformCom->Scaling(3.f, 3.f, 3.f);
            break;
        case Client::BULLET_HIT_TYPE::PLAYER04:
            m_pTransformCom->Scaling(2.f, 2.f, 2.f);
            break;
        case Client::BULLET_HIT_TYPE::TYPE_END:
            break;
        default:
            break;
        }
    }
}

void CBulletHit::LateUpdate(_float fTimeDelta)
{
    if (IsActive())
        m_pGameInst->AddRenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CBulletHit::Render()
{
    if (IsActive())
    {
        if (FAILED(BindShaderResources()))
            return E_FAIL;

        switch (m_eHitType)
        {
        case Client::BULLET_HIT_TYPE::PLAYER01:
            break;
        case Client::BULLET_HIT_TYPE::PLAYER02:
            break;
        case Client::BULLET_HIT_TYPE::PLAYER03:
            m_pShaderCom->Begin(16);
            break;
        case Client::BULLET_HIT_TYPE::PLAYER04:
            m_pShaderCom->Begin(17);
            break;
        case Client::BULLET_HIT_TYPE::TYPE_END:
            break;
        default:
            break;
        }

        m_pVIBufferCom->BindBufferDesc();

        m_pVIBufferCom->Render();
    }


    return S_OK;
}

HRESULT CBulletHit::ReadyComponents()
{
    /* Com_Shader */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    // Tex
    {
        switch (m_eHitType)
        {
        case Client::BULLET_HIT_TYPE::PLAYER01:
            break;
        case Client::BULLET_HIT_TYPE::PLAYER02:
            break;
        case Client::BULLET_HIT_TYPE::PLAYER03:
        {
            if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_BulletHit03"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
                return E_FAIL;
        }

            break;
        case Client::BULLET_HIT_TYPE::PLAYER04:
        {
            if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_BulletHit04"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
                return E_FAIL;
        }

            break;
        case Client::BULLET_HIT_TYPE::TYPE_END:
            break;
        default:
            break;
        }
    }


    return S_OK;
}

HRESULT CBulletHit::BindShaderResources()
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

CBulletHit* CBulletHit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBulletHit* pInstance = new CBulletHit(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CBulletHit");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CBulletHit::Clone(void* pArg)
{
    CBulletHit* pInstance = new CBulletHit(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CBulletHit");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CBulletHit::Free()
{
    __super::Free();
}
