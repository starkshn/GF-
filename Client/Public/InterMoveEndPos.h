#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CCollider;

END

BEGIN(Client)

class CInterMoveEndPos final : public CGameObject
{
public:
	typedef struct spawner_desc : public GAMEOBJECT_DESC
	{

		_int iInitRotType;

	} SPAWNER_DESC;


private:
	CInterMoveEndPos(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInterMoveEndPos(const CInterMoveEndPos& Prototype);
	virtual ~CInterMoveEndPos() override = default;

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

#ifdef _DEBUG
private:
	HRESULT					ReadyComponents();
#endif

private:
	CCollider*				m_pColliderCom = nullptr;
	_bool					m_bUsed = false;
	_int					m_iInitRotType = -1;

public:
	static CInterMoveEndPos*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;


};

END