#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CCollider;

END

BEGIN(Client)

class CSpawner final : public CGameObject
{
public:
	typedef struct spawner_desc : public GAMEOBJECT_DESC
	{

		_int iInitRotType;

	} SPAWNER_DESC;


private:
	CSpawner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpawner(const CSpawner& Prototype);
	virtual ~CSpawner() override = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	_bool					IsUsed() { return m_bUsed; }
	void					SetUse() { m_bUsed = true; }

	void					SetSpawnMonsterType(SPAWN_MONSTER_TYPE eType) { m_eSpawnMonsterType = eType; }
	SPAWN_MONSTER_TYPE		GetSpawnMonsterType() { return m_eSpawnMonsterType; }

public:
	HRESULT					SpawnMonster();

	_wstring				GetLayerTag() { return m_strLayerTag; }

	void					SetSpawnNavMeshIdx(_int iIdx) { m_iNavMeshIdx = iIdx; }
	_int					GetspawnNavMeshIdx() { return m_iNavMeshIdx; }

	HRESULT					CreateMonsterHPBar(CGameObject* pOwner, _int iLevelIdx, _int iMaxHP);

private:
	HRESULT					ReadyComponents();

private:
	CCollider*				m_pColliderCom = nullptr;
	_bool					m_bUsed = false;
	SPAWN_MONSTER_TYPE		m_eSpawnMonsterType = SPAWN_MONSTER_TYPE::MONSTER01;
	_int					m_iInitRotType = -1;
	_wstring					m_strLayerTag;

	_int m_iNavMeshIdx = 0;


public:
	static CSpawner*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;


};

END