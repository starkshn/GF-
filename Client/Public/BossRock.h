#pragma once
#include "Client_Defines.h"
#include "Projectile.h"

BEGIN(Engine)

class CModel;

END

BEGIN(Client)

class CBossRock final : public CProjectile
{
private:
	CBossRock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBossRock(const CBossRock& Prototype);
	virtual ~CBossRock() override = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	void					Active();

private:
	void					DeActvie();

	void					ActiveFollowLighting(_float fTimeDelta);

public:
	virtual void			Fire() override;

private:
	const _float			m_fMaxMoveTime = 5.f;
	_float					m_fAccTime = 0.f;
	_bool					m_bActive = false;

private:
	const _float			m_fActiveCycle = 0.15f;

private:
	CCollider*				m_pColliderRange = nullptr;

private:
	virtual HRESULT			ReadyComponents() override;
	virtual HRESULT			BindShaderResources() override;

public:
	static	CBossRock*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual	CGameObject*	Clone(void* pArg) override;
	virtual	void			Free() override;
};

END