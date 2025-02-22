#pragma once
#include "Client_Defines.h"
#include "EffectTrail.h"

BEGIN(Client)

class CChaseTrail final : public CEffectTrail
{
private:
	CChaseTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CChaseTrail(const CChaseTrail& Prototype);
	virtual ~CChaseTrail() = default;

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

public:
	static CChaseTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void				Free() override;
};

END