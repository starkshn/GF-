#pragma once
#include "Base.h"

BEGIN(Engine)

class CShader;
class CVIBufferRect;

class CRenderTarget final : public CBase
{
private:
	CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderTarget() = default;

public:
	HRESULT						Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT						Clear();

public:
	ID3D11RenderTargetView*		GetRTV() const { return m_pRTV; }
	HRESULT						BindShaderResource(CShader* pShader, const _char* pConstantName);
	HRESULT						CopyResource(ID3D11Texture2D* pDst);

#ifdef _DEBUG
	
public:
	HRESULT						ReadyRTVDebug(_float fScreenX, _float fScreenY, _float fSizeX, _float fSizeY);
	HRESULT						RenderRTVDebug(CShader* pShader, CVIBufferRect* pVIBuffer);

public:

#endif

private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;
	ID3D11Texture2D*			m_pTexture2D = nullptr;
	ID3D11RenderTargetView*		m_pRTV = nullptr;
	ID3D11ShaderResourceView*	m_pSRV = nullptr;
	_float4						m_vClearColor = {};

#ifdef _DEBUG

	_float4x4					m_WorldMatrix = {};
	
#endif


public:
	static CRenderTarget*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	virtual void				Free() override;
	
};

END