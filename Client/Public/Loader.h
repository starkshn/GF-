#pragma once

#include "Client_Defines.h"
#include "Base.h"


// 서브 스레드를 생성한다.
// 다음 레벨에 필요한 자원을 로드하는 역할

BEGIN(Engine)

class CGameInstance;

END

BEGIN(Client)

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	HRESULT					Initialize(LEVELID eNextLevelID);
	HRESULT					Loading();

	void					Render();


#ifdef _DEBUG
public:
	void					Show_Debug();
#endif

public:
	_bool					IsFinished() const { return m_bIsFinished; }



private:
	HRESULT					LoaReadyLoad();

	// Load Levels...
private:
	HRESULT					LoadingLevelLogo();
	HRESULT					LoadingLevelGamePlay();
	HRESULT					LoadingLevelBoss();

private:
	HRESULT					LoadLevelGlobalShaders();
	HRESULT					LoadLevelGlobalComponents();
	HRESULT					LoadLevelGlobalProjectiles();
	HRESULT					LoadLevelGlobalParticles();
	HRESULT					LoadLevelGlobalPlayer();
	HRESULT					LoadLevelGlobalMonsters();
	HRESULT					LoadLevelGlobalTriggers();
	HRESULT					LoadLevelGlobalCamera();
	HRESULT					LoadLevelGlobalEffect();
	HRESULT					LoadLevelGlobalEffectModel();
	HRESULT					LoadLevelGlobalEffectTrail();

	HRESULT					LoadLevelGlobalUI();

	HRESULT					LoadLevelGlobalTorch();


private:
	HRESULT					LoadGamePlayStages();

	HRESULT					LoadGamePlayTerrain();

	HRESULT					LoadGamePlaySkyBox();

	HRESULT					LoadGamePlayWallAABB();

private:
	HRESULT					LoadLevelBossCamera();
	HRESULT					LoadLevelBossStages();
	HRESULT					LoadLevelBossPlayer();
	HRESULT					LoadLevelBossBoss();
	HRESULT					LoadLevelBossSkyBox();
	HRESULT					LoadLevelBossProjectile();

	
private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	LEVELID					m_eNextLevelID = { LEVEL_END };
	HANDLE					m_hThread = {};
	CRITICAL_SECTION		m_Critical_Section = { nullptr };
	CGameInstance*			m_pGameInst = nullptr;
	
private:
	_bool					m_bIsFinished = { false };
	_tchar					m_szLoadingText[MAX_PATH] = {};

public:
	static CLoader*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID);
	virtual void			Free() override;
};

END