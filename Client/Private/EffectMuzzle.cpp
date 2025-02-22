#include "pch.h"
#include "EffectMuzzle.h"
#include "GameInstance.h"
#include "Player.h"

CEffectMuzzle::CEffectMuzzle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CEffect(pDevice, pContext)
{
}

CEffectMuzzle::CEffectMuzzle(const CEffectMuzzle& Prototype)
    :
    CEffect(Prototype)
{
}

HRESULT CEffectMuzzle::InitializePrototype()
{
    return S_OK;
}

HRESULT CEffectMuzzle::Initialize(void* pArg)
{
    if (pArg)
        m_eMuzzleType = static_cast<MUZZLE_EFFECT_DESC*>(pArg)->eMuzzleType;
    else
        return E_FAIL;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    return S_OK;
}

void CEffectMuzzle::PriorityUpdate(_float fTimeDelta)
{

}

void CEffectMuzzle::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    if (IsActive())
    {
        BillBoard();

        // m_pTransformCom->Scaling(3.5f, 30.f, 2.f);

        /*m_fAccScale += fTimeDelta * m_fSP;
        m_fAccScale = min(m_fMaxScale, m_fAccScale);
        m_pTransformCom->Scaling(m_fAccScale, m_fAccScale, m_fAccScale);*/
    }
    else
    {
        // m_fAccScale = 0.f;
    }

}

void CEffectMuzzle::LateUpdate(_float fTimeDelta)
{
    if (IsActive())
    {
        m_pGameInst->AddRenderObject(CRenderer::RG_GLOW, this);
    }
}

HRESULT CEffectMuzzle::Render()
{
    if (IsActive())
    {
        if (FAILED(BindShaderResources()))
            return E_FAIL;


        auto pObj = m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GAMEPLAY, TEXT("A"));
        if (pObj)
        {
            CPlayer* pPlayer = static_cast<CPlayer*>(pObj);
            if (pPlayer)
            {
                _uint iModel = pPlayer->GetCurrentModelIndex();
                if (iModel == 1)
                {
                    int a = 10;
                    m_pShaderCom->Begin(18);

                }
                else
                {
                    m_pShaderCom->Begin(15);
                }
            }
        }
        else
        {
            auto pObj = m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_BOSS, TEXT("A"));
            if (pObj)
            {
                CPlayer* pPlayer = static_cast<CPlayer*>(pObj);
                if (pPlayer)
                {
                    _uint iModel = pPlayer->GetCurrentModelIndex();
                    if (iModel == 1)
                    {
                        m_pShaderCom->Begin(18);

                    }
                    else
                    {

                        m_pShaderCom->Begin(15);

                    }
                }
            }
        }

        // m_pShaderCom->Begin(15);

        m_pVIBufferCom->BindBufferDesc();

        m_pVIBufferCom->Render();
    }


    return S_OK;
}

HRESULT CEffectMuzzle::ReadyComponents()
{
    /* Com_Shader */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    // MuzzleEffect Tex
    {
        switch (m_eMuzzleType)
        {
        case Client::MUZZLE_EFFECT_TYPE::PLAYER01:
        {
            if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_Player01Muzzle"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
                return E_FAIL;
        }
            break;
        case Client::MUZZLE_EFFECT_TYPE::PLAYER02:
        {
            if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_Player02Muzzle"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
                return E_FAIL;
        }
            break;
        case Client::MUZZLE_EFFECT_TYPE::PLAYER03:
        {
            if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_Player03Muzzle"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
                return E_FAIL;
        }
            break;
        case Client::MUZZLE_EFFECT_TYPE::PLAYER04:
        {
            if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_Player04Muzzle"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
                return E_FAIL;
        }
            break;
        case Client::MUZZLE_EFFECT_TYPE::TYPE_END:
            break;
        default:
            break;
        }
    }

    return S_OK;
}

HRESULT CEffectMuzzle::BindShaderResources()
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

CEffectMuzzle* CEffectMuzzle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEffectMuzzle* pInstance = new CEffectMuzzle(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CEffectMuzzle");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CEffectMuzzle::Clone(void* pArg)
{
    CEffectMuzzle* pInstance = new CEffectMuzzle(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CEffectMuzzle");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CEffectMuzzle::Free()
{
    __super::Free();
}
