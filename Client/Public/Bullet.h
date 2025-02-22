#pragma once
#include "Projectile.h"

class CBullet : public CProjectile
{
protected:
	CBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBullet(const CBullet& Prototype);
	virtual ~CBullet() override = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	virtual HRESULT			ReadyComponents() override;
	virtual HRESULT			BindShaderResources() override;

public:
	static	CBullet*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual	CGameObject*	Clone(void* pArg) override;
	virtual	void			Free() override;
};

