#include "UIElement.h"

CUIElement::CUIElement(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CUIParent(pDevice, pContext)
{

}

CUIElement::CUIElement(const CUIElement& Prototype)
    :
    CUIParent(Prototype)
{

}

HRESULT CUIElement::InitializePrototype()
{
    return S_OK;
}

HRESULT CUIElement::Initialize(void* pArg)
{
    if (!pArg) 
        return E_FAIL;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CUIElement::Free()
{
    __super::Free();
}
