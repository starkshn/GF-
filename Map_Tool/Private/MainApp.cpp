#include "MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"

CMainApp::CMainApp()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    SafeAddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
    ENGINE_DESC EngineDESC{};

    EngineDESC.hInstance = g_hInst;
    EngineDESC.hWnd = g_hWnd;
    EngineDESC.isWindowed = true;
    EngineDESC.iNumLevels = LEVEL_END;
    EngineDESC.iViewportWidth = g_iWinSizeX;
    EngineDESC.iViewportHeight = g_iWinSizeY;

    if (FAILED(m_pGameInstance->InitializeEngine(EngineDESC, &m_pDevice, &m_pContext)))
        return E_FAIL;

    if (FAILED(SetUp_StartLevel(LEVEL_TOOL)))
        return E_FAIL;

    return S_OK;
}

void CMainApp::Update(const _float& fTimeDelta)
{
    if (nullptr != m_pGameInstance)
        m_pGameInstance->UpdateEngine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
    if (nullptr == m_pGameInstance)
        return E_FAIL;

    if (FAILED(m_pGameInstance->RenderBegin(_float4{0.f, 0.f, 0.f, 1.f})))
        return E_FAIL;

    m_pGameInstance->Draw();

    if (FAILED(m_pGameInstance->Render_End()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMainApp::SetUp_StartLevel(const LEVELID eLevelID)
{
    if (FAILED(m_pGameInstance->OpenLevel(Tool::LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID))))
        return E_FAIL;
    return S_OK;
}

CMainApp* CMainApp::Create()
{
    CMainApp* pInst = new CMainApp();
    
    if (FAILED(pInst->Initialize()))
    {
        MSG_BOX("Failed to Created : CMainApp");
        SafeRelease(pInst);
    }

    return pInst;
}

void CMainApp::Free()
{

    // 부모 멤버를 먼저 정리한다.
    __super::Free();

    // 내 맴버를 정리한다.
    SafeRelease(m_pGameInstance);
    SafeRelease(m_pDevice);
    SafeRelease(m_pContext);
    
    CGameInstance::ReleaseEngine();
}