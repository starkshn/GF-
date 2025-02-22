#pragma once
#include "Client_Defines.h"
#include "MonsterBase.h"
#include "MonsterBody02.h"

BEGIN(Engine)

class CCollider;

END

BEGIN(Client)

class CMonster02 final : public CMonsterBase
{
public:
	enum ATTACK_INDEX
	{
		ATTACK_DEFAULT = 0, ATTACK_RUSH = 1, ATTACK_END = 2,
	};

private:
	CMonster02(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster02(const CMonster02& Prototype);
	virtual ~CMonster02() = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

	virtual _bool			UpdateStateCloseRangeAttackMonster(_float fTimeDelta) override;


	virtual void			CheckTrailAlertPoint() override;


private:
	const _float			m_fRushTime = 0.3f;
	_float					m_fRushAcctime = 0.f;
	_float					m_fRushSpeed = 4.f;


private:
	virtual HRESULT			ReadyComponents() override;
	virtual HRESULT			ReadyPartObjects() override;

public:
	static CMonster02*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END