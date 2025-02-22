#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Client)

class CBulletHit final : public CEffect
{
public:
	typedef struct bullet_hit : public EFFECT_DESC {

		BULLET_HIT_TYPE eHitType;
	} BULLET_HIT_DESC;


private:
	CBulletHit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBulletHit(const CBulletHit& Prototype);
	virtual ~CBulletHit() = default;

public:
	virtual HRESULT				InitializePrototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				PriorityUpdate(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				LateUpdate(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

private:
	BULLET_HIT_TYPE				m_eHitType;

private:
	virtual HRESULT				ReadyComponents() override;
	virtual HRESULT				BindShaderResources() override;


public:
	static CBulletHit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void				Free() override;
};

END
