#pragma once
#include "Client_Defines.h"
#include "MonsterBase.h"
#include "MonsterBody02.h"

BEGIN(Engine)

class CCollider;

END

BEGIN(Client)

class CMonster06 final : public CMonsterBase
{
public:
	enum ATTACK_INDEX
	{
		ATTACK_DEFAULT = 0, ATTACK_RUSH = 1, ATTACK_END = 2,
	};

private:
	CMonster06(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster06(const CMonster06& Prototype);
	virtual ~CMonster06() = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

	virtual _bool			UpdateStateCloseRangeAttackMonster(_float fTimeDelta) override;

private:
	virtual HRESULT			ReadyComponents() override;
	virtual HRESULT			ReadyPartObjects() override;

	virtual void			CheckTrailAlertPoint() override;


private:
	const _float			m_fRushTime = 0.3f;
	_float					m_fRushAcctime = 0.f;
	_float					m_fRushSpeed = 3.f;

public:
	static CMonster06*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END