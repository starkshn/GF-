#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Client)

class CMonster04Fire final : public CEffect
{
private:
	CMonster04Fire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster04Fire(const CMonster04Fire& Prototype);
	virtual ~CMonster04Fire() = default;

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
	static CMonster04Fire* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void				Free() override;
};

END