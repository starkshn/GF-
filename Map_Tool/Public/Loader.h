#pragma once

#include "Tool_Defines.h"
#include "Base.h"


// 서브 스레드를 생성한다.
// 다음 레벨에 필요한 자원을 로드하는 역할


// 

BEGIN(Engine)

class CGameInstance;

END

BEGIN(Tool)

class CLoader final : public Engine::CBase
{
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	HRESULT					Initialize(LEVELID eNextLevelID);
	HRESULT					Loading();


	// Load Levels...
private:
	HRESULT					LoadingLevelTool();
	HRESULT					LoadStage();


private:
	HRESULT					ReadyModelPlayer();
	HRESULT					ReadyModelMonsters();
	HRESULT					ReadyTerrain();
	HRESULT					ReadyTestModel();
	HRESULT					ReadyBuffers();
	HRESULT					ReadyCompBoundingBox();
	HRESULT					ReadyCollider();
	HRESULT					ReadyCellLine();
	HRESULT					ReadyParticles();
	
	HRESULT					ReadySpawnTrigger();
	HRESULT					ReadyInterMove();

	HRESULT					ReadyNavMesh();
	

#ifdef _DEBUG
public:
	void					Show_Debug();
#endif

public:
	_bool					IsFinished() const { return m_bIsFinished; }


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