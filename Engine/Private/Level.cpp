#include "Level.h"

CLevel::CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	SafeAddRef(m_pGameInstance);
	SafeAddRef(m_pContext);
	SafeAddRef(m_pDevice);
}

HRESULT CLevel::Initialize()
{
	return S_OK;
}

void CLevel::Update(const _float& fTimeDelta)
{
}

HRESULT CLevel::Render()
{
	return S_OK;
}

HRESULT CLevel::ReadyGaraMaskTexture()
{
	ID3D11Texture2D* pTexture2D = { nullptr };
	D3D11_TEXTURE2D_DESC    TexDesc = {};

	// Texture의 가로 세로는 항상 2^N 으로 크기를 잡아주어야한다. 장치가 최적화 하기 때문ㅇ..?
	TexDesc.Width = 256;
	TexDesc.Height = 256;

	TexDesc.MipLevels = 1; // MipMap사용안한다.
	TexDesc.ArraySize = 1;
	TexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // ABGR 다 써준다.
	TexDesc.SampleDesc.Quality = 0;
	TexDesc.SampleDesc.Count = 1;

	// 우리는 학습용 + Mask텍스쳐 만들 목적으로 만들고 나서도 텍스쳐의 색을 변경하고싶다. 따라서 동적인 용도로 만들어주어야한다.
	// 버퍼의 다이나믹으로 Usage를 설정해도 되는데 DYNAMIC의 용도를 정확히 알아야한다. DYNAMIC은 버퍼는 맞는데 갱신하는데 초점이 맞춰져있는 옵션이다.(DYNAMIC의 주 목적은 write이다)
	// 텍스쳐의 경우 DYNAMIC로 주면 터진다. 텍스쳐를 LOCK, UNLOCK하고싶다면 STAGING 옵션을 주어야한다/
	// STAGING은 텍스쳐를 읽고 쓰는데 최적화가 되어 있다.
	TexDesc.Usage = D3D11_USAGE_STAGING;
	TexDesc.BindFlags = 0;
	TexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	TexDesc.MiscFlags = 0;

	_uint* pPixel = new _uint[256 * 256];
	ZeroMemory(pPixel, sizeof(_uint) * 256 * 256);

	for (_uint i = 0; i < 256; i++)
	{
		for (_uint j = 0; j < 256; j++)
		{
			_uint		iIndex = i * 256 + j;

			pPixel[iIndex] = 0xffffffff;
		}
	}

	/* a b g r */
	pPixel[0] = 0xff0000ff;

	D3D11_SUBRESOURCE_DATA		InitialData{};

	InitialData.pSysMem = pPixel;
	InitialData.SysMemPitch = 256 * 4;

	if (FAILED(m_pDevice->CreateTexture2D(&TexDesc, &InitialData, &pTexture2D)))
		return E_FAIL;

	D3D11_MAPPED_SUBRESOURCE		Subresource{};

	m_pContext->Map(pTexture2D, 0, D3D11_MAP_READ_WRITE, 0, &Subresource);

	for (_uint i = 0; i < 256; i++)
	{
		for (_uint j = 0; j < 256; j++)
		{
			_uint		iIndex = i * 256 + j;

			if (j < 128)
				static_cast<_uint*>(Subresource.pData)[iIndex] = 0xffffffff;
			else
				static_cast<_uint*>(Subresource.pData)[iIndex] = 0xff000000;

		}
	}

	m_pContext->Unmap(pTexture2D, 0);

	if (FAILED(SaveDDSTextureToFile(m_pContext, pTexture2D, TEXT("../Bin/Resources/Textures/Terrain/Mask.dds"))))
		return E_FAIL;

	SafeRelease(pTexture2D);

	SafeDeleteArray(pPixel);


	return S_OK;
}

void CLevel::Free()
{
	__super::Free();

	SafeRelease(m_pGameInstance);
	SafeRelease(m_pContext);
	SafeRelease(m_pDevice);
}
