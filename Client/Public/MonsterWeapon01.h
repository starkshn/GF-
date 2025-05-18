#pragma once
#include "Client_Defines.h"
#include "MonsterWeaponBase.h"


BEGIN(Client)

class CMonsterWeapon01 final : public CMonsterWeaponBase
{
private:
	CMonsterWeapon01(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterWeapon01(const CMonsterWeapon01& Prototype);
	virtual ~CMonsterWeapon01() = default;

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
	static CMonsterWeapon01*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg);
	virtual void					Free() override;
};

END