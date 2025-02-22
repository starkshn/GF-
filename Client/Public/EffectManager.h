#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "Effect.h"


BEGIN(Engine)

class CTransform;

END

BEGIN(Client)

class CEffectManager : public CBase
{
	DECLARE_SINGLETON(CEffectManager)

public:
	enum EFFECT_TYPE { PROJECTILE_WALL_HIT, PROJECTILE_WALL_HIT_FRONT, PROJECTILE_WALL_HIT_SPOT, PROJECTILE_MONSTER_HIT, BULLET, BULLET_TRAIL, BOSS_ROCK_ALERT, BOSS_ROCK_ALERT_FINAL, FOLLOW_LIGHTING, 
		
		CIRCLE_1004,
		HIT_1004,
		SPHERE_1004,
		
		TEST_TRAIL,

		SWORD_TRAIL,
		PIERCING_TRAIL,
		PIERCING_SWING_TRAIL,
		CHASE_TRAIL,

		MONSTER03_FIRE,
		MONSTER04_FIRE,

		PLAYER01_TRAIL,
		PLAYER02_TRAIL,
		PLAYER03_TRAIL,
		PLAYER04_TRAIL,
		
		MONSTER03_TRAIL,
		MONSTER04_TRAIL,

		PLAYER01_MUZZLE,
		PLAYER02_MUZZLE,
		PLAYER03_MUZZLE,
		PLAYER04_MUZZLE,

		BULLET_HIT03,
		BULLET_HIT04,

		BOSS_EYE,

		EFFECT_DOOR,

		ROAR_NOISE,

		TYPE_END, };

private:
	CEffectManager();
	virtual ~CEffectManager() = default;

public:
	HRESULT				Initialize();

public:
	void				Update(_float fTimeDelta);

	void				CheckOwner();
	void				ActiveEffect(EFFECT_TYPE eType, HIT_RESULT HitResult, CGameObject* pOwner = nullptr);
	void				DeActiveEffect(EFFECT_TYPE eType);

private:
	// HitResult로 부터 Right, Up, Look을 계산하는 함수
	void				CalNormal(CTransform* pCom, HIT_RESULT Hit);
	void				CalNormalWallHitFront(CTransform* pCom, HIT_RESULT Hit);

public:
	// void				StopEffect();

private:
	CEffect* m_pEffectBullets[50] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	CEffect* m_pEffectBulletTrails[50] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	CEffect* m_pEffectProjectileWallHitSpots[50] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };


	CEffect* m_pEffectProjectileWallHits[30] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	CEffect* m_pEffectProjectileWallHitFronts[30] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };


	CEffect* m_pEffectFollowLightings[20] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

	CEffect* m_pEffectCircle1004[20] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

	CEffect* m_pEffectHit1004[20] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

	CEffect* m_pEffectProjectileMonsterHits[20] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

	CEffect* m_pEffectSphere1004[10] = {};


	CEffect* m_pEffectBossRockAlert[3] = { nullptr , nullptr, nullptr };
	CEffect* m_pEffectBossRockAlertFinal[3] = { nullptr, nullptr , nullptr };

	CEffect* m_pEffectTestTrail = nullptr;

	// Trails
	CEffect* m_pEffectSwordTrails[10];
	CEffect* m_pEffectPiercingTrails[10];
	CEffect* m_pEffectPiercingSwingTrails[10];
	CEffect* m_pEffectChaseTrails[10];

	// Monster fire Effect
	CEffect* m_pEffectMonster03Fire[5] = {};
	CEffect* m_pEffectMonster04Fire[5] = {};

	// Player Trail
	CEffect* m_pEffectPlayer01Trail[30] = {};
	CEffect* m_pEffectPlayer02Trail[30] = {};
	CEffect* m_pEffectPlayer03Trail[30] = {};
	CEffect* m_pEffectPlayer04Trail[30] = {};

	// Monster Trail
	CEffect* m_pEffectMonster03Trail[20] = {};
	CEffect* m_pEffectMonster04Trail[20] = {};

	// Muzzle
	CEffect* m_pEffectPlayer01Muzzle[5] = {};
	CEffect* m_pEffectPlayer02Muzzle[5] = {};
	CEffect* m_pEffectPlayer03Muzzle[5] = {};
	CEffect* m_pEffectPlayer04Muzzle[5] = {};

	CEffect* m_pEffectBulletHit03[5] = {};
	CEffect* m_pEffectBulletHit04[10] = {};



	CEffect* m_pEffectBossEye[2] = {};

	CEffect* m_pEffectDoor = {};
	CEffect* m_pEffectRoarNoise = { nullptr };



private:
	CGameInstance*		m_pGameInst = nullptr;

public:
	virtual void		Free() override;
};

END