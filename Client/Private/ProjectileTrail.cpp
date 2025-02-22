#include "pch.h"
#include "ProjectileTrail.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "PartObject.h"

CProjectileTrail::CProjectileTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CEffectTrail(pDevice, pContext)
{

}

CProjectileTrail::CProjectileTrail(const CProjectileTrail& Prototype)
    :
    CEffectTrail(Prototype)
{

}

HRESULT CProjectileTrail::InitializePrototype()
{
    return S_OK;
}

HRESULT CProjectileTrail::Initialize(void* pArg)
{
    if (pArg)
        m_eTrailType = static_cast<PROJECTILE_TRAIL_DESC*>(pArg)->eTrailType;
    else
        return E_FAIL;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    return S_OK;
}

void CProjectileTrail::PriorityUpdate(_float fTimeDelta)
{

}

void CProjectileTrail::Update(_float fTimeDelta)
{
    if (IsActive())
    {
        __super::Update(fTimeDelta);

        if (!m_pOwner)
        {
            DeActive();
            m_pVIBufferTrailCom->ResetTrailBuffers();
            return;
        }

        if (m_pOwner->GetRemoveFromLayerNextTick())
        {
            DeActive();
            m_pVIBufferTrailCom->ResetTrailBuffers();
            m_pOwner = nullptr;
            return;
        }

        if (m_pOwner)
        {
            if (CPartObject* pPartObj = dynamic_cast<CPartObject*>(m_pOwner))
            {
                _matrix MyWorld = m_pTransformCom->GetWorldMatrix();
                _matrix WorldMatrix = pPartObj->GetSocketWorldMatrix();
                m_pVIBufferTrailCom->AddVertices(MyWorld * WorldMatrix, fTimeDelta);
            }
            else if (CGameObject* pObj = dynamic_cast<CGameObject*>(m_pOwner))
            {
                CCollider* pCol = static_cast<CCollider*>(pObj->GetComponent(TEXT("Com_Collider")));
                if (pCol)
                {
                    if (pCol->IsEndCollision())
                    {
                        DeActive();
                        m_pVIBufferTrailCom->ResetTrailBuffers();
                        m_pOwner = nullptr;
                        return;
                    }
                    else
                    {
                        _matrix WorldMatrix = static_cast<CTransform*>(pObj->GetComponent(TEXT("Com_Transform")))->GetWorldMatrix();
                        m_pVIBufferTrailCom->AddVertices(WorldMatrix, fTimeDelta);
                    }
                }
            }
        }

        if (!IsActive())
        {
            m_pVIBufferTrailCom->ResetTrailBuffers();
            m_pOwner = nullptr;
        }
    }
}

void CProjectileTrail::LateUpdate(_float fTimeDelta)
{
    if (IsActive())
        m_pGameInst->AddRenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CProjectileTrail::Render()
{
    if (IsActive())
    {
        if (FAILED(BindShaderResources()))
            return E_FAIL;

        m_pShaderCom->Begin(0);

        m_pVIBufferTrailCom->BindBufferDesc();

        m_pVIBufferTrailCom->Render();
    }


    return S_OK;
}

HRESULT CProjectileTrail::ReadyComponents()
{
    /* Com_Shader */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex_Time"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    // TrailBuffer
    {
        CVIBufferTrail::TRAIL_DESC Desc = {};
        Desc.fTrailLength = 1.f;

        switch (m_eTrailType)
        {
        case Client::PROJECTILE_TRAIL_TYPE::PLAYER01:
        {
            Desc.iNumMaxVertices = 200;
            Desc.fOffset = 0.1f;
            Desc.fLifeTime = 0.25f;
        }
            break;
        case Client::PROJECTILE_TRAIL_TYPE::PLAYER02:
        {
            Desc.iNumMaxVertices = 200;
            Desc.fOffset = 0.15f;
            Desc.fLifeTime = 0.25f; // Æ®·¹ÀÏ ¾ËÆÄ ºí·»µù ½Ã°£, 0.25 Áö³ª¸é ¾ËÆÄºí·»µù µÊ.
        }
            break;
        case Client::PROJECTILE_TRAIL_TYPE::PLAYER03:
        {
            Desc.iNumMaxVertices = 80;
            Desc.fOffset = 0.15f;
            Desc.fLifeTime = 0.25f; // Æ®·¹ÀÏ ¾ËÆÄ ºí·»µù ½Ã°£, 0.25 Áö³ª¸é ¾ËÆÄºí·»µù µÊ.
        }
            break;
        case Client::PROJECTILE_TRAIL_TYPE::PLAYER04:
        {
            Desc.iNumMaxVertices = 200;
            Desc.fOffset = 0.15f;
            Desc.fLifeTime = 0.25f; // Æ®·¹ÀÏ ¾ËÆÄ ºí·»µù ½Ã°£, 0.25 Áö³ª¸é ¾ËÆÄºí·»µù µÊ.
        }
            break;
        case Client::PROJECTILE_TRAIL_TYPE::MONSTER03:
        {
            Desc.iNumMaxVertices = 50;
            Desc.fOffset = 0.3f;
            Desc.fLifeTime = 0.15f; // Æ®·¹ÀÏ ¾ËÆÄ ºí·»µù ½Ã°£, 0.25 Áö³ª¸é ¾ËÆÄºí·»µù µÊ.
        }
            break;
        case Client::PROJECTILE_TRAIL_TYPE::MONSTER04:
        {
            Desc.iNumMaxVertices = 50;
            Desc.fOffset = 0.3f;
            Desc.fLifeTime = 0.15f; // Æ®·¹ÀÏ ¾ËÆÄ ºí·»µù ½Ã°£, 0.25 Áö³ª¸é ¾ËÆÄºí·»µù µÊ.
        }
            break;
        case Client::PROJECTILE_TRAIL_TYPE::TYPE_END:
            break;
        default:
            break;
        }

        if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Trail"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferTrailCom), &Desc)))
            return E_FAIL;
    }

    // Trail Texture
    {
        switch (m_eTrailType)
        {
        case Client::PROJECTILE_TRAIL_TYPE::PLAYER01:
        {
            if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Player01Trail"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
                return E_FAIL;
        }
            break;
        case Client::PROJECTILE_TRAIL_TYPE::PLAYER02:
        {
            if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Player02Trail"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
                return E_FAIL;
        }
            break;
        case Client::PROJECTILE_TRAIL_TYPE::PLAYER03:
        {
            if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Player03Trail"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
                return E_FAIL;
        }
            break;
        case Client::PROJECTILE_TRAIL_TYPE::PLAYER04:
        {
            if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Player04Trail"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
                return E_FAIL;
        }
            break;
        case Client::PROJECTILE_TRAIL_TYPE::MONSTER03:
        {
            if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Monster03Trail"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
                return E_FAIL;
        }
            break;
        case Client::PROJECTILE_TRAIL_TYPE::MONSTER04:
        {
            if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Monster04Trail"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
                return E_FAIL;
        }
            break;
        case Client::PROJECTILE_TRAIL_TYPE::TYPE_END:
            break;
        default:
            break;
        }
    }

    return S_OK;
}

HRESULT CProjectileTrail::BindShaderResources()
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

    if (FAILED(m_pGameInst->BindRTShaderResource(m_pShaderCom, "g_DepthTexture", TEXT("TargetDepth"))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindRawValue("g_MaxLifeTime", &m_fMaxLifeTime, sizeof _float)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindRawValue("g_CurLifeTime", &m_fAccLifeTime, sizeof _float)))
        return E_FAIL;


    return S_OK;
}

CProjectileTrail* CProjectileTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CProjectileTrail* pInstance = new CProjectileTrail(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CProjectileTrail");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CProjectileTrail::Clone(void* pArg)
{
    CProjectileTrail* pInstance = new CProjectileTrail(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CProjectileTrail");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CProjectileTrail::Free()
{
    __super::Free();
}
