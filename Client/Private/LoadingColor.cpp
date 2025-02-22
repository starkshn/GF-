#include "pch.h"
#include "LoadingColor.h"
#include "GameInstance.h"

CLoadingColor::CLoadingColor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CUIObject{ pDevice, pContext }
{
}

CLoadingColor::CLoadingColor(const CLoadingColor& Prototype)
    :
    CUIObject(Prototype)
{

}

HRESULT CLoadingColor::InitializePrototype()
{
    return S_OK;
}

HRESULT CLoadingColor::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    //// 스크린 상의 사이즈 설정
    m_fSSizeX = 730.f;
    m_fSSizeY = 20.f;

    // 스크린 상의 위치(중점) 설정
    m_fSX = (g_iWinSizeX * 0.5f) - 5;
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

void CLoadingColor::PriorityUpdate(_float fTimeDelta)
{

}

void CLoadingColor::Update(_float fTimeDelta)
{

}

void CLoadingColor::LateUpdate(_float fTimeDelta)
{
    if (GetActive())
        m_pGameInst->AddRenderObject(CRenderer::RG_UI, this);
}

HRESULT CLoadingColor::Render()
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

HRESULT CLoadingColor::Ready_Components()
{
    {
        if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex_LoadingBar"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
            return E_FAIL;
    }

    // Com_VIBuffer
    {
        if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
            return E_FAIL;
    }

    // Com_Texture
    {
        if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_LoadingColor"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CLoadingColor::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->BindShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->BindShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindRawValue("g_iTotLoadCnt", &m_iTotalLoadCnt, sizeof _int)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindRawValue("g_iCurLoadCnt", &m_iCurLoadCnt, sizeof _int)))
        return E_FAIL;

    return S_OK;
}

CLoadingColor* CLoadingColor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLoadingColor* pInst = new CLoadingColor(pDevice, pContext);

    if (FAILED(pInst->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CLoadingColor");
        SafeRelease(pInst);
    }

    return pInst;
}

CGameObject* CLoadingColor::Clone(void* pArg)
{
    // 복사 생성자를 명시적으로 호출한다.
    CLoadingColor* pInst = new CLoadingColor(*this);

    if (FAILED(pInst->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CLoadingColor");
        SafeRelease(pInst);
    }

    return pInst;
}

void CLoadingColor::Free()
{
    __super::Free();

    SafeRelease(m_pShaderCom);
    SafeRelease(m_pVIBufferCom);
    SafeRelease(m_pTextureCom);
}
