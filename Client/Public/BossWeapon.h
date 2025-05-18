#pragma once
#include "Client_Defines.h"
#include "MonsterWeaponBase.h"

BEGIN(Client)

class CBossWeapon final : public CMonsterWeaponBase
{
private:
	CBossWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBossWeapon(const CBossWeapon& Prototype);
	virtual ~CBossWeapon() = default;

public:
	virtual HRESULT					InitializePrototype() override;
	virtual HRESULT					Initialize(void* pArg) override;
	virtual void					PriorityUpdate(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					LateUpdate(_float fTimeDelta) override;
	virtual HRESULT					Render() override;

public:
	_matrix							GetBossHandWorldMatrix() { return XMLoadFloat4x4(&m_BossHandMatrix); }
	_matrix							GetBossSmashWorldMatrix() { return XMLoadFloat4x4(&m_BossSmashMatrix); }

private:
	_float4x4						m_BossHandMatrix;
	_float4x4						m_BossSmashMatrix;

private:
	virtual		HRESULT				ReadyComponents() override;
	virtual		HRESULT				BindShaderResources() override;

public:
	static CBossWeapon*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg);
	virtual void					Free() override;
};

END