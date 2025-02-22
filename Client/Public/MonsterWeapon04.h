#pragma once
#include "MonsterWeaponBase.h"


BEGIN(Client)

class CMonsterWeapon04 final : public CMonsterWeaponBase
{
private:
	CMonsterWeapon04(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterWeapon04(const CMonsterWeapon04& Prototype);
	virtual ~CMonsterWeapon04() = default;

public:
	virtual HRESULT					InitializePrototype() override;
	virtual HRESULT					Initialize(void* pArg) override;
	virtual void					PriorityUpdate(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					LateUpdate(_float fTimeDelta) override;
	virtual HRESULT					Render() override;

private:
	virtual		HRESULT				ReadyComponents() override;
	virtual		HRESULT				BindShaderResources() override;

public:
	static CMonsterWeapon04*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg);
	virtual void					Free() override;
};

END