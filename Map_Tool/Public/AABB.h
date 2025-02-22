#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CCollider;

END


BEGIN(Tool)

class CAABB final : public CGameObject
{
private:
	CAABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAABB(const CAABB& Prototype);
	virtual ~CAABB() override = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	HRESULT					ReadyComponents();


private:
	CCollider*				m_pColliderCom = nullptr;

public:
	static CAABB*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END