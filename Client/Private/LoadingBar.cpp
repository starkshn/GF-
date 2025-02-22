#include "pch.h"
#include "LoadingBar.h"
#include "GameInstance.h"

CLoadingBar::CLoadingBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CUIObject{ pDevice, pContext }
{
}

CLoadingBar::CLoadingBar(const CLoadingBar& Prototype)
    :
    CUIObject(Prototype)
{

}

HRESULT CLoadingBar::InitializePrototype()
{
    return S_OK;
}

HRESULT CLoadingBar::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    //// 스크린 상의 사이즈 설정
    m_fSSizeX = 800.f;
    m_fSSizeY = 30.f;

    // 스크린 상의 위치(중점) 설정
    m_fSX = (g_iWinSizeX * 0.5f);
    m_fSY = (g_iWinSizeY * 0.5f) + 300.f;

    _uint iNumViewPorts = 1;
    D3D11_VIEWPORT ViewportDesc = {};
    m_pContext->RSGetViewports(&iNumViewPorts, &ViewportDesc);

    // 직교 투영을 위한 뷰 행렬을 만들어준다.
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

    m_pTransformCom->Scaling(m_fSSizeX, m_fSSizeY, 1.f);
    m_pTransformCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(m_fSX - ViewportDesc.Width * 0.5f, -m_fSY + ViewportDesc.Height * 0.5f, 0.f, 1.f));

    return S_OK;
}

void CLoadingBar::PriorityUpdate(_float fTimeDelta)
{

}

void CLoadingBar::Update(_float fTimeDelta)
{

}

void CLoadingBar::LateUpdate(_float fTimeDelta)
{
    if (GetActive())
        m_pGameInst->AddRenderObject(CRenderer::RG_UI, this);
}

HRESULT CLoadingBar::Render()
{
    if (GetActive())
    {
        if (FAILED(Bind_ShaderResources()))
            return E_FAIL;

        m_pShaderCom->Begin(0);
        m_pVIBufferCom->BindBufferDesc();
        m_pVIBufferCom->Render();
    }

    return S_OK;
}

HRESULT CLoadingBar::Ready_Components()
{
    {
        if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
            return E_FAIL;
    }

    // Com_VIBuffer
    {
        if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
            return E_FAIL;
    }

    // Com_Texture
    {
        if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_LoadingBG"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CLoadingBar::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->BindShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->BindShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    return S_OK;
}

CLoadingBar* CLoadingBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLoadingBar* pInst = new CLoadingBar(pDevice, pContext);

    if (FAILED(pInst->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CLoadingBar");
        SafeRelease(pInst);
    }

    return pInst;
}

CGameObject* CLoadingBar::Clone(void* pArg)
{
    // 복사 생성자를 명시적으로 호출한다.
    CLoadingBar* pInst = new CLoadingBar(*this);

    if (FAILED(pInst->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CLoadingBar");
        SafeRelease(pInst);
    }

    return pInst;
}

void CLoadingBar::Free()
{
    __super::Free();

    SafeRelease(m_pShaderCom);
    SafeRelease(m_pVIBufferCom);
    SafeRelease(m_pTextureCom);
}
