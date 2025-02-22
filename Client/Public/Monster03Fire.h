#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Client)

class CMonster03Fire final : public CEffect
{
private:
	CMonster03Fire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster03Fire(const CMonster03Fire& Prototype);
	virtual ~CMonster03Fire() = default;

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
	static CMonster03Fire*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END