#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Client)

class CDoorEffect final : public CEffect
{
private:
	CDoorEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDoorEffect(const CDoorEffect& Prototype);
	virtual ~CDoorEffect() = default;

public:
	virtual HRESULT				InitializePrototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				PriorityUpdate(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				LateUpdate(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

private:
	void						UpdateScaling(_float fTimeDelat);
	void						UpdateLoop(_float fTimeDelta);

private:
	virtual HRESULT				ReadyComponents() override;
	virtual HRESULT				BindShaderResources() override;

public:
	static CDoorEffect*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END