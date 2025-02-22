#pragma once
#include "Client_Defines.h"
#include "Trigger.h"


BEGIN(Client)

class CInterTrigger : public CTrigger
{
public:
	enum INTER_TYPE { INTER_MOVE, INTER_NONE, };

public:
	typedef struct inter_trigger_desc : public TRIGGER_DESC
	{
		
		INTER_TYPE eInterType;

	} INTER_TRIGGER_DESC;

protected:
	CInterTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInterTrigger(const CInterTrigger& Prototype);
	virtual ~CInterTrigger() = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	INTER_TYPE				GetInterType() { return m_eInterType; }

protected:
	INTER_TYPE				m_eInterType = INTER_NONE;

private:
	virtual HRESULT			ReadyComponents() override;
	virtual HRESULT			ReadyComponents(void* pArg) override;

public:
	virtual void			Free() override;
	
};

END