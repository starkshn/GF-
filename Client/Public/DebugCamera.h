#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CPlayer;

class CDebugCamera final : public CCamera
{
public:
	typedef struct camera_free_desc : public CCamera::CAMERA_DESC
	{
		_float fMouseSensor = {};
	} CAMERA_FREE_DESC;

private:
	CDebugCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDebugCamera(const CDebugCamera& Prototype);
	virtual ~CDebugCamera() = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	void					UpdateImGameModeCamera(_float fTimeDelta);
	void					UpdateDebugModeCamera(_float fTimeDelta);

private:
	_float					m_fMouseSensor = {1.f};
	_bool					m_bBlockCamMove = false;
	_bool					m_bAttachToPlayer = true;

public:
	static CDebugCamera*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END