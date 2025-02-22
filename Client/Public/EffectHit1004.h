#pragma once
#include "Client_Defines.h"
#include "Effect.h"


BEGIN(Client)

class CEffectHit1004 final : public CEffect
{
private:
	CEffectHit1004(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffectHit1004(const CEffectHit1004& Prototype);
	virtual ~CEffectHit1004() = default;

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
	static CEffectHit1004* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void				Free() override;
};

END