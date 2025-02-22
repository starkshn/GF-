#include "pch.h"
#include "BackGround.h"
#include "GameInstance.h"

CBackGround::CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : 
    CUIObject{ pDevice, pContext}
{
}

CBackGround::CBackGround(const CBackGround& Prototype)
    :
    CUIObject(Prototype)
{
    
}

HRESULT CBackGround::InitializePrototype()
{
    return S_OK;
}

HRESULT CBackGround::Initialize(void* pArg)
{
    //BACKGROUND_DESC* pDesc = static_cast<BACKGROUND_DESC*>(pArg);

    //// 사이즈 설정
    //pDesc->fSSizeX = g_iWinSizeX;
    //pDesc->fSSizeY = g_iWinSizeY;

    //// 중점 잡기
    //pDesc->fSX = (g_iWinSizeX * 0.5f);
    //pDesc->fSY = (g_iWinSizeY * 0.5f);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
   
    if (FAILED(Ready_Components()))
        return E_FAIL;

    //// 스크린 상의 사이즈 설정
    m_fSSizeX = g_iWinSizeX;
    m_fSSizeY = g_iWinSizeY;

    // 스크린 상의 위치(중점) 설정
    m_fSX = (g_iWinSizeX * 0.5f);
    m_fSY = (g_iWinSizeY * 0.5f);

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

void CBackGround::PriorityUpdate(_float fTimeDelta)
{

}

void CBackGround::Update(_float fTimeDelta)
{

}

void CBackGround::LateUpdate(_float fTimeDelta)
{
    if (GetActive())
        m_pGameInst->AddRenderObject(CRenderer::RG_UI, this);
}

HRESULT CBackGround::Render()
{
    if (GetActive())
    {
        if (FAILED(Bind_ShaderResources()))
            return E_FAIL;

        // 어떤 pass를 사용할 것인지 설정해주고 IASetInputLayout을 설정
        m_pShaderCom->Begin(0);

        // 이곳에서 IASet VertexBuffer, IndexBuffer, IASetTopology 셋팅을 한다.
        m_pVIBufferCom->BindBufferDesc();

        // DrawIndexed를 호출하여 VS가 동작하기 시작한다.
        m_pVIBufferCom->Render();
    }


    return S_OK;
}

HRESULT CBackGround::Ready_Components()
{
    // Loader에서 원형객체들을 다 만들어 두었다. 컴포넌트 포함해서
    // 이제 원형으로 부터 사본을 만드는데 사본 초기화 함수 호출시 Ready_Components가 불려지며 아래 컴포넌트들을 Add_Component 함수로 추가한다.
    // 원형에다가 컴포넌트를 추가하는 방식이 아니다.

    // static_cast<CComponent**>가 불가한 이유
    // - m_pShader는 CShader*타입이다. 근데 CComponent**는 CComponent*에 대한 포인터를 가르키는 이중 포인터 타입인데 CChader*를 CComponent*로 형변환하는 것은 논리적인 형변환이라 가능하지만 CShader** -> CComponent**는 불가능하다.
    // - 일단 단일 레벨의 포인터에 대해서만 캐스팅을 지원하기도 하는데 CShader*가 CComponent*를 상속받고 있지 않기 때문에 이또한 논리적이지 않다. 따라서 강제 캐스팅을 지원하는 C++ 문법은 reinterpret_cast를 사용한다.
    // Com_Shader
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
        if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Logo"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CBackGround::Bind_ShaderResources()
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

CBackGround* CBackGround::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBackGround* pInst = new CBackGround(pDevice, pContext);

    if (FAILED(pInst->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CBackGround");
        SafeRelease(pInst);
    }

    return pInst;
}

CGameObject* CBackGround::Clone(void* pArg)
{
    // 복사 생성자를 명시적으로 호출한다.
    CBackGround* pInst = new CBackGround(*this);

    if (FAILED(pInst->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CBackGround");
        SafeRelease(pInst);
    }

    return pInst;
}

void CBackGround::Free()
{
    __super::Free();

    SafeRelease(m_pShaderCom);
    SafeRelease(m_pVIBufferCom);
    SafeRelease(m_pTextureCom);
}
