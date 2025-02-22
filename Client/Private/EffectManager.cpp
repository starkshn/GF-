#include "pch.h"
#include "EffectManager.h"
#include "GameInstance.h"
#include "Layer.h"
#include "PlayerBullet.h"
#include "StateObjectWeapon.h"

IMPLEMENT_SINGLETON(CEffectManager);


CEffectManager::CEffectManager()
    :
    m_pGameInst(CGameInstance::GetInstance())
{
    
}

HRESULT CEffectManager::Initialize()
{
    // WallHit
    {
        CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_ProjectileWallHit"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        for (_int i = 0; i < 30; ++i)
        {
            m_pEffectProjectileWallHits[i] = static_cast<CEffect*>(*(iter++));
            SafeAddRef(m_pEffectProjectileWallHits[i]);
        }
    }
    

    // WallHitFront
    {
        CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_ProjectileWallHitFront"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        for (_int i = 0; i < 30; ++i)
        {
            m_pEffectProjectileWallHitFronts[i] = static_cast<CEffect*>(*(iter++));
            SafeAddRef(m_pEffectProjectileWallHitFronts[i]);
        }
    }

    // WallHitSpot
    {
        CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_ProjectileWallHitSpot"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        for (_int i = 0; i < 50; ++i)
        {
            m_pEffectProjectileWallHitSpots[i] = static_cast<CEffect*>(*(iter++));
            SafeAddRef(m_pEffectProjectileWallHitSpots[i]);
        }
    }

    // MonsterHit
    {
        CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_ProjectileMonsterHit"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        for (_int i = 0; i < 20; ++i)
        {
            m_pEffectProjectileMonsterHits[i] = static_cast<CEffect*>(*(iter++));
            SafeAddRef(m_pEffectProjectileMonsterHits[i]);
        }
    }

    // Bullet
    {
        CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_Bullet"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        for (_int i = 0; i < 50; ++i)
        {
            m_pEffectBullets[i] = static_cast<CEffect*>(*(iter++));
            SafeAddRef(m_pEffectBullets[i]);
        }
    }

    // BulletTrail
    {
        CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_BulletTrail"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        for (_int i = 0; i < 50; ++i)
        {
            m_pEffectBulletTrails[i] = static_cast<CEffect*>(*(iter++));
            SafeAddRef(m_pEffectBulletTrails[i]);
        }
    }
    
    // Effect BossRockAlert
    {
        CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_BossRockAlert"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        for (_int i = 0; i < 3; ++i)
        {
            m_pEffectBossRockAlert[i] = static_cast<CEffect*>(*(iter++));
            SafeAddRef(m_pEffectBossRockAlert[i]);
        }
    }

    // Effect BossRockAlertFinal
    {
        CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_BossRockAlertFinal"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        for (_int i = 0; i < 3; ++i)
        {
            m_pEffectBossRockAlertFinal[i] = static_cast<CEffect*>(*(iter++));
            SafeAddRef(m_pEffectBossRockAlertFinal[i]);
        }
    }


    // EffectFollowLighting
    {
        CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_FollowLighting"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        for (_int i = 0; i < 20; ++i)
        {
            m_pEffectFollowLightings[i] = static_cast<CEffect*>(*(iter++));
            SafeAddRef(m_pEffectFollowLightings[i]);
        }
    }


    // EffectCircle1004
    {
        CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_Circle1004"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        for (_int i = 0; i < 20; ++i)
        {
            m_pEffectCircle1004[i] = static_cast<CEffect*>(*(iter++));
            SafeAddRef(m_pEffectCircle1004[i]);
        }
    }


    // EffectHit004
    {
        CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_Hit1004"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        for (_int i = 0; i < 20; ++i)
        {
            m_pEffectHit1004[i] = static_cast<CEffect*>(*(iter++));
            SafeAddRef(m_pEffectHit1004[i]);
        }
    }


    // EffectSphere004
    {
        CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_Sphere1004"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        for (_int i = 0; i < 10; ++i)
        {
            m_pEffectSphere1004[i] = static_cast<CEffect*>(*(iter++));
            SafeAddRef(m_pEffectSphere1004[i]);
        }
    }

    // EffectTestTrail
    {
        CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_TestTrail"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        m_pEffectTestTrail = static_cast<CEffect*>(*(iter++));
        SafeAddRef(m_pEffectTestTrail);
    }


    // Sword
    {
        CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_SwordTrail"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        for (_int i = 0; i < 10; ++i)
        {
            m_pEffectSwordTrails[i] = static_cast<CEffect*>(*(iter++));
            SafeAddRef(m_pEffectSwordTrails[i]);
        }
    }


    // Piercing
    {
        CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_PiercingTrail"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        for (_int i = 0; i < 10; ++i)
        {
            m_pEffectPiercingTrails[i] = static_cast<CEffect*>(*(iter++));
            SafeAddRef(m_pEffectPiercingTrails[i]);
        }
    }


    // PiercingSwing
    {
        CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_PiercingSwingTrail"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        for (_int i = 0; i < 10; ++i)
        {
            m_pEffectPiercingSwingTrails[i] = static_cast<CEffect*>(*(iter++));
            SafeAddRef(m_pEffectPiercingSwingTrails[i]);
        }
    }


    // Chase
    {
        /*CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_ChaseTrail"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        for (_int i = 0; i < 10; ++i)
        {
            m_pEffectChaseTrails[i] = static_cast<CEffect*>(*(iter++));
            SafeAddRef(m_pEffectChaseTrails[i]);
        }*/
    }

    // Monster03Fire
    {
        CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_Monster03Fire"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        for (_int i = 0; i < 5; ++i)
        {
            m_pEffectMonster03Fire[i] = static_cast<CEffect*>(*(iter++));
            SafeAddRef(m_pEffectMonster03Fire[i]);
        }
    }


    // Monster04Fire
    {
        CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_Monster04Fire"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        for (_int i = 0; i < 5; ++i)
        {
            m_pEffectMonster04Fire[i] = static_cast<CEffect*>(*(iter++));
            SafeAddRef(m_pEffectMonster04Fire[i]);
        }
    }

    // Player01 02 03 04 Trail
    {
        // 01
        {
            CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_Player01Trail"));
            if (nullptr == pLayer)
                return E_FAIL;

            auto GameObjects = pLayer->GetGameObjectList();
            auto iter = GameObjects.begin();

            for (_int i = 0; i < 30; ++i)
            {
                m_pEffectPlayer01Trail[i] = static_cast<CEffect*>(*(iter++));
                SafeAddRef(m_pEffectPlayer01Trail[i]);
            }
        }

        // 02
        {
            CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_Player02Trail"));
            if (nullptr == pLayer)
                return E_FAIL;

            auto GameObjects = pLayer->GetGameObjectList();
            auto iter = GameObjects.begin();

            for (_int i = 0; i < 30; ++i)
            {
                m_pEffectPlayer02Trail[i] = static_cast<CEffect*>(*(iter++));
                SafeAddRef(m_pEffectPlayer02Trail[i]);
            }
        }

        // 03
        {
            CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_Player03Trail"));
            if (nullptr == pLayer)
                return E_FAIL;

            auto GameObjects = pLayer->GetGameObjectList();
            auto iter = GameObjects.begin();

            for (_int i = 0; i < 30; ++i)
            {
                m_pEffectPlayer03Trail[i] = static_cast<CEffect*>(*(iter++));
                SafeAddRef(m_pEffectPlayer03Trail[i]);
            }
        }

        // 04
        {
            CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_Player04Trail"));
            if (nullptr == pLayer)
                return E_FAIL;

            auto GameObjects = pLayer->GetGameObjectList();
            auto iter = GameObjects.begin();

            for (_int i = 0; i < 30; ++i)
            {
                m_pEffectPlayer04Trail[i] = static_cast<CEffect*>(*(iter++));
                SafeAddRef(m_pEffectPlayer04Trail[i]);
            }
        }
    }


    // Monster 03 04 Trail
    {
        // 03
        {
            CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_Monster03Trail"));
            if (nullptr == pLayer)
                return E_FAIL;

            auto GameObjects = pLayer->GetGameObjectList();
            auto iter = GameObjects.begin();

            for (_int i = 0; i < 20; ++i)
            {
                m_pEffectMonster03Trail[i] = static_cast<CEffect*>(*(iter++));
                SafeAddRef(m_pEffectMonster03Trail[i]);
            }

            // 04
            {
                CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_Monster04Trail"));
                if (nullptr == pLayer)
                    return E_FAIL;

                auto GameObjects = pLayer->GetGameObjectList();
                auto iter = GameObjects.begin();

                for (_int i = 0; i < 20; ++i)
                {
                    m_pEffectMonster04Trail[i] = static_cast<CEffect*>(*(iter++));
                    SafeAddRef(m_pEffectMonster04Trail[i]);
                }
            }
        }
    }


    // Player Muzzle
    {
        _int iCnt = 5;
        // 01
        {
            CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_Player01Muzzle"));
            if (nullptr == pLayer)
                return E_FAIL;

            auto GameObjects = pLayer->GetGameObjectList();
            auto iter = GameObjects.begin();

            for (_int i = 0; i < iCnt; ++i)
            {
                m_pEffectPlayer01Muzzle[i] = static_cast<CEffect*>(*(iter++));
                SafeAddRef(m_pEffectPlayer01Muzzle[i]);
            }
        }



        // 02
        {
            CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_Player02Muzzle"));
            if (nullptr == pLayer)
                return E_FAIL;

            auto GameObjects = pLayer->GetGameObjectList();
            auto iter = GameObjects.begin();

            for (_int i = 0; i < iCnt; ++i)
            {
                m_pEffectPlayer02Muzzle[i] = static_cast<CEffect*>(*(iter++));
                SafeAddRef(m_pEffectPlayer02Muzzle[i]);
            }
        }



        // 03 
        {
            CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_Player03Muzzle"));
            if (nullptr == pLayer)
                return E_FAIL;

            auto GameObjects = pLayer->GetGameObjectList();
            auto iter = GameObjects.begin();

            for (_int i = 0; i < iCnt; ++i)
            {
                m_pEffectPlayer03Muzzle[i] = static_cast<CEffect*>(*(iter++));
                SafeAddRef(m_pEffectPlayer03Muzzle[i]);
            }
        }


        // 04
        {
            CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_Player04Muzzle"));
            if (nullptr == pLayer)
                return E_FAIL;

            auto GameObjects = pLayer->GetGameObjectList();
            auto iter = GameObjects.begin();

            for (_int i = 0; i < iCnt; ++i)
            {
                m_pEffectPlayer04Muzzle[i] = static_cast<CEffect*>(*(iter++));
                SafeAddRef(m_pEffectPlayer04Muzzle[i]);
            }
        }
    }

    // bullet hit
    {
        // 03
        {
            CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_BulletHit03"));
            if (nullptr == pLayer)
                return E_FAIL;

            auto GameObjects = pLayer->GetGameObjectList();
            auto iter = GameObjects.begin();

            for (_int i = 0; i < 5; ++i)
            {
                m_pEffectBulletHit03[i] = static_cast<CEffect*>(*(iter++));
                SafeAddRef(m_pEffectBulletHit03[i]);
            }
        }

        // 04
        {
            CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_BulletHit04"));
            if (nullptr == pLayer)
                return E_FAIL;

            auto GameObjects = pLayer->GetGameObjectList();
            auto iter = GameObjects.begin();

            for (_int i = 0; i < 10; ++i)
            {
                m_pEffectBulletHit04[i] = static_cast<CEffect*>(*(iter++));
                SafeAddRef(m_pEffectBulletHit04[i]);
            }
        }
    }


    // BossEye
    {
        CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_BossEye"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        for (_int i = 0; i < 2; ++i)
        {
            m_pEffectBossEye[i] = static_cast<CEffect*>(*(iter++));
            SafeAddRef(m_pEffectBossEye[i]);
        }
    }


    // EffectDoor
    {
        CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_Door"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        for (_int i = 0; i < 1; ++i)
        {
            m_pEffectDoor = static_cast<CEffect*>(*(iter++));
            SafeAddRef(m_pEffectDoor);
        }
    }


    // Layer_Effect_RoarNoise
    {
        CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Effect_RoarNoise"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        for (_int i = 0; i < 1; ++i)
        {
            m_pEffectRoarNoise = static_cast<CEffect*>(*(iter++));
            SafeAddRef(m_pEffectRoarNoise);
        }
    }


    return S_OK;
}

void CEffectManager::Update(_float fTimeDelta)
{
    CheckOwner();
}

void CEffectManager::CheckOwner()
{
    //for (_int i = 0; i < 30; ++i)
    //{
    //    m_pEffectProjectileWallHits[i]->GetOwner();
    //    m_pEffectProjectileWallHitFronts[i]->GetOwner();
    //    m_pEffectProjectileWallHitSpots[i]->GetOwner();
    //}

    //for (_int i = 0; i < 20; ++i)
    //{
    //    m_pEffectProjectileMonsterHits[i]->GetOwner();
    //}

    for (_int i = 0; i < 50; ++i)
    {
        if (m_pEffectBullets[i]->GetOwner())
        {
            CPlayerBullet* pBullet = static_cast<CPlayerBullet*>(m_pEffectBullets[i]->GetOwner());
            if (pBullet && pBullet->IsFire())
            {
                CCollider* pCol = static_cast<CCollider*>(m_pEffectBullets[i]->GetOwner()->GetComponent(TEXT("Com_Collider")));
                if (pCol->IsEndCollision())
                {
                    m_pEffectBullets[i]->ForceDeActive();
                    m_pEffectBullets[i]->SetOwner(nullptr);
                }
            }
        }

        if (m_pEffectBulletTrails[i]->GetOwner())
        {
            CPlayerBullet* pBullet = static_cast<CPlayerBullet*>(m_pEffectBulletTrails[i]->GetOwner());
            if (pBullet && pBullet->IsFire())
            {
                CCollider* pCol = static_cast<CCollider*>(m_pEffectBulletTrails[i]->GetOwner()->GetComponent(TEXT("Com_Collider")));
                if (pCol->IsEndCollision())
                {
                    m_pEffectBulletTrails[i]->ForceDeActive();
                    m_pEffectBulletTrails[i]->SetOwner(nullptr);
                }
            }
        }
    }






    //for (_int i = 0; i < 3; ++i)
    //{
    //    m_pEffectBossRockAlert[i]->GetOwner();
    //    m_pEffectBossRockAlertFinal[i]->GetOwner();

    //}

    //for (_int i = 0; i < 20; ++i)
    //{
    //    m_pEffectFollowLightings[i]->GetOwner();
    //}
}

void CEffectManager::ActiveEffect(EFFECT_TYPE eType, HIT_RESULT HitResult, CGameObject* pOwner)
{
    switch (eType)
    {
    case Client::CEffectManager::PROJECTILE_WALL_HIT:
    {
        for (_int i = 0; i < 30; ++i)
        {
            if (nullptr == m_pEffectProjectileWallHits[i])
                continue;

            if (m_pEffectProjectileWallHits[i]->IsActive())
                continue;

            CTransform* pCom = static_cast<CTransform*>(m_pEffectProjectileWallHits[i]->GetComponent(TEXT("Com_Transform")));
            if (nullptr == pCom)
                return;

            m_pEffectProjectileWallHits[i]->Active();
            CalNormal(pCom, HitResult);
            break;
        }
    }
        break;
    case CEffectManager::PROJECTILE_WALL_HIT_FRONT:
    {
        for (_int i = 0; i < 30; ++i)
        {
            if (nullptr == m_pEffectProjectileWallHitFronts[i])
                continue;

            if (m_pEffectProjectileWallHitFronts[i]->IsActive())
                continue;

            CTransform* pCom = static_cast<CTransform*>(m_pEffectProjectileWallHitFronts[i]->GetComponent(TEXT("Com_Transform")));
            if (nullptr == pCom)
                return;

            m_pEffectProjectileWallHitFronts[i]->Active();
            CalNormalWallHitFront(pCom, HitResult);
            pCom->Scaling(3.f, 1.f, 1.f);

            break;
        }
    }
    break;
    case CEffectManager::PROJECTILE_WALL_HIT_SPOT:
    {
        for (_int i = 0; i < 50; ++i)
        {
            if (nullptr == m_pEffectProjectileWallHitSpots[i])
                continue;

            if (m_pEffectProjectileWallHitSpots[i]->IsActive())
                continue;

            CTransform* pCom = static_cast<CTransform*>(m_pEffectProjectileWallHitSpots[i]->GetComponent(TEXT("Com_Transform")));
            if (nullptr == pCom)
                return;

            m_pEffectProjectileWallHitSpots[i]->Active();
            CalNormal(pCom, HitResult);
            break;
        }
    }
    break;
    case CEffectManager::PROJECTILE_MONSTER_HIT:
    {
        for (_int i = 0; i < 20; ++i)
        {
            if (nullptr == m_pEffectProjectileMonsterHits[i])
                continue;

            if (m_pEffectProjectileMonsterHits[i]->IsActive())
                continue;

            CTransform* pCom = static_cast<CTransform*>(m_pEffectProjectileMonsterHits[i]->GetComponent(TEXT("Com_Transform")));
            if (nullptr == pCom)
                return;

            m_pEffectProjectileMonsterHits[i]->Active();
            CalNormal(pCom, HitResult);

            break;
        }
    }
    break;
    // 총알 자체에 대한 Effect    // TODO
    case CEffectManager::BULLET:
    {
        for (_int i = 0; i < 50; ++i)
        {
            if (nullptr == m_pEffectBullets[i] || m_pEffectBullets[i]->IsActive())
                continue;

            CTransform* pCom = static_cast<CTransform*>(m_pEffectBullets[i]->GetComponent(TEXT("Com_Transform")));
            if (nullptr == pCom)
                return;

            // vLook
            _vector vLook = XMVector3Normalize(XMLoadFloat3(&HitResult.vNormal));
            vLook.m128_f32[3] = 0.f;

            _vector vRight = XMVector3Normalize(XMVector3Cross(vLook, _vector{ 0.f, 1.f, 0.f }));
            _vector vUp = XMVector3Normalize(XMVector3Cross(vRight, vLook));
            vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));


            pCom->SetTransformState(CTransform::STATE_RIGHT, vRight);
            pCom->SetTransformState(CTransform::STATE_UP, vUp);
            pCom->SetTransformState(CTransform::STATE_LOOK, vLook);
            pCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f));

            pCom->Scaling(0.25f, 0.25f, 0.25f);

            m_pEffectBullets[i]->Active();
            m_pEffectBullets[i]->Move();
            m_pEffectBullets[i]->SetMoveDir(vLook);

            if (pOwner) 
                m_pEffectBullets[i]->SetOwner(pOwner);

            break;
        }
    }
    break;
    case CEffectManager::BULLET_TRAIL:
    {
        for (_int i = 0; i < 50; ++i)
        {
            if (nullptr == m_pEffectBulletTrails[i] || m_pEffectBulletTrails[i]->IsActive())
                continue;

            CTransform* pCom = static_cast<CTransform*>(m_pEffectBulletTrails[i]->GetComponent(TEXT("Com_Transform")));
            if (nullptr == pCom)
                return;

            // vLook
            _vector vLook = XMVector3Normalize(XMLoadFloat3(&HitResult.vNormal));
            vLook.m128_f32[3] = 0.f;

            _vector vRight = XMVector3Normalize(XMVector3Cross(vLook, _vector{ 0.f, 1.f, 0.f }));
            _vector vUp = XMVector3Normalize(XMVector3Cross(vRight, vLook));
            vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

            pCom->SetTransformState(CTransform::STATE_RIGHT, vLook);
            pCom->SetTransformState(CTransform::STATE_UP, vUp);
            pCom->SetTransformState(CTransform::STATE_LOOK, vRight);
            pCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f));

            pCom->Scaling(10.f, 1.f, 1.f);
            m_pEffectBulletTrails[i]->Active();
            m_pEffectBulletTrails[i]->Move();
            m_pEffectBulletTrails[i]->SetMoveDir(vLook);

            if (pOwner)
                m_pEffectBulletTrails[i]->SetOwner(pOwner);

            break;
        }
    }
    break;
    case CEffectManager::BOSS_ROCK_ALERT:
    {
        for (_int i = 0; i < 3; ++i)
        {
            if (nullptr == m_pEffectBossRockAlert[i] || m_pEffectBossRockAlert[i]->IsActive())
                continue;

            CTransform* pCom = static_cast<CTransform*>(m_pEffectBossRockAlert[i]->GetComponent(TEXT("Com_Transform")));
            if (nullptr == pCom)
                return;

            m_pEffectBossRockAlert[i]->Active();

            pCom->SetTransformState(CTransform::STATE_RIGHT, _vector{ 1.f, 0.f, 0.f });
            pCom->SetTransformState(CTransform::STATE_UP, _vector{ 0.f, 0.f, 1.f });
            pCom->SetTransformState(CTransform::STATE_LOOK, _vector{ 0.f, 1.f, 0.f });
            pCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f));
            break;
        }
    }
    break;
    case CEffectManager::BOSS_ROCK_ALERT_FINAL:
    {
        for (_int i = 0; i < 3; ++i)
        {
            if (nullptr == m_pEffectBossRockAlertFinal[i] || m_pEffectBossRockAlertFinal[i]->IsActive())
                continue;

            CTransform* pCom = static_cast<CTransform*>(m_pEffectBossRockAlertFinal[i]->GetComponent(TEXT("Com_Transform")));
            if (nullptr == pCom)
                return;

            m_pEffectBossRockAlertFinal[i]->Active();

            pCom->SetTransformState(CTransform::STATE_RIGHT, _vector{ 1.f, 0.f, 0.f });
            pCom->SetTransformState(CTransform::STATE_UP, _vector{ 0.f, 0.f, 1.f });
            pCom->SetTransformState(CTransform::STATE_LOOK, _vector{ 0.f, 1.f, 0.f });
            pCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f));
            break;
        }
    }
    break;
    case CEffectManager::FOLLOW_LIGHTING:
    {
        for (_int i = 0; i < 20; ++i)
        {
            if (nullptr == m_pEffectFollowLightings[i] || m_pEffectFollowLightings[i]->IsActive())
                continue;

            CTransform* pCom = static_cast<CTransform*>(m_pEffectFollowLightings[i]->GetComponent(TEXT("Com_Transform")));
            if (nullptr == pCom)
                return;

            m_pEffectFollowLightings[i]->Active();

            pCom->SetTransformState(CTransform::STATE_RIGHT, _vector{ 1.f, 0.f, 0.f });
            pCom->SetTransformState(CTransform::STATE_UP, _vector{ 0.f, 0.f, 1.f });
            pCom->SetTransformState(CTransform::STATE_LOOK, _vector{ 0.f, 1.f, 0.f });
            pCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f));

            break;
        }
    }
    break;
    case CEffectManager::CIRCLE_1004:
    {
        for (_int i = 0; i < 20; ++i)
        {
            if (nullptr == m_pEffectCircle1004[i] || m_pEffectCircle1004[i]->IsActive())
                continue;

            CTransform* pCom = static_cast<CTransform*>(m_pEffectCircle1004[i]->GetComponent(TEXT("Com_Transform")));
            if (nullptr == pCom)
                return;

            m_pEffectCircle1004[i]->Active();


            CalNormalWallHitFront(pCom, HitResult);


           /* pCom->SetTransformState(CTransform::STATE_RIGHT, _vector{ 1.f, 0.f, 0.f });
            pCom->SetTransformState(CTransform::STATE_UP, _vector{ 0.f, 0.f, 1.f });
            pCom->SetTransformState(CTransform::STATE_LOOK, _vector{ 0.f, 1.f, 0.f });
            pCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f));*/

            break;
        }
    }
    break;
    case CEffectManager::HIT_1004:
    {
        for (_int i = 0; i < 20; ++i)
        {

            if (nullptr == m_pEffectHit1004[i] || m_pEffectHit1004[i]->IsActive())
                continue;

            CTransform* pCom = static_cast<CTransform*>(m_pEffectHit1004[i]->GetComponent(TEXT("Com_Transform")));
            if (nullptr == pCom)
                return;

            m_pEffectHit1004[i]->Active();

            CalNormalWallHitFront(pCom, HitResult);

            // pCom->SetTransformState(CTransform::STATE_RIGHT, _vector{ 1.f, 0.f, 0.f });
            // pCom->SetTransformState(CTransform::STATE_UP, _vector{ 0.f, 0.f, 1.f });
            // pCom->SetTransformState(CTransform::STATE_LOOK, _vector{ 0.f, 1.f, 0.f });
            pCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f));

            break;
        }
    }
    break;
    case CEffectManager::SPHERE_1004:
    {
        for (_int i = 0; i < 10; ++i)
        {
            if (nullptr == m_pEffectSphere1004[i] || m_pEffectSphere1004[i]->IsActive())
                continue;

            CTransform* pCom = static_cast<CTransform*>(m_pEffectSphere1004[i]->GetComponent(TEXT("Com_Transform")));
            if (nullptr == pCom)
                return;

            m_pEffectSphere1004[i]->Active();

   /*         pCom->SetTransformState(CTransform::STATE_RIGHT, _vector{ 1.f, 0.f, 0.f });
            pCom->SetTransformState(CTransform::STATE_UP, _vector{ 0.f, 0.f, 1.f });
            pCom->SetTransformState(CTransform::STATE_LOOK, _vector{ 0.f, 1.f, 0.f });*/
            pCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f));

            break;
        }
    }
    break;
    case CEffectManager::TEST_TRAIL:
    {
        if (nullptr == m_pEffectTestTrail || m_pEffectTestTrail->IsActive())
            return;

        CTransform* pCom = static_cast<CTransform*>(m_pEffectTestTrail->GetComponent(TEXT("Com_Transform")));
        if (nullptr == pCom)
            return; 

        m_pEffectTestTrail->Active();

        /*         pCom->SetTransformState(CTransform::STATE_RIGHT, _vector{ 1.f, 0.f, 0.f });
                    pCom->SetTransformState(CTransform::STATE_UP, _vector{ 0.f, 0.f, 1.f });
                    pCom->SetTransformState(CTransform::STATE_LOOK, _vector{ 0.f, 1.f, 0.f });*/
        pCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f));

        if (pOwner)
            m_pEffectTestTrail->SetOwner(pOwner);
    }
    break;
    case CEffectManager::SWORD_TRAIL:
    {
        for (_int i = 0; i < 10; ++i)
        {
            if (nullptr == m_pEffectSwordTrails[i] || m_pEffectSwordTrails[i]->IsActive())
                continue;

            CTransform* pCom = static_cast<CTransform*>(m_pEffectSwordTrails[i]->GetComponent(TEXT("Com_Transform")));
            if (nullptr == pCom)
                break;

            m_pEffectSwordTrails[i]->Active();
            // pCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f));

            if (pOwner)
                m_pEffectSwordTrails[i]->SetOwner(pOwner);

            break;
        }
    }
    break;
    case CEffectManager::PIERCING_TRAIL:
    {
        for (_int i = 0; i < 10; ++i)
        {
            if (nullptr == m_pEffectPiercingTrails[i] || m_pEffectPiercingTrails[i]->IsActive())
                continue;

            CTransform* pCom = static_cast<CTransform*>(m_pEffectPiercingTrails[i]->GetComponent(TEXT("Com_Transform")));
            if (nullptr == pCom)
                break;

            m_pEffectPiercingTrails[i]->Active();
            // pCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f));

            if (pOwner)
                m_pEffectPiercingTrails[i]->SetOwner(pOwner);

            break;
        }
    }
    break;
    case CEffectManager::PIERCING_SWING_TRAIL:
    {
        for (_int i = 0; i < 10; ++i)
        {
            if (nullptr == m_pEffectPiercingSwingTrails[i] || m_pEffectPiercingSwingTrails[i]->IsActive())
                continue;

            CTransform* pCom = static_cast<CTransform*>(m_pEffectPiercingSwingTrails[i]->GetComponent(TEXT("Com_Transform")));
            if (nullptr == pCom)
                break;

            m_pEffectPiercingSwingTrails[i]->Active();
            // pCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f));

            if (pOwner)
                m_pEffectPiercingSwingTrails[i]->SetOwner(pOwner);

            break;
        }
    }
    break;
    case CEffectManager::CHASE_TRAIL:
    {
        for (_int i = 0; i < 10; ++i)
        {
            if (nullptr == m_pEffectChaseTrails[i] || m_pEffectChaseTrails[i]->IsActive())
                continue;

            CTransform* pCom = static_cast<CTransform*>(m_pEffectChaseTrails[i]->GetComponent(TEXT("Com_Transform")));
            if (nullptr == pCom)
                break;

            m_pEffectChaseTrails[i]->Active();
            // pCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f));

            if (pOwner)
                m_pEffectChaseTrails[i]->SetOwner(pOwner);

            break;
        }
    }
    break;
    case CEffectManager::MONSTER03_FIRE:
    {
        for (_int i = 0; i < 5; ++i)
        {
            if (nullptr == m_pEffectMonster03Fire[i] || m_pEffectMonster03Fire[i]->IsActive())
                continue;

            CTransform* pCom = static_cast<CTransform*>(m_pEffectMonster03Fire[i]->GetComponent(TEXT("Com_Transform")));
            if (nullptr == pCom)
                break;

            m_pEffectMonster03Fire[i]->Active();
            
            _vector vLook = XMVector3Normalize(XMLoadFloat3(&HitResult.vNormal));
            vLook.m128_f32[3] = 0.f;
            _vector vRight = XMVector3Normalize(XMVector3Cross(vLook, _vector{ 0.f, 1.f, 0.f }));
            _vector vUp = XMVector3Normalize(XMVector3Cross(vRight, vLook));
            vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

            pCom->SetTransformState(CTransform::STATE_RIGHT, vRight);
            pCom->SetTransformState(CTransform::STATE_UP, vUp);
            pCom->SetTransformState(CTransform::STATE_LOOK, vLook);
            pCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f));

            if (pOwner)
                m_pEffectMonster03Fire[i]->SetOwner(pOwner);

            break;
        }
    }
    break;
    case CEffectManager::MONSTER04_FIRE:
    {
        for (_int i = 0; i < 5; ++i)
        {
            if (nullptr == m_pEffectMonster04Fire[i] || m_pEffectMonster04Fire[i]->IsActive())
                continue;

            CTransform* pCom = static_cast<CTransform*>(m_pEffectMonster04Fire[i]->GetComponent(TEXT("Com_Transform")));
            if (nullptr == pCom)
                break;

            m_pEffectMonster04Fire[i]->Active();

            _vector vLook = XMVector3Normalize(XMLoadFloat3(&HitResult.vNormal));
            vLook.m128_f32[3] = 0.f;
            _vector vRight = XMVector3Normalize(XMVector3Cross(vLook, _vector{ 0.f, 1.f, 0.f }));
            _vector vUp = XMVector3Normalize(XMVector3Cross(vRight, vLook));
            vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

            pCom->SetTransformState(CTransform::STATE_RIGHT, vRight);
            pCom->SetTransformState(CTransform::STATE_UP, vUp);
            pCom->SetTransformState(CTransform::STATE_LOOK, vLook);
            pCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f));

            if (pOwner)
                m_pEffectMonster04Fire[i]->SetOwner(pOwner);

            break;
        }
    }
    break;
    case CEffectManager::PLAYER01_TRAIL:
    {
        for (_int i = 0; i < 30; ++i)
        {
            if (nullptr == m_pEffectPlayer01Trail[i] || m_pEffectPlayer01Trail[i]->IsActive())
                continue;

            CTransform* pCom = static_cast<CTransform*>(m_pEffectPlayer01Trail[i]->GetComponent(TEXT("Com_Transform")));
            if (nullptr == pCom)
                break;

            m_pEffectPlayer01Trail[i]->Active();

            if (pOwner)
                m_pEffectPlayer01Trail[i]->SetOwner(pOwner);

            break;
        }
    }
    break;
    case CEffectManager::PLAYER02_TRAIL:
    {
        for (_int i = 0; i < 30; ++i)
        {
            if (nullptr == m_pEffectPlayer02Trail[i] || m_pEffectPlayer02Trail[i]->IsActive())
                continue;

            CTransform* pCom = static_cast<CTransform*>(m_pEffectPlayer02Trail[i]->GetComponent(TEXT("Com_Transform")));
            if (nullptr == pCom)
                break;

            m_pEffectPlayer02Trail[i]->Active();



            if (pOwner)
                m_pEffectPlayer02Trail[i]->SetOwner(pOwner);

            break;
        }
    }
    break;
    case CEffectManager::PLAYER03_TRAIL:
    {
        for (_int i = 0; i < 30; ++i)
        {
            if (nullptr == m_pEffectPlayer03Trail[i] || m_pEffectPlayer03Trail[i]->IsActive())
                continue;

            CTransform* pCom = static_cast<CTransform*>(m_pEffectPlayer03Trail[i]->GetComponent(TEXT("Com_Transform")));
            if (nullptr == pCom)
                break;

            m_pEffectPlayer03Trail[i]->Active();



            if (pOwner)
                m_pEffectPlayer03Trail[i]->SetOwner(pOwner);

            break;
        }
    }
    break;
    case CEffectManager::PLAYER04_TRAIL:
    {
        for (_int i = 0; i < 30; ++i)
        {
            if (nullptr == m_pEffectPlayer04Trail[i] || m_pEffectPlayer04Trail[i]->IsActive())
                continue;

            CTransform* pCom = static_cast<CTransform*>(m_pEffectPlayer04Trail[i]->GetComponent(TEXT("Com_Transform")));
            if (nullptr == pCom)
                break;

            m_pEffectPlayer04Trail[i]->Active();

            if (pOwner)
                m_pEffectPlayer04Trail[i]->SetOwner(pOwner);

            break;
        }
    }
    break;
    case CEffectManager::MONSTER03_TRAIL:
    {
        for (_int i = 0; i < 10; ++i)
        {
            if (nullptr == m_pEffectMonster03Trail[i] || m_pEffectMonster03Trail[i]->IsActive())
                continue;

            CTransform* pCom = static_cast<CTransform*>(m_pEffectMonster03Trail[i]->GetComponent(TEXT("Com_Transform")));
            if (nullptr == pCom)
                break;

            m_pEffectMonster03Trail[i]->Active();

            if (pOwner)
                m_pEffectMonster03Trail[i]->SetOwner(pOwner);

            break;
        }
    }
    break;
    case CEffectManager::MONSTER04_TRAIL:
    {
        for (_int i = 0; i < 10; ++i)
        {
            if (nullptr == m_pEffectMonster04Trail[i] || m_pEffectMonster04Trail[i]->IsActive())
                continue;

            CTransform* pCom = static_cast<CTransform*>(m_pEffectMonster04Trail[i]->GetComponent(TEXT("Com_Transform")));
            if (nullptr == pCom)
                break;

            m_pEffectMonster04Trail[i]->Active();

            if (pOwner)
                m_pEffectMonster04Trail[i]->SetOwner(pOwner);

            break;
        }
    }
    break;
    case CEffectManager::BOSS_EYE:
    {
        for (_int i = 0; i < 2; ++i)
        {
            if (nullptr == m_pEffectBossEye[i] || m_pEffectBossEye[i]->IsActive())
                continue;

            CTransform* pCom = static_cast<CTransform*>(m_pEffectBossEye[i]->GetComponent(TEXT("Com_Transform")));
            if (nullptr == pCom)
                break;

            m_pEffectBossEye[i]->Active();

            if (pOwner)
                m_pEffectBossEye[i]->SetOwner(pOwner);

            break;
        }
    }
    break;
    case CEffectManager::PLAYER01_MUZZLE:
    {
        for (_int i = 0; i < 5; ++i)
        {
            if (nullptr == m_pEffectPlayer01Muzzle[i] || m_pEffectPlayer01Muzzle[i]->IsActive())
                continue;

            CTransform* pCom = static_cast<CTransform*>(m_pEffectPlayer01Muzzle[i]->GetComponent(TEXT("Com_Transform")));
            if (nullptr == pCom)
                break;

            _vector vLook = XMVector3Normalize(XMLoadFloat3(&HitResult.vNormal));
            vLook.m128_f32[3] = 0.f;

            _vector vRight = XMVector3Normalize(XMVector3Cross(vLook, _vector{ 0.f, 1.f, 0.f }));
            _vector vUp = XMVector3Normalize(XMVector3Cross(vRight, vLook));
            vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

            pCom->SetTransformState(CTransform::STATE_RIGHT, vLook);
            pCom->SetTransformState(CTransform::STATE_UP, vUp);
            pCom->SetTransformState(CTransform::STATE_LOOK, vRight);
            pCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f));

            m_pEffectPlayer01Muzzle[i]->Active();

            break;
        }
    }
    break;
    case CEffectManager::PLAYER02_MUZZLE:
    {
        for (_int i = 0; i < 5; ++i)
        {
            if (nullptr == m_pEffectPlayer02Muzzle[i] || m_pEffectPlayer02Muzzle[i]->IsActive())
                continue;

            CTransform* pCom = static_cast<CTransform*>(m_pEffectPlayer02Muzzle[i]->GetComponent(TEXT("Com_Transform")));
            if (nullptr == pCom)
                break;

            _vector vLook = XMVector3Normalize(XMLoadFloat3(&HitResult.vNormal));
            vLook.m128_f32[3] = 0.f;

            _vector vRight = XMVector3Normalize(XMVector3Cross(vLook, _vector{ 0.f, 1.f, 0.f }));
            _vector vUp = XMVector3Normalize(XMVector3Cross(vRight, vLook));
            vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

            pCom->SetTransformState(CTransform::STATE_RIGHT, vLook);
            pCom->SetTransformState(CTransform::STATE_UP, vUp);
            pCom->SetTransformState(CTransform::STATE_LOOK, vRight);
            pCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f));

            m_pEffectPlayer02Muzzle[i]->Active();
            break;
        }
    }
    break;
    case CEffectManager::PLAYER03_MUZZLE:
    {
        for (_int i = 0; i < 5; ++i)
        {
            if (nullptr == m_pEffectPlayer03Muzzle[i] || m_pEffectPlayer03Muzzle[i]->IsActive())
                continue;

            CTransform* pCom = static_cast<CTransform*>(m_pEffectPlayer03Muzzle[i]->GetComponent(TEXT("Com_Transform")));
            if (nullptr == pCom)
                break;

            _vector vLook = XMVector3Normalize(XMLoadFloat3(&HitResult.vNormal));
            vLook.m128_f32[3] = 0.f;

            _vector vRight = XMVector3Normalize(XMVector3Cross(vLook, _vector{ 0.f, 1.f, 0.f }));
            _vector vUp = XMVector3Normalize(XMVector3Cross(vRight, vLook));
            vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

            pCom->SetTransformState(CTransform::STATE_RIGHT, vLook);
            pCom->SetTransformState(CTransform::STATE_UP, vUp);
            pCom->SetTransformState(CTransform::STATE_LOOK, vRight);
            pCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f));

            m_pEffectPlayer03Muzzle[i]->Active();
            break;
        }
    }
    break;
    case CEffectManager::PLAYER04_MUZZLE:
    {
        for (_int i = 0; i < 5; ++i)
        {
            if (nullptr == m_pEffectPlayer04Muzzle[i] || m_pEffectPlayer04Muzzle[i]->IsActive())
                continue;

            CTransform* pCom = static_cast<CTransform*>(m_pEffectPlayer04Muzzle[i]->GetComponent(TEXT("Com_Transform")));
            if (nullptr == pCom)
                break;

            _vector vLook = XMVector3Normalize(XMLoadFloat3(&HitResult.vNormal));
            vLook.m128_f32[3] = 0.f;

            _vector vRight = XMVector3Normalize(XMVector3Cross(vLook, _vector{ 0.f, 1.f, 0.f }));
            _vector vUp = XMVector3Normalize(XMVector3Cross(vRight, vLook));
            vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

            pCom->SetTransformState(CTransform::STATE_RIGHT, vLook);
            pCom->SetTransformState(CTransform::STATE_UP, vUp);
            pCom->SetTransformState(CTransform::STATE_LOOK, vRight);
            pCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f));

            m_pEffectPlayer04Muzzle[i]->Active();
            break;
        }
    }
    break;
    case CEffectManager::BULLET_HIT03:
    {
        for (_int i = 0; i < 5; ++i)
        {
            if (nullptr == m_pEffectBulletHit03[i] || m_pEffectBulletHit03[i]->IsActive())
                continue;

            CTransform* pCom = static_cast<CTransform*>(m_pEffectBulletHit03[i]->GetComponent(TEXT("Com_Transform")));
            if (nullptr == pCom)
                break;

            _vector vLook = XMVector3Normalize(XMLoadFloat3(&HitResult.vNormal));
            vLook.m128_f32[3] = 0.f;

            _vector vRight = XMVector3Normalize(XMVector3Cross(vLook, _vector{ 0.f, 1.f, 0.f }));
            _vector vUp = XMVector3Normalize(XMVector3Cross(vRight, vLook));
            vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

            pCom->SetTransformState(CTransform::STATE_RIGHT, vLook);
            pCom->SetTransformState(CTransform::STATE_UP, vUp);
            pCom->SetTransformState(CTransform::STATE_LOOK, vRight);
            pCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f));

            m_pEffectBulletHit03[i]->Active();
            break;
        }
    }
    break;
    case CEffectManager::BULLET_HIT04:
    {
        for (_int i = 0; i < 10; ++i)
        {
            if (nullptr == m_pEffectBulletHit04[i] || m_pEffectBulletHit04[i]->IsActive())
                continue;

            CTransform* pCom = static_cast<CTransform*>(m_pEffectBulletHit04[i]->GetComponent(TEXT("Com_Transform")));
            if (nullptr == pCom)
                break;

            _vector vLook = XMVector3Normalize(XMLoadFloat3(&HitResult.vNormal));
            vLook.m128_f32[3] = 0.f;

            _vector vRight = XMVector3Normalize(XMVector3Cross(vLook, _vector{ 0.f, 1.f, 0.f }));
            _vector vUp = XMVector3Normalize(XMVector3Cross(vRight, vLook));
            vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

            pCom->SetTransformState(CTransform::STATE_RIGHT, vLook);
            pCom->SetTransformState(CTransform::STATE_UP, vUp);
            pCom->SetTransformState(CTransform::STATE_LOOK, vRight);
            pCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f));

            m_pEffectBulletHit04[i]->Active();
            break;
        }
    }
    break;

    case CEffectManager::EFFECT_DOOR:
    {
        if (nullptr == m_pEffectDoor || m_pEffectDoor->IsActive())
            return;

        CTransform* pCom = static_cast<CTransform*>(m_pEffectDoor->GetComponent(TEXT("Com_Transform")));
        if (nullptr == pCom)
            break;

        _vector vLook = XMVector3Normalize(XMLoadFloat3(&HitResult.vNormal));
        vLook.m128_f32[3] = 0.f;

        _vector vRight = XMVector3Normalize(XMVector3Cross(vLook, _vector{ 0.f, 1.f, 0.f }));
        _vector vUp = XMVector3Normalize(XMVector3Cross(vRight, vLook));
        vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

        pCom->SetTransformState(CTransform::STATE_RIGHT, vLook);
        pCom->SetTransformState(CTransform::STATE_UP, vUp);
        pCom->SetTransformState(CTransform::STATE_LOOK, vRight);
        pCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f));

        m_pEffectDoor->Active();
        break;
    }
    break;

    case CEffectManager::ROAR_NOISE:
    {
        if (nullptr == m_pEffectRoarNoise || m_pEffectRoarNoise->IsActive())
            return;

        CTransform* pCom = static_cast<CTransform*>(m_pEffectRoarNoise->GetComponent(TEXT("Com_Transform")));
        if (nullptr == pCom)
            break;

        _vector vLook = XMVector3Normalize(XMLoadFloat3(&HitResult.vNormal));
        vLook.m128_f32[3] = 0.f;

        _vector vRight = XMVector3Normalize(XMVector3Cross(vLook, _vector{ 0.f, 1.f, 0.f }));
        _vector vUp = XMVector3Normalize(XMVector3Cross(vRight, vLook));
        vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

        pCom->SetTransformState(CTransform::STATE_RIGHT, vLook);
        pCom->SetTransformState(CTransform::STATE_UP, vUp);
        pCom->SetTransformState(CTransform::STATE_LOOK, vRight);
        pCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f));

        m_pEffectRoarNoise->Active();
        break;
    }
    break;

    case Client::CEffectManager::TYPE_END:
        break;
    default:
        break;
    }
}

void CEffectManager::DeActiveEffect(EFFECT_TYPE eType)
{
    switch (eType)
    {
    case Client::CEffectManager::PROJECTILE_WALL_HIT:
        break;
    case Client::CEffectManager::PROJECTILE_WALL_HIT_FRONT:
        break;
    case Client::CEffectManager::PROJECTILE_WALL_HIT_SPOT:
        break;
    case Client::CEffectManager::PROJECTILE_MONSTER_HIT:
        break;
    case Client::CEffectManager::BULLET:
        break;
    case Client::CEffectManager::BULLET_TRAIL:
        break;
    case Client::CEffectManager::BOSS_ROCK_ALERT:
        break;
    case Client::CEffectManager::TYPE_END:
        break;
    case CEffectManager::EFFECT_DOOR:
    {
        m_pEffectDoor->DeActive();
    }
    default:
        break;
    }
}

void CEffectManager::CalNormal(CTransform* pCom, HIT_RESULT Hit)
{
    pCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(Hit.vHitPos.x, Hit.vHitPos.y, Hit.vHitPos.z, 1.f));

    _vector vLook = XMVector3Normalize(XMLoadFloat3(&Hit.vNormal));
    vLook.m128_f32[3] = 0.f;

    if (XMVector3Equal(vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f)))
    {
        // vLook이 Y축 방향이면, vRight를 X축 방향 벡터로 설정
        _vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f); // 기본 X축 방향
        _vector vUp = XMVector3Normalize(XMVector3Cross(vRight, vLook)); // vUp은 vRight와 vLook의 외적
        vRight = XMVector3Normalize(XMVector3Cross(vLook, vUp)); // vRight를 재정의하여 직교 관계 유지

        pCom->SetTransformState(CTransform::STATE_RIGHT, vRight);
        pCom->SetTransformState(CTransform::STATE_UP, vUp);
        pCom->SetTransformState(CTransform::STATE_LOOK, vLook);
    }
    else if (XMVector3Equal(vLook, XMVectorSet(0.f, -1.f, 0.f, 0.f)))
    {
        // vLook이 Y축 방향이면, vRight를 X축 방향 벡터로 설정
        _vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f); // 기본 X축 방향
        _vector vUp = XMVector3Normalize(XMVector3Cross(vRight, vLook)); // vUp은 vRight와 vLook의 외적
        vRight = XMVector3Normalize(XMVector3Cross(vLook, vUp)); // vRight를 재정의하여 직교 관계 유지

        pCom->SetTransformState(CTransform::STATE_RIGHT, vRight);
        pCom->SetTransformState(CTransform::STATE_UP, vUp);
        pCom->SetTransformState(CTransform::STATE_LOOK, vLook);
    }
    else
    {
        _vector vRight = XMVector3Normalize(XMVector3Cross(vLook, _vector{ 0.f, 1.f, 0.f }));
        _vector vUp = XMVector3Normalize(XMVector3Cross(vRight, vLook));
        vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

        pCom->SetTransformState(CTransform::STATE_RIGHT, vRight);
        pCom->SetTransformState(CTransform::STATE_UP, vUp);
        pCom->SetTransformState(CTransform::STATE_LOOK, vLook);
    }
}

void CEffectManager::CalNormalWallHitFront(CTransform* pCom, HIT_RESULT Hit)
{
    pCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(Hit.vHitPos.x, Hit.vHitPos.y, Hit.vHitPos.z, 1.f));

    _vector vLook = XMVector3Normalize(XMLoadFloat3(&Hit.vNormal));
    vLook.m128_f32[3] = 0.f;

    if (XMVector3Equal(vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f)))
    {
        // vLook이 Y축 방향이면, vRight를 X축 방향 벡터로 설정
        _vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f); // 기본 X축 방향
        _vector vUp = XMVector3Normalize(XMVector3Cross(vRight, vLook)); // vUp은 vRight와 vLook의 외적
        vRight = XMVector3Normalize(XMVector3Cross(vLook, vUp)); // vRight를 재정의하여 직교 관계 유지

        pCom->SetTransformState(CTransform::STATE_RIGHT, vLook);
        pCom->SetTransformState(CTransform::STATE_UP, vUp);
        pCom->SetTransformState(CTransform::STATE_LOOK, vRight);
    }
    else if (XMVector3Equal(vLook, XMVectorSet(0.f, -1.f, 0.f, 0.f)))
    {
        // vLook이 Y축 방향이면, vRight를 X축 방향 벡터로 설정
        _vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f); // 기본 X축 방향
        _vector vUp = XMVector3Normalize(XMVector3Cross(vRight, vLook)); // vUp은 vRight와 vLook의 외적
        vRight = XMVector3Normalize(XMVector3Cross(vLook, vUp)); // vRight를 재정의하여 직교 관계 유지

        pCom->SetTransformState(CTransform::STATE_RIGHT, vLook);
        pCom->SetTransformState(CTransform::STATE_UP, vUp);
        pCom->SetTransformState(CTransform::STATE_LOOK, vRight);
    }
    else
    {
        _vector vRight = XMVector3Normalize(XMVector3Cross(vLook, _vector{ 0.f, 1.f, 0.f }));
        _vector vUp = XMVector3Normalize(XMVector3Cross(vRight, vLook));
        vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

        pCom->SetTransformState(CTransform::STATE_RIGHT, vLook);
        pCom->SetTransformState(CTransform::STATE_UP, vUp);
        pCom->SetTransformState(CTransform::STATE_LOOK, vRight);
    }
}

void CEffectManager::Free()
{
    __super::Free();

    for (_int i = 0; i < 50; ++i)
    {
        if (m_pEffectProjectileWallHitSpots[i])
            SafeRelease(m_pEffectProjectileWallHitSpots[i]);

        if (m_pEffectBulletTrails[i])
            SafeRelease(m_pEffectBulletTrails[i]);

        if (m_pEffectBullets[i])
            SafeRelease(m_pEffectBullets[i]);
    }


    for (_int i = 0; i < 30; ++i)
    {
        if (m_pEffectProjectileWallHits[i])
            SafeRelease(m_pEffectProjectileWallHits[i]);

        if (m_pEffectProjectileWallHitFronts[i])
            SafeRelease(m_pEffectProjectileWallHitFronts[i]);
    }


    for (_int i = 0; i < 20; ++i)
    {
        if (m_pEffectFollowLightings[i])
            SafeRelease(m_pEffectFollowLightings[i]);

        if (m_pEffectCircle1004[i])
            SafeRelease(m_pEffectCircle1004[i]);

        if (m_pEffectHit1004[i])
            SafeRelease(m_pEffectHit1004[i]);

        if (m_pEffectProjectileMonsterHits[i])
            SafeRelease(m_pEffectProjectileMonsterHits[i]);
    }
    

    for (_int i = 0; i < 10; ++i)
    {
        if (m_pEffectSphere1004[i])
            SafeRelease(m_pEffectSphere1004[i]);
    }

    for (_int i = 0; i < 3; ++i)
    {
        if (m_pEffectBossRockAlert[i])
            SafeRelease(m_pEffectBossRockAlert[i]);

        if (m_pEffectBossRockAlertFinal[i])
            SafeRelease(m_pEffectBossRockAlertFinal[i]);
    }


    if (m_pEffectTestTrail)
        SafeRelease(m_pEffectTestTrail);

    for (_int i = 0; i < 2; ++i)
    {
        if (m_pEffectBossEye[i])
            SafeRelease(m_pEffectBossEye[i]);
    }

    SafeRelease(m_pEffectDoor);

    SafeRelease(m_pEffectRoarNoise);
}
