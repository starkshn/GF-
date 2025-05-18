#include "pch.h"
#include "Boss.h"
#include "GameInstance.h"
#include "Collider.h"
#include "Navigation.h"
#include "BossBody.h"
#include "MonsterWeapon01.h"
#include "BossRock.h"
#include "BossWeapon.h"
#include "EffectManager.h"
#include "ParticleManager.h"
#include "Layer.h"
#include "Trigger.h"
#include "Spawner.h"
#include "SpawnTrigger.h"
#include "UITex.h"
#include "MonsterManager.h"

CBoss::CBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CMonsterBase(pDevice, pContext)
{

}

CBoss::CBoss(const CBoss& Prototype)
    :
    CMonsterBase(Prototype)
{

}

HRESULT CBoss::InitializePrototype()
{
    return S_OK;
}

HRESULT CBoss::Initialize(void* pArg)
{
    CMonsterBase::MONSTER_DESC Desc = {};
    Desc.iNumPartObjects = CMonsterBase::PART_END;
    Desc.fSpeedPerSec = 3.f;
    Desc.fRotSpeedPerSec = XMConvertToRadians(180.f);
    Desc.strObjectTag = "Boss";
    Desc.bUseSpawnPos = true;


    // CLevel_Boss::ReadyLayerBoss 함수에서 체력을 설정한다.
    Desc.iHP = 2000;

    // 점프후 바닥 양손으로 찍기, 돌덩이 던지기, 한손으로 바닥 내려찍기, 몬스터 소환
    _float vAttackRanges[4] = { 25.f, 35.f, 7.f, 60.f};
    Desc.pAttackRanges = vAttackRanges;

    _float vAttackCoolTimes[4] = { 15.f, 10.f, 5.f, 25.f };
    Desc.pAttackCoolTimes = vAttackCoolTimes;
    Desc.iNumAttackRanges = 4; 

    // Test 241210
    {
        /*_float vAttackRanges[4] = { 25.f, 1.f, 6.f, 30.f };
        Desc.pAttackRanges = vAttackRanges;

        _float vAttackCoolTimes[4] = { 100.f, 100.f, 100.f, 6.f};
        Desc.pAttackCoolTimes = vAttackCoolTimes;
        Desc.iNumAttackRanges = 4;*/
    }

    Desc.iLevelIndex = LEVEL_BOSS;

    // TODO -> 여기서 파일에서 읽어와서 값을 써야한다.
    _vector vSpawnPos{ 0.f, 0.f, 50.f, 1.f };
    XMStoreFloat3(&Desc.vSpawnPos, vSpawnPos);

    SetForceState(MONSTER_STATE::MS_DEACTIVE);
    m_iPrevState = MONSTER_STATE::MS_DEACTIVE;

    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    SetNavMeshIdx(CStateObjectContainer::NAVMESH_IDX::S_BOSS);

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    if (FAILED(ReadyPartObjects()))
        return E_FAIL;

    if (FAILED(ReadyBossRock()))
        return E_FAIL;

    if (FAILED(ReadyMeteors()))
        return E_FAIL;

    m_pTransformCom->Rotation(m_pTransformCom->GetTransformState(CTransform::STATE_UP), XMConvertToDegrees(180.f));
    SetActive(true);

    CEffectManager::GetInstance()->ActiveEffect(CEffectManager::BOSS_EYE, HIT_RESULT{}, m_PartObjects[PART_EFFECT]);
    CEffectManager::GetInstance()->ActiveEffect(CEffectManager::BOSS_EYE, HIT_RESULT{}, m_PartObjects[PART_EFFECT2]);

    return S_OK;
}

void CBoss::PriorityUpdate(_float fTimeDelta)
{
    __super::PriorityUpdate(fTimeDelta);
}

void CBoss::Update(_float fTimeDelta)
{
    if (!CheckDead())
        m_bUpdateState = UpdateBossAction(fTimeDelta);
    else
    {
        static _bool bDead = false;

        if (!bDead)
        {
            m_bUpdateState = true;
            bDead = true;
        }
    }

    __super::Update(fTimeDelta);

    m_pColliderCom->Update(m_pTransformCom->GetWorldMatrix());

    // ActivePazeTwo(fTimeDelta);
}

void CBoss::LateUpdate(_float fTimeDelta)
{
    __super::LateUpdate(fTimeDelta);

    m_pGameInst->AddRenderObject(CRenderer::RG_NONLIGHT, this);

#ifdef _DEBUG
    // m_pGameInst->AddDebugComponent(m_pColliderCom);
#endif
}

HRESULT CBoss::Render()
{
    // m_pColliderCom->Render();
    return S_OK;
}

_bool CBoss::UpdateBossState(_float fTimeDelta)
{
    if (CheckActive())
        return true;

    // ActiveAnimation이 끝나지 않았다면 애니매이션을 변경 못하도록 막는다.
    if (false == m_bEndActiveAnim)
        return false;
    
    if (CheckUpdateStateTime(fTimeDelta))
    {
        if (false == m_bFindPlayer)
            m_bFindPlayer = SearchPlayer();
        else
        {
            // 플레이어 찾았으면 상태를 변경한다는 의미에서 true 반환한다.
            if (MONSTER_STATE::MS_IDLE & m_iState)
            {
                SetForceState(MONSTER_STATE::MS_CHASE);

                HIT_RESULT Hit = {};
                CParticleManager::GetInstance()->ActiveParticle(CParticleManager::BOSS_WALK, Hit);

                return true;
            }
            else if (MONSTER_STATE::MS_CHASE & m_iState)
            {
                if (IsTargetInLookRange() && (MONSTER_STATE::MS_ATTACK ^ m_iState))
                {
                    if (IsTargetInAttackRange())
                    {
                        RememberPrevState();
                        SubState(m_iState);
                        SetForceState(MONSTER_STATE::MS_ATTACK);

                        // CParticleManager::GetInstance()->DeActivateParticle(CParticleManager::BOSS_WALK);

                        if (ATTACK02 == GetAttackAnimIndex())
                            StartRock();
                        else if (ATTACK03 == GetAttackAnimIndex())
                            StartOneHandAttkack();

                        return true;
                    }
                    // 공격 가능 한 상태이지만 전부다 쿨 타임일 때
                    else
                    {
                        // TODO
                    }
                }
            }
        }
    }

    return false;
}

void CBoss::UpdateBossRockWorldMatrix(_float fTimeDelta)
{
    _matrix BossHandMatrix = static_cast<CBossWeapon*>(m_PartObjects[PART_WEAPON])->GetBossHandWorldMatrix();
    _float4 vRight, vUp, vLook, vHandPos;
    XMStoreFloat4(&vRight, BossHandMatrix.r[0]);
    XMStoreFloat4(&vUp, BossHandMatrix.r[1]);
    XMStoreFloat4(&vLook, BossHandMatrix.r[2]);
    XMStoreFloat4(&vHandPos, BossHandMatrix.r[3]);

    // 소켓에 해당하는 Hand의 월드 행렬을 가져온다.
    CTransform* pBossRockTransform = static_cast<CTransform*>(m_pBossRock->GetComponent(TEXT("Com_Transform")));
    pBossRockTransform->SetTransformState(CTransform::STATE_RIGHT, XMLoadFloat4(&vRight));
    pBossRockTransform->SetTransformState(CTransform::STATE_UP, XMLoadFloat4(&vUp));
    pBossRockTransform->SetTransformState(CTransform::STATE_LOOK, XMLoadFloat4(&vLook));
    pBossRockTransform->SetTransformState(CTransform::STATE_POS, XMLoadFloat4(&vHandPos));

    m_fAccCurScalingTime += fTimeDelta;

    // 1.7초보다 작을 때 까지만 scale을 증가시킨다.
    if (m_fMaxScalingTime > m_fAccCurScalingTime)
    {
        m_fAccScaling += m_fScalingSP * fTimeDelta;
        pBossRockTransform->Scaling(m_fAccScaling, m_fAccScaling, m_fAccScaling);

        _vector vPos = XMLoadFloat4(&vHandPos);
        _vector vHandUp = XMLoadFloat4(&vUp);
        
        // 파티클의 위치를 업데이트 한다.
        CParticleManager::GetInstance()->UpdateParticlePos(CParticleManager::BOSS_ROCK, vPos);          
        CParticleManager::GetInstance()->UpdateParticlePos(CParticleManager::BOSS_ROCK_DUST, vPos);
    }
}

void CBoss::ResetBossRockScale()
{
    m_fAccScaling = 0.f;
    m_fAccCurScalingTime = 0.f;

    _matrix BossHandMatrix = static_cast<CBossWeapon*>(m_PartObjects[PART_WEAPON])->GetBossHandWorldMatrix();
    _float4 vRight, vUp, vLook, vHandPos;
    XMStoreFloat4(&vRight, BossHandMatrix.r[0]);
    XMStoreFloat4(&vUp, BossHandMatrix.r[1]);
    XMStoreFloat4(&vLook, BossHandMatrix.r[2]);
    XMStoreFloat4(&vHandPos, BossHandMatrix.r[3]);

    CTransform* pBossRockTC = static_cast<CTransform*>(m_pBossRock->GetComponent(TEXT("Com_Transform")));

    pBossRockTC->SetTransformState(CTransform::STATE_RIGHT, XMLoadFloat4(&vRight));
    pBossRockTC->SetTransformState(CTransform::STATE_UP, XMLoadFloat4(&vUp));
    pBossRockTC->SetTransformState(CTransform::STATE_LOOK, XMLoadFloat4(&vLook));
    pBossRockTC->SetTransformState(CTransform::STATE_POS, XMLoadFloat4(&vHandPos));
}

_bool CBoss::UpdateBossAction(_float fTimeDelta)
{
    _bool bUpdate = UpdateBossState(fTimeDelta);

    if (MONSTER_STATE::MS_ACTIVE & m_iState)
    {
        // ACTIVE알람이 울려서 이제 종료를 할 차례이다.
        if (false == strcmp(m_strCurrentAlertPointName.c_str(), "ACTIVE"))
        {
            m_strCurrentAlertPointName = "NONE";
            EndActiveAnim();
            SetForceState(CMonsterBase::MONSTER_STATE::MS_IDLE);
            m_pGameInst->RenderOnPlayerUI();
            return true;
        }
    }

    if (false == bUpdate)
    {
        if (MONSTER_STATE::MS_CHASE & m_iState)
        {
            m_pTransformCom->GoFoward(fTimeDelta, m_pNavigationComs[GetNavMeshIdx()]);
            RotateMonsterToPlayer(fTimeDelta);
        }
        else if (MONSTER_STATE::MS_ATTACK & m_iState)
        {
            CheckBossAttackAnimAlert(fTimeDelta);
        }

        CheckBossAttackAnimIndex(fTimeDelta);
    }

    _vector vPos = m_pNavigationComs[GetNavMeshIdx()]->SetUpHeightOnNavigation(m_pTransformCom->GetTransformState(CTransform::STATE_POS));
    m_pTransformCom->SetTransformState(CTransform::STATE_POS, vPos);

    return bUpdate;
}

void CBoss::StartRock()
{
    ResetBossRockScale();
    m_pBossRock->Active();
    ActiveParticle(BOSS_PARTICLE_TYPE::PARITICLE_ROCK);
    ActiveParticle(BOSS_PARTICLE_TYPE::PARITICLE_ROCK_DUST);
    m_pGameInst->ActivePointLight("BossRock");
    FindAlertPos();
}

void CBoss::EndRock()
{

}

void CBoss::ThrowRock()
{
    m_pBossRock->Fire();

    CalThorwDir();

    // 파티클 비활성화
    CParticleManager::GetInstance()->DeActivateParticle(CParticleManager::BOSS_ROCK);
    CParticleManager::GetInstance()->DeActivateParticle(CParticleManager::BOSS_ROCK_DUST);
}

void CBoss::FindAlertPos()
{
    _vector vDst = GetTargetPos(m_pTargetPlayer);
    vDst.m128_f32[1] = 0.25f;
    
    HIT_RESULT Hit = {};
    XMStoreFloat3(&Hit.vHitPos, vDst);

    m_vAlertPos = vDst;
    CEffectManager::GetInstance()->ActiveEffect(CEffectManager::BOSS_ROCK_ALERT, Hit);
    CEffectManager::GetInstance()->ActiveEffect(CEffectManager::BOSS_ROCK_ALERT_FINAL, Hit);
}

void CBoss::FindAlertPosOneHand()
{
    _vector vPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);
    _vector vLook = m_pTransformCom->GetTransformState(CTransform::STATE_LOOK);
    vPos += vLook * 5.f;
    
    HIT_RESULT Hit = {};
    XMStoreFloat3(&Hit.vHitPos, vPos);

    m_vAlertPos = vPos;

    CEffectManager::GetInstance()->ActiveEffect(CEffectManager::BOSS_ROCK_ALERT, Hit);
    CEffectManager::GetInstance()->ActiveEffect(CEffectManager::BOSS_ROCK_ALERT_FINAL, Hit);
}

void CBoss::StartSpawnMonsters()
{
    ActiveSpawnTriggers();
}

_bool CBoss::CheckDead()
{
    static _bool bDead = false;
    if (m_iHP <= 0.f && m_bDead)
    {
        if (!bDead)
        {
            bDead = true;
            SetForceState(MONSTER_STATE::MS_BOSS_DEAD);

            CMonsterManager::GetInstance()->RemoveAllMonsters();

            /*CUITex* pUI = static_cast<CUITex*>(m_pGameInst->GetEleUI("UITexVictory"));
            if (pUI)
            {
                if (pUI->GetStartScaling())
                    return true;


                CUITex::UI_TEX_SCALE_DESC Desc;
                Desc.fScalingMaxSizeX = 800.f;
                Desc.fScalingMaxSizeY = 600.f;
                Desc.fScalingMinSizeX = 0.f;
                Desc.fScalingMinSizeY = 0.f;

                Desc.bHold = true;
                Desc.fHoldTime = 2.f;

                pUI->SetScalingDesc(Desc);
                pUI->StartScaling();
            }*/

            return true;
        }
    }
    return false;
}

void CBoss::CalThorwDir()
{
    _vector vSrc = static_cast<CTransform*>(m_pBossRock->GetComponent(TEXT("Com_Transform")))->GetTransformState(CTransform::STATE_POS);
    vSrc.m128_f32[1] -= 1.5f;
    _vector vDir = m_vAlertPos - vSrc;
    m_pBossRock->SetDir(XMVector3Normalize(vDir));
}

void CBoss::StartJump()
{
    _vector vSrc = m_pTransformCom->GetTransformState(CTransform::STATE_POS);
    _vector vDst = GetTargetPos(m_pTargetPlayer);
    m_vJumpEndPosDir = XMVector3Normalize(vDst - vSrc);
    m_bJumpMoveStart = true;
    FindAlertPos();

    _float fDist = XMVectorGetX(XMVector4Length(m_vAlertPos - vSrc));

    // 플레이어까지의 거리에 따른 속도를 구한다.
    _float fSP = fDist / m_fMaxAttack01MoveTime;
    m_fAttackMoveSP = fSP;
}

void CBoss::Jumping(_float fTimeDelta)
{
    
}

void CBoss::StartSmash()
{
    ActiveParticle(BOSS_PARTICLE_TYPE::PARTICLE_SMASH);
    ActiveParticle(BOSS_PARTICLE_TYPE::PARTICLE_SMASH_DUST);

    EndSmash();
}

void CBoss::EndSmash()
{
    // CEffectManager::GetInstance()->DeActiveEffect(CEffectManager::BOSS_ROCK_ALERT);
    // CEffectManager::GetInstance()->DeActiveEffect(CEffectManager::BOSS_ROCK_ALERT_FINAL);
}

void CBoss::StartOneHandAttkack()
{
    FindAlertPosOneHand();
}

void CBoss::OnHandAttackAlert()
{
    ActiveParticle(BOSS_PARTICLE_TYPE::PARTICLE_SMASH_DUST);
    ActiveParticle(BOSS_PARTICLE_TYPE::PARTICLE_SMASH);
    // CParticleManager::GetInstance()->ActivateParticle(CParticleManager::BOSS_SMASH);

}

_bool CBoss::IsTargetInAttackRange()
{
    _float fDistance = GetDistanceFromMonsterToTarget(m_pTargetPlayer);
    _float fMin = 99999.f;

    for (_int i = 0; i < m_iNumAttackRanges; ++i)
    {
        // if (fDistance <= m_AttackRanges[i])

        // 쿨 타임이 아닌것들 중에서 가장 가까운 공격 Range를 가지는 공격을 찾는다.
        if (false == IsAttackAnimIndexInCoolTime(i))
        {
            if (fMin > fabs(fDistance - m_AttackRanges[i]))
            {
                fMin = fabs(fDistance - m_AttackRanges[i]);
                m_iAttackAnimIndex = i;
            }
        }
        // true라면 현재 쿨탐임 중이기 때문에 continue로 다른 공격 인덱스를 찾는다.
        else continue;
    }

    if (99999.f != fMin)
        return true;
    // 전부다 쿨 타임중이라면 false를 반환하여 공격 상태가 못 되도록 해준다. || 찾은게 없다면
    return false;
}

void CBoss::CheckBossAttackAnimIndex(_float fTimeDelta)
{
    if (MONSTER_STATE::MS_ATTACK & m_iState)
    {
        if (ATTACK01 == GetAttackAnimIndex())
        {
            if (m_bJumpMoveStart)
            {
                m_fAccAttack01 += fTimeDelta;
                if (m_fMaxAttack01MoveTime > m_fAccAttack01)
                {
                    _vector vPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);
                    vPos = vPos + m_vJumpEndPosDir * fTimeDelta * m_fAttackMoveSP;
                    m_pTransformCom->SetTransformState(CTransform::STATE_POS, vPos);
                }
                else
                {
                    m_fAccAttack01 = 0.f;
                    m_bJumpMoveStart = false;
                }
            }
        }
        else if (ATTACK02 == GetAttackAnimIndex())
        {
            if (false == m_pBossRock->IsFire())
                UpdateBossRockWorldMatrix(fTimeDelta);
        }
        else if (ATTACK03 == GetAttackAnimIndex())
        {
            
        }
        
    }
}

void CBoss::CheckBossAttackAnimAlert(_float fTimeDelta)
{
    static _bool bOnce = false;

    if (ATTACK01 == GetAttackAnimIndex())
    {
        if (false == strcmp("ATTACK01", m_strCurrentAlertPointName.c_str()))
        {
            StartJump();
        }
        else if (false == strcmp("SMASH", m_strCurrentAlertPointName.c_str()))
        {
            StartSmash();
        }
    }
    // 이때 BossRock 던진다.
    else if (ATTACK02 == GetAttackAnimIndex() && (false == strcmp("ATTACK02", m_strCurrentAlertPointName.c_str())))
    {
        ThrowRock();
    }
    else if (ATTACK03 == GetAttackAnimIndex() && (false == strcmp("ATTACK03", m_strCurrentAlertPointName.c_str())))
    {
        OnHandAttackAlert();
    }
    else if (ATTACK04 == GetAttackAnimIndex() && (false == strcmp("ATTACK04", m_strCurrentAlertPointName.c_str())))
    {
        StartSpawnMonsters();
        bOnce = true;
    }

    if (bOnce)
    {
        static _float fAcc = 0.f;
        fAcc += fTimeDelta;
        if (fAcc > 1.25f)
        {
            fAcc = 0.f;
            bOnce = false;
            
            HIT_RESULT Hit = {};
            _vector vPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);
            _vector vLook = XMVector3Normalize(m_pTransformCom->GetTransformState(CTransform::STATE_LOOK));
            _vector vUp = XMVector3Normalize(m_pTransformCom->GetTransformState(CTransform::STATE_UP));
            _vector vRight = XMVector3Normalize(m_pTransformCom->GetTransformState(CTransform::STATE_RIGHT));

            vPos += vUp * 3.f;
            vPos += vLook * 3.f;

            XMStoreFloat3(&Hit.vHitPos, vPos);
            XMStoreFloat3(&Hit.vNormal, -vRight);
            CEffectManager::GetInstance()->ActiveEffect(CEffectManager::ROAR_NOISE, Hit);
        }
    }
   
    m_strCurrentAlertPointName = "NONE";
    SetCurrentAttackAnimIndexInCoolTime();
}

void CBoss::ActiveParticle(BOSS_PARTICLE_TYPE eType)
{
    switch (eType)
    {
    case BOSS_PARTICLE_TYPE::PARITICLE_ROCK:
    {
        HIT_RESULT Hit = {};
        XMStoreFloat3(&Hit.vHitPos, GetPariticlePos(POS_ROCK));
        CParticleManager::GetInstance()->ActiveParticle(CParticleManager::BOSS_ROCK, Hit);
    }
        break;
    case BOSS_PARTICLE_TYPE::PARITICLE_ROCK_DUST:
    {
        HIT_RESULT Hit = {};
        XMStoreFloat3(&Hit.vHitPos, GetPariticlePos(POS_ROCK));
        CParticleManager::GetInstance()->ActiveParticle(CParticleManager::BOSS_ROCK_DUST, Hit);
    }
    break;
    case BOSS_PARTICLE_TYPE::PARTICLE_SMASH:
    {
        HIT_RESULT Hit = {};
        XMStoreFloat3(&Hit.vHitPos, GetPariticlePos(POS_SMASH));
        CParticleManager::GetInstance()->ActiveParticle(CParticleManager::BOSS_SMASH, Hit);
    }
        break;
    case BOSS_PARTICLE_TYPE::PARTICLE_SMASH_DUST:
    {
        HIT_RESULT Hit = {};
        XMStoreFloat3(&Hit.vHitPos, GetPariticlePos(POS_SMASH));
        CParticleManager::GetInstance()->ActiveParticle(CParticleManager::BOSS_SMASH_DUST, Hit);
    }
    break;
    case BOSS_PARTICLE_TYPE::PARTICLE_TYPE_END:
        break;
    default:
        break;
    }
}

_vector CBoss::GetPariticlePos(BOSS_PARTICLE_POS eType)
{
    switch (eType)
    {
    case BOSS_PARTICLE_POS::POS_ROCK:
    {
        _matrix BossHandMatrix = static_cast<CBossWeapon*>(m_PartObjects[PART_WEAPON])->GetBossHandWorldMatrix();
        return BossHandMatrix.r[3];
    }
    break;
    case BOSS_PARTICLE_POS::POS_SMASH:
    {
        _matrix SmashMatrix = static_cast<CBossWeapon*>(m_PartObjects[PART_WEAPON])->GetBossSmashWorldMatrix();
        return SmashMatrix.r[3];
    }
        break;
    case BOSS_PARTICLE_POS::POS_END:
        break;
    default:
        break;
    }
    return _vector();
}

void CBoss::ActiveSpawnTriggers()
{
    CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_BOSS, TEXT("Layer_SpawnTrigger"));
    if (pLayer)
    {
        auto& pObjs = pLayer->GetGameObjectList();
        for (auto& pObj : pObjs)
        {
            if (nullptr != pObj)
            {
                CSpawnTrigger* pST = static_cast<CSpawnTrigger*>(pObj);
                if (pST)
                    pST->ActiveSpawnTrigger();
            }
        }
    }
}

void CBoss::ActivePazeTwo(_float fTimeDelta)
{
    static _float fAcc = 0.f;
    fAcc += fTimeDelta;
    if (fAcc >= 10.f)
    {
        fAcc = 0.f;
        SpawnMeteor();
    }
}

void CBoss::SpawnMeteor()
{
    /*for (_int i = 0; i < 1; ++i)
    {
        if (m_pMeteors[i])
        {
            if (false == m_pMeteors[i]->IsFire())
            {
                CalRandomEndPos(m_pMeteors[i]);
            }
        }
    }*/
}

void CBoss::CalRandomEndPos(CMeteor* pRock)
{
    _vector vEndPos{}, vStartPos{};
    CLayer* pLayer = m_pGameInst->FindLayer(LEVEL_BOSS, TEXT("Layer_SpawnTrigger"));
    auto pObj = pLayer->GetGameObjectList().back();
    CTransform* pCom = static_cast<CTransform*>(pObj->GetComponent(TEXT("Com_Transform")));
    if (pCom)
    {
        vEndPos = vStartPos = pCom->GetTransformState(CTransform::STATE_POS);
        vEndPos.m128_f32[1] += 20.f;
        vStartPos.m128_f32[1] += 20.f;
    }

    _float fRandX = m_pGameInst->ComputeRandom(-50.f, 50.f);
    _float fRandZ = m_pGameInst->ComputeRandom(-50.f, 50.f);
    
    XMVectorSetX(vEndPos, XMVectorGetX(vEndPos) + fRandX);
    XMVectorSetZ(vEndPos, XMVectorGetZ(vEndPos) + fRandZ);

    vStartPos = vEndPos;
    XMVectorSetY(vEndPos, 0.f);
    XMVectorSetZ(vStartPos, vStartPos.m128_f32[2] - 5.f);

    CTransform* pRockCom = static_cast<CTransform*>(pRock->GetComponent(TEXT("Com_Transform")));
    pRockCom->SetTransformState(CTransform::STATE_POS, vStartPos);

    _vector vDir = XMVector3Normalize(vEndPos - vStartPos);
    pRock->SetDir(vDir);
    pRock->Fire();
}

HRESULT CBoss::ReadyComponents()
{
    // Com_Collider
    {
        CBoundingAABB::BOUND_AABB_DESC		ColliderDesc{};
        ColliderDesc.vExtents = _float3(3.f, 3.f, 3.f);
        ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

        if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
            return E_FAIL;

        m_pColliderCom->SetComOwner(this);
    }

    SetNavMeshIdx(CStateObjectContainer::NAVMESH_IDX::S_BOSS);
    if (FAILED(ReadyNavMesh()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBoss::ReadyPartObjects()
{
    // Body
    {
        CStateObjectBody::STATE_OBJECT_BODY_DESC Desc = {};
        Desc.pParentMatrix = m_pTransformCom->GetWorldMatrixPtr();
        Desc.pState = &m_iState;
        Desc.pPrevState = &m_iPrevState;
        Desc.pOwner = this;
        Desc.iNumModels = 1;

        m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInst->ClonePrototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_BOSS, TEXT("Prototype_GameObject_BossBody"), &Desc));

        if (nullptr == m_PartObjects[PART_BODY])
            return E_FAIL;
    }


    // Weapon
    {
        CStateObjectWeapon::STATE_OBJECT_WEAPON_DESC Desc = {};
        Desc.pOwner = this;
        Desc.pState = &m_iState;
        Desc.pParentMatrix = m_pTransformCom->GetWorldMatrixPtr();

        // Bip001 R Hand
        Desc.pSocketMatrix = static_cast<CBossBody*>(m_PartObjects[PART_BODY])->GetBoneCombiedMatrix("Bip001 R Hand");

        m_PartObjects[PART_WEAPON] = static_cast<CPartObject*>(m_pGameInst->ClonePrototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_BOSS, TEXT("Prototype_GameObject_BossWeapon"), &Desc));
        if (nullptr == m_PartObjects[PART_WEAPON])
            return E_FAIL;
    }

    // Eye Left
    {
        CStateObjectWeapon::STATE_OBJECT_WEAPON_DESC Desc = {};
        Desc.pOwner = this;
        Desc.pState = &m_iState;
        Desc.pParentMatrix = m_pTransformCom->GetWorldMatrixPtr();
        Desc.pSocketMatrix = static_cast<CBossBody*>(m_PartObjects[PART_BODY])->GetBoneCombiedMatrix("3501_Head_eye_L");

        m_PartObjects[PART_EFFECT] = static_cast<CPartObject*>(m_pGameInst->ClonePrototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_BOSS, TEXT("Prototype_GameObject_BossEye"), &Desc));
        if (nullptr == m_PartObjects[PART_EFFECT])
            return E_FAIL;
    }

    // Eye Right
    {
        CStateObjectWeapon::STATE_OBJECT_WEAPON_DESC Desc = {};
        Desc.pOwner = this;
        Desc.pState = &m_iState;
        Desc.pParentMatrix = m_pTransformCom->GetWorldMatrixPtr();
        Desc.pSocketMatrix = static_cast<CBossBody*>(m_PartObjects[PART_BODY])->GetBoneCombiedMatrix("3501_Head_eye_R");

        m_PartObjects[PART_EFFECT2] = static_cast<CPartObject*>(m_pGameInst->ClonePrototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_BOSS, TEXT("Prototype_GameObject_BossEye"), &Desc));
        if (nullptr == m_PartObjects[PART_EFFECT2])
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CBoss::ReadyBossRock()
{
    CGameObject* pObj = nullptr;
    if (nullptr == (pObj = m_pGameInst->GetGameObjctInLayerBack(LEVEL_BOSS, LEVEL_BOSS, TEXT("Layer_BossRock"))))
        return E_FAIL;
    
    m_pBossRock = static_cast<CBossRock*>(pObj);
    return S_OK;
}

HRESULT CBoss::ReadyMeteors()
{
    auto GameObjects = m_pGameInst->FindLayer(LEVEL_BOSS, TEXT("Layer_Meteor"))->GetGameObjectList();
    auto iter = GameObjects.begin();

    for (_int i = 0; i < 10; ++i)
    {
        m_pMeteors[i] = static_cast<CMeteor*>(*(iter++));
        if (!m_pMeteors[i])
            return E_FAIL;

        m_pMeteors[i]->SetLightName(m_MNames[i]);
    }
    return S_OK;
}

CBoss* CBoss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBoss* pInstance = new CBoss(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CBoss");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CBoss::Clone(void* pArg)
{
    CBoss* pInstance = new CBoss(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CBoss");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CBoss::Free()
{
    __super::Free();
}
