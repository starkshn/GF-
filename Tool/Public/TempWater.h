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

	
	void					Set_Light_Direction(_fvector vLDir) { m_vLightDir = vLDir;  }
	void					Set_Light_Diffuse(_fvector vAmbientLightColor) { m_vAmbientLightColor = vAmbientLightColor; }
	void					Set_Light_Ambient(_fvector vAmbientMaterial) { m_vAmbientMaterial = vAmbientMaterial; }

private:
	CShader*				m_pShaderCom = nullptr;
	CVIBuffer_TempWater*	m_pVIBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	_vector					m_vLightDir = { 0.f, 1.f, 0.f };
	_vector					m_vAmbientLightColor = { 1.f, 1.f, 1.f };
	_vector					m_vAmbientMaterial = { 1.f, 1.f, 1.f };

public:
	static CTempWater*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END