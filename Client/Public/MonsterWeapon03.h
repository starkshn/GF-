#pragma once
#include "Client_Defines.h"
#include "MonsterWeaponBase.h"


BEGIN(Client)

class CMonsterWeapon03 final : public CMonsterWeaponBase
{
private:
	CMonsterWeapon03(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterWeapon03(const CMonsterWeapon03& Prototype);
	virtual ~CMonsterWeapon03() = default;

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
	static CMonsterWeapon03*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg);
	virtual void					Free() override;
};

END