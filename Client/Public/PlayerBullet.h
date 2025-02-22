#pragma once
#include "Client_Defines.h"
#include "Bullet.h"

BEGIN(Client)

class CPlayerBullet final : public CBullet
{
private:
	CPlayerBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerBullet(const CPlayerBullet& Prototype);
	virtual ~CPlayerBullet() override = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	// void					CheckCol(_float fTimeDelta);

private:
	virtual HRESULT			ReadyComponents() override;
	virtual HRESULT			BindShaderResources() override;

public:
	static	CPlayerBullet*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual	CGameObject*	Clone(void* pArg) override;
	virtual	void			Free() override;
};

END