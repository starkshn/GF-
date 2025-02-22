#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CCollider;

END

BEGIN(Tool)

class CInterMoveEndPos final : public CGameObject
{
private:
	CInterMoveEndPos(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInterMoveEndPos(const CInterMoveEndPos& Prototype);
	virtual ~CInterMoveEndPos() override = default;

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

private:
	HRESULT					ReadyComponents();


private:
	CCollider*				m_pColliderCom = nullptr;
	_bool					m_bUsed = false;
	SPAWN_MONSTER_TYPE		m_eSpawnMonsterType = SPAWN_MONSTER_TYPE::MONSTER01;
	

public:
	static CInterMoveEndPos* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void			Free() override;
	

};

END