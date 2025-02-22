#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CCollider;

END

BEGIN(Tool)

class CInterMoveTool : public CGameObject
{
private:
	CInterMoveTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInterMoveTool(const CInterMoveTool& Prototype);
	virtual ~CInterMoveTool() override = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	_bool					IsUsed() { return m_bUsed; }
	void					SetUse() { m_bUsed = true; }

private:
	HRESULT					ReadyComponents();


private:
	CCollider* m_pColliderCom = nullptr;
	_bool					m_bUsed = false;

public:
	static CInterMoveTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END