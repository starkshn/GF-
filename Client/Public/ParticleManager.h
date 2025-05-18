#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "Particle.h"

BEGIN(Engine)

class CGameInstance;

END


BEGIN(Client)

class CParticleManager : public CBase
{
	DECLARE_SINGLETON(CParticleManager)

public:
	enum PARTICLE_TYPE 
	{ 
		WALL_HIT, MONSTER_HIT, MONSTER_SPAWN, PROJETILE_HIT, BOSS_ROCK, BOSS_ROCK_DUST, BOSS_SMASH, BOSS_SMASH_DUST, BOSS_WALK, 
		MONSTER_DEAD, BOSS_ROCK_COL, BOSS_ROCK_COL_DUST, BOSS_ROCK_COL_DUST_LIGHTING, FOLLOW_LIGHTING_DUST, MONSTER_CHASE, TYPE_END, 
	};

private:
	CParticleManager();
	virtual ~CParticleManager() = default;

public:
	HRESULT				Initialize();

public:
	void				Update(_float fTimeDelta);
	void				ActiveParticle(PARTICLE_TYPE eType, HIT_RESULT HitResult);
	void				UpdateParticlePos(PARTICLE_TYPE eType, _fvector vPos);

public:
	void				DeActivateParticle(PARTICLE_TYPE eType);

public:
	CParticle* m_pParticleProjectileHits[50] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

	CParticle* m_pParticleWallHits[30] =  { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	CParticle* m_pParticleMonsterHits[30] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

	CParticle* m_pParticleMonsterSpawns[20] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

	CParticle* m_pParticleFollowLightingDust[20] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

	CParticle* m_pParticleBossRockCol[11] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	CParticle* m_pParticleBossRockColDust[11] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	CParticle* m_pParticleBossRockColDustLighting[11] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

	CParticle* m_pParticleMonsterDeads[10] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

	CParticle* m_pParticleBossRock = nullptr;
	CParticle* m_pParticleBossRockDust = nullptr;
	CParticle* m_pParticleBossSmash = nullptr;
	CParticle* m_pParticleBossSmashDust[3] = { nullptr , nullptr, nullptr};
	CParticle* m_pParticleBossWalk = nullptr;

	CParticle* m_pParticleMonsterChase[30] = {};


private:
	CGameInstance*		m_pGameInst = nullptr;

public:
	virtual void		Free() override;
};

END