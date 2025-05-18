#pragma once
#include "Client_Defines.h"
#include "MonsterWeaponBase.h"

BEGIN(Client)

class CBossEye final : public CMonsterWeaponBase
{
private:
	CBossEye(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBossEye(const CBossEye& Prototype);
	virtual ~CBossEye() = default;

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
	static CBossEye* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void					Free() override;
};

END