#pragma once
#include "Client_Defines.h"
#include "MonsterBase.h"
#include "MonsterBody01.h"

BEGIN(Engine)

class CCollider;

END

BEGIN(Client)

class CMonster01 final : public CMonsterBase
{
public:

	enum ATTACK_INDEX
	{
		ATTACK_DEFAULT = 0, // ÈÖµÎ¸£±â
		ATTACK_STAB = 1,	// Âî¸£±â
	};

private:
	CMonster01(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster01(const CMonster01& Prototype);
	virtual ~CMonster01() = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	virtual _bool			UpdateStateCloseRangeAttackMonster(_float fTimeDelta) override;


private:
	virtual void			CheckTrailAlertPoint() override;
	virtual	void			EndTrailEffect() override;

private:
	virtual HRESULT			ReadyComponents() override;
	virtual HRESULT			ReadyPartObjects() override;


public:
	static CMonster01*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END