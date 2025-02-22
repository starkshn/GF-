#pragma once
#include "Base.h"


BEGIN(Engine)

class CRenderTarget;
class CShader;
class CVIBufferRect;

class CRenderTargetManager final : public CBase
{
private:
	CRenderTargetManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderTargetManager() = default;

public:
	HRESULT										Initialize();
	HRESULT										AddRenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT										AddMRT(const _wstring& strMRTTag, const _wstring& strTargetTag);
	HRESULT										BeginMRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr, _bool bIsClear = true);
	HRESULT										EndMRT();
	HRESULT										BindShaderResource(CShader* pShader, const _char* pConstantName, const _wstring& strRenderTargetTag);
	HRESULT										CopyRTResource(const _wstring& strTargetTag, ID3D11Texture2D* pDst);

#ifdef _DEBUG

public:
	HRESULT										ReadyRTVDebug(const _wstring& strTargetTags, _float fScreenX, _float fScreenY, _float fSizeX, _float fSizeY);
	HRESULT										RenderRTVDebug(const _wstring& strMRTTag, CShader* pShader, CVIBufferRect* pVIBufferRect);

#endif 

private:
	ID3D11Device*								m_pDevice = nullptr;
	ID3D11DeviceContext*						m_pContext = nullptr;
	ID3D11RenderTargetView*						m_pBackRTV = nullptr; // 백버퍼는 하나로 유지하는게 좋다.
	ID3D11DepthStencilView*						m_pOriginalDSV = nullptr;
	map<const _wstring, CRenderTarget*>			m_RenderTargets;
	map<const _wstring, list<CRenderTarget*>>	m_MRTs;
	

public:
	CRenderTarget*								FindRenderTarget(const _wstring& strTargetTag);
	list<CRenderTarget*>*						FindMRT(const _wstring& strMRTTag);

public:
	static CRenderTargetManager*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void								Free() override;
};

END