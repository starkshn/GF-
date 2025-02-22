#include "pch.h"
#include "GameInstance.h"
#include "MonsterManager.h"
#include "Layer.h"
#include "MonsterBase.h"
#include "EffectManager.h"
#include "ParticleManager.h"
#include "Projectile.h"
#include "PlayerBullet.h"
#include "UIParent.h"
#include "UIPanel.h"
#include "UIElement.h"
#include "UIFont.h"
#include "UIHPBar.h"
#include "UIBG.h"
#include "UITex.h"
#include "Trigger.h"

IMPLEMENT_SINGLETON(CMonsterManager);


CMonsterManager::CMonsterManager()
    :
    m_pGameInst(CGameInstance::GetInstance())
{

}

HRESULT CMonsterManager::Initialize()
{
    return S_OK;
}

void CMonsterManager::Update(_float fTimeDelta)
{
    UpdateMonsterLayers();
    UpdatePlayerBulletLayers();
    UpdateMonsterHPUI();
    UpdateMiniMapDoor();



    static _float fAcc = 0.f;
    if (!m_bDeadSoundOnce)
    {
        fAcc += fTimeDelta;
        if (fAcc > 0.5f)
        {
            m_bDeadSoundOnce = true;
            fAcc = 0.f;
        }
    }
}



void CMonsterManager::FindAndSetLayers()
{
    _uint iLevelID = m_pGameInst->GetCurrentLevelIndex();

    for (_int i = 0; i < 6; ++i)
        if (nullptr == m_pMonsterLayers[i])
            m_pMonsterLayers[i] = m_pGameInst->FindLayer(iLevelID, m_strLayerNames[i]);

    if (nullptr == m_pBossLayer)
        m_pBossLayer = m_pGameInst->FindLayer(iLevelID, m_strBossLayer);
}

void CMonsterManager::UpdateMonsterLayers()
{
    RemoveMonsterFromLayer();
    FindAndSetLayers();
    CheckLayerSizes();
    FindDeadMonster();

}

void CMonsterManager::UpdatePlayerBulletLayers()
{
   RemoveBulletFromLayer();
   CheckBulletLayerSizes();
   FindAndSetBulletLayers();
   FindDeadBullet();
}

void CMonsterManager::UpdateMonsterHPUI()
{
    RemoveUIFromLayer();
    CheckUILayerSizes();
    FindAndSetUILayers();
    FindDeadMonsters();
}

void CMonsterManager::CheckLayerSizes()
{
    for (_int i = 0; i < iLoop; ++i)
    {
        if (nullptr != m_pMonsterLayers[i])
        {
            if (0 == m_pMonsterLayers[i]->GetGameObjectList().size())
                m_pMonsterLayers[i] = nullptr;
        }
    }

    if (nullptr != m_pBossLayer)
        if (0 == m_pBossLayer->GetGameObjectList().size())
            m_pBossLayer = nullptr;
}

// UIMiniMapBG
void CMonsterManager::FindDeadMonster()
{
    static vector<_vector> vWorldPoses{};

    for (_int i = 0; i < iLoop; ++i)
    {
        if (nullptr != m_pMonsterLayers[i])
        {
            auto Objs = m_pMonsterLayers[i]->GetGameObjectList();
            for (auto pObj : Objs)
            {
                CMonsterBase* pMonster = static_cast<CMonsterBase*>(pObj);
                if (nullptr != pMonster && pMonster->IsDead())
                {
                    pMonster->SetRemoveFromLayerNextTick();
                    
                    HIT_RESULT Hit = {};
                    Hit.vHitPos = {};
                    _vector vPos = static_cast<CTransform*>(pObj->GetComponent(TEXT("Com_Transform")))->GetTransformState(CTransform::STATE_POS);
                    XMStoreFloat3(&Hit.vHitPos, vPos);
                    CParticleManager::GetInstance()->ActiveParticle(CParticleManager::MONSTER_DEAD, Hit);


                    if (m_bDeadSoundOnce)
                    {
                        m_bDeadSoundOnce = false;
                        // m_pGameInst->PlayEffect(TEXT("MonsterDead.mp3"), 1.f);
                    }
                }

                // 안 죽은 경우
                if (nullptr != pMonster && !pMonster->IsDead() && !pMonster->GetRemoveFromLayerNextTick())
                {
                    _int iCurLevel = m_pGameInst->GetCurrentLevelIndex();
                    CTransform* pCom = static_cast<CTransform*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, iCurLevel, TEXT("A"))->GetComponent(TEXT("Com_Transform")));
                    CTransform* pMonCom = static_cast<CTransform*>(pMonster->GetComponent(TEXT("Com_Transform")));


                    if (pCom && pMonCom)
                    {
                        const static _float fSCX = 125.f + 30.f;
                        const static _float fSCY = 125.f + 30.f;

                        const static _float fMinSX = fSCX - 80.f;
                        const static _float fMaxSX = fSCX + 80.f;
                        const static _float fMinSY = fSCY - 80.f;
                        const static _float fMaxSY = fSCY + 80.f;

                        _vector vPlayerPos = pCom->GetTransformState(CTransform::STATE_POS);
                        _vector vMonsterPos = pMonCom->GetTransformState(CTransform::STATE_POS);

                        _float fDistX = XMVectorGetX(vMonsterPos) - XMVectorGetX(vPlayerPos);
                        _float fDistZ = XMVectorGetZ(vMonsterPos) - XMVectorGetZ(vPlayerPos);

                        // 월드에서 미니맵이 표현할 최대 거리
                        _float fWorldRange = 30.f;

                        _float fMiniMapX = fSCX + (fDistX / fWorldRange) * (250.f * 0.5f);
                        _float fMiniMapY = fSCY - (fDistZ / fWorldRange) * (250.f * 0.5f);

                        if (fMiniMapX < fMinSX || fMiniMapX > fMaxSX || fMiniMapY < fMinSY || fMiniMapY > fMaxSY)
                            continue;

                        D3D11_VIEWPORT  ViewPortDesc = m_pGameInst->GetViewPortDesc();

                        _vector vNewWorldPos = { fMiniMapX - ViewPortDesc.Width * 0.5f, -fMiniMapY + ViewPortDesc.Height * 0.5f, 0.0f, 1.f };

                        vWorldPoses.push_back(vNewWorldPos);
                    }
                    else
                    {
                        MSG_BOX("FindDeadMonster Player nullptr");
                        return;
                    }
                }
            }
        }
    }

    CUIPanel* pPanel = m_pGameInst->GetPanelUI("UIMiniMapBG");
    if (pPanel)
    {
        pPanel->SetTypeMiniMap();
        pPanel->SetEleWorldPoses(vWorldPoses);
        vWorldPoses.clear();
    }


    if (nullptr != m_pBossLayer)
    {
        _int iCurLevel = m_pGameInst->GetCurrentLevelIndex();

        auto Objs = m_pBossLayer->GetGameObjectList();
        auto pBoss = Objs.back();

        _vector vBossPos = {}, vPlayerPos = {};

        if (pBoss)
        {
            vBossPos = static_cast<CTransform*>(pBoss->GetComponent(TEXT("Com_Transform")))->GetTransformState(CTransform::STATE_POS);
        }

        CTransform* pCom = static_cast<CTransform*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, iCurLevel, TEXT("A"))->GetComponent(TEXT("Com_Transform")));
        vPlayerPos = pCom->GetTransformState(CTransform::STATE_POS);

        const static _float fSCX = 125.f + 30.f;
        const static _float fSCY = 125.f + 30.f;

        const static _float fMinSX = fSCX - 85.f;
        const static _float fMaxSX = fSCX + 85.f;
        const static _float fMinSY = fSCY - 85.f;
        const static _float fMaxSY = fSCY + 85.f;

        _float fDistX = XMVectorGetX(vBossPos) - XMVectorGetX(vPlayerPos);
        _float fDistZ = XMVectorGetZ(vBossPos) - XMVectorGetZ(vPlayerPos);

        // 월드에서 미니맵이 표현할 최대 거리
        _float fWorldRange = 30.f;

        _float fMiniMapX = fSCX + (fDistX / fWorldRange) * (250.f * 0.5f);
        _float fMiniMapY = fSCY - (fDistZ / fWorldRange) * (250.f * 0.5f);

        if (fMiniMapX < fMinSX || fMiniMapX > fMaxSX || fMiniMapY < fMinSY || fMiniMapY > fMaxSY)
        {
            if (pPanel)
                pPanel->SetBossPos(_vector{}, false);
            return;
        }

        D3D11_VIEWPORT  ViewPortDesc = m_pGameInst->GetViewPortDesc();

        _vector vNewWorldPos = { fMiniMapX - ViewPortDesc.Width * 0.5f, -fMiniMapY + ViewPortDesc.Height * 0.5f, 0.0f, 1.f };

        vWorldPoses.push_back(vNewWorldPos);

        if (pPanel)
            pPanel->SetBossPos(vWorldPoses.back(), true);
        vWorldPoses.clear();
    }

}

void CMonsterManager::RemoveMonsterFromLayer()
{
    for (_int i = 0; i < iLoop; ++i)
    {
        if (nullptr != m_pMonsterLayers[i])
        {
            auto Objs = m_pMonsterLayers[i]->GetGameObjectList();
            for (auto pObj : Objs)
            {
                if (pObj && pObj->GetRemoveFromLayerNextTick())
                {
                    _uint iLevelID = m_pGameInst->GetCurrentLevelIndex();
                    m_pGameInst->RemoveObject(iLevelID, pObj);
                }
            }
        }
    }

    //if (nullptr != m_pBossLayer)
    //{
    //    auto Objs = m_pBossLayer->GetGameObjectList();
    //    for (auto pObj : Objs)
    //    {
    //        if (pObj && pObj->GetRemoveFromLayerNextTick())
    //        {
    //            _uint iLevelID = m_pGameInst->GetCurrentLevelIndex();
    //            m_pGameInst->RemoveObject(iLevelID, pObj);
    //        }
    //    }
    //}
}

void CMonsterManager::CheckBulletLayerSizes()
{
    for (_int i = 0; i < 2; ++i)
    {
        if (nullptr != m_pBulletLayers[i])
        {
            if (0 == m_pBulletLayers[i]->GetGameObjectList().size())
                m_pBulletLayers[i] = nullptr;
        }
    }
}

void CMonsterManager::FindAndSetBulletLayers()
{
    _uint iLevelID = m_pGameInst->GetCurrentLevelIndex();

    for (_int i = 0; i < 2; ++i)
        if (nullptr == m_pBulletLayers[i])
            m_pBulletLayers[i] = m_pGameInst->FindLayer(iLevelID, m_strBulletLayerNames[i]);
}

void CMonsterManager::FindDeadBullet()
{
    for (_int i = 0; i < 2; ++i)
    {
        if (nullptr != m_pBulletLayers[i])
        {
            auto Objs = m_pBulletLayers[i]->GetGameObjectList();
            for (auto pObj : Objs)
            {
                CPlayerBullet* pBullet = static_cast<CPlayerBullet*>(pObj);
                if (nullptr != pBullet)
                {
                    CCollider* pCol = static_cast<CCollider*>(pBullet->GetComponent(TEXT("Com_Collider")));
                    if (pCol->IsEndCollision())
                    {
                        _uint iLevelID = m_pGameInst->GetCurrentLevelIndex();
                        m_pGameInst->RemoveObject(iLevelID, pObj);
                    }
                }
            }
        }
    }
}

void CMonsterManager::RemoveBulletFromLayer()
{
    for (_int i = 0; i < 2; ++i)
    {
        if (nullptr != m_pBulletLayers[i])
        {
            auto Objs = m_pBulletLayers[i]->GetGameObjectList();
            for (auto pObj : Objs)
            {
                if (pObj && pObj->GetRemoveFromLayerNextTick())
                {
                    _uint iLevelID = m_pGameInst->GetCurrentLevelIndex();
                    m_pGameInst->RemoveObject(iLevelID, pObj);
                }
            }
        }
    }
}

void CMonsterManager::CheckUILayerSizes()
{
    for (_int i = 0; i < m_iLayerSize; ++i)
    {
        if (nullptr != m_pUILayers[i])
        {
            if (0 == m_pUILayers[i]->GetGameObjectList().size())
                m_pUILayers[i] = nullptr;
        }
    }
}

void CMonsterManager::FindAndSetUILayers()
{
    _uint iLevelID = m_pGameInst->GetCurrentLevelIndex();

    for (_int i = 0; i < m_iLayerSize; ++i)
    {
        if (nullptr == m_pUILayers[i])
        {
            m_pUILayers[i] = m_pGameInst->FindLayer(iLevelID, m_strUILayers[i]);
            if (m_pUILayers[i])
            {
                _int a = 10;
            }
        }
    }
}

void CMonsterManager::FindDeadMonsters()
{
    for (_int i = 0; i < m_iLayerSize; ++i)
    {
        if (nullptr != m_pUILayers[i])
        {
            auto Objs = m_pUILayers[i]->GetGameObjectList();
            for (auto pObj : Objs)
            {
                CUIParent* pUI = static_cast<CUIParent*>(pObj);
                if (nullptr != pUI)
                {
                    if (CGameObject* pUIOwner = pUI->GetUIOwner())
                    {
                        if (pUIOwner->GetRemoveFromLayerNextTick() && !pUI->GetRemoveFromLayerNextTick())
                        {
                            // SafeRelease(pUIOwner);
                            pUI->SetUIOwner(nullptr);
                            pUI->SetRemoveFromLayerNextTick();
                        }
                    }
                }
            }
        }
    }
}

void CMonsterManager::RemoveUIFromLayer()
{

}

void CMonsterManager::UpdateMiniMapDoor()
{
    static vector<_vector> vWorldPoses{};

    if (m_bActiveUpdate)
    {
        _uint iLevel = m_pGameInst->GetCurrentLevelIndex();
        auto pObj = m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, iLevel, TEXT("A"));
        if (pObj)
        {
            CTransform* pCom = static_cast<CTransform*>(pObj->GetComponent(TEXT("Com_Transform")));
            _vector vPlayerWorldPos = pCom->GetTransformState(CTransform::STATE_POS);


            if (pCom)
            {
                _float fMax = 99999999.f;

                
                // Layer Loop
                auto pLayer = m_pGameInst->FindLayer(iLevel, TEXT("Layer_InterMove"));
                if (pLayer)
                {
                    auto pObjs = pLayer->GetGameObjectList();
                    auto BreakIter = pObjs.begin();
                    if (!pObjs.empty())
                    {
                        for (auto iter = pObjs.begin(); iter != pObjs.end(); ++iter)
                        {
                            if (*iter)
                            {
                                auto pInterMoveCom = static_cast<CTransform*>((*iter)->GetComponent(TEXT("Com_Transform")));
                                _vector vInterMoveWorldPos = pInterMoveCom->GetTransformState(CTransform::STATE_POS);
                                
                                _float fDist = XMVectorGetX(XMVector3Length(vPlayerWorldPos - vInterMoveWorldPos));
                                if (fDist < fMax)
                                {
                                    BreakIter = iter;
                                    fMax = fDist;
                                }
                            }
                        }
                    }

                    
                    {
                        CUIPanel* pPanel = m_pGameInst->GetPanelUI("UIMiniMapBG");
                        if (!pPanel)
                        {
                            MSG_BOX("UpdateMiniMapDoor Error");
                        }

                        auto pCom = static_cast<CTransform*>((*BreakIter)->GetComponent(TEXT("Com_Transform")));
                        _vector vDoorMinWorldPos = pCom->GetTransformState(CTransform::STATE_POS);

                        const static _float fSCX = 125.f + 30.f;
                        const static _float fSCY = 125.f + 30.f;

                        const static _float fMinSX = fSCX - 80.f;
                        const static _float fMaxSX = fSCX + 80.f;
                        const static _float fMinSY = fSCY - 80.f;
                        const static _float fMaxSY = fSCY + 80.f;

                        _float fDistX = XMVectorGetX(vDoorMinWorldPos) - XMVectorGetX(vPlayerWorldPos);
                        _float fDistZ = XMVectorGetZ(vDoorMinWorldPos) - XMVectorGetZ(vPlayerWorldPos);

                        // 월드에서 미니맵이 표현할 최대 거리
                        _float fWorldRange = 30.f;

                        _float fMiniMapX = fSCX + (fDistX / fWorldRange) * (250.f * 0.5f);
                        _float fMiniMapY = fSCY - (fDistZ / fWorldRange) * (250.f * 0.5f);

                        if (fMiniMapX < fMinSX || fMiniMapX > fMaxSX || fMiniMapY < fMinSY || fMiniMapY > fMaxSY)
                        {
                            return;
                        }

                        D3D11_VIEWPORT  ViewPortDesc = m_pGameInst->GetViewPortDesc();
                        _vector vNewWorldPos = { fMiniMapX - ViewPortDesc.Width * 0.5f, -fMiniMapY + ViewPortDesc.Height * 0.5f, 0.0f, 1.f };

                        vWorldPoses.push_back(vNewWorldPos);

                        if (pPanel)
                            pPanel->SetDoorPos(vWorldPoses.back(), true);
                        vWorldPoses.clear();
                    }

                }
            }
            
        }
    }
    else
    {
        CUIPanel* pPanel = m_pGameInst->GetPanelUI("UIMiniMapBG");
        if (pPanel)
        {
            if (pPanel)
                pPanel->SetDoorPos(_vector{}, false);
        }
    }
}

void CMonsterManager::RemoveAllMonsters()
{
    for (_int i = 0; i < iLoop; ++i)
    {
        if (nullptr != m_pMonsterLayers[i])
        {
            auto Objs = m_pMonsterLayers[i]->GetGameObjectList();
            for (auto pObj : Objs)
            {
                CMonsterBase* pMonster = static_cast<CMonsterBase*>(pObj);
                if (nullptr != pMonster && !pMonster->IsDead())
                {
                    if (pMonster->GetObjectTag() == "Boss")
                        continue;

                    pMonster->ForceDead();
                    // pMonster->SetRemoveFromLayerNextTick();
                }
            }
        }
    }
}

void CMonsterManager::Clear()
{
    for (_int i = 0; i < 6; ++i)
        m_pMonsterLayers[i] = nullptr;

    for (_int i = 0; i < 2; ++i)
    {
        if (nullptr != m_pBulletLayers[i])
        {
            auto Objs = m_pBulletLayers[i]->GetGameObjectList();
            for (auto pObj : Objs)
            {
                CPlayerBullet* pBullet = static_cast<CPlayerBullet*>(pObj);
                if (nullptr != pBullet)
                {
                    _uint iLevelID = m_pGameInst->GetCurrentLevelIndex();
                    m_pGameInst->RemoveObject(iLevelID, pObj);   
                }
            }

            Objs.clear();
        }

        m_pBulletLayers[i] = nullptr;
    }

    for (_int i = 0; i < 1; ++i)
    {
        m_pUILayers[i] = nullptr;
    }



    CUIPanel* pPanel = m_pGameInst->GetPanelUI("UIMiniMapBG");
    if (pPanel)
    {
        if (pPanel)
            pPanel->SetDoorPos(_vector{}, false);
    }

    {
        auto pLayer = m_pGameInst->FindLayer(LEVEL_GAMEPLAY, TEXT("Layer_S1UITrigger"));
        if (pLayer)
        {
            auto Objs = pLayer->GetGameObjectList();
            for (auto pObj : Objs)
            {
                static_cast<CTrigger*>(pObj)->SetNotUse();
            }
        }
    }

    {
        auto pLayer = m_pGameInst->FindLayer(LEVEL_GAMEPLAY, TEXT("Layer_S2UITrigger"));
        if (pLayer)
        {
            auto Objs = pLayer->GetGameObjectList();
            for (auto pObj : Objs)
            {
                static_cast<CTrigger*>(pObj)->SetNotUse();
            }
        }
    }

    {
        auto pLayer = m_pGameInst->FindLayer(LEVEL_GAMEPLAY, TEXT("Layer_S3UITrigger"));
        if (pLayer)
        {
            auto Objs = pLayer->GetGameObjectList();
            for (auto pObj : Objs)
            {
                static_cast<CTrigger*>(pObj)->SetNotUse();
            }
        }
    }



    // m_pUILayers
}

void CMonsterManager::Free()
{
    __super::Free();
}
