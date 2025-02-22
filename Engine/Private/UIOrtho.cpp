#include "UIOrtho.h"

CUIOrtho::CUIOrtho(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CUIParent(pDevice, pContext)
{
}

CUIOrtho::CUIOrtho(const CUIOrtho& Prototype)
    :
    CUIParent(Prototype)
{
    m_eShowType = UI_SHOW_TYPE::UI_ORTHO;
}

HRESULT CUIOrtho::InitializePrototype()
{
    m_eShowType = UI_SHOW_TYPE::UI_ORTHO;
    return S_OK;
}

HRESULT CUIOrtho::Initialize(void* pArg)
{
    if (!pArg) return E_FAIL;
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CUIOrtho::Free()
{
    __super::Free();
}
