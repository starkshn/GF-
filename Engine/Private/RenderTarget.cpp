#include "RenderTarget.h"
#include "Shader.h"
#include "VIBufferRect.h"

CRenderTarget::CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	m_pDevice(pDevice),
	m_pContext(pContext)
{
	SafeAddRef(m_pDevice);
	SafeAddRef(m_pContext);
}

HRESULT CRenderTarget::Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	m_vClearColor = vClearColor;

	D3D11_TEXTURE2D_DESC TexDesc = {};
	
	TexDesc.Width = iWidth;
	TexDesc.Height = iHeight;
	TexDesc.MipLevels = 1;
	TexDesc.ArraySize = 1;
	TexDesc.Format = ePixelFormat;

	TexDesc.SampleDesc.Quality = 0;
	TexDesc.SampleDesc.Count = 1;
	TexDesc.Usage = D3D11_USAGE_DEFAULT;

	// RTV용도로도 쓸 것이고 ShaderResource용도로도 쓸 것이다.
	TexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TexDesc.CPUAccessFlags = 0;
	TexDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TexDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRTV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pSRV)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderTarget::Clear()
{
	m_pContext->ClearRenderTargetView(m_pRTV, (_float*)&m_vClearColor);

	return S_OK;
}

#ifdef _DEBUG

HRESULT CRenderTarget::ReadyRTVDebug(_float fScreenX, _float fScreenY, _float fSizeX, _float fSizeY)
{
	_uint iNumViewPorts = 1;
	D3D11_VIEWPORT ViewPortDesc = {};

	m_pContext->RSGetViewports(&iNumViewPorts, &ViewPortDesc);

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	m_WorldMatrix._11 = fSizeX;
	m_WorldMatrix._22 = fSizeY;

	m_WorldMatrix._41 = fScreenX - ViewPortDesc.Width * 0.5f;
	m_WorldMatrix._42 = -fScreenY + ViewPortDesc.Height * 0.5f;

	return S_OK;
}

HRESULT CRenderTarget::RenderRTVDebug(CShader* pShader, CVIBufferRect* pVIBuffer)
{
	if (FAILED(pShader->BindMatrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(pShader->BindSRV("g_Texture", m_pSRV)))
		return E_FAIL;

	pShader->Begin(0);

	pVIBuffer->Render();

	return S_OK;
}

#endif 


HRESULT CRenderTarget::BindShaderResource(CShader* pShader, const _char* pConstantName)
{
	return pShader->BindSRV(pConstantName, m_pSRV);
}

HRESULT CRenderTarget::CopyResource(ID3D11Texture2D* pDst)
{
	if (nullptr == m_pTexture2D)
		return E_FAIL;
	
	m_pContext->CopyResource(pDst, m_pTexture2D);
}

CRenderTarget* CRenderTarget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	CRenderTarget* pInstance = new CRenderTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iWidth, iHeight, ePixelFormat, vClearColor)))
	{
		MSG_BOX("Failed to Created : CRenderTarget");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CRenderTarget::Free()
{
	__super::Free();

	SafeRelease(m_pTexture2D);
	SafeRelease(m_pRTV);
	SafeRelease(m_pSRV);

	SafeRelease(m_pDevice);
	SafeRelease(m_pContext);
}
