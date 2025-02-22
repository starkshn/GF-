#include "UIObject.h"

CUIObject::CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CGameObject(pDevice, pContext)
{
}

CUIObject::CUIObject(const CUIObject& Prototype)
    :
    CGameObject(Prototype)
{
}

HRESULT CUIObject::InitializePrototype()
{
    return S_OK;
}

HRESULT CUIObject::Initialize(void* pArg)
{
   /* if (!pArg) 
        return E_FAIL;*/

    // 부모 초기화 함수 무조건 호출해야 Transform이 만들어진다.
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    //UI_OBJECT_DESC* pDesc = static_cast<UI_OBJECT_DESC*>(pArg);

    //// 스크린 상의 사이즈 설정
    //m_fSSizeX = pDesc->fSSizeX;
    //m_fSSizeY = pDesc->fSSizeY;

    //// 스크린 상의 위치(중점) 설정
    //m_fSX = pDesc->fSX;
    //m_fSY = pDesc->fSY;

    //_uint iNumViewPorts = 1;
    //D3D11_VIEWPORT ViewportDesc = {};
    //m_pContext->RSGetViewports(&iNumViewPorts, &ViewportDesc);

    //// 직교 투영을 위한 뷰 행렬을 만들어준다.
    //XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    //XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

    //m_pTransformCom->Scaling(m_fSSizeX, m_fSSizeY, 1.f);
    //m_pTransformCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(m_fSX - ViewportDesc.Width * 0.5f, -m_fSY + ViewportDesc.Height * 0.5f, 0.f, 1.f));
    
    return S_OK;
}

void CUIObject::Free()
{
    __super::Free();
}
