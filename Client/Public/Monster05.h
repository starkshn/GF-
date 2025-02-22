#pragma once
#include "Client_Defines.h"
#include "MonsterBase.h"
#include "MonsterBody03.h"

BEGIN(Engine)

class CCollider;

END

BEGIN(Client)

class CMonster05 final : public CMonsterBase
{
public:
	enum ATTACK_INDEX
	{
		ATTACK_SHIELD = 0,
		ATTACK_SWORD_DOWN = 1,
		ATTACK_SWORD_SWING = 2,
	};

private:
	CMonster05(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster05(const CMonster05& Prototype);
	virtual ~CMonster05() = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	virtual _bool			UpdateStateCloseRangeAttackMonster(_float fTimeDelta) override;
	virtual void			CheckTrailAlertPoint() override;

private:
	virtual HRESULT			ReadyComponents() override;
	virtual HRESULT			ReadyPartObjects() override;

public:
	static CMonster05*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END