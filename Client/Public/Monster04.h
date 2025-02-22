#pragma once
#include "Client_Defines.h"
#include "MonsterBase.h"
#include "MonsterBody03.h"

BEGIN(Engine)

class CCollider;

END

BEGIN(Client)

class CMonster04 final : public CMonsterBase
{
public:
	enum ATTACK_INDEX
	{
		ATTACK_DEFAULT = 0, 
		ATTACK_STAB = 1,	
	};

private:
	CMonster04(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster04(const CMonster04& Prototype);
	virtual ~CMonster04() = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	virtual _bool			UpdateStateFarRangeAttackMonster(_float fTimeDelta) override;

	virtual void			CheckTrailAlertPoint() override;


private:
	virtual HRESULT			ReadyComponents() override;
	virtual HRESULT			ReadyPartObjects() override;

public:
	static CMonster04*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END