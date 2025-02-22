#pragma once
#include "Client_Defines.h"
#include "InterTrigger.h"
#include "InterMoveEndPos.h"

BEGIN(Client)

class CInterMove final : public CInterTrigger
{
public:
	enum INTER_MOVE_TYPE { DEFAULT_MOVE, MOVE_BOSS, MOVE_NONE, };
	enum INTER_MOVE_IDX { S1_TO_S2, S2_TO_S3, S_BOSS, IDX_END};

public:
	typedef struct inter_move_desc : public INTER_TRIGGER_DESC
	{

		_float3 vMoveEndPos;

	} INTER_MOVE_DESC;

private:
	CInterMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInterMove(const CInterMove& Prototype);
	virtual ~CInterMove() = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	void					SetInterMoveEndPos(CInterMoveEndPos* pInterMoveEndPos) { m_pInterMoveEndPos = pInterMoveEndPos ; }
	void					SetInterMoveIdx(INTER_MOVE_IDX eType) { m_eInterMoveIdx = eType; }

private:
	void					MovePlayerToEndPos();

	void					ShowUI();
	void					CloseUI();

private:
	void					DeActivePointLightsAndTorchEffect();

private:
	_float3					m_vMoveEndPos = { 0.f, 0.f, 0.f };
	CInterMoveEndPos*		m_pInterMoveEndPos = nullptr;
	INTER_MOVE_TYPE			m_eInterMoveType = MOVE_NONE;
	INTER_MOVE_IDX			m_eInterMoveIdx = IDX_END;

private:
	_bool					m_bInterWithPlayer = false;

private:
	virtual HRESULT			ReadyComponents() override;
	virtual HRESULT			ReadyComponents(void* pArg) override;

public:
	static CInterMove*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END