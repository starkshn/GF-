#pragma once
#include "MonsterWeaponBase.h"


BEGIN(Client)

class CMonsterWeapon02 final : public CMonsterWeaponBase
{
private:
	CMonsterWeapon02(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterWeapon02(const CMonsterWeapon02& Prototype);
	virtual ~CMonsterWeapon02() = default;

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
	static CMonsterWeapon02*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg);
	virtual void					Free() override;
};

END