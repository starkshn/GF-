#include "pch.h"
#include "UITrigger.h"
#include "GameInstance.h"
#include "Layer.h"
#include "UITexPerspective.h"
#include "MonsterManager.h"
#include "EffectManager.h"

CUITrigger::CUITrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CTrigger(pDevice, pContext)
{
}

CUITrigger::CUITrigger(const CUITrigger& Prototype)
    :
    CTrigger(Prototype)
{
}

HRESULT CUITrigger::InitializePrototype()
{
    return S_OK;
}

HRESULT CUITrigger::Initialize(void* pArg)
{
    if (!pArg)
        return E_FAIL;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents(pArg)))
        return E_FAIL;

    return S_OK;
}

void CUITrigger::PriorityUpdate(_float fTimeDelta)
{
}

void CUITrigger::Update(_float fTimeDelta)
{
    m_pColliderCom->Update(m_pTransformCom->GetWorldMatrix());
}

void CUITrigger::LateUpdate(_float fTimeDelta)
{
    static _uint iSoundID = 0;
    static _bool bOnce = 1;


    m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);

    if (m_pColliderCom->Intersect(GetTargetCollider()))
    {
        if (m_bNotUse)
            return;

        string strTargetInterMoveObjName = "";
        if (GetObjectTag() == "UITrigger0")
            strTargetInterMoveObjName = "InterMove0";
        else if (GetObjectTag() == "UITrigger1")
            strTargetInterMoveObjName = "InterMove1";
        else if (GetObjectTag() == "UITrigger2")
            strTargetInterMoveObjName = "InterMove2";

        auto pLayer = m_pGameInst->FindLayer(m_iLevelIndex, TEXT("Layer_InterMove"));
        if (pLayer)
        {
            auto pObjs = pLayer->GetGameObjectList();
            if (!pObjs.empty())
            {
                auto iter = pObjs.begin();
                for (; iter != pObjs.end(); ++iter)
                    if ((*iter)->GetObjectTag() == strTargetInterMoveObjName)
                        break;

                if (iter != pObjs.end())
                {
                    auto pUITexPers = static_cast<CUITexPerspective*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, TEXT("Layer_UI_Perspetive")));
                    if (pUITexPers)
                    {
                        pUITexPers->ActiveRender();
                        pUITexPers->SetTargetObj(*iter);
                        m_bActiveUI = true;

                        CMonsterManager::GetInstance()->SetUpdateDoor(true);


                        auto pComInterMove = static_cast<CTransform*>((*iter)->GetComponent(TEXT("Com_Transform")));
                        _vector vPosInterMove = pComInterMove->GetTransformState(CTransform::STATE_POS);
                        HIT_RESULT Hit = {};
                        XMStoreFloat3(&Hit.vHitPos, vPosInterMove);


                        if (GetObjectTag() == "UITrigger0")
                        {
                            _vector vRight = pComInterMove->GetTransformState(CTransform::STATE_LOOK);
                            XMStoreFloat3(&Hit.vNormal, vRight);
                        }
                        else if (GetObjectTag() == "UITrigger1")
                        {
                            _vector vRight = pComInterMove->GetTransformState(CTransform::STATE_RIGHT);
                            XMStoreFloat3(&Hit.vNormal, vRight);
                        }
                        else if (GetObjectTag() == "UITrigger2")
                        {
                            _vector vRight = pComInterMove->GetTransformState(CTransform::STATE_RIGHT);
                            XMStoreFloat3(&Hit.vNormal, vRight);
                        }

                        CEffectManager::GetInstance()->ActiveEffect(CEffectManager::EFFECT_DOOR, Hit);
                    }
                }
            }

            if (bOnce)
            {
                bOnce = 0;
                m_pGameInst->PlayEffect(TEXT("DoorActive.mp3"), 0.8f);
                iSoundID = m_pGameInst->GetNowEffectID();
            }
        }
    }
    else
    {
        if (m_bActiveUI)
        {
            auto pUITexPers = static_cast<CUITexPerspective*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, TEXT("Layer_UI_Perspetive")));
            if (pUITexPers)
            {
                pUITexPers->DeActiveRender();
                pUITexPers->SetTargetObj(nullptr);
                m_bActiveUI = false;

                CMonsterManager::GetInstance()->SetUpdateDoor(false);
                CEffectManager::GetInstance()->DeActiveEffect(CEffectManager::EFFECT_DOOR);

                if (bOnce == 0)
                {
                    m_pGameInst->StopSound(iSoundID);
                    bOnce = 1;
                }
            }
        }
    }
}

HRESULT CUITrigger::Render()
{
    // m_pColliderCom->Render();
    return S_OK;
}

void CUITrigger::ActiveUIsAndEffects()
{
}

void CUITrigger::DeActiveUIsAndEffects()
{
}

HRESULT CUITrigger::ReadyComponents()
{
    CBoundingAABB::BOUND_AABB_DESC		ColliderDesc{};
    ColliderDesc.vExtents = _float3(0.6f, 1.3f, 5.f);

    _vector vPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);
    ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUITrigger::ReadyComponents(void* pArg)
{
    if (nullptr != pArg)
    {
        TRIGGER_DESC* pDesc = static_cast<CTrigger::TRIGGER_DESC*>(pArg);
        CBoundingAABB::BOUND_AABB_DESC		ColliderDesc{};
        ColliderDesc.vExtents = pDesc->vExtents;
        if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
            return E_FAIL;
    }

    return S_OK;
}

CUITrigger* CUITrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUITrigger* pInstance = new CUITrigger(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CUITrigger");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CUITrigger::Clone(void* pArg)
{
    CUITrigger* pInstance = new CUITrigger(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CUITrigger");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CUITrigger::Free()
{
    __super::Free();
}
