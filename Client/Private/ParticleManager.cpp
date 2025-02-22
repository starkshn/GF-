#include "pch.h"
#include "ParticleManager.h"
#include "GameInstance.h"
#include "Layer.h"


IMPLEMENT_SINGLETON(CParticleManager);

CParticleManager::CParticleManager()
    :
    m_pGameInst(CGameInstance::GetInstance())
{
}

HRESULT CParticleManager::Initialize()
{
    CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Particles_WallHit"));
    if (nullptr == pLayer)
        return E_FAIL;

    // WallHit
    {
        auto WallHits = pLayer->GetGameObjectList();
        auto iter = WallHits.begin();

        for (_int i = 0; i < 30; ++i)
        {
            m_pParticleWallHits[i] = static_cast<CParticle*>(*(iter++));
            SafeAddRef(m_pParticleWallHits[i]);
        }
    }

    
    // MonsterSpawn
    {
        pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Particles_MonsterSpawn"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        for (_int i = 0; i < 20; ++i)
        {
            m_pParticleMonsterSpawns[i] = static_cast<CParticle*>(*(iter++));
            SafeAddRef(m_pParticleMonsterSpawns[i]);
        }
    }
    
    // MonsterHit
    {
        pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Particles_MonsterHit"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        for (_int i = 0; i < 30; ++i)
        {
            m_pParticleMonsterHits[i] = static_cast<CParticle*>(*(iter++));
            SafeAddRef(m_pParticleMonsterHits[i]);
        }
    }
    
    // ProjectileHit
    {
        pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Particles_ProjectileHit"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        for (_int i = 0; i < 50; ++i)
        {
            m_pParticleProjectileHits[i] = static_cast<CParticle*>(*(iter++));
            SafeAddRef(m_pParticleProjectileHits[i]);
        }
    }


    // ParticleBossRock
    {
        pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Particles_BossRock"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();
        
        m_pParticleBossRock = static_cast<CParticle*>(*(iter++));
        SafeAddRef(m_pParticleBossRock);
    }

    // ParticleBossRockDust
    {
        pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Particles_BossRockDust"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        m_pParticleBossRockDust = static_cast<CParticle*>(*(iter++));
        SafeAddRef(m_pParticleBossRockDust);
    }

    // ParticleBossRockCol
    {
        pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Particles_BossRockCol"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        for (_int i = 0; i < 11; ++i)
        {
            m_pParticleBossRockCol[i] = static_cast<CParticle*>(*(iter++));
            SafeAddRef(m_pParticleBossRockCol[i]);
        }
    }


    // ParticleBossRockColDust
    {
        pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Particles_BossRockColDust"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        for (_int i = 0; i < 11; ++i)
        {
            m_pParticleBossRockColDust[i] = static_cast<CParticle*>(*(iter++));
            SafeAddRef(m_pParticleBossRockColDust[i]);
        }
    }


    // ParticleBossRockColDustLighting
    {
        pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Particles_BossRockColDustLighting"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        for (_int i = 0; i < 11; ++i)
        {
            m_pParticleBossRockColDustLighting[i] = static_cast<CParticle*>(*(iter++));
            SafeAddRef(m_pParticleBossRockColDustLighting[i]);
        }
    }

    // ParticleBossSmash
    {
        pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Particles_BossSmash"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        m_pParticleBossSmash = static_cast<CParticle*>(*(iter++));
        SafeAddRef(m_pParticleBossSmash);
    }


    // ParticleSmashDust
    {
        pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Particles_BossSmashDust"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        for (_int i = 0; i < 3; ++i)
        {
            m_pParticleBossSmashDust[i] = static_cast<CParticle*>(*(iter++));
            SafeAddRef(m_pParticleBossSmashDust[i]);
        }
    }

    // MonsterSpawn
    {
        pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Particles_MonsterDead"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        for (_int i = 0; i < 10; ++i)
        {
            m_pParticleMonsterDeads[i] = static_cast<CParticle*>(*(iter++));
            SafeAddRef(m_pParticleMonsterDeads[i]);
        }
    }
   
    // ParticleBossWalk
    {
        pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Particles_BossWalk"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        m_pParticleBossWalk = static_cast<CParticle*>(*(iter++));
        SafeAddRef(m_pParticleBossWalk);
    }


    // FollowLightingDust
    {
        pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Particles_FollowLightingDust"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        for (_int i = 0; i < 20; ++i)
        {
            m_pParticleFollowLightingDust[i] = static_cast<CParticle*>(*(iter++));
            SafeAddRef(m_pParticleFollowLightingDust[i]);
        }
    }

    // MonsterChase
    {
        pLayer = m_pGameInst->FindLayer(LEVEL_GLOBAL, TEXT("Layer_Particles_MonsterChase"));
        if (nullptr == pLayer)
            return E_FAIL;

        auto GameObjects = pLayer->GetGameObjectList();
        auto iter = GameObjects.begin();

        for (_int i = 0; i < 20; ++i)
        {
            m_pParticleMonsterChase[i] = static_cast<CParticle*>(*(iter++));
            SafeAddRef(m_pParticleMonsterChase[i]);
        }
    }

	return S_OK;
}

void CParticleManager::Update(_float fTimeDelta)
{
    for (_int i = 0; i < 10; ++i)
    {
        if (nullptr != m_pParticleWallHits[i] && false == m_pParticleWallHits[i]->IsActivate())
        {
            m_pParticleWallHits[i]->ResetParticles();
            m_pParticleWallHits[i]->DeActivate();
        }

        if (nullptr != m_pParticleMonsterSpawns[i] && false == m_pParticleMonsterSpawns[i]->IsActivate())
        {
            m_pParticleMonsterSpawns[i]->ResetParticles();
            m_pParticleMonsterSpawns[i]->DeActivate();
        }

        if (nullptr != m_pParticleMonsterHits[i] && false == m_pParticleMonsterHits[i]->IsActivate())
        {
            m_pParticleMonsterHits[i]->ResetParticles();
            m_pParticleMonsterHits[i]->DeActivate();
        }

        if (nullptr != m_pParticleProjectileHits[i] && false == m_pParticleProjectileHits[i]->IsActivate())
        {
            m_pParticleProjectileHits[i]->ResetParticles();
            m_pParticleProjectileHits[i]->DeActivate();
        }

        if (nullptr != m_pParticleMonsterDeads[i] && false == m_pParticleMonsterDeads[i]->IsActivate())
        {
            m_pParticleMonsterDeads[i]->ResetParticles();
            m_pParticleMonsterDeads[i]->DeActivate();
        }

       /* if (nullptr != m_pParticleBossRock && false == m_pParticleBossRock->IsActivate())
        {
            m_pParticleBossRock->ResetParticles();
            m_pParticleBossRock->DeActivate();
        }*/
    }

    if (nullptr != m_pParticleBossSmash && false == m_pParticleBossSmash->IsActivate())
    {
        m_pParticleBossSmash->ResetParticles();
        m_pParticleBossSmash->DeActivate();
    }

    for (_int i = 0; i < 3; ++i)
    {
        if (nullptr != m_pParticleBossSmashDust[i] && false == m_pParticleBossSmashDust[i]->IsActivate())
        {
            m_pParticleBossSmashDust[i]->ResetParticles();
            m_pParticleBossSmashDust[i]->DeActivate();
        }
    }


    for (_int i = 0; i < 11; ++i)
    {
        if (nullptr != m_pParticleBossRockCol[i] && false == m_pParticleBossRockCol[i]->IsActivate())
        {
            m_pParticleBossRockCol[i]->ResetParticles();
            m_pParticleBossRockCol[i]->DeActivate();
        }

        if (nullptr != m_pParticleBossRockColDust[i] && false == m_pParticleBossRockColDust[i]->IsActivate())
        {
            m_pParticleBossRockColDust[i]->ResetParticles();
            m_pParticleBossRockColDust[i]->DeActivate();
        }

        if (nullptr != m_pParticleBossRockColDustLighting[i]  && false == m_pParticleBossRockColDustLighting[i]->IsActivate())
        {
            m_pParticleBossRockColDustLighting[i]->ResetParticles();
            m_pParticleBossRockColDustLighting[i]->DeActivate();
        }
    }
    
    

   

    for (_int i = 0; i < 20; ++i)
    {
        if (nullptr != m_pParticleFollowLightingDust[i] && false == m_pParticleFollowLightingDust[i]->IsActivate())
        {
            m_pParticleFollowLightingDust[i]->ResetParticles();
            m_pParticleFollowLightingDust[i]->DeActivate();
        }

    }

    for (_int i = 0; i < 30; ++i)
    {
        if (nullptr != m_pParticleMonsterChase[i] && false == m_pParticleMonsterChase[i]->IsActivate())
        {
            m_pParticleMonsterChase[i]->ResetParticles();
            m_pParticleMonsterChase[i]->DeActivate();
        }
    }

    // Test
    // m_pParticleBossSmash->UpdateParticleWorldPos(_vector{ 0.f, 3.f, 8.f, 1.f });
}

void CParticleManager::ActiveParticle(PARTICLE_TYPE eType, HIT_RESULT HitResult)
{
    switch (eType)
    {
    case Client::CParticleManager::WALL_HIT:
    {
        for (_int i = 0; i < 10; ++i)
        {
            if (nullptr == m_pParticleWallHits[i])
                continue;

            if (m_pParticleWallHits[i]->IsActivate())
                continue;

            // cout << i << " 번째 파티클 발동" << endl;
            m_pParticleWallHits[i]->Activate();

            _matrix WorldMatrix = XMMatrixIdentity();
            WorldMatrix.r[3] = XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f);

            m_pParticleWallHits[i]->UpdateParticleWorldMatrix(WorldMatrix);
            m_pParticleWallHits[i]->SetPivot(m_pGameInst->ConvertHitNormalToPivotPos(HitResult.vNormal) / 1.5f);
            // m_pParticleWallHits[i]->SetPivot(XMLoadFloat3(&HitResult.vNormal));
            break;
        }
    }
        break;
    case Client::CParticleManager::MONSTER_HIT:
    {
        for (_int i = 0; i < 10; ++i)
        {
            if (nullptr == m_pParticleMonsterHits[i])
                continue;

            if (m_pParticleMonsterHits[i]->IsActivate())
                continue;

            m_pParticleMonsterHits[i]->Activate();

            _matrix WorldMatrix = XMMatrixIdentity();
            WorldMatrix.r[3] = XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f);

            m_pParticleMonsterHits[i]->UpdateParticleWorldMatrix(WorldMatrix);
            // m_pParticleMonsterHits[i]->SetPivot(m_pGameInst->ConvertHitNormalToPivotPos(HitResult.vNormal));
            m_pParticleMonsterHits[i]->SetPivot(_vector{0.f, -2.f, 0.f, 0.f});
            break;
        }
    }
        break;
    case Client::CParticleManager::MONSTER_SPAWN:
    {
        for (_int i = 0; i < 10; ++i)
        {
            if (nullptr == m_pParticleMonsterSpawns[i])
                continue;

            if (m_pParticleMonsterSpawns[i]->IsActivate())
                continue;

            m_pParticleMonsterSpawns[i]->Activate();

            _matrix WorldMatrix = XMMatrixIdentity();
            WorldMatrix.r[3] = XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f);

            m_pParticleMonsterSpawns[i]->UpdateParticleWorldMatrix(WorldMatrix);
            // m_pParticleMonsterSpawns[i]->SetPivot(m_pGameInst->ConvertHitNormalToPivotPos(HitResult.vNormal));
            break;
        }
    }
    break;
    case CParticleManager::PROJETILE_HIT:
    {
        for (_int i = 0; i < 10; ++i)
        {
            if (nullptr == m_pParticleProjectileHits[i])
                continue;

            if (m_pParticleProjectileHits[i]->IsActivate())
                continue;

            m_pParticleProjectileHits[i]->Activate();

            _matrix WorldMatrix = XMMatrixIdentity();
            WorldMatrix.r[3] = XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f);

            _vector vLook = XMVector3Normalize(XMLoadFloat3(&HitResult.vNormal));
            vLook.m128_f32[3] = 0.f;

            _vector vRight = XMVector3Normalize(XMVector3Cross(vLook, _vector{0.f, 1.f, 0.f}));
            _vector vUp = XMVector3Normalize(XMVector3Cross(vRight, vLook));
            vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

            WorldMatrix.r[0] = XMVectorSet(vRight.m128_f32[0], vRight.m128_f32[1], vRight.m128_f32[2], 0.f);
            WorldMatrix.r[1] = XMVectorSet(vUp.m128_f32[0], vUp.m128_f32[1], vUp.m128_f32[2], 0.f);
            WorldMatrix.r[2] = XMVectorSet(vLook.m128_f32[0], vLook.m128_f32[1], vLook.m128_f32[2], 0.f);

            m_pParticleProjectileHits[i]->UpdateParticleWorldMatrix(WorldMatrix);

            // m_pParticleMonsterSpawns[i]->SetPivot(m_pGameInst->ConvertHitNormalToPivotPos(HitResult.vNormal));

            m_pParticleProjectileHits[i]->SetPivot(_vector{0.f, 0.f, 0.f});

            break;
        }
    }
    break;
    case CParticleManager::BOSS_ROCK:
    {
        if (nullptr == m_pParticleBossRock)
            return;

        if (m_pParticleBossRock->IsActivate())
            return;

        m_pParticleBossRock->Activate();

        _matrix WorldMatrix = XMMatrixIdentity();
        WorldMatrix.r[3] = XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f);
        m_pParticleBossRock->UpdateParticleWorldMatrix(WorldMatrix);
    }
    break;
    case CParticleManager::BOSS_ROCK_DUST:
    {
        if (nullptr == m_pParticleBossRockDust)
            return;

        if (m_pParticleBossRockDust->IsActivate())
            return;

        m_pParticleBossRockDust->Activate();

        _matrix WorldMatrix = XMMatrixIdentity();
        WorldMatrix.r[3] = XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f);
        m_pParticleBossRockDust->UpdateParticleWorldMatrix(WorldMatrix);
    }
    break;
    case CParticleManager::BOSS_ROCK_COL:
    {
        for (_int i = 0; i < 11; ++i)
        {
            if (nullptr == m_pParticleBossRockCol[i] || m_pParticleBossRockCol[i]->IsActivate())
                return;

            m_pParticleBossRockCol[i]->Activate();
            _matrix WorldMatrix = XMMatrixIdentity();
            WorldMatrix.r[3] = XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f);
            m_pParticleBossRockCol[i]->UpdateParticleWorldMatrix(WorldMatrix);

            break;
        }
    }
    break;
    case CParticleManager::BOSS_ROCK_COL_DUST:
    {
        for (_int i = 0; i < 11; ++i)
        {
            if (nullptr == m_pParticleBossRockColDust[i] || m_pParticleBossRockColDust[i]->IsActivate())
                return;

            m_pParticleBossRockColDust[i]->Activate();

            _matrix WorldMatrix = XMMatrixIdentity();
            WorldMatrix.r[3] = XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f);
            m_pParticleBossRockColDust[i]->UpdateParticleWorldMatrix(WorldMatrix);

            break;
        }
    }
    break;
    case CParticleManager::BOSS_ROCK_COL_DUST_LIGHTING:
    {
        for (_int i = 0; i < 11; ++i)
        {
            if (nullptr == m_pParticleBossRockColDustLighting[i] || m_pParticleBossRockColDustLighting[i]->IsActivate())
                return;

            m_pParticleBossRockColDustLighting[i]->Activate();

            _matrix WorldMatrix = XMMatrixIdentity();
            WorldMatrix.r[3] = XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f);
            m_pParticleBossRockColDustLighting[i]->UpdateParticleWorldMatrix(WorldMatrix);

            break;
        }
    }
    break;
    case CParticleManager::BOSS_SMASH:
    {
        if (nullptr == m_pParticleBossSmash)
            return;

        if (m_pParticleBossSmash->IsActivate())
            return;

        m_pParticleBossSmash->Activate();
        
        _vector vWorldPos = XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f);
        m_pParticleBossSmash->UpdateParticleWorldPos(vWorldPos);
    }
    break;
    case CParticleManager::BOSS_SMASH_DUST:
    {
        for (_int i = 0; i < 3; ++i)
        {
            if (nullptr == m_pParticleBossSmashDust[i] || m_pParticleBossSmashDust[i]->IsActivate())
                continue;

            m_pParticleBossSmashDust[i]->Activate();
            _vector vWorldPos = XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f);
            m_pParticleBossSmashDust[i]->UpdateParticleWorldPos(vWorldPos);
            break;
        }
    }
    break;
    case CParticleManager::BOSS_WALK:
    {
        if (nullptr == m_pParticleBossWalk)
            return;

        if (m_pParticleBossWalk->IsActivate())
            return;

        m_pParticleBossWalk->Activate();
    }
    break;
    case CParticleManager::MONSTER_DEAD:
    {
        for(_int i = 0; i < 10; ++i)
        {
            if (nullptr == m_pParticleMonsterDeads[i])
                continue;

            if (m_pParticleMonsterDeads[i]->IsActivate())
                continue;

            m_pParticleMonsterDeads[i]->Activate();

            _matrix WorldMatrix = XMMatrixIdentity();
            WorldMatrix.r[3] = XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f);

            m_pParticleMonsterDeads[i]->UpdateParticleWorldMatrix(WorldMatrix);
            // m_pParticleMonsterDeads[i]->SetPivot(m_pGameInst->ConvertHitNormalToPivotPos(HitResult.vNormal));
            break;
        }
    }
    break;
    case CParticleManager::FOLLOW_LIGHTING_DUST:
    {
        for (_int i = 0; i < 20; ++i)
        {
            if (nullptr == m_pParticleFollowLightingDust[i])
                continue;

            if (m_pParticleFollowLightingDust[i]->IsActivate())
                continue;

            m_pParticleFollowLightingDust[i]->Activate();

            _matrix WorldMatrix = XMMatrixIdentity();
            WorldMatrix.r[3] = XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f);

            m_pParticleFollowLightingDust[i]->UpdateParticleWorldMatrix(WorldMatrix);
            break;
        }
    }
    break;
    case CParticleManager::MONSTER_CHASE:
    {
        for (_int i = 0; i < 30; ++i)
        {
            if (nullptr == m_pParticleMonsterChase[i] || m_pParticleMonsterChase[i]->IsActivate())
                continue;

            m_pParticleMonsterChase[i]->Activate();

            _matrix WorldMatrix = XMMatrixIdentity();
            WorldMatrix.r[3] = XMVectorSet(HitResult.vHitPos.x, HitResult.vHitPos.y, HitResult.vHitPos.z, 1.f);
            m_pParticleMonsterChase[i]->UpdateParticleWorldMatrix(WorldMatrix);
            break;
        }
    }
    break;
    case Client::CParticleManager::TYPE_END:
        break;
    default:
        break;
    }
}

// 활성화 되어 있는 첫번째 파티클을 찾아서 위치를 업데이트
void CParticleManager::UpdateParticlePos(PARTICLE_TYPE eType, _fvector vPos)
{
    switch (eType)
    {
    case Client::CParticleManager::WALL_HIT:
        break;
    case Client::CParticleManager::MONSTER_HIT:
        break;
    case Client::CParticleManager::MONSTER_SPAWN:
        break;
    case Client::CParticleManager::PROJETILE_HIT:
        break;
    case Client::CParticleManager::BOSS_ROCK:
    {
        if (m_pParticleBossRock->IsActivate())
            m_pParticleBossRock->UpdateParticleWorldPos(vPos);
    }
        break;
    case Client::CParticleManager::BOSS_ROCK_DUST:
    {
        if (m_pParticleBossRockDust->IsActivate())
            m_pParticleBossRockDust->UpdateParticleWorldPos(vPos);
    }
    break;
    case Client::CParticleManager::BOSS_SMASH:
    {
        // m_pParticleBossSmash->UpdateParticleWorldPos(vPos);
    }
        break;

    case Client::CParticleManager::TYPE_END:
        break;
    default:
        break;
    }
}

void CParticleManager::DeActivateParticle(PARTICLE_TYPE eType)
{
    switch (eType)
    {
    case Client::CParticleManager::WALL_HIT:
        break;
    case Client::CParticleManager::MONSTER_HIT:
        break;
    case Client::CParticleManager::MONSTER_SPAWN:
        break;
    case Client::CParticleManager::PROJETILE_HIT:
        break;
    case Client::CParticleManager::BOSS_ROCK:
    {
        if (m_pParticleBossRock->IsActivate())
        {
            m_pParticleBossRock->DeActivate();
            m_pParticleBossRock->ResetParticles();
        }
    }
        break;
    case Client::CParticleManager::BOSS_ROCK_DUST:
    {
        if (m_pParticleBossRockDust->IsActivate())
        {
            m_pParticleBossRockDust->DeActivate();
            m_pParticleBossRockDust->ResetParticles();
        }
    }
    break;
    case CParticleManager::BOSS_WALK:
    {
        m_pParticleBossWalk->DeActivate();
    }
    break;
    case Client::CParticleManager::TYPE_END:
        break;
    default:
        break;
    }
}

void CParticleManager::Free()
{
    __super::Free();


    for (_int i = 0; i < 30; ++i)
    {
        if (m_pParticleWallHits[i])
            SafeRelease(m_pParticleWallHits[i]);

        if (m_pParticleMonsterHits[i])
            SafeRelease(m_pParticleMonsterHits[i]);

        if (m_pParticleMonsterChase[i])
            SafeRelease(m_pParticleMonsterChase[i]);
    }

    for (_int i = 0; i < 50; ++i)
    {
        if (m_pParticleProjectileHits[i])
            SafeRelease(m_pParticleProjectileHits[i]);
    }

    for (_int i = 0; i < 20; ++i)
    {
        if (m_pParticleMonsterSpawns[i])
            SafeRelease(m_pParticleMonsterSpawns[i]);

        if (m_pParticleFollowLightingDust[i])
            SafeRelease(m_pParticleFollowLightingDust[i]);
    }

    for (_int i = 0; i < 11; ++i)
    {
        if (m_pParticleBossRockCol[i])
            SafeRelease(m_pParticleBossRockCol[i]);

        if (m_pParticleBossRockColDust[i])
            SafeRelease(m_pParticleBossRockColDust[i]);

        if (m_pParticleBossRockColDustLighting[i])
            SafeRelease(m_pParticleBossRockColDustLighting[i]);

    }

    for (_int i = 0; i < 10; ++i)
    {
        if (m_pParticleMonsterDeads[i])
            SafeRelease(m_pParticleMonsterDeads[i]);
    }

    for (_int i = 0; i < 3; ++i)
        if (m_pParticleBossSmashDust[i])
            SafeRelease(m_pParticleBossSmashDust[i]);

    SafeRelease(m_pParticleBossRock);
    SafeRelease(m_pParticleBossRockDust);
    SafeRelease(m_pParticleBossSmash);
    SafeRelease(m_pParticleBossWalk);
}

