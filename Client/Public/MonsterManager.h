#pragma once
#include "Client_Defines.h"
#include "Base.h"


BEGIN(Engine)

class CGameInstance;
class CLayer;

END



BEGIN(Client)

class CMonsterManager : public CBase
{
	DECLARE_SINGLETON(CMonsterManager)

private:
	CMonsterManager();
	virtual ~CMonsterManager() = default;

public:
	HRESULT				Initialize();
	void				Update(_float fTimeDelta);

private:
	void				UpdateMonsterLayers();
	void				UpdatePlayerBulletLayers();
	void				UpdateMonsterHPUI();


private:
	void				CheckLayerSizes();
	void				FindAndSetLayers();
	void				FindDeadMonster();
	void				RemoveMonsterFromLayer();

	void				CheckBulletLayerSizes();
	void				FindAndSetBulletLayers();
	void				FindDeadBullet();
	void				RemoveBulletFromLayer();

	void				CheckUILayerSizes();
	void				FindAndSetUILayers();
	void				FindDeadMonsters();
	void				RemoveUIFromLayer();

public:
	void				SetUpdateDoor(_bool bActive) 
	{
		m_bActiveUpdate = bActive;
	}

private:
	void				UpdateMiniMapDoor();

	
public:
	void				RemoveAllMonsters();


public:
	void				Clear();

private:
	CLayer*				m_pMonsterLayers[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
	_wstring			m_strLayerNames[6] =
	{
		TEXT("Layer_Monster01"), TEXT("Layer_Monster02"), TEXT("Layer_Monster03"), TEXT("Layer_Monster04"), TEXT("Layer_Monster05"), TEXT("Layer_Monster06")
	};
	const _int			iLoop = 6;


	CLayer*				m_pBossLayer = nullptr;
	_wstring			m_strBossLayer = TEXT("Layer_Boss");

	
	CLayer*				m_pBulletLayers[4] = { nullptr,nullptr };
	_wstring			m_strBulletLayerNames[2] =
	{
		TEXT("Layer_PlayerBullet"), TEXT("Layer_MonsterBullet")
	};


	const _int			m_iLayerSize = 1;
	_wstring			m_strUILayers[1] = { TEXT("Layer_UI_MonsterHPBG")};
	CLayer*				m_pUILayers[1] = { nullptr };


	_bool				m_bActiveUpdate = false;

private:
	const _float		m_fMiniMapSize = 250.f;



private:
	_bool				m_bDeadSoundOnce = true;

private:
	CGameInstance*		m_pGameInst = nullptr;

public:
	virtual void		Free() override;
};

END