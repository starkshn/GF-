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
                    }
                }

                // 안 죽은 경우 (pMonster nullptr이 아니고 && 죽지 않았고 && 다음틱에 사라질 예정이 아닌 경우
                if (nullptr != pMonster && !pMonster->IsDead() && !pMonster->GetRemoveFromLayerNextTick())
                {
                    _int iCurLevel = m_pGameInst->GetCurrentLevelIndex();
                    CTransform* pPlayerTransformCom = static_cast<CTransform*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, iCurLevel, TEXT("A"))->GetComponent(TEXT("Com_Transform")));
                    CTransform* pMonsterTransformCom = static_cast<CTransform*>(pMonster->GetComponent(TEXT("Com_Transform")));

                    if (pPlayerTransformCom && pMonsterTransformCom)
                    {
                        // 미니맵 UI 객체의 스크린 좌표계 기준 중심 위치
                        // 미니맵 스크린 좌표계 중심점 : [ 155, 155 ]
                        const static _float fMiniMapCenterPosX = 125.f + 30.f;    // MiniMap 중심 ScreenX
                        const static _float fMiniMapCenterPosY = 125.f + 30.f;    // MiniMap 중심 ScreenY

                        const static _float fMiniMapMinPosX = fMiniMapCenterPosX - 80.f;   // MiniMap 최소 X 위치
                        const static _float fMiniMapMinPosY = fMiniMapCenterPosY - 80.f;   // MiniMap 최소 Y 위치

                        const static _float fMiniMapMaxPosX = fMiniMapCenterPosX + 80.f;   // MiniMap 최대 X 위치
                        const static _float fMiniMapMaxPosY = fMiniMapCenterPosY + 80.f;   // MiniMap 최대 Y 위치

                        // --- 플레이어와 몬스터의 월드 좌표 가져오기 ---
                        _vector vPlayerPos = pPlayerTransformCom->GetTransformState(CTransform::STATE_POS);
                        _vector vMonsterPos = pMonsterTransformCom->GetTransformState(CTransform::STATE_POS);

                        // --- 플레이어와 몬스터 간 거리 계산 ---
                        // 월드에서 몬스터와 플레이어의 상대 거리 (fDistX, fDistZ)
                        _float fDistX = XMVectorGetX(vMonsterPos) - XMVectorGetX(vPlayerPos);   // X 축 거리
                        _float fDistZ = XMVectorGetZ(vMonsterPos) - XMVectorGetZ(vPlayerPos);   // Z 축 거리 (Y가 아니라 Z사용)

                        // --- 월드에서 미니맵이 표현할 최대 거리 ---
                        const _float fWorldRange = 30.f; // 플레이어를 기준으로 반경 30까지의 몬스터 위치만 미니맵에 표시
                        
                        // --- 월드 좌표 -> 미니맵 좌표 변환 ---
                        // 미니맵 크기 (250x250) 중 절반을 기준으로 비율을 조정하여 위치를 변환한다.
                        // (fDistX / fWorldRange) : 몬스터가 플레이어 기준으로 얼마나 떨어져 있는지를 비율(정규화 된 값)으로 변환. fWorldRange 안에서는 -1.0 ~ 1.0 사이의 값이 나오게 됩니다.
                        // (m_fMiniMapSize * 0.5f) : 미니맵의 크기를 반으로 나눈 값(125)를 곱하여 미니맵에서의 상대적 좌표를 구합니다.
                        _float fInMiniMapPosX = fMiniMapCenterPosX + (fDistX / fWorldRange) * (m_fMiniMapSize * 0.5f);

                        // (fDistZ / fWorldRange) : Z축 거리도 X축과 같은 방식으로 정규화 합니다.
                        // (m_fMiniMapSize * 0.5f) : 마찬가지로 미니맵에서의 상대적인 거리 값을 구하기 위해 곱합니다.
                        // (fMiniMapCenterPosY - ) : Y축은 -를 붙여서 반전합니다. DX11 월드 좌표계에서는 Z축이 앞뒤 방향을 담당하는데 +Z는 앞으로 가는 방향 -Z는 뒤로가는 방향입니다.
                        // - 하지만 미니맵에서는 +Y 방향이 위쪽이 되어야 하므로, -fMiniMapCenterPosY - ... 로 Y축을 반전시켜줍니다.
                        _float fInMiniMapPosY = fMiniMapCenterPosY - (fDistZ / fWorldRange) * (m_fMiniMapSize * 0.5f);  // Y축은 반전(-) 시켜야 미니맵과 월드의 방향이 일치
                       
                        // 플레이어 (0, 0, 0), 몬스터 (10, 0, -15)인 경우
                        // 
                        // fInMiniMapPosX = 155 + (10 / 30) * (250 * 0.5)
                        // = 155 + (10 / 30) * 125
                        // = 155 + (1 / 3) * 125
                        // = 155 + 41.67
                        // = 196.67

                        // fInMiniMapPosY = 155 - (-15 / 30) * (250 * 0.5)
                        // = 155 - (-15 / 30) * 125
                        // = 155 - (-1 / 2) * 125
                        // = 155 + 62.5
                        // = 217.5

                        // --- 미니맵 내부에 존재하는지 확인 ---
                        if (fInMiniMapPosX < fMiniMapMinPosX || fInMiniMapPosX > fMiniMapMaxPosX || fInMiniMapPosY < fMiniMapMinPosY || fInMiniMapPosY > fMiniMapMaxPosY)  // 미니맵 범위를 벗어난 경우 표시하지 않음
                            continue;

                        // --- 현재 뷰포트(화면 크기) 정보 가져오기 ---
                        D3D11_VIEWPORT  ViewPortDesc = m_pGameInst->GetViewPortDesc();

                        // --- 미니맵 좌표를 화면 좌표계(World Space)로 변환 ---
                        // (뷰포트의 중심을 기준으로 위치를 조정)
                        _vector vNewWorldPos = 
                        { 
                            fInMiniMapPosX - ViewPortDesc.Width * 0.5f,      // X 좌표 이동
                            -fInMiniMapPosY + ViewPortDesc.Height * 0.5f,    // Y 좌표 반전 및 이동
                            0.0f,       // Z 좌표 (미니맵에서는 사용되지 않는다)
                            1.f         // W값
                        };


                        // 변환된 좌표를 저장 (추후 미니맵에 렌더링할 위치로 사용)
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

        const static _float fMiniMapMinPosX = fSCX - 85.f;
        const static _float fMaxSX = fSCX + 85.f;
        const static _float fMinSY = fSCY - 85.f;
        const static _float fMaxSY = fSCY + 85.f;

        _float fDistX = XMVectorGetX(vBossPos) - XMVectorGetX(vPlayerPos);
        _float fDistZ = XMVectorGetZ(vBossPos) - XMVectorGetZ(vPlayerPos);

        // 월드에서 미니맵이 표현할 최대 거리
        _float fWorldRange = 30.f;

        _float fMiniMapX = fSCX + (fDistX / fWorldRange) * (m_fMiniMapSize * 0.5f);
        _float fMiniMapY = fSCY - (fDistZ / fWorldRange) * (m_fMiniMapSize * 0.5f);

        if (fMiniMapX < fMiniMapMinPosX || fMiniMapX > fMaxSX || fMiniMapY < fMinSY || fMiniMapY > fMaxSY)
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

                        const static _float fMiniMapMinPosX = fSCX - 80.f;
                        const static _float fMaxSX = fSCX + 80.f;
                        const static _float fMinSY = fSCY - 80.f;
                        const static _float fMaxSY = fSCY + 80.f;

                        _float fDistX = XMVectorGetX(vDoorMinWorldPos) - XMVectorGetX(vPlayerWorldPos);
                        _float fDistZ = XMVectorGetZ(vDoorMinWorldPos) - XMVectorGetZ(vPlayerWorldPos);

                        // 월드에서 미니맵이 표현할 최대 거리
                        _float fWorldRange = 30.f;

                        _float fMiniMapX = fSCX + (fDistX / fWorldRange) * (m_fMiniMapSize * 0.5f);
                        _float fMiniMapY = fSCY - (fDistZ / fWorldRange) * (m_fMiniMapSize * 0.5f);

                        if (fMiniMapX < fMiniMapMinPosX || fMiniMapX > fMaxSX || fMiniMapY < fMinSY || fMiniMapY > fMaxSY)
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
