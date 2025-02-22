#pragma once
#include "Base.h"

BEGIN(Engine)

class CShader;
class CVIBufferRect;

class CLight final : public CBase
{
private:
	CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLight() override = default;

public:
	const LIGHT_DESC*		GetLightDesc() const { return &m_LightDesc; }

public:
	HRESULT					Initialize(const LIGHT_DESC& LightDesc);
	HRESULT					Render(CShader* pShader, CVIBufferRect* pVIBuffer);

public:
	_bool					IsActive() { return m_bActive; }
	void					ActiveLight() { m_bActive = true; }
	void					DeActiveLight() { m_bActive = false; }

	void					SetPointLightRange(_float fRange) { m_LightDesc.fRange = fRange; }
	void					SetPointLightPos(_fvector vWorldPos) { XMStoreFloat4(&m_LightDesc.vPosition, vWorldPos); }

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	LIGHT_DESC				m_LightDesc{};

private:
	_bool					m_bActive = true;
	_float					m_fMaxActiveTime = 0.f;
	_float					m_fCurActivetime = 0.f;

public:
	static CLight*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHT_DESC& LightDesc);
	virtual void			Free() override;
};

END