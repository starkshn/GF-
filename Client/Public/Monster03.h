#pragma once
#include "Client_Defines.h"
#include "MonsterBase.h"
#include "MonsterBody03.h"

BEGIN(Engine)

class CCollider;

END

BEGIN(Client)

class CMonster03 final : public CMonsterBase
{
private:
	CMonster03(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster03(const CMonster03& Prototype);
	virtual ~CMonster03() = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	virtual _bool			UpdateStateFarRangeAttackMonster(_float fTimeDelta) override;

private:
	virtual HRESULT			ReadyComponents() override;
	virtual HRESULT			ReadyPartObjects() override;


private:
	_bool					IsTargetInAmingRange() {};

private:
	virtual void			CheckTrailAlertPoint() override;


public:
	static CMonster03*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END