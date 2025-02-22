#pragma once
#include "Base.h"

/* 1. 현재 화면에 보여줘야할 레벨객체를 들고 있는다. */
/* 2. 새로운 레벨로 교체하고 기존레벨을 삭제한다. */
/* - 2_1. 기존 레벨용 자원(리소스, 게임오브젝트)들을 삭제한다. */
/* 3. 활성화된 레벨의 반복적인 업데이트를 수행한다. */

BEGIN(Engine)

class CLevel;
class CGameInstance;

class CLevel_Manager : public CBase
{
private:
							CLevel_Manager();
	virtual					~CLevel_Manager() override = default;

public:
	HRESULT					OpenLevel(_int iLevelIndex, CLevel* pNewLevel);
	void					Update(const _float& fTimeDelta);
	HRESULT					Render();

public:
	CLevel*					GetCurrentLevel();
	_uint					GetCurrentLevelIndex() { return m_iCurrentLevelID; }

private:
	_int					m_iCurrentLevelID = { -1 };
	CLevel*					m_pCurrentLevel = { nullptr };
	CGameInstance*			m_pGameInstance = { nullptr };

public:
	static CLevel_Manager*	Create();
	virtual void			Free() override;
};

END