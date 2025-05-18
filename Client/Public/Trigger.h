#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CCollider;

END

BEGIN(Client)

class CTrigger : public CGameObject
{
public:
	enum TRIGGER_TYPE { TYPE_SPAWN_TRIGGER, TYPE_INTER_MOVE, TYPE_END };

public:
	typedef struct trigger_desc : public GAMEOBJECT_DESC
	{

		TRIGGER_TYPE	eTriggerType;
		_float3			vCenter;
		_float3			vExtents;

	} TRIGGER_DESC;
	
protected:
	CTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrigger(const CTrigger& Prototype);
	virtual ~CTrigger() override = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;

public:
	CCollider*				GetTargetCollider() { return m_pTargetCollider; }
	TRIGGER_TYPE			GetTriggerType() { return m_eTriggerType; }
	_bool					GetIsTriggered() { return m_bIsTriggered; }
	
	void					SetIsTriggered(_bool bIsTriggered) { m_bIsTriggered = bIsTriggered; }
	void					SetTargetCollider(CCollider* pTargetCol) { m_pTargetCollider = pTargetCol; }

public:
	void					SetNotUse() { m_bNotUse = true; }


protected:
	CCollider*				m_pColliderCom = nullptr;

private:
	CCollider*				m_pTargetCollider = nullptr;
	TRIGGER_TYPE			m_eTriggerType = TRIGGER_TYPE::TYPE_END;
	_bool					m_bIsTriggered = false;

protected:
	_bool					m_bNotUse = false;

private:
	virtual HRESULT			ReadyComponents();
	virtual HRESULT			ReadyComponents(void* pArg) { return S_OK; }


public:
	virtual	void			Free() override;
};

END