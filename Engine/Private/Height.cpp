#include "Height.h"
#include "GameInstance.h"

CHeight::CHeight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	m_pDevice(pDevice),
	m_pContext(pContext),
	m_pGameInst(CGameInstance::GetInstance())
{
	SafeAddRef(m_pDevice);
	SafeAddRef(m_pContext);
	SafeAddRef(m_pGameInst);

}

HRESULT CHeight::Initialize()
{
	_uint					iNumViewports = { 1 };
	D3D11_VIEWPORT			ViewportDesc{};

	/*m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);*/
	D3D11_TEXTURE2D_DESC	TextureDesc{};

	TextureDesc.Width = m_iWidth = /*ViewportDesc.Width*/g_iMaxWidth;
	TextureDesc.Height = m_iHeight = /*ViewportDesc.Height*/g_iMaxHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R32_FLOAT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_STAGING;
	TextureDesc.BindFlags = 0;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	m_pHeights = new _float[g_iMaxWidth * g_iMaxHeight];
	ZeroMemory(m_pHeights, sizeof(_float) * g_iMaxWidth * g_iMaxHeight);

	return S_OK;
}

void CHeight::Update()
{
	if (FAILED(m_pGameInst->CopyRTResource(TEXT("TargetHeight"), m_pTexture2D)))
		return;

	D3D11_MAPPED_SUBRESOURCE SubRes{};

	m_pContext->Map(m_pTexture2D, 0, D3D11_MAP_READ_WRITE, 0, &SubRes);
	memcpy(m_pHeights, SubRes.pData, sizeof _float * (g_iMaxWidth * g_iMaxHeight));
	m_pContext->Unmap(m_pTexture2D, 0);
}

_float CHeight::ComputeHeight(_fvector vWorldPos)
{
	_vector vPos = XMVector3TransformCoord(vWorldPos, XMLoadFloat4x4(&m_TransformMatrices[D3DTS_VIEW]));
	vPos = XMVector3TransformCoord(vPos, XMLoadFloat4x4(&m_TransformMatrices[D3DTS_PROJ]));

	_float2 vViewportPos{};

	vViewportPos.x = XMVectorGetX(vPos) * (g_iMaxWidth * 0.5f) + g_iMaxWidth * 0.5f;
	vViewportPos.y = XMVectorGetY(vPos) * (g_iMaxHeight * -0.5f) + g_iMaxHeight * 0.5f;

	_uint			iPixelIndex = _uint(vViewportPos.y) * g_iMaxWidth + _uint(vViewportPos.x);

	_uint			iIndices[9] = {
		iPixelIndex - g_iMaxWidth - 1,
		iPixelIndex - g_iMaxWidth,
		iPixelIndex - g_iMaxWidth + 1,
		iPixelIndex - 1,
		iPixelIndex,
		iPixelIndex + 1,
		iPixelIndex + g_iMaxWidth - 1,
		iPixelIndex + g_iMaxWidth,
		iPixelIndex + g_iMaxWidth + 1,
	};

	_float		fHeight = {};

	for (size_t i = 0; i < 9; i++)
		fHeight += m_pHeights[iIndices[i]];

	return fHeight / 9.f;

	return _float();
}

CHeight* CHeight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHeight* pInstance = new CHeight(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CHeight");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CHeight::Free()
{
	__super::Free();

	SafeDeleteArray(m_pHeights);
	SafeRelease(m_pGameInst);
	SafeRelease(m_pTexture2D);
	SafeRelease(m_pDevice);
	SafeRelease(m_pContext);
}
