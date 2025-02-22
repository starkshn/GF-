#pragma once
#include "Client_Defines.h"
#include "StateObjectContainer.h"
#include "GlobalFuncs.h"
#include "EffectManager.h"
#include "ParticleManager.h"

BEGIN(Engine)

class CCollider;
class CGameInstance;

END

BEGIN(Client)

class CPlayer;
class CBoss;
class CProjectile;

class CMonsterBase : public CStateObjectContainer
{
public:
	typedef struct monster_desc : public CONTAINER_OBJECT_DESC
	{
		_float*		pAttackRanges;
		_int		iNumAttackRanges;
		_float*		pAttackCoolTimes;
		_int		iNavMeshIdx;
		_int		iHP;

	} MONSTER_DESC;

	enum MONSTER_STATE
	{
		MS_RESET = 0x00000000,
		MS_IDLE = 0x00000001,
		MS_CHASE = MS_IDLE << 1,
		MS_ATTACK = MS_IDLE << 2,
		MS_GETHIT = MS_IDLE << 3,
		MS_ROLL_RIGHT = MS_IDLE << 4,
		MS_ROLL_LEFT = MS_IDLE << 5,
		MS_DETACH = MS_IDLE << 6,
		MS_DEACTIVE = MS_IDLE << 7,
		MS_ACTIVE = MS_IDLE << 8,

		// 03, 04 가 쓴다.
		MS_MOVE_LEFT = MS_IDLE << 9,
		MS_MOVE_RIGHT = MS_IDLE << 10,
		MS_MOVE_FRONT = MS_IDLE << 11,
		MS_MOVE_BACK = MS_IDLE << 12,

		// 04가 쓴다.
		MS_ROLL_BACK = MS_IDLE << 13,

		MS_CHARGE_HP = MS_IDLE << 14,	// 보스 전용

		MS_BOSS_DEAD = MS_IDLE << 15,
		
	};

protected:
	CMonsterBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterBase(const CMonsterBase& Prototype);
	virtual ~CMonsterBase() = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	_bool					CheckActive();
	friend class CBoss;

	_bool					CheckUpdateStateTime(_float fTimeDelta);

private:
	void					UpdateAttackCoolTimes(_float fTimeDelta);

protected:
	void					UpdateChaseParticle(_float fTimeDelta);

protected:

protected:
	virtual _bool			UpdateStateCloseRangeAttackMonster(_float fTimeDelta);
	virtual _bool			UpdateStateFarRangeAttackMonster(_float fTimeDelta);

protected:
	_bool					SearchPlayer();
	void					RotateMonsterToPlayer(_float fTimeDelta);
	
public:
	_vector					GetTargetPos(CGameObject* pTarget);
	_float					GetDistanceFromMonsterToTarget(CGameObject* pTarget);
	_vector					GetDirectionToTarget(CGameObject* pTarget);
	_bool					GetUpdateState() { return m_bUpdateState; }
	_int					GetAttackAnimIndex() { return m_iAttackAnimIndex; }
	_bool					GetEndActiveAnim() { return m_bEndActiveAnim; }
	_float					GetAminingRange() { return m_fAimingRange; };

	_vector					GetPlayerPos();
	_float					GetDistanceFromMonsterToPlayer();
	_vector					GetDirectionToPlayer();


protected:
	void					EndActiveAnim() 
	{ 
		m_bEndActiveAnim = true; 
		// SetForcePrevState(MONSTER_STATE::MS_IDLE);
		// SetForceState(MONSTER_STATE::MS_IDLE);
	}

public:
	_bool					IsNotRollState() { return (MONSTER_STATE::MS_ROLL_LEFT ^ m_iState) && (MONSTER_STATE::MS_ROLL_RIGHT ^ m_iState); }

	virtual _bool			IsTargetInAttackRange();
	_bool					IsTargetInLookRange();
	_bool					IsTargetInRollRange();
	_bool					IsAttackAnimIndexInCoolTime(_int iIndex) 
	{ 
		return m_AttackCoolTimeList[iIndex]; 
	}
	_bool					IsTargetInAimingRange();
	_bool					IsTargetInMinDistance();

public:
	void					SetAlertPointName(string strAlertPointName) { m_strCurrentAlertPointName = strAlertPointName; }

protected:
	void					SetCurrentAttackAnimIndexInCoolTime() { m_AttackCoolTimeList[m_iAttackAnimIndex] = true;  }

	void					CheckHP();
	_float					m_fDAcc = 0.f;
	_bool					m_bDamageSoundOnce = true;

public:
	_bool					IsDead() { return m_bDead; }

protected:
	void					CheckColPlayerBullet();
	void					StartMonsterHitEffect(PROJECTILE_TYPE eType);
	void					StartMonsterHitParticle(PROJECTILE_TYPE eType);

public:
	virtual void			CalHitPosAndNormal(_fvector vPlayerPos, _fvector vPlayerToBulletDir, PROJECTILE_TYPE eType) override;

public:
	_uint					GetCurrentLevelID() { return m_iLevelIndex; }

protected:
	virtual void			CheckTrailAlertPoint() {};
	virtual	void			EndTrailEffect() {};
	_vector					GetSocketWorldPos();

protected:
	virtual void			UpdateHPBar();

public:
	void					SetHPBarNames(vector<string> strNames)
	{
		for (auto iter = strNames.begin(); iter != strNames.end(); ++iter)
		{
			m_strHPBarNames.push_back(*iter);
		}
	}
	
public:
	void					ForceDead();


public:
	_vector					CalScreenPosFromWorldPos(_fvector vWorldPos);
	
protected:
	void					PlayWalkSound(_float fTimeDelta);

public:
	_int					GetMaxHP() { return m_iMaxHP; }
	_int					GetCurHP() { return m_iHP; }
	_float					GetHPVerticalRatio() { return m_fHPVerticalRatio; }

private:
	const _float			m_fCycleTime = 0.3f;
	_float					m_fAccCycleTime = 0;
	
protected:
	CGameObject*			m_pTargetPlayer = nullptr;

private:

	_bool					m_bFindPlayer = false;
	_float					m_fDefaultAttackRange = 0.f;
	
protected:
	_bool					m_bUpdateState = false;

protected:
	_bool					m_bCanRoll = false;

private:
	vector<_float>			m_AttackRanges;
	_int					m_iNumAttackRanges = 0;
	_int					m_iAttackAnimIndex = 0;

private:
	vector<_float>			m_AttackCoolTimes;
	vector<_float>			m_CurrentAccAttackCoolTimes;
	vector<_bool>			m_AttackCoolTimeList;

private:
	_bool					m_bEndActiveAnim = false;
	friend class CBoss;


protected:
	_float					m_fRollRange = 0.f;
	const _float			m_fRollTime = 0.2f;
	_float					m_fRollAccTime = 0.f;
	_float					m_fRollSpeed = 3.5f;
	_bool					m_bCanRollBack = false;

protected:
	_float					m_fAimingRange = 0.f;				// AminingRange안에 들어왔다면 left, right, front중에 골라서 다가간다.
	_float					m_fMinDistanceFromTarget = 0.f;		// <- RollRange와 비슷, 해당 범위 안이라면 back으로 움직인다.

protected:
	_int					m_iCurrentMoveIndex = 0;
	
protected:
	string					m_strCurrentAlertPointName = "NONE";

protected:
	_bool					m_bStartTrailEffect = false;


protected:
	const _float			m_fChaseParticleMaxTime = 0.3f;
	_float					m_fChaseParticleAccTime = 0.f;

protected:
	_int					m_iMaxHP = {};
	vector<string>			m_strHPBarNames;

protected:
	const _float			m_fMaxDist = 12.f;
	_float					m_fCurDist = 0.f;
	_float					m_fRatio = 0.f;
	_float					m_fWalkAcc = 0.f;
	const _float			m_fMaxWalk = 0.32f;
	_int					m_iWalkIdx = 0;

private:
	_bool					m_bDeadOnce = true;

private:
	_float					m_fHPVerticalRatio = 1.f;

private:
	virtual HRESULT			ReadyComponents() override;
	virtual HRESULT			ReadyPartObjects() override;

public:
	virtual void			Free() override;
};

END