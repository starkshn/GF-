#pragma once
#include "Tool_Defines.h"
#include "ContainerObject.h"

BEGIN(Engine)

class CColBoundingBox;	// Picking¿ë
class CCollider;

END

BEGIN(Tool)

class CTool_Player final : public CContainerObject
{
public:
	enum COLLIDER { COLL_AABB, COLL_OBB, COLL_SPHERE, COLL_END };
	enum PARTOBJECT { PART_BODY, PART_WEAPON, PART_EFFECT, PART_END };
	enum STATE
	{
		STATE_RESET = 0,
		STATE_IDLE = 0x00000001,
		STATE_WALK = STATE_IDLE << 1,
		STATE_ATTACK = STATE_IDLE << 2,
	};

private:
	CTool_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTool_Player(const CTool_Player& Prototype);
	virtual ~CTool_Player() override = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	_uint					m_iState = { STATE_IDLE };
	CCollider*				m_pColliderCom[COLL_END] = { nullptr, nullptr, nullptr };

private:
	CColBoundingBox*		m_pBoundingBox = nullptr;
	_bool					m_bPicked = false;

private:
	HRESULT					ReadyComponents();
	HRESULT					ReadyPartObjects();

public:
	static CTool_Player*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;

};

END