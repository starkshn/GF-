#include "pch.h"
#include "MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "BackGround.h"
#include "LoadingBar.h"
#include "UIObject.h"
#include "LoadingColor.h"

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

    if (FAILED(ReadyLoadUI()))
        return E_FAIL;

    if (FAILED(SetUpStartLevel(LEVEL_LOGO)))
        return E_FAIL;

    if (FAILED(ReadyGara()))
        return E_FAIL;


    // m_pGameInstance->AdjustBlur();

    return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
    while (ShowCursor(FALSE) >= 0) 
    {
        // 커서를 숨길 때 반환값이 0보다 작아질 때까지 반복
    }

    if (fTimeDelta > 1.f / 60.0f)
        fTimeDelta = 1.f / 60.0f;

    if (nullptr != m_pGameInstance)
        m_pGameInstance->UpdateEngine(fTimeDelta);

#ifdef _DEBUG
    m_fTimeAcc += fTimeDelta;
#endif
}

HRESULT CMainApp::Render()
{
    if (nullptr == m_pGameInstance)
        return E_FAIL;

    if (FAILED(m_pGameInstance->RenderBegin()))
        return E_FAIL;

    m_pGameInstance->Draw();

#ifdef _DEBUG

    ++m_iNumDraw;

    if (m_fTimeAcc >= 1.f)
    {
        wsprintf(m_szFPS, TEXT("FPS : %d"), m_iNumDraw);

        m_iNumDraw = 0;
        m_fTimeAcc = 0.f;
    }

#endif

    m_pGameInstance->RenderFont(TEXT("Font_147"), m_szFPS, _float2(0.f, 0.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));

    if (FAILED(m_pGameInstance->Render_End()))
        return E_FAIL;


    return S_OK;
}

HRESULT CMainApp::SetUpStartLevel(const LEVELID eLevelID)
{
    if (FAILED(m_pGameInstance->OpenLevel(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID))))
        return E_FAIL;
    return S_OK;
}

HRESULT CMainApp::ReadyLoadUI()
{
    {
        if (FAILED(m_pGameInstance->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pContext))))
            return E_FAIL;

        if (FAILED(m_pGameInstance->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Logo"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Textures/Default_BG.png")))))
            return E_FAIL;

        if (FAILED(m_pGameInstance->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTX_POSTEX::Elements, VTX_POSTEX::iNumElements))))
            return E_FAIL;

        if (FAILED(m_pGameInstance->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Rect"), CVIBufferRect::Create(m_pDevice, m_pContext))))
            return E_FAIL;


        _wstring strLayerTag = TEXT("Layer_Loading_UI");
        CBackGround::BACKGROUND_DESC Desc{};
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_BackGround"), LEVEL_GLOBAL, strLayerTag, &Desc)))
            return E_FAIL;

        auto pUI = static_cast<CUIObject*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, TEXT("Layer_Loading_UI")));
        
        pUI->SetActive(true);
        pUI->SetDepth(2);
    }


    // LoadingBar
    {
        if (FAILED(m_pGameInstance->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex_LoadingBar"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTexLoadingBar.hlsl"), VTX_POSTEX::Elements, VTX_POSTEX::iNumElements))))
            return E_FAIL;


        if (FAILED(m_pGameInstance->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_LoadingBG"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Loading/bar_bg_loading.png")))))
            return E_FAIL;

        if (FAILED(m_pGameInstance->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_LoadingColor"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Textures/UI/Loading/bar_loading.png")))))
            return E_FAIL;

        if (FAILED(m_pGameInstance->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_LoadingBar"), CLoadingBar::Create(m_pDevice, m_pContext))))
            return E_FAIL;

        _wstring strLayerTag = TEXT("Layer_LoadingBar");
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_LoadingBar"), LEVEL_GLOBAL, strLayerTag)))
            return E_FAIL;


        auto pObj = static_cast<CUIObject*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, TEXT("Layer_LoadingBar")));
        pObj->SetActive(false);
        pObj->SetDepth(1);
    }


    {
        if (FAILED(m_pGameInstance->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_LoadingColor"), CLoadingColor::Create(m_pDevice, m_pContext))))
            return E_FAIL;

        _wstring strLayerTag = TEXT("Layer_LoadingColor");
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_LoadingColor"), LEVEL_GLOBAL, strLayerTag)))
            return E_FAIL;


        auto pObj = static_cast<CUIObject*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
        pObj->SetActive(false);
        pObj->SetDepth(0);
    }


    return S_OK;
}

HRESULT CMainApp::ReadyGara()
{
    _ulong dwByte = {};
    HANDLE hFile = CreateFile(TEXT("../Bin/DataFiles/Navigation.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    if (0 == hFile)
        return E_FAIL;

    _float3     vPoints[3] = {};

    vPoints[0] = _float3(0.f, 0.f, 10.f);
    vPoints[1] = _float3(10.f, 0.f, 0.f);
    vPoints[2] = _float3(0.f, 3.f, 0.f);
    WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

    vPoints[0] = _float3(0.f, 0.f, 10.f);
    vPoints[1] = _float3(10.f, 0.f, 10.f);
    vPoints[2] = _float3(10.f, 0.f, 0.f);
    WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

    vPoints[0] = _float3(0.f, 0.f, 20.f);
    vPoints[1] = _float3(10.f, 0.f, 10.f);
    vPoints[2] = _float3(0.f, 0.f, 10.f);
    WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

    vPoints[0] = _float3(10.f, 0.f, 10.f);
    vPoints[1] = _float3(20.f, 0.f, 0.f);
    vPoints[2] = _float3(10.f, 0.f, 0.f);
    WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

    CloseHandle(hFile);


    // Font
    {
        if (FAILED(m_pGameInstance->AddFont(TEXT("Font_147"), TEXT("../Bin/Resources/Fonts/146ex.spritefont"))))
            return E_FAIL;
    }



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