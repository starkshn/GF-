#include "Picking.h"
#include "GameInstance.h"
#include "GlobalFuncs.h"

CPicking::CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    m_pDevice(pDevice),
    m_pContext(pContext),
    m_pGameInst(CGameInstance::GetInstance())
{
    SafeAddRef(m_pDevice);
    SafeAddRef(m_pContext);
    SafeAddRef(m_pGameInst);
}

HRESULT CPicking::Initialize(HWND hWnd)
{
    m_hWnd = hWnd;
    
    _uint           inumViewPort = 1;
    D3D11_VIEWPORT  ViewPortDesc = {};
    
    m_pContext->RSGetViewports(&inumViewPort, &ViewPortDesc);

    D3D11_TEXTURE2D_DESC TexDesc = {};

    TexDesc.Width = m_iWidth    = (_uint)ViewPortDesc.Width;
    TexDesc.Height = m_iHeight  = (_uint)ViewPortDesc.Height;
    TexDesc.MipLevels = 1;
    TexDesc.ArraySize = 1;
    TexDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

    TexDesc.SampleDesc.Quality = 0;
    TexDesc.SampleDesc.Count = 1;

    TexDesc.Usage = D3D11_USAGE_STAGING;
    TexDesc.BindFlags = 0;
    TexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
    TexDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&TexDesc, nullptr, &m_pPickingTexture)))
        return E_FAIL;

    return S_OK;
}

void CPicking::Update()
{
    if (FAILED(m_pGameInst->CopyRTResource(TEXT("TargetPickingWorld"), m_pPickingTexture)))
        return;
}

_bool CPicking::IsPicked(_float4* pWorldPos)
{
    _bool   bIsPicked = false;
    POINT   MousePos = {};
    GetCursorPos(&MousePos);
    ScreenToClient(m_hWnd, &MousePos);

    // 픽셀의 인덱스를 구해준다.
    // 마우스 Y 좌표 * 화면의 가로 너비(픽셀 개수) + 마우스 X 좌표
    _uint iPixelIndex = MousePos.y * m_iWidth + MousePos.x;
    
    D3D11_MAPPED_SUBRESOURCE    SubResource = {};
    
    m_pContext->Map(m_pPickingTexture, 0, D3D11_MAP_READ_WRITE, 0, &SubResource);
    _float4* pPixels = static_cast<_float4*>(SubResource.pData);
    
    // 원래 PickingTarget의 w값을 0으로 채워두었는데 0 초과라면 뭔가 값이 기록된 것이라 간주가 가능하다.
    // - 수업에서는 0보다 크다고 했는데 나는 1과 거의 같은지 아닌지 함수를 통해서 비교한다.
    //if (AlmostEqual(1.f, pPixels[iPixelIndex].w))
    //{
    //    bIsPicked = true;
    //    *pWorldPos = pPixels[iPixelIndex];
    //}

    if (0.f < pPixels[iPixelIndex].w)
    {
        bIsPicked = true;
        *pWorldPos = pPixels[iPixelIndex];
    }

    m_pContext->Unmap(m_pPickingTexture, 0);
    
    return bIsPicked;
}

D3D11_VIEWPORT CPicking::GetViewPortDesc()
{
    _uint iNumViewPorts = 1;
    D3D11_VIEWPORT ViewportDesc = {};
    m_pContext->RSGetViewports(&iNumViewPorts, &ViewportDesc);
    return ViewportDesc;
}


CPicking* CPicking::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hwnd)
{
    CPicking* pInstance = new CPicking(pDevice, pContext);

    if (FAILED(pInstance->Initialize(hwnd)))
    {
        MSG_BOX("Failed to Created : CPicking");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CPicking::Free()
{
    __super::Free();

    SafeRelease(m_pPickingTexture);
    SafeRelease(m_pDevice);
    SafeRelease(m_pContext);
    SafeRelease(m_pGameInst);
}
