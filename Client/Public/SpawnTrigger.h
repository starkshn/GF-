#pragma once
#include "Client_Defines.h"
#include "Trigger.h"

BEGIN(Client)

class CSpawner;

class CSpawnTrigger final : public CTrigger
{
private:
	CSpawnTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpawnTrigger(const CSpawnTrigger& Prototype);
	virtual ~CSpawnTrigger() = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;


public:
	void					AddOwnSpawner(CSpawner* pSpawner) { m_Spawners.push_back(pSpawner); }

private:
	vector<CSpawner*>		m_Spawners;

public:
	void					ActiveSpawnTrigger();

private:
	void					ActiveLightAndEffect();

private:
	virtual HRESULT			ReadyComponents() override;
	virtual HRESULT			ReadyComponents(void* pArg) override;


public:
	static CSpawnTrigger*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END