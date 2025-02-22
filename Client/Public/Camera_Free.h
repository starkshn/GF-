#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CPlayer;

class CCamera_Free final : public CCamera
{
public:
	typedef struct camera_free_desc : public CCamera::CAMERA_DESC
	{
		_float fMouseSensor = {};
	} CAMERA_FREE_DESC;


	enum CAMERA_EFFECT { EFFECT_DEFAULT,  EFFECT_CINEMATIC, EFFECT_SHAKE, EFFECT_END};





private:
	CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Free(const CCamera_Free& Prototype);
	virtual ~CCamera_Free() = default;

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
	void					UpdateShake(_float fTimeDelta);


	void					UpdateCinematic(_float fTimeDelta);
public:
	void					SetAttachToPlayer(_bool bAttach) { m_bAttachToPlayer = bAttach; }

	void					SetCameraEffectType(CAMERA_EFFECT eType) { m_eEffectType = eType; }

private:
	_float					m_fMouseSensor = {1.f};
	_bool					m_bBlockCamMove = false;
	_bool					m_bAttachToPlayer = true;

	CAMERA_EFFECT			m_eEffectType = EFFECT_DEFAULT;
	

private:
	CPlayer*				m_pPlayer = nullptr;
	CTransform*				m_pPlayerTransform = nullptr;

public:
	static CCamera_Free*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END