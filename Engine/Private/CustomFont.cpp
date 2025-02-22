#include "CustomFont.h"

CCustomFont::CCustomFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    m_pDevice(pDevice),
    m_pContext(pContext)
{
    SafeAddRef(m_pDevice);
    SafeAddRef(m_pContext);
}

HRESULT CCustomFont::Initialize(const _tchar* pFontFilePath)
{
    m_pFont = new SpriteFont(m_pDevice, pFontFilePath);

    m_pBatch = new SpriteBatch(m_pContext);

    return S_OK;
}

HRESULT CCustomFont::Render(const _tchar* pText, const _float2& vPosition, _fvector vColor, _float fRotation, const _float2& vOrigin)
{
    // 241115 기준으로 POINT INSTANCE에서 GS를 사용중이기 때문에 내가 직접만든 쉐이더를 통해서 Font는 렌더하지 않아 GS를 쓰지 않겠다고 명시함
    m_pContext->GSSetShader(nullptr, nullptr, 0);

    m_pBatch->Begin();

    m_pFont->DrawString(m_pBatch, pText, vPosition, vColor, fRotation, vOrigin);

    m_pBatch->End();

    return S_OK;
}

CCustomFont* CCustomFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontFilePath)
{
    CCustomFont* pInstance = new CCustomFont(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pFontFilePath)))
    {
        MSG_BOX("Failed to Created : CCustomFont");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CCustomFont::Free()
{
    __super::Free();

    SafeRelease(m_pContext);
    SafeRelease(m_pDevice);

    SafeDelete(m_pFont);
    SafeDelete(m_pBatch);
}
