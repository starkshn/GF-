#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CShader;
class CVIBuffer_TempWater;
class CTexture;

END

BEGIN(Tool)

class CTempWater final : public CGameObject
{
private:
	CTempWater(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTempWater(const CTempWater& Prototype);
	virtual ~CTempWater() override = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	HRESULT					Ready_Components();
	HRESULT					Bind_ShaderResources();

public:
	void					Reset_Vtx_Cnt(_int iCntX, _int iCntZ);

	
	void					Set_Light_Direction(_fvector vDir) { XMStoreFloat4(&m_vLightDir, vDir); }
	void					Set_Light_Diffuse(_fvector vDiffuse) { XMStoreFloat4(&m_vLightDiffuse, vDiffuse); }
	void					Set_Light_Ambient(_fvector vAmbient) { XMStoreFloat4(&m_vLightAmbient, vAmbient); }
	void					Set_Light_Specular(_fvector vSpecular) { XMStoreFloat4(&m_vLightSpecular, vSpecular); }
	void					Set_Light_Specular_P(_float fP) { m_fP = fP; }

	void					Set_TempWater_Ambient(_fvector vAmbient) { XMStoreFloat4(&m_vTempWaterAmbient, vAmbient); }


private:
	CShader*				m_pShaderCom = nullptr;
	CVIBuffer_TempWater*	m_pVIBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

private:
	_float4					m_vLightDir = { 0.f, 0.f, 0.f, 0.f };
	_float4					m_vLightDiffuse = { 1.f, 1.f, 1.f, 1.f };
	_float4					m_vLightAmbient = { 1.f, 1.f, 1.f, 1.f };
	_float4					m_vLightSpecular = { 1.f, 1.f, 1.f, 1.f };

	_float4					m_vTempWaterAmbient = { 1.f, 1.f, 1.f, 1.f };

	_float					m_fP = 50.f;


public:
	static CTempWater*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END