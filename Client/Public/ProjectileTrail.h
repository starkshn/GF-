#pragma once
#include "Client_Defines.h"
#include "EffectTrail.h"


BEGIN(Client)

class CProjectileTrail : public CEffectTrail
{
public:
	typedef struct proj_desc : public EFFECT_DESC
	{

		PROJECTILE_TRAIL_TYPE eTrailType;
		

	} PROJECTILE_TRAIL_DESC;
	
private:
	CProjectileTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProjectileTrail(const CProjectileTrail& Prototype);
	virtual ~CProjectileTrail() = default;

public:
	virtual HRESULT				InitializePrototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				PriorityUpdate(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				LateUpdate(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

private:
	virtual HRESULT				ReadyComponents() override;
	virtual HRESULT				BindShaderResources() override;

private:
	PROJECTILE_TRAIL_TYPE		m_eTrailType = PROJECTILE_TRAIL_TYPE::TYPE_END;

public:
	static CProjectileTrail*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END