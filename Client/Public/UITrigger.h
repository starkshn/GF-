#pragma once
#include "Client_Defines.h"
#include "Trigger.h"

BEGIN(Client)
class CUITrigger final : public CTrigger
{
private:
	CUITrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUITrigger(const CUITrigger& Prototype);
	virtual ~CUITrigger() = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	void					ActiveUIsAndEffects();
	void					DeActiveUIsAndEffects();

private:
	_bool					m_bActiveUI = false;

private:
	virtual HRESULT			ReadyComponents() override;
	virtual HRESULT			ReadyComponents(void* pArg) override;


public:
	static CUITrigger*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END