#include "FontManager.h"
#include "CustomFont.h"

CFontManager::CFontManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    m_pDevice(pDevice),
    m_pContext(pContext)
{
    SafeAddRef(m_pContext);
    SafeAddRef(m_pDevice);
}

HRESULT CFontManager::AddFont(const _wstring& strFontTag, const _tchar* pFontFilePath)
{
    if (nullptr != FindFont(strFontTag))
        return E_FAIL;

    CCustomFont* pFont = CCustomFont::Create(m_pDevice, m_pContext, pFontFilePath);
    if (nullptr == pFont)
        return E_FAIL;

    m_Fonts.emplace(strFontTag, pFont);

    return S_OK;
}

HRESULT CFontManager::RenderFont(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor, _float fRotation, const _float2& vOrigin)
{
    CCustomFont* pFont = FindFont(strFontTag);
    if (nullptr == pFont)
        return E_FAIL;


    return pFont->Render(pText, vPosition, vColor, fRotation, vOrigin);
}

CCustomFont* CFontManager::FindFont(const _wstring& strFontTag)
{
    auto	iter = m_Fonts.find(strFontTag);

    if (iter == m_Fonts.end())
        return nullptr;

    return iter->second;
}

CFontManager* CFontManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    return new CFontManager(pDevice, pContext);
}

void CFontManager::Free()
{
    __super::Free();

    for (auto& Pair : m_Fonts)
        SafeRelease(Pair.second);

    m_Fonts.clear();

    SafeRelease(m_pContext);
    SafeRelease(m_pDevice);
}
