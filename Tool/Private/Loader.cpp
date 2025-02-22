#include "Loader.h"
#include "GameInstance.h"
#include "VIBuffer_TempWater.h"
#include "TempWater.h"
#include "Tool_Camera.h"
#include "VIBuffer_Terrain.h"
#include "Tool_Terrain.h"
#include "Tool_Main_Camera.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
    , m_pGameInst{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInst);
}


_uint APIENTRY LoadingMain(void* pArg)
{
    CLoader* pLoader = static_cast<CLoader*>(pArg);

    // 비정상 종료 일때 return 1을 해준다.
    if (FAILED(pLoader->Loading()))
        return 1;

    return 0;
}

HRESULT CLoader::Initialize(LEVELID eNextLevelID)
{
    m_eNextLevelID = eNextLevelID;

    InitializeCriticalSection(&m_Critical_Section);

    m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);

    if (0 == m_hThread)
        return E_FAIL;

    return S_OK;
}

#ifdef _DEBUG

void CLoader::Show_Debug()
{
    SetWindowText(g_hWnd, m_szLoadingText);
}

#endif

HRESULT CLoader::Loading()
{
    EnterCriticalSection(&m_Critical_Section);

    CoInitializeEx(nullptr, 0);

    HRESULT hr = {};

    switch (m_eNextLevelID)
    {
    case Tool::LEVEL_TOOL:
        hr = Loading_Level_Tool();
        break;
    case Tool::LEVEL_END:
        break;
    default:
        break;
    }

    LeaveCriticalSection(&m_Critical_Section);

    return S_OK;
}

HRESULT CLoader::Loading_Level_Tool()
{
    lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

    /* For.Prototype_Component_Texture_Logo */
    {
        // TempWater Tex
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Water"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Textures/water.jpg")))))
            return E_FAIL;

        // Terrain Tex
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Terrain"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Textures/Terrain/Tile0.jpg")))))
            return E_FAIL;
    }

    lstrcpy(m_szLoadingText, TEXT("사운드를 로딩중입니다."));

    lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));

    // Shader 로드
    {
        // 실제 Shader 로드 코드
        {
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxNorTex_TempWater"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex_TempWater.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
                return E_FAIL;


            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxNorTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
                return E_FAIL;
        }
    }

    lstrcpy(m_szLoadingText, TEXT("모델(을)를 로딩중입니다."));


    // 컴포넌트로 들어갈 VIBuffer_Rect 로드
    {
        // For. Prototype_Component_VIBuffer_TempWater
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_TempWater"), CVIBuffer_TempWater::Create(m_pDevice, m_pContext))))
            return E_FAIL;

        // For. Prototype_Component_VIBuffer_Terrain
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Textures/Terrain/Height.bmp")))))
            return E_FAIL;
    }

    lstrcpy(m_szLoadingText, TEXT("객체원형(을)를 로딩중입니다."));


    // 실제 객체 원형 로드
    {

        // Test Camera
        // if (FAILED(m_pGameInst->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Camera"), CTool_Camera::Create(m_pDevice, m_pContext))))
        //     return E_FAIL;

        // Main Camera



        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Camera"), CTool_Main_Camera::Create(m_pDevice, m_pContext))))
            return E_FAIL;


        // Water
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_TempWater"), CTempWater::Create(m_pDevice, m_pContext))))
            return E_FAIL;

       

        // Terrain
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Terrain"), CTool_Terrain::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }

    lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

    m_bIsFinished = true;

    return S_OK;
}



CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID)
{
    CLoader* pInst = new CLoader(pDevice, pContext);

    if (FAILED(pInst->Initialize(eNextLevelID)))
    {
        MSG_BOX("Failed to Created : CLoader");
        Safe_Release(pInst);
    }

    return pInst;
}

void CLoader::Free()
{
    __super::Free();

    WaitForSingleObject(m_hThread, INFINITE);

    DeleteObject(m_hThread);
    // _endthread();

    // 끝으로 초기화 함수가 호출 되는 과정에서 할당된 리소스들이 존재하는데, 다음 함수를 사용해서 이를 반환 해야한다.
    DeleteCriticalSection(&m_Critical_Section);

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInst);
}
