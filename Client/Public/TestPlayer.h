#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"

BEGIN(Client)

class CTestPlayer final : public CContainerObject
{
public:
	enum PARTOBJECT { PART_BODY, PART_WEAPON, PART_EFFECT, PART_END };
	enum STATE
	{
		STATE_RESET = 0,
		STATE_IDLE = 0x00000001,
		STATE_WALK = STATE_IDLE << 1,
		STATE_ATTACK = STATE_IDLE << 2,
	};

private:
	CTestPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTestPlayer(const CTestPlayer& Prototype);
	virtual ~CTestPlayer() = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	_uint					m_iState = { STATE_IDLE };
	
private:
	HRESULT					Ready_Components();
	HRESULT					Ready_PartObjects();

public:
	static CTestPlayer*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END