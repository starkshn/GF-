#pragma once
#include "Camera.h"

class CTool_Main_Camera : public CCamera
{
public:
	typedef struct tool_main_camera_desc : public CCamera::CAMERA_DESC
	{
		_float fMouseSensor = {};
	} TOOL_MAIN_CAM_DESC;

private:
	CTool_Main_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTool_Main_Camera(const CTool_Main_Camera& Prototype);
	virtual ~CTool_Main_Camera() = default;

public:
	virtual HRESULT				InitializePrototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				PriorityUpdate(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				LateUpdate(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

private:
	_float						m_fMouseSensor = { 1.f };

	_bool						m_bBlockCamMove = false;

public:
	static CTool_Main_Camera*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

