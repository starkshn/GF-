#include "RenderTargetManager.h"
#include "RenderTarget.h"

CRenderTargetManager::CRenderTargetManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    m_pDevice(pDevice),
    m_pContext(pContext)
{
    SafeAddRef(m_pDevice);
    SafeAddRef(m_pContext);
}

HRESULT CRenderTargetManager::Initialize()
{
    return S_OK;
}

HRESULT CRenderTargetManager::AddRenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
    if (nullptr != FindRenderTarget(strTargetTag))
        return E_FAIL;

    CRenderTarget* pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, iWidth, iHeight, ePixelFormat, vClearColor);
    if (nullptr == pRenderTarget)
        return E_FAIL;

    m_RenderTargets.emplace(strTargetTag, pRenderTarget);
    return S_OK;
}

HRESULT CRenderTargetManager::AddMRT(const _wstring& strMRTTag, const _wstring& strTargetTag)
{
    CRenderTarget* pRenderTarget = FindRenderTarget(strTargetTag);
    if (nullptr == pRenderTarget)
        return E_FAIL;
    
    list<CRenderTarget*>* pMRTList = FindMRT(strMRTTag);
    if (nullptr == pMRTList)
    {
        list<CRenderTarget*> MRTList = { pRenderTarget };
        m_MRTs.emplace(strMRTTag, MRTList);
    }
    else
        pMRTList->push_back(pRenderTarget);

    SafeAddRef(pRenderTarget);

    return S_OK;
}

HRESULT CRenderTargetManager::BeginMRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV, _bool bIsClear)
{
    // 모아놓은 RTV들을 장치에 바인딩한다.
    list<CRenderTarget*>* pMRTList = FindMRT(strMRTTag);
    if (nullptr == pMRTList)
        return E_FAIL;

    // 현재 1번의 RTV, DSV를 가져온다.
    m_pContext->OMGetRenderTargets(1, &m_pBackRTV, &m_pOriginalDSV);

    ID3D11RenderTargetView* pRenderTargets[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
    
    _uint iNumRenderTargets = 0;
    
    for (auto& pRT : *pMRTList)
    {
        // 그리기전에 vClearColor로 밀어야한다.
        if (bIsClear)
            pRT->Clear();
        pRenderTargets[iNumRenderTargets++] = pRT->GetRTV();
    }

    if (nullptr != pDSV)
        m_pContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
    
    m_pContext->OMSetRenderTargets(iNumRenderTargets, pRenderTargets, nullptr == pDSV ? m_pOriginalDSV : pDSV);

    return S_OK;
}

HRESULT CRenderTargetManager::EndMRT()
{
    // 그리는게 끝났으면 원래대로 되돌려준다.
    m_pContext->OMSetRenderTargets(1, &m_pBackRTV, m_pOriginalDSV);

    // BeginMRT를 호출하면 자동적으로 Ref 카운트가 증가 하기 때문에 EndMRT에서 내려준다.
    // - 내려주지 않으면 무한히 증가한다.
    SafeRelease(m_pOriginalDSV);
    SafeRelease(m_pBackRTV);

    return S_OK;
}

HRESULT CRenderTargetManager::BindShaderResource(CShader* pShader, const _char* pConstantName, const _wstring& strRenderTargetTag)
{
    CRenderTarget* pRT = FindRenderTarget(strRenderTargetTag);

    if (nullptr == pRT)
        return E_FAIL;

    return pRT->BindShaderResource(pShader, pConstantName);
}

HRESULT CRenderTargetManager::CopyRTResource(const _wstring& strTargetTag, ID3D11Texture2D* pDst)
{
    CRenderTarget* pRT = FindRenderTarget(strTargetTag);

    if (nullptr == pRT)
        return E_FAIL;

    return pRT->CopyResource(pDst);
}

HRESULT CRenderTargetManager::ReadyRTVDebug(const _wstring& strTargetTags, _float fScreenX, _float fScreenY, _float fSizeX, _float fSizeY)
{
    CRenderTarget* pRT = FindRenderTarget(strTargetTags);
    if (nullptr == pRT)
        return E_FAIL;

    return pRT->ReadyRTVDebug(fScreenX, fScreenY, fSizeX, fSizeY);
}

HRESULT CRenderTargetManager::RenderRTVDebug(const _wstring& strMRTTag, CShader* pShader, CVIBufferRect* pVIBufferRect)
{
    list<CRenderTarget*>* pMRTList = FindMRT(strMRTTag);
    if (nullptr == pMRTList)
        return E_FAIL;

    for (auto& pRT : *pMRTList)
        pRT->RenderRTVDebug(pShader, pVIBufferRect);

    return S_OK;
}

CRenderTarget* CRenderTargetManager::FindRenderTarget(const _wstring& strTargetTag)
{
    auto iter = m_RenderTargets.find(strTargetTag);
    if (iter == m_RenderTargets.end())
        return nullptr;
    return iter->second;
}

list<CRenderTarget*>* CRenderTargetManager::FindMRT(const _wstring& strMRTTag)
{
    auto	iter = m_MRTs.find(strMRTTag);

    if (iter == m_MRTs.end())
        return nullptr;

    return &iter->second;
}

CRenderTargetManager* CRenderTargetManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRenderTargetManager* pInstance = new CRenderTargetManager(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CRenderTargetManager");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CRenderTargetManager::Free()
{
    __super::Free();

    SafeRelease(m_pContext);
    SafeRelease(m_pDevice);

    for (auto& Pair : m_RenderTargets)
        SafeRelease(Pair.second);

    for (auto& Pair : m_MRTs)
    {
        for (auto& pRenderTarget : Pair.second)
            SafeRelease(pRenderTarget);

        Pair.second.clear();
    }
    m_MRTs.clear();

    m_RenderTargets.clear();
}
