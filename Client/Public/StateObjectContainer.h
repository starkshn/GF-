#pragma once
#include "Client_Defines.h"
#include "ContainerObject.h"
#include "StateObjectBody.h"

BEGIN(Engine)

class CCollider;
class CNavigation;

END

BEGIN(Client)

class CStateObjectContainer : public CContainerObject
{
public:
	enum PARTOBJECT { PART_BODY, PART_WEAPON, PART_EFFECT, PART_EFFECT2, PART_END };

	enum OBJECT_STATE
	{
		STATE_RESET = 0,
		STATE_IDLE = 0x00000001,
		STATE_WALK = STATE_IDLE << 1,
		STATE_JUMP = STATE_IDLE << 2,
		STATE_FIRE = STATE_IDLE << 3,
		STATE_RELOAD = STATE_IDLE << 4,
		STATE_PUT = STATE_IDLE << 5,
		STATE_DRAW = STATE_IDLE << 6,
	};

	enum NAVMESH_IDX  { S1, S2, S3, S_BOSS, S_END};

protected:
	CStateObjectContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStateObjectContainer(const CStateObjectContainer& Prototype);
	virtual ~CStateObjectContainer() = default;


public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	void					RememberPrevState() { m_iPrevState = m_iState; }
	void					SetForceState(_uint iState) { m_iState &= OBJECT_STATE::STATE_RESET; m_iState = iState; }
	void					SetForcePrevState(_uint iState) { m_iPrevState &= OBJECT_STATE::STATE_RESET; m_iPrevState = iState; }
	void					AddState(_uint iState) { if (!(m_iState & iState)) m_iState |= iState; }
	void					SubState(_uint iState) { if (m_iState & iState) m_iState &= ~iState; }

public:
	CModel*					GetModelByModelName(string strName);
	const _uint				GetState() { return m_iState; }
	const OBJECT_STATE		GetState(_uint iState);

public:
	void					SetNavMeshIdx(_int iIdx);
	_int					GetNavMeshIdx() { return m_iNavMeshIdx; }

public:
	virtual void			CalHitPosAndNormal(_fvector vPlayerPos, _fvector vPlayerToBulletDir, PROJECTILE_TYPE eType) {};
	virtual	void			CalNotAccuarteHitPosAndNormal(_fvector vDstPos, _fvector vNotAccuarteFromPos, _fvector vFromDir, _float3& vOutPos, _float3& vOutNormal) override;

public:
	void					SetHP(_int iHP) { m_iHP = iHP; }
	virtual void			SetDamage(_int iDamage);

protected:
	CCollider*				m_pColliderCom = nullptr;
	CNavigation*			m_pNavigationComs[NAVMESH_IDX::S_END] = { nullptr, nullptr, nullptr, nullptr };

protected:
	_uint					m_iState = STATE_IDLE;
	_uint					m_iPrevState = STATE_IDLE;

protected:
	_int					m_iNavMeshIdx = 0;

	_int					m_iHP = 1000;
	_bool					m_bDead = false;

#ifdef _DEBUG
public:
	string					GetStateDebug();
	string					GetPrevStateDebug();
#endif

protected:
	virtual void			UpdateHPUI() {};


private:
	virtual HRESULT			ReadyComponents();
	virtual HRESULT			ReadyPartObjects();

protected:
	HRESULT					ReadyNavMesh();

public:
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END