#pragma once
#include "Client_Defines.h"
#include "EffectTrail.h"


BEGIN(Client)

class CSwordTrail final : public CEffectTrail
{
private:
	CSwordTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSwordTrail(const CSwordTrail& Prototype);
	virtual ~CSwordTrail() = default;

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
	static CSwordTrail*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END