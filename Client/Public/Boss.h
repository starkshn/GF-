#pragma once
#include "Client_Defines.h"
#include "MonsterBase.h"
#include "Meteor.h"

BEGIN(Client)

class CBossRock;
class CSpawner;

class CBoss final : public CMonsterBase
{
public:
	// 0 : ÇÑ¼Õ ¹Ù´ÚÂï±â, 1 : µ¹µ¢ÀÌ ´øÁö±â, 2 : ¾ç¼Õ ¹Ù´Ú Âï±â
	enum ATTACK_INDEX {ATTACK01 = 0, ATTACK02 = 1, ATTACK03 = 2, ATTACK04 = 3,};
	enum BOSS_PARTICLE_TYPE { PARITICLE_ROCK, PARITICLE_ROCK_DUST, PARTICLE_SMASH, PARTICLE_SMASH_DUST, PARTICLE_TYPE_END };
	enum BOSS_POINT_LIGHT { LIGHT_ROCK, LIGHT_TYPE_END,};
	enum BOSS_PARTICLE_POS { POS_ROCK, POS_SMASH, POS_END };

private:
	CBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss(const CBoss& Prototype);
	virtual ~CBoss() = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	_bool					UpdateBossState(_float fTimeDelta);
	_bool					UpdateBossAction(_float fTimeDelta);

private:
	void					StartRock();
	void					CalThorwDir();
	void					EndRock();
	void					ThrowRock();

	void					StartJump();
	void					Jumping(_float fTimeDelta);
	void					StartSmash();
	void					EndSmash();

	void					StartOneHandAttkack();
	void					OnHandAttackAlert();

	void					FindAlertPos();
	void					FindAlertPosOneHand();

	void					StartSpawnMonsters();
	

	_bool					CheckDead();

private:
	void					UpdateBossRockWorldMatrix(_float fTimeDelta);
	void					ResetBossRockScale();

private:
	virtual _bool			IsTargetInAttackRange() override;

private:
	void					CheckBossAttackAnimIndex(_float fTimeDelta);
	void					CheckBossAttackAnimAlert(_float fTimeDelta);

private:
	void					ActiveParticle(BOSS_PARTICLE_TYPE eType);

private:
	_vector					GetPariticlePos(BOSS_PARTICLE_POS eType);

private:
	void					ActiveSpawnTriggers();

private:
	void					ActivePazeTwo(_float fTimeDelta);
	void					SpawnMeteor();
	void					CalRandomEndPos(CMeteor* pRock);

private:
	CBossRock*				m_pBossRock = nullptr;
	const _float			m_fMaxScalingTime = 1.7f;
	_float					m_fAccScaling = 0.f;
	const _float			m_fMaxScale = 1.5f;
	_float					m_fScalingSP = 0.65f;
	_float					m_fAccCurScalingTime = 0.f;

	_bool					m_bJumpMoveStart = false;
	const _float			m_fMaxAttack01MoveTime = 0.7f;
	_float					m_fAccAttack01 = 0.f;
	_float					m_fAttackMoveSP = 30.f;

	_vector					m_vJumpEndPosDir;
	_vector					m_vAlertPos;

	CMeteor*				m_pMeteors[10] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
	string					m_MNames[10] = { "M1", "M2", "M3", "M4", "M5", "M6", "M7", "M8", "M9", "M10" };

private:
	virtual HRESULT			ReadyComponents() override;
	virtual HRESULT			ReadyPartObjects() override;
	HRESULT					ReadyBossRock();
	HRESULT					ReadyMeteors();

public:
	static CBoss*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END