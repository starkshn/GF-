#pragma once
#include "Client_Defines.h"
#include "Level.h"

BEGIN(Engine)

class CModel;
class CTransform;

END



BEGIN(Client)

class CPlayer;
class CCamera_Free;


class CLevel_GamePlay final : public CLevel
{
private:
	CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT			Initialize() override;
	virtual void			Update(const _float& fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	HRESULT					ReadyLights();
	HRESULT					ReadyLayerFromGlobalPlayer(const _wstring& strLayerTag);

	HRESULT					ReadyLayerCameraFromGlobalFree(const _wstring& strLayerTag);
	HRESULT					ReadyShadowCamera();

	HRESULT					ReadyLayerFromGlobalMonsters(const _wstring& strLayerTag);
	HRESULT					ReadyLayerSpawnTriggersFromGlobal(const _wstring& strLayerTag);
	HRESULT					ReadyLayerProjectilesFromGlobal(const _wstring& strLayerTag);

	HRESULT					ReadyLayerParticlesFromGlobal(const _wstring& strLayerTag);
	HRESULT					ReadyLayerParticleWallHitFromGlobal(const _wstring& strLayerTag);
	HRESULT					ReadyLayerParticleMonsterSpawnFromGlobal(const _wstring& strLayerTag);
	HRESULT					ReadyLayerParticleMonsterHitFromGlobal(const _wstring& strLayerTag);
	HRESULT					ReadyLayerParticleProjectileHitFromGlobal(const _wstring& strLayerTag);
	HRESULT					ReadyLayerParticleBossRockFromBlobal(const _wstring& strLayerTag);
	HRESULT					ReadyLayerParticleBossRockDustFromBlobal(const _wstring& strLayerTag);
	HRESULT					ReadyLayerParticleBossRockColFromBlobal(const _wstring& strLayerTag);
	HRESULT					ReadyLayerParticleBossRockColDustFromBlobal(const _wstring& strLayerTag);
	HRESULT					ReadyLayerParticleBossRockColDustLightingFromBlobal(const _wstring& strLayerTag);
	HRESULT					ReadyLayerParticleBossSmashFromBlobal(const _wstring& strLayerTag);
	HRESULT					ReadyLayerParticleBossSmashDustFromBlobal(const _wstring& strLayerTag);
	HRESULT					ReadyLayerParticleBossWalkFromBlobal(const _wstring& strLayerTag);
	HRESULT					ReadyLayerParticleMonsterDeadFromBlobal(const _wstring& strLayerTag);
	HRESULT					ReadyLayerParticleFollowLightingDustFromBlobal(const _wstring& strLayerTag);
	HRESULT					ReadyLayerParticleMonsterChase(const _wstring& strLayerTag);

	HRESULT					ReadyLayerEffectProjectileWallHitFromGlobal(const _wstring& strLayerTag);
	HRESULT					ReadyLayerEffectProjectileWallHitFrontFromGlobal(const _wstring& strLayerTag);
	HRESULT					ReadyLayerEffectProjectileWallHitSpotFromGlobal(const _wstring& strLayerTag);
	HRESULT					ReadyLayerEffectProjectileMonsterHitFromGlobal(const _wstring& strLayerTag);
	HRESULT					ReadyLayerEffectBulletFromGlobal(const _wstring& strLayerTag);
	HRESULT					ReadyLayerEffectBulletTrailFromGlobal(const _wstring& strLayerTag);
	HRESULT					ReadyLayerEffectBossRockAlertFromGlobal(const _wstring& strLayerTag);
	HRESULT					ReadyLayerEffectBossRockAlertFinalFromGlobal(const _wstring& strLayerTag);
	HRESULT					ReadyLayerEffectFollowLightingFromGlobal(const _wstring& strLayerTag);

	HRESULT					ReadyLayerEffectTrail(const _wstring& strLayerTag);
	HRESULT					ReadyLayerEffectSwordTrail(const _wstring& strLayerTag);
	HRESULT					ReadyLayerEffectPiercingTrail(const _wstring& strLayerTag);
	HRESULT					ReadyLayerEffectPiercingSwingTrail(const _wstring& strLayerTag);
	HRESULT					ReadyLayerEffectChaseTrail(const _wstring& strLayerTag);
	HRESULT					ReadyLayerEffectBossEye(const _wstring& starLayerTag);

	HRESULT					ReadyLayerEffectMonster03Fire(const _wstring& strLayerTag);
	HRESULT					ReadyLayerEffectMonster04Fire(const _wstring& strLayerTag);

	HRESULT					ReadyLayerEffectPlayer01Trail(const _wstring& strLayerTag);
	HRESULT					ReadyLayerEffectPlayer02Trail(const _wstring& strLayerTag);
	HRESULT					ReadyLayerEffectPlayer03Trail(const _wstring& strLayerTag);
	HRESULT					ReadyLayerEffectPlayer04Trail(const _wstring& strLayerTag);

	HRESULT					ReadyLayerEffectMonster03Trail(const _wstring& strLayerTag);
	HRESULT					ReadyLayerEffectMonster04Trail(const _wstring& strLayerTag);

	HRESULT					ReadyLayerEffectPlayer01muzzle(const _wstring& strLayerTag);
	HRESULT					ReadyLayerEffectPlayer02muzzle(const _wstring& strLayerTag);
	HRESULT					ReadyLayerEffectPlayer03muzzle(const _wstring& strLayerTag);
	HRESULT					ReadyLayerEffectPlayer04muzzle(const _wstring& strLayerTag);

	HRESULT					ReadyLayerEffectBulletHit03(const _wstring& strLayerTag);
	HRESULT					ReadyLayerEffectBulletHit04(const _wstring& strLayerTag);

	HRESULT					ReadyLayerEffectCircle1004(const _wstring& strLayerTag);
	HRESULT					ReadyLayerEffectHit1004(const _wstring& strLayerTag);
	HRESULT					ReadyLayerEffectSphere(const _wstring& strLayerTag);

	HRESULT					ReadyLayerEffectDoor(const _wstring& strLayerTag);
	HRESULT					ReadyLayerEffectRoarNoise(const _wstring& strLayerTag);

	HRESULT					ReadyLayerStages(const _wstring& strLayerTag);
	HRESULT					ReadyLayerTerrain(const _wstring& strLayerTag);

	HRESULT					ReadyLayerSkyBox(const _wstring& strLayerTag);

private:
	HRESULT					ReadyManagers();


	// UI
	HRESULT					ReadyTestPanel(const _wstring& strLayerTag);
	HRESULT					ReadyPlayerInfoUI();
	HRESULT					ReadyPlayerGunUI();
	HRESULT					ReadyMiniMap();									// ¹Ì´Ï¸Ê
	HRESULT					ReadyPlayerSkillEffectUI();
	HRESULT					ReadyPlayerKeyUI();
	HRESULT					ReadyDamageUIFont();
	HRESULT					ReadyUIVertical();

	HRESULT					ReadyDoorUI();
	

	HRESULT					ReadyTestS();



public:
	void					GoBossLevel();

#ifdef _DEBUG

private:

	void					InitializeImgui();
	void					RenderImgui();

#endif

public:
	_bool					IsGoToBossLevel() { return m_bMoveBossLevel; }

	CPlayer*				m_pPlayer = nullptr;
	CTransform*				m_pPlayerTransformCom = nullptr;
	vector<CModel*>	m_pPlayerModels;
	_int					m_iCurrentModelIndex = 0;
	_bool					m_bPlayerAttach = false;
	_bool					m_bCameraAttach = false;

	CCamera_Free*			m_pCurCamera = nullptr;
	_bool					m_bMoveBossLevel = false;

private:
	HRESULT					LoadAABB();
	void					ReadAABB(string strFilePath);
	HRESULT					LoadSpawnTriggers();
	void					ReadSpawnTriggers(string strFilePath, string strspawnTriggerTag, _int iNavMeshIdx, _int iDir = -1);
	HRESULT					LoadInterMove();
	void					ReadInterMoves(string strFilePath, _int iIdx);

	HRESULT					LoadPointLights();
	HRESULT					ReadPointlights(string strFilePath, const _wstring& strLayerTagTorch, const _wstring& strLayerTagTorchEffect);

	HRESULT					LoadUITrigger();
	HRESULT					ReadUITrigger(string strFilePath, const _wstring& strUITriggerLayerTag, string strUITriggerObjTag);

public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void			Free() override;
};

END