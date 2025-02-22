#include "pch.h"
#include "Level_Logo.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "BackGround.h"


CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
    
}

HRESULT CLevel_Logo::Initialize()
{
    if (FAILED(ReadyLayerBackGround(TEXT("Layer_BackGround"))))
        return E_FAIL;

    return S_OK;
}

void CLevel_Logo::Update(const _float& fTimeDelta)
{
    if (GetKeyState(VK_RETURN) & 0x8000)
    {
        {
            auto pObj = static_cast<CUIObject*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, TEXT("Layer_LoadingBar")));
            pObj->SetActive(true);
        }

        {
            auto pObj = static_cast<CUIObject*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, TEXT("Layer_LoadingColor")));
            pObj->SetActive(true);
        }

        m_pGameInstance->OpenLevel(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY));
    }
}

HRESULT CLevel_Logo::Render()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, TEXT("로고레벨입니다."));
#endif

    return S_OK;
}

HRESULT CLevel_Logo::ReadyLayerBackGround(const _wstring& strLayerTag)
{

    // CBackGround::BACKGROUND_DESC Desc{};

    // 마지막 인자 pArg는 default 로 nullptr이다.
    // 넣을게 있다면 DESC같은거 넣어준다.
    //if (FAILED(m_pGameInstance->AddGameObjectToLayer(_uint(LEVEL_GLOBAL), TEXT("Prototype_GameObject_BackGround"), _uint(LEVEL_LOGO), strLayerTag, &Desc)))
    //    return E_FAIL;S
    
    return S_OK;
}

CLevel_Logo* CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Logo* pInst = new CLevel_Logo(pDevice, pContext);

    if (FAILED(pInst->Initialize()))
    {
        MSG_BOX("Failed to Created : CLevel_Logo");
        SafeRelease(pInst);
    }

    return pInst;
}

void CLevel_Logo::Free()
{
    __super::Free();
}
