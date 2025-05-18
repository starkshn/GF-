#include "pch.h"
#include "MonsterBase.h"
#include "GameInstance.h"
#include "Player.h"
#include "EffectManager.h"
#include "ParticleManager.h"
#include "MonsterWeaponBase.h"
#include "UIManager.h"
#include "UIHPBar.h"

CMonsterBase::CMonsterBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CStateObjectContainer(pDevice, pContext)
{
}

CMonsterBase::CMonsterBase(const CMonsterBase& Prototype)
	:
	CStateObjectContainer(Prototype)
{
}

HRESULT CMonsterBase::InitializePrototype()
{
	return S_OK;
}

HRESULT CMonsterBase::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		MONSTER_DESC* pDesc = static_cast<MONSTER_DESC*>(pArg);
		
		m_iNumAttackRanges = pDesc->iNumAttackRanges;
		m_iMaxHP = pDesc->iHP;
		SetHP(pDesc->iHP);
		for (_int i = 0; i < m_iNumAttackRanges; ++i)
		{
			m_AttackRanges.push_back(static_cast<_float>(*(pDesc->pAttackRanges + i)));
			m_AttackCoolTimes.push_back(static_cast<_float>(*(pDesc->pAttackCoolTimes + i)));
			m_AttackCoolTimeList.push_back(false);
			m_CurrentAccAttackCoolTimes.push_back(0.f);
		}

		m_iNavMeshIdx = pDesc->iNavMeshIdx;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMonsterBase::PriorityUpdate(_float fTimeDelta)
{
	__super::PriorityUpdate(fTimeDelta);
}

void CMonsterBase::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	CheckTrailAlertPoint();
	UpdateChaseParticle(fTimeDelta);


	if (!m_bDamageSoundOnce)
	{
		m_fDAcc += fTimeDelta;
		if (m_fDAcc > 0.3f)
		{
			m_fDAcc = 0.f;
			m_bDamageSoundOnce = 1;
		}
	}
}

void CMonsterBase::LateUpdate(_float fTimeDelta)
{
	__super::LateUpdate(fTimeDelta);
}

HRESULT CMonsterBase::Render()
{
	return S_OK;
}

_bool CMonsterBase::CheckActive()
{
	if (false == GetActive())
		return false;
	else
	{
		if (MONSTER_STATE::MS_DEACTIVE & m_iState)
		{
			SetForcePrevState(MONSTER_STATE::MS_DEACTIVE);
			SetForceState(MONSTER_STATE::MS_ACTIVE);
			return true;
		}
		return false;
	}
}

_bool CMonsterBase::CheckUpdateStateTime(_float fTimeDelta)
{
	m_fAccCycleTime += fTimeDelta;

	UpdateAttackCoolTimes(fTimeDelta);

	if (m_fCycleTime <= m_fAccCycleTime)
	{
		m_fAccCycleTime = 0.f;
		return true;
	}
	return false;
}

void CMonsterBase::UpdateAttackCoolTimes(_float fTimeDelta)
{
	for (_int i = 0; i < m_iNumAttackRanges; ++i)
	{
		// 쿨타임이 0초가 아니고 뭔가 공격을 한번해서 i번째 공격이 불가능 한 상태라면, 즉 쿨타임 상태라면 fTimeDelta를 i번째 공격에 누적한다.
		if (m_AttackCoolTimes[i] != 0.f && m_AttackCoolTimeList[i])
		{
			m_CurrentAccAttackCoolTimes[i] += fTimeDelta;
			if (m_CurrentAccAttackCoolTimes[i] >= m_AttackCoolTimes[i])
			{
				m_CurrentAccAttackCoolTimes[i] = 0.f;

				// 사용할 수 있으므로 false로 만들어준다.
				m_AttackCoolTimeList[i] = false;
			}
		}
	}
}

void CMonsterBase::UpdateChaseParticle(_float fTimeDelta)
{
	if ((MS_CHASE & m_iState) || (MS_MOVE_BACK & m_iState) || (MS_MOVE_FRONT & m_iState) || (MS_MOVE_RIGHT & m_iState) || (MS_MOVE_LEFT & m_iState) || (MS_MOVE_BACK & m_iState))
	{
		m_fChaseParticleAccTime += fTimeDelta;
		if (m_fChaseParticleAccTime >= m_fChaseParticleMaxTime)
		{
			m_fChaseParticleAccTime = 0.f;
			_vector vPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);
			HIT_RESULT Hit = {};
			XMStoreFloat3(&Hit.vHitPos, vPos);
			CParticleManager::GetInstance()->ActiveParticle(CParticleManager::MONSTER_CHASE, Hit);
		}
	}
}


_bool CMonsterBase::UpdateStateCloseRangeAttackMonster(_float fTimeDelta)
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

		// 최초에 한번 실행되는 분기문
		if (m_bFindPlayer)
		{
			if (MONSTER_STATE::MS_IDLE & m_iState)
			{
				SetForceState(MONSTER_STATE::MS_CHASE);
				return true;
			}

			if (MONSTER_STATE::MS_CHASE & m_iState)
			{
				// CHASE상태인데, 구르는 범위안에 있고 구르는 상태가 아니라면 구르는 상태로 상태를 변경한다.
				if (IsTargetInRollRange() && IsNotRollState())
				{
					RememberPrevState();
					SubState(m_iState);
					_int iRand = rand() % 2;
					MONSTER_STATE eRollState = iRand == 0 ? MONSTER_STATE::MS_ROLL_LEFT : MONSTER_STATE::MS_ROLL_RIGHT;
					SetForceState(eRollState);
					return true;
				}

				// CHASE 상태인데, 공격 범위 안에 타겟이 있고, 시야각 안에 타겟이 있고, 공격 상태가 아니라면 공격 상태로 변경한다.
				if (IsTargetInAttackRange() && IsTargetInLookRange() && (MONSTER_STATE::MS_ATTACK ^ m_iState))
				{
					RememberPrevState();
					SubState(m_iState);
					SetForceState(MONSTER_STATE::MS_ATTACK);
					return true;
				}
				// CHASE상태인데, 공격 범위에 있지 않다면
				else
				{
					return false;
				}
			}
		}
		else
		{
			// 플레이어 못찾았다면 IDLE로 되돌린다.
			m_bFindPlayer = false;
			SetForceState(MONSTER_STATE::MS_IDLE);
			return true;
		}
	}

	return false;
}

_bool CMonsterBase::UpdateStateFarRangeAttackMonster(_float fTimeDelta)
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

		if (m_bFindPlayer)
		{
			// CHASE상태 일때
			if (MONSTER_STATE::MS_CHASE & m_iState)
			{
				if (IsTargetInRollRange())
				{
					if (m_bCanRollBack && ((MONSTER_STATE::MS_ROLL_RIGHT ^ m_iState) && (MONSTER_STATE::MS_ROLL_LEFT ^ m_iState) && (MONSTER_STATE::MS_ROLL_BACK ^ m_iState)))
					{
						SetForceState(MONSTER_STATE::MS_ROLL_BACK);
					}
					else
					{
						if ((MONSTER_STATE::MS_ROLL_RIGHT ^ m_iState) && (MONSTER_STATE::MS_ROLL_LEFT ^ m_iState))
						{
							_int iRand = rand() % 2;
							MONSTER_STATE eRollState = iRand == 0 ? MONSTER_STATE::MS_ROLL_LEFT : MONSTER_STATE::MS_ROLL_RIGHT;
							SetForceState(eRollState);
						}
					}

					return true;
				}
				else if (IsTargetInMinDistance() && (MONSTER_STATE::MS_MOVE_BACK ^ m_iState))
				{
					SetForceState(MONSTER_STATE::MS_MOVE_BACK);
					return true;
				}
				// 타겟까지의 거리가 공격 범위보다는 크고 AmingRange 보다는 작을 때
				else if (IsTargetInAimingRange() && (MONSTER_STATE::MS_MOVE_FRONT ^ m_iState) && (MONSTER_STATE::MS_MOVE_LEFT ^ m_iState) && (MONSTER_STATE::MS_MOVE_RIGHT ^ m_iState))
				{
					_int iRand = rand() % 3;

					if (0 == iRand)
						SetForceState(MONSTER_STATE::MS_MOVE_FRONT);
					else if (1 == iRand)
						SetForceState(MONSTER_STATE::MS_MOVE_LEFT);
					else if (2 == iRand)
						SetForceState(MONSTER_STATE::MS_MOVE_RIGHT);

					// SetForceState(MONSTER_STATE::MS_MOVE_FRONT);
					return true;
				}
				// 타겟까지의 거리가 MinDistance보다는 크고 AttackRange중 쿨타임 중이 아닌 공격 범위 안에 있을 때
				else if (IsTargetInAttackRange() && (MONSTER_STATE::MS_ATTACK ^ m_iState))
				{
					SetForceState(MONSTER_STATE::MS_ATTACK);
					return true;
				}
			}
			// 앞으로 걷는 중일 때 Aiming 범위 안에 들어와서 걷는다.
			else if (MONSTER_STATE::MS_MOVE_FRONT & m_iState)
			{
				if (IsTargetInAttackRange() && (MONSTER_STATE::MS_ATTACK ^ m_iState))
				{
					SetForceState(MONSTER_STATE::MS_ATTACK);
					return true;
				}
				
			}
			// 앞으로 걷는 중일 때 Aiming 범위 안에 들어와서 걷는다.
			else if (MONSTER_STATE::MS_MOVE_LEFT & m_iState)
			{
				if (IsTargetInAttackRange() && (MONSTER_STATE::MS_ATTACK ^ m_iState))
				{
					SetForceState(MONSTER_STATE::MS_ATTACK);
					return true;
				}

			}
			// 앞으로 걷는 중일 때 Aiming 범위 안에 들어와서 걷는다.
			else if (MONSTER_STATE::MS_MOVE_RIGHT & m_iState)
			{
				if (IsTargetInAttackRange() && (MONSTER_STATE::MS_ATTACK ^ m_iState))
				{
					SetForceState(MONSTER_STATE::MS_ATTACK);
					return true;
				}

			}
			else if (MONSTER_STATE::MS_MOVE_BACK & m_iState)
			{
				
			}

			if (MONSTER_STATE::MS_IDLE & m_iState)
			{
				SetForceState(MONSTER_STATE::MS_CHASE);
				return true;
			}
			else if (GetDistanceFromMonsterToTarget(m_pTargetPlayer) >= m_fAimingRange && (MONSTER_STATE::MS_CHASE ^ m_iState))
			{
				SetForceState(MONSTER_STATE::MS_CHASE);
				return true;
			}
		}
		else
		{
			// 플레이어 못찾았다면 IDLE로 되돌린다.
			m_bFindPlayer = false;
			SetForceState(MONSTER_STATE::MS_IDLE);
		}
	}

	return false;
}

// 내적의 결과에 따라서 true, false반환한다. 0~1사이라면 true반환 하여 찾았다고 해준다.
_bool CMonsterBase::SearchPlayer()
{
	m_pTargetPlayer = dynamic_cast<CPlayer*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GAMEPLAY, m_iLevelIndex, TEXT("A")));
	if (nullptr == m_pTargetPlayer)
		return false;

	_vector vTargetDir = GetDirectionToTarget(m_pTargetPlayer);
	
	_vector vMonsterLook;
	vMonsterLook = XMVector4Normalize(m_pTransformCom->GetTransformState(CTransform::STATE_LOOK));

	_vector vDotResult = XMVector4Dot(vMonsterLook, vTargetDir);
	_float vDotRet = XMVectorGetX(vDotResult);

	if (0.f <= vDotRet && vDotRet <= 1.f)
		return true;

	return false;
}

void CMonsterBase::RotateMonsterToPlayer(_float fTimeDelta)
{
	// TODO
	_vector vTargetDir, vMonsterLook;
	vTargetDir = GetDirectionToTarget(m_pTargetPlayer);
	vTargetDir = XMVector3Normalize(XMVectorSet(XMVectorGetX(vTargetDir), 0.0f, XMVectorGetZ(vTargetDir), 0.0f));

	vMonsterLook = XMVector3Normalize(m_pTransformCom->GetTransformState(CTransform::STATE_LOOK));
	vMonsterLook = XMVector3Normalize(XMVectorSet(XMVectorGetX(vMonsterLook), 0.0f, XMVectorGetZ(vMonsterLook), 0.0f));
	
	_float fDot = XMVectorGetX(XMVector3Dot(vTargetDir, vMonsterLook));

	// 플레이어와 몬스터의 각도 차이가 거의 없다면, 약 8도 이내라면 그냥 return한다
	// - 이거 때문에 시간 다 잡아 먹음 씨발!! -> 이거 예외 안넣으면 덜덜덜 떨린다.
	if (fabs(fDot) >= 0.99f)
		return;

	_float fMinus = 1.f;

	// 두 축에 직교하는 벡터가 나온다. 이 직교하는 벡터의 Y성분이 음수라면 몬스터 기준으로 플레이어가 왼쪽에 있는 것이고 그게 아니라면 오른쪽에 있는 것이다.
	_vector vCross = XMVector3Cross(vMonsterLook, vTargetDir);
	if (XMVectorGetY(vCross) < 0.f)
	{
		fMinus = fMinus * -1.f;
	}
	
	m_pTransformCom->Turn(m_pTransformCom->GetTransformState(CTransform::STATE_UP), fTimeDelta * fMinus);
}

_vector CMonsterBase::GetTargetPos(CGameObject* pTarget)
{
	_vector vTargetPos = static_cast<CTransform*>(pTarget->GetComponent(TEXT("Com_Transform")))->GetTransformState(CTransform::STATE_POS);
	return vTargetPos;
}

_float CMonsterBase::GetDistanceFromMonsterToTarget(CGameObject* pTarget)
{
	_vector vTargetPos = GetTargetPos(pTarget);
	vTargetPos.m128_f32[1] = 0.f;
	_vector vPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);
	return XMVectorGetX(XMVector3Length(vTargetPos - vPos));
}

_vector CMonsterBase::GetDirectionToTarget(CGameObject* pTarget)
{
	_vector vTargetPos = GetTargetPos(pTarget);
	_vector vPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);
	return XMVector3Normalize(vTargetPos - vPos);
}

_vector CMonsterBase::GetPlayerPos()
{
	if (m_pTargetPlayer)
		return static_cast<CTransform*>(m_pTargetPlayer->GetComponent(TEXT("Com_Transform")))->GetTransformState(CTransform::STATE_POS);

	return _vector{};
}

_float CMonsterBase::GetDistanceFromMonsterToPlayer()
{
	if (m_pTargetPlayer)
	{
		_vector vTargetPos = GetTargetPos(m_pTargetPlayer);
		_vector vPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);
		return XMVectorGetX(XMVector3Length(vTargetPos - vPos));
	}
		
	return _float{0.f};
}

_vector CMonsterBase::GetDirectionToPlayer()
{
	if (m_pTargetPlayer)
	{
		_vector vTargetPos = GetPlayerPos();
		_vector vPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);
		return XMVector3Normalize(vTargetPos - vPos);
	}
	return _vector{};
}


_bool CMonsterBase::IsTargetInAttackRange()
{
	_float fDistance = GetDistanceFromMonsterToTarget(m_pTargetPlayer);

	// 구를 수 있는데 RollRange보다 Dist가 작다면 false
	if (m_bCanRoll && fDistance <= m_fRollRange)
		return false;

	// 0.f != m_fMinDistanceFromTarget : FarRangeAttack 몬스터이다.
	// - fDistance가 m_fMinDistanceFromTarget 보다 더 작다면 공격을 못하는 범위안에 있다. 즉, 너무 가깝다 -> 내가 그냥 공격 못하도록 막는다.
	if (0.f != m_fMinDistanceFromTarget && fDistance < m_fMinDistanceFromTarget)
		return false;
	
	for (_int i = 0; i < m_iNumAttackRanges; ++i)
	{
		if (fDistance <= m_AttackRanges[i])
		{
			if (false == IsAttackAnimIndexInCoolTime(i))
			{
				m_iAttackAnimIndex = i;
				return true;
			}
			// true라면 현재 쿨탐임 중이기 때문에 continue로 다른 공격 인덱스를 찾는다.
			else
				continue;
		}
	}

	// 전부다 쿨 타임중이라면 false를 반환하여 공격 상태가 못 되도록 해준다.
	return false;
}

_bool CMonsterBase::IsTargetInLookRange()
{
	_vector vLook = m_pTransformCom->GetTransformState(CTransform::STATE_LOOK);
	_vector vTargetDir = GetDirectionToTarget(m_pTargetPlayer);

	_float fDot = XMVectorGetX(XMVector3Dot(vLook, vTargetDir));

	_float fCos20 = cosf(XMConvertToRadians(30.0f));
	if (fDot >= fCos20)
		return true;

	return false;
}

_bool CMonsterBase::IsTargetInRollRange()
{
	if (!m_bCanRoll)
		return false;

	_float vDistance = GetDistanceFromMonsterToTarget(m_pTargetPlayer);

	// 가장 짧은 공격 범위보다 Distance가 더 작다면 '구른다'
	if (vDistance <= m_fRollRange)
		return true;
	return false;
}

_bool CMonsterBase::IsTargetInAimingRange()
{
	_float fDistFromTarget = GetDistanceFromMonsterToTarget(m_pTargetPlayer);

	if (m_fMinDistanceFromTarget > fDistFromTarget)
		return false;

	if (0 != m_fAimingRange && fDistFromTarget > m_fAimingRange)
		return false;

	// 공격 범위 안에 있는 어떤 공격 범위보다 더 작은게 있다면 false를 반환한다.
	for (_int i = 0; i < m_iNumAttackRanges; ++i)
		if (fDistFromTarget < m_AttackRanges[i])
			return false;
	
	return true;
}

_bool CMonsterBase::IsTargetInMinDistance()
{
	_float fDist = GetDistanceFromMonsterToTarget(m_pTargetPlayer);

	if (m_fRollRange > fDist)
		return false;

	if (0.f != m_fMinDistanceFromTarget && fDist <= m_fMinDistanceFromTarget)
		return true;
	return false;
}

void CMonsterBase::CheckHP()
{
	auto pObj = m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, m_iLevelIndex, TEXT("A"));
	if (pObj)
	{
		_uint iModel = static_cast<CPlayer*>(pObj)->GetCurrentModelIndex();
		if (iModel == 2)
			m_iHP -= 100;
		else
			m_iHP -= 25;
	}

	m_fHPVerticalRatio = (_float)(m_iHP / m_iMaxHP);


	if (m_iHP <= 0)
	{
		m_iHP = 0;
		m_bDead = true;

		if (m_bDeadOnce)
		{
			m_bDeadOnce = false;
			m_pGameInst->PlayEffect(TEXT("MonsterDead.mp3"), 1.f);
		}
	}
	else
	{
		if (m_bDamageSoundOnce)
		{
			m_bDamageSoundOnce = false;
			m_pGameInst->PlayEffect(TEXT("MonsterHit.mp3"), 1.f);
		}
	}

	UpdateHPBar();
}

void CMonsterBase::CheckColPlayerBullet()
{
	// if (m_pGameInst->CheckIntersectInLayer(m_iLevelIndex, TEXT("Layer_PlayerBullet"), m_pColliderCom, m_HitResult))
	// {
	// 	StartMonsterHitEffect();
	// 	StartMonsterHitParticle();
	// 	CheckHP();
	// }
}

void CMonsterBase::StartMonsterHitEffect(PROJECTILE_TYPE eType)
{
	CTransform* pCom = static_cast<CTransform*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, m_iLevelIndex, TEXT("A"))->GetComponent(TEXT("Com_Transform")));
	_vector vPlayerPos = pCom->GetTransformState(CTransform::STATE_POS); // Dst
	_vector vNotAccuartePos = XMLoadFloat3(&m_HitRet.vHitPos);
	_vector vDir = vNotAccuartePos - vNotAccuartePos;
	vDir = XMVector3Normalize(vDir);
	CalNotAccuarteHitPosAndNormal(vPlayerPos, vNotAccuartePos, vDir, m_HitRet.vHitPos, m_HitRet.vNormal);



	switch (eType)
	{
	case Client::PROJECTILE_TYPE::PJ_DEFAULT:
	{
		CEffectManager::GetInstance()->ActiveEffect(CEffectManager::PROJECTILE_WALL_HIT, m_HitRet);
		CEffectManager::GetInstance()->ActiveEffect(CEffectManager::PROJECTILE_MONSTER_HIT, m_HitRet);
	}
		break;
	case Client::PROJECTILE_TYPE::PJ_1004:
	{
		CEffectManager::GetInstance()->ActiveEffect(CEffectManager::CIRCLE_1004, m_HitRet);
		CEffectManager::GetInstance()->ActiveEffect(CEffectManager::SPHERE_1004, m_HitRet);
	}
		break;
	case Client::PROJECTILE_TYPE::PJ_1503_DEFAULT:
	{
		CEffectManager::GetInstance()->ActiveEffect(CEffectManager::BULLET_HIT03, m_HitRet);
	}
		break;
	case Client::PROJECTILE_TYPE::PJ_1508_DEFAULT:
	{
		CEffectManager::GetInstance()->ActiveEffect(CEffectManager::BULLET_HIT04, m_HitRet);
	}
		break;
	case Client::PROJECTILE_TYPE::PJ_END:
		break;
	default:
		break;
	}


}

void CMonsterBase::StartMonsterHitParticle(PROJECTILE_TYPE eType)
{
	CTransform* pCom = static_cast<CTransform*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, m_iLevelIndex, TEXT("A"))->GetComponent(TEXT("Com_Transform")));
	_vector vPlayerPos = pCom->GetTransformState(CTransform::STATE_POS); // Dst
	_vector vNotAccuartePos = XMLoadFloat3(&m_HitRet.vHitPos);
	_vector vDir = vNotAccuartePos - vNotAccuartePos;
	vDir = XMVector3Normalize(vDir);
	CalNotAccuarteHitPosAndNormal(vPlayerPos, vNotAccuartePos, vDir, m_HitRet.vHitPos, m_HitRet.vNormal);

	switch (eType)
	{
	case Client::PROJECTILE_TYPE::PJ_DEFAULT:
	{
		CParticleManager::GetInstance()->ActiveParticle(CParticleManager::PARTICLE_TYPE::MONSTER_HIT, m_HitRet);

	}
	break;
	case Client::PROJECTILE_TYPE::PJ_1004:
	{
	
	}
	break;
	case Client::PROJECTILE_TYPE::PJ_1503_DEFAULT:
		break;
	case Client::PROJECTILE_TYPE::PJ_1508_DEFAULT:
		break;
	case Client::PROJECTILE_TYPE::PJ_END:
		break;
	default:
		break;
	}

}

void CMonsterBase::CalHitPosAndNormal(_fvector vPlayerPos, _fvector vPlayerToBulletDir, PROJECTILE_TYPE eType)
{
	CBoundingAABB* pAABB = static_cast<CBoundingAABB*>(m_pColliderCom->GetBoundingRaw());
	if (nullptr == pAABB)
		return;

	_float fDist = 0.f;
	_vector vHitPos = {};
	if (pAABB->GetBounding()->Intersects(vPlayerPos, vPlayerToBulletDir, fDist))
		vHitPos = vPlayerPos + vPlayerToBulletDir * fDist;

	_float3 vCenter = const_cast<CBoundingAABB*>(pAABB)->GetBounding()->Center;
	_float3 vExtents = const_cast<CBoundingAABB*>(pAABB)->GetBounding()->Extents;
	_float3 vMin = const_cast<CBoundingAABB*>(pAABB)->GetMin();
	_float3 vMax = const_cast<CBoundingAABB*>(pAABB)->GetMax();

	// 충돌 지점과 AABB 중심의 상대 위치 계산
	_float3 vTempHitPos;
	XMStoreFloat3(&vTempHitPos, vHitPos);
	_float3 vRelativePos = { vTempHitPos.x - vCenter.x, vTempHitPos.y - vCenter.y, vTempHitPos.z - vCenter.z };

	// 노말 벡터 초기화
	_float3 vNormal = { 0.0f, 0.0f, 0.0f };

	// 충돌 방향을 기준으로 가장 가까운 축 계산
	if (abs(vRelativePos.x) / vExtents.x > abs(vRelativePos.y) / vExtents.y && abs(vRelativePos.x) / vExtents.x > abs(vRelativePos.z) / vExtents.z)
	{
		// X축 방향
		vNormal = (vRelativePos.x > 0) ? _float3(1, 0, 0) : _float3(-1, 0, 0);
	}
	else if (abs(vRelativePos.y) / vExtents.y > abs(vRelativePos.z) / vExtents.z)
	{
		// Y축 방향
		vNormal = (vRelativePos.y > 0) ? _float3(0, 1, 0) : _float3(0, -1, 0);
	}
	else
	{
		// Z축 방향
		vNormal = (vRelativePos.z > 0) ? _float3(0, 0, 1) : _float3(0, 0, -1);
	}

	XMStoreFloat3(&m_HitRet.vHitPos, vHitPos);
	m_HitRet.vNormal = vNormal;


	StartMonsterHitEffect(eType);
	StartMonsterHitParticle(eType);

	CheckHP();


	// 스크린 좌표
	{
		_vector vScreenPos = CalScreenPosFromWorldPos(vHitPos);

		_int iColor = _int(m_pGameInst->ComputeRandom(0.f, 1.999f));
		_int iRand = _int(m_pGameInst->ComputeRandom(100.f, 99999.f));
		
		// cout << "SetDamageFont : " << iColor << endl;
		m_pGameInst->SetDamageFont(iRand, iColor, XMVectorGetX(vScreenPos), XMVectorGetY(vScreenPos));

		// cout << "맞은 부분 SX : " << XMVectorGetX(vScreenPos) << " SY : " << XMVectorGetY(vScreenPos) << endl;
	}


}

_vector CMonsterBase::GetSocketWorldPos()
{
	if (m_PartObjects[PART_WEAPON])
	{
		_matrix SocketWorldMatrix = static_cast<CMonsterWeaponBase*>(m_PartObjects[PART_WEAPON])->GetSocketWorldMatrix();
		return SocketWorldMatrix.r[3];
	}
	return _vector{};
}

void CMonsterBase::UpdateHPBar()
{
	if (m_strHPBarNames.size() != 3)
		return;

	_int iMaxHP = m_iMaxHP;
	_int iHalf = iMaxHP / 2;
	_int iCurHP = m_iHP;

	// 절반 보다 큰 경우
	if (iCurHP > iHalf)
	{
		CUIHPBar* pUIHPUp = static_cast<CUIHPBar*>(m_pGameInst->GetEleUIFromPanel(m_strHPBarNames[0], m_strHPBarNames[1]));
		CUIHPBar* pUIHPDown = static_cast<CUIHPBar*>(m_pGameInst->GetEleUIFromPanel(m_strHPBarNames[0], m_strHPBarNames[2]));

		if (GetObjectTag() == "Boss")
		{
			pUIHPUp->SetHPValPlayer(iCurHP % iHalf);
			pUIHPDown->SetHPValPlayer(iHalf);
			return;
		}



		pUIHPUp->SetHPVal(iCurHP % iHalf);
		pUIHPDown->SetHPVal(iHalf);
	}
	// 절반과 같거나 작은 경우
	else
	{
		CUIHPBar* pUIHPUp = static_cast<CUIHPBar*>(m_pGameInst->GetEleUIFromPanel(m_strHPBarNames[0], m_strHPBarNames[1]));
		CUIHPBar* pUIHPDown = static_cast<CUIHPBar*>(m_pGameInst->GetEleUIFromPanel(m_strHPBarNames[0], m_strHPBarNames[2]));

		if (GetObjectTag() == "Boss")
		{
			pUIHPUp->SetHPValPlayer(0);
			pUIHPDown->SetHPValPlayer(iCurHP);
			return;
		}
		

		pUIHPUp->SetHPVal(0);
		pUIHPDown->SetHPVal(iCurHP);
	}

}

void CMonsterBase::ForceDead()
{
	m_iHP = 0;
	if (m_iHP <= 0)
	{
		m_iHP = 0;
		m_bDead = true;
	}

	UpdateHPBar();
}

_vector CMonsterBase::CalScreenPosFromWorldPos(_fvector vWorldPos)
{
	auto ViewDesc = m_pGameInst->GetViewPortDesc();
	_matrix View = m_pGameInst->GetTransformMatrix(CPipeLine::D3DTS_VIEW);
	_matrix Proj = m_pGameInst->GetTransformMatrix(CPipeLine::D3DTS_PROJ);

	_vector vViewPos = XMVector4Transform(vWorldPos, View);
	_vector vClipSpace = XMVector4Transform(vViewPos, Proj);

	// 뷰 스페이스 상의 Z값
	_float fW = XMVectorGetW(vClipSpace);

	// Z나누기 수행
	vClipSpace = XMVectorSetX(vClipSpace, XMVectorGetX(vClipSpace) / fW);
	vClipSpace = XMVectorSetY(vClipSpace, XMVectorGetY(vClipSpace) / fW);

	_vector vNDC = vClipSpace;

	return { (XMVectorGetX(vNDC) + 1.f) * ViewDesc.Width * 0.5f, -(XMVectorGetY(vNDC) - 1.f) * ViewDesc.Height * 0.5f };
}

void CMonsterBase::PlayWalkSound(_float fTimeDelta)
{
	m_fCurDist = GetDistanceFromMonsterToPlayer();
	if (m_fCurDist <= m_fMaxDist)
	{
		m_fRatio = 1.f - (m_fCurDist / m_fMaxDist);
		m_fRatio = min(m_fRatio, 0.65f);

		if ((m_iState & MS_CHASE))
		{
			m_fWalkAcc += fTimeDelta;
			if (m_fWalkAcc >= 0.3f)
			{
				m_fWalkAcc = 0.f;
				if (m_iWalkIdx++ == 0)
				{
					m_pGameInst->PlayEffect(TEXT("MonsterWalk1.mp3"), m_fRatio);
				}

				if (m_iWalkIdx++ == 1)
				{
					m_pGameInst->PlayEffect(TEXT("MonsterWalk2.mp3"), m_fRatio);
				}

				if (m_iWalkIdx == 2)
				{
					m_pGameInst->PlayEffect(TEXT("MonsterWalk3.mp3"), m_fRatio);
					m_iWalkIdx = 0;
				}
			}
		}
		else
		{
			m_fWalkAcc = 0.f;
		}
	}
}


HRESULT CMonsterBase::ReadyComponents()
{
	return S_OK;
}

HRESULT CMonsterBase::ReadyPartObjects()
{
	return S_OK;
}

void CMonsterBase::Free()
{
	__super::Free();
}