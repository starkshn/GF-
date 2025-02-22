#include "Loader.h"
#include "GameInstance.h"
#include "VIBuffer_TempWater.h"
#include "TempWater.h"
#include "VIBuffer_Terrain.h"
#include "Tool_Terrain.h"
#include "Tool_Main_Camera.h"
#include "Model.h"


#include "VIBuffer_TempCube.h"
#include "TempCube.h"

#include "TestModel.h"
#include "TestModelBody.h"
#include "TestModelWeapon.h"

#include "ColCube.h"
#include "VIBuffer_ColCube.h"


// Player
#include "Tool_Player.h"

// ColBoudingBox
#include "ColBoundingBox.h"

// Monsters
#include "Tool_Default_Monster_01.h"
#include "Monster02.h"
#include "Monster03.h"
#include "Monster04.h"
#include "Monster05.h"
#include "Monster06.h"
#include "Monster07.h"
#include "Monster08.h"

#include "BossTool.h"

// Stages
#include "Tool_Stage_01.h"
#include "Tool_Boss_Stage.h"


#include "Converter.h"


// 스테이지 밑 바닥 오브젝트
#include "ColFloor.h"
#include "AABB.h"
#include "CellLine.h"

#include "VIBufferParticleRect.h"
#include "ParticleSnow.h"



#include "SpawnTrigger.h"
#include "Spawner.h"

#include "Navigation.h"

#include "InterMoveTool.h"
#include "InterMoveEndPos.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
    , m_pGameInst{ CGameInstance::GetInstance() }
{
    SafeAddRef(m_pDevice);
    SafeAddRef(m_pContext);
    SafeAddRef(m_pGameInst);
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
        hr = LoadingLevelTool();
        break;
    case Tool::LEVEL_END:
        break;
    default:
        break;
    }

    LeaveCriticalSection(&m_Critical_Section);

    return S_OK;
}

HRESULT CLoader::ReadyModelPlayer()
{
    // 컴포넌트 모델 로드 (.dat 파일 안쓰는 경우)
    {
        // PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
        // vector<_int> Useless = { -1 };
        // if (FAILED(m_pGameInst->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Fiona"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/Fiona/Fiona.fbx", PreTransformMatrix, Useless)))) return E_FAIL;
    }

    _matrix PreTransformMatrix = XMMatrixIdentity();

    // 플레이어 CModel로드
    {
        // CConverter::GetInstance()->ExportAssetFile("../Bin/Resources/Models/Fiona/Fiona.fbx", "../Bin/Resources/Models/Fiona/Fiona.dat", CModel::TYPE_ANIM, PreTransformMatrix);

        // 기본 딱총 1202_Right
        {
            PreTransformMatrix = XMMatrixIdentity();

            // 읽은 정보로 로드 하는 경우
            {
                // CConverter::GetInstance()->ExportAssetFile("../Bin/resources/Models/GF_Player/1201_Right/1201_Right.fbx", "../Bin/resources/Models/GF_Player/1201_Right/1201_Right.dat", CModel::TYPE_ANIM, PreTransformMatrix);


                // 1202_Right
                vector<_int> UselessAnimIndex = { 1, 3 };
                PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
                if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Player_1201"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Player/1201_Right/1201_Right.dat", PreTransformMatrix, UselessAnimIndex, true, true, false, "1201_Right")))) return E_FAIL;


                // CConverter::GetInstance()->ExportAssetBonesToXML("../Bin/resources/Models/GF_Player/1201_Right/1201_Right.fbx", "../Bin/resources/Models/GF_Player/1201_Right/1201_Right_Bones.xml", CModel::TYPE_ANIM);


                // CConverter::GetInstance()->ExportAnimToXML("../Bin/resources/Models/GF_Player/1201_Right/1201_Right.fbx", "../Bin/resources/Models/GF_Player/1201_Right/1201_Right_Anim.xml");

            }


            // 쌩으로 로드하는 경우
            {
                // if (FAILED(m_pGameInst->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Player_1201"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Player/Player_1201_Right/1201_Right.fbx", PreTransformMatrix, true)))) return E_FAIL;
            }
        }

        // 1205 권총
        {
            {
                // CConverter::GetInstance()->ExportAssetFile("../Bin/resources/Models/GF_Player/1205_Right/1205_Right.fbx", "../Bin/resources/Models/GF_Player/1205_Right/1205_Right.dat", CModel::TYPE_ANIM, PreTransformMatrix);


                // 뼈 XML파싱
                // CConverter::GetInstance()->ExportAssetBonesToXML("../Bin/resources/Models/GF_Player/1205_Right/1205_Right.fbx", "../Bin/resources/Models/GF_Player/1205_Right/1205_Right_Bones.xml", CModel::TYPE_ANIM);

                // 1205_Right
                vector<_int> UselessAnimIndex = { 1 };
                PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
                if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Player_1205"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Player/1205_Right/1205_Right.dat", PreTransformMatrix, UselessAnimIndex, true, true, false, "1205_Right")))) return E_FAIL;
            }

            // 쌩 로드
            {

            }
        }


        // 1901 : 물고기
        {
            // dat 파일로 부터 로드
            {
                // CConverter::GetInstance()->ExportAssetFile("../Bin/resources/Models/GF_Player/1901_Right/1901_Right.fbx", "../Bin/resources/Models/GF_Player/1901_Right/1901_Right.dat", CModel::TYPE_ANIM, PreTransformMatrix);


                // vector<_int> Useless = { -1 };
                // PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
                // if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Player_1901"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Player/1901_Right/1901_Right.dat", PreTransformMatrix, Useless, true, true)))) return E_FAIL;
            }

            // 쌩으로 로드
            {
                // PreTransformMatrix = XMMatrixIdentity();
                // if (FAILED(m_pGameInst->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Player_1901"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Player/Player_1901_Right/1901_Right.fbx", PreTransformMatrix, true)))) return E_FAIL;
            }
        }




        // 1004_Right, 불꽃 비늘
        {
            // dat로 변환
            {
                // PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
                // CConverter::GetInstance()->ExportAssetFile("../Bin/resources/Models/GF_Player/1004_Right/1004_Right.fbx", "../Bin/resources/Models/GF_Player/1004_Right/1004_Right.dat", CModel::TYPE_ANIM, PreTransformMatrix);
            }

            // 뼈 -> XML 파싱
            {
                // CConverter::GetInstance()->ExportAssetBonesToXML("../Bin/resources/Models/GF_Player/1004_Right/1004_Right.fbx", "../Bin/resources/Models/GF_Player/1004_Right/1004_Right_Bones.xml", CModel::TYPE_ANIM);
            }

            // dat 로드
            {
                vector<_int> Useless = { -1 };
                PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
                if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Player_1004"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Player/1004_Right/1004_Right.dat", PreTransformMatrix, Useless, true, true, true, "1004_Right"))))  return E_FAIL;
            }
        }


        // 1003 : 핑크 총, 레인보우 아치
        {
            // dat파일로 로드
            {
                // CConverter::GetInstance()->ExportAssetFile("../Bin/resources/Models/GF_Player/1003_Right/1003_Right.fbx", "../Bin/resources/Models/GF_Player/1003_Right/1003_Right.dat", CModel::TYPE_ANIM, PreTransformMatrix);



                // 뼈 XML파싱
                // CConverter::GetInstance()->ExportAssetBonesToXML("../Bin/resources/Models/GF_Player/1003_Right/1003_Right.fbx", "../Bin/resources/Models/GF_Player/1003_Right/1003_Right_Bones.xml", CModel::TYPE_ANIM);


                vector<_int> Useless = { -1 };
                PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
                if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Player_1003"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Player/1003_Right/1003_Right.dat", PreTransformMatrix, Useless, true, true, false, "1003_Right"))))  return E_FAIL;
            }

            // 쌩으로 로드
            {
                // PreTransformMatrix = XMMatrixIdentity();
                // if (FAILED(m_pGameInst->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Player_1003"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Player/Player_1003_Right/1003_Right.fbx", PreTransformMatrix, true)))) return E_FAIL;
            }
        }


        // 1401_Right, 대포
        {
            // dat로 변환
            {
                // PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
                // CConverter::GetInstance()->ExportAssetFile("../Bin/resources/Models/GF_Player/1401_Right/1401_Right.fbx", "../Bin/resources/Models/GF_Player/1401_Right/1401_Right.dat", CModel::TYPE_ANIM, PreTransformMatrix);
            }

            // 뼈 -> XML 파싱
            {
                // CConverter::GetInstance()->ExportAssetBonesToXML("../Bin/resources/Models/GF_Player/1401_Right/1401_Right.fbx", "../Bin/resources/Models/GF_Player/1401_Right/1401_Right_Bones.xml", CModel::TYPE_ANIM);
            }

            // dat 로드
            {
                vector<_int> Useless = { -1 };
                PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
                if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Player_1401"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Player/1401_Right/1401_Right.dat", PreTransformMatrix, Useless, true, true, true, "1401_Right"))))  return E_FAIL;
            }
        }



        // 1008 : 파란색 총
        {
            // dat파일로 로드
            {
                // CConverter::GetInstance()->ExportAssetFile("../Bin/resources/Models/GF_Player/1008_Right/1008_Right.fbx", "../Bin/resources/Models/GF_Player/1008_Right/1008_Right.dat", CModel::TYPE_ANIM, PreTransformMatrix);


                // 뼈 XML파싱
                // CConverter::GetInstance()->ExportAssetBonesToXML("../Bin/resources/Models/GF_Player/1008_Right/1008_Right.fbx", "../Bin/resources/Models/GF_Player/1008_Right/1008_Right_Bones.xml", CModel::TYPE_ANIM);


                vector<_int> Useless = { -1 };
                PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
                if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Player_1008"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Player/1008_Right/1008_Right.dat", PreTransformMatrix, Useless, true, true)))) return E_FAIL;
            }

            // 쌩으로 로드
            {
                // 파란색 장총
                // PreTransformMatrix = XMMatrixIdentity();
                // if (FAILED(m_pGameInst->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Player_1008"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Player/Player_1008_Right/1008_Right.fbx", PreTransformMatrix, true)))) return E_FAIL;
            }
        }

        // 1503_Right
        {
            // CConverter::GetInstance()->ExportAssetFile("../Bin/resources/Models/GF_Player/1503_Right/1503_Right.fbx", "../Bin/resources/Models/GF_Player/1503_Right/1503_Right.dat", CModel::TYPE_ANIM, PreTransformMatrix);


            // 뼈 XML파싱
            // CConverter::GetInstance()->ExportAssetBonesToXML("../Bin/resources/Models/GF_Player/1503_Right/1503_Right.fbx", "../Bin/resources/Models/GF_Player/1503_Right/1503_Right_Bones.xml", CModel::TYPE_ANIM);

            
            vector<_int> Useless = { -1 };
            PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Player_1503"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Player/1503_Right/1503_Right.dat", PreTransformMatrix, Useless, true, true, true, "1503_Right"))))
                return E_FAIL;

            // if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Player_1305"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Player/Player_1305_Right/1305_Right.fbx", PreTransformMatrix, Useless, true)))) return E_FAIL;
        }



        // 1508_Right
        {
            CConverter::GetInstance()->ExportAssetFile("../Bin/resources/Models/GF_Player/1508_Right/1508_Right.fbx", "../Bin/resources/Models/GF_Player/1508_Right/1508_Right.dat", CModel::TYPE_ANIM, PreTransformMatrix);


            // 뼈 XML파싱
            CConverter::GetInstance()->ExportAssetBonesToXML("../Bin/resources/Models/GF_Player/1508_Right/1508_Right.fbx", "../Bin/resources/Models/GF_Player/1508_Right/1508_Right_Bones.xml", CModel::TYPE_ANIM);


            vector<_int> Useless = { -1 };
            PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Player_1508"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Player/1508_Right/1508_Right.dat", PreTransformMatrix, Useless, true, true, true, "1508_Right"))))
                return E_FAIL;

            // if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Player_1305"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Player/Player_1305_Right/1305_Right.fbx", PreTransformMatrix, Useless, true)))) return E_FAIL;
        }
    }

    return S_OK;
}

HRESULT CLoader::ReadyModelMonsters()
{
    // Boss
    {
        vector<_int> Useless = { -1 };
        _matrix PreTransformMatrix = XMMatrixIdentity();

        // CConverter::GetInstance()->ExportAssetFile("../Bin/Resources/Models/GF_Boss/3909_temp.fbx", "../Bin/resources/Models/GF_Boss/Boss.dat", CModel::TYPE_ANIM, PreTransformMatrix);


        // Read From .dat
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Boss"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/GF_Boss/3909_temp.fbx", PreTransformMatrix, Useless, false, false)))) return E_FAIL;

    }



    // Monster01
    {
        // 기본 칼 쓰는 몬스터 (진흙색)
        vector<_int> Useless = { -1 };
        _matrix PreTransformMatrix = XMMatrixIdentity();
        // CConverter::GetInstance()->ExportAssetFile("../Bin/resources/Models/GF_Monsters/Monster01/Monster01.fbx", "../Bin/resources/Models/GF_Monsters/Monster01/Monster01.dat", CModel::TYPE_ANIM, PreTransformMatrix);


        // Read From .dat
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Monster01"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Monsters/Monster01/Monster01.dat", PreTransformMatrix, Useless, false, true)))) return E_FAIL;
    }


    // Monster02
    {
        _matrix PreTransformMatrix = XMMatrixIdentity();
        // CConverter::GetInstance()->ExportAssetFile("../Bin/resources/Models/GF_Monsters/Monster02/Monster02.fbx", "../Bin/resources/Models/GF_Monsters/Monster02/Monster02.dat", CModel::TYPE_ANIM, PreTransformMatrix);

        // 기본 창 쓰는 몬스터 (빨간색)
        vector<_int> Useless = { -1 };
        // if (FAILED(m_pGameInst->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Monster02"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Monsters/Monster02/Monster02.fbx", PreTransformMatrix, Useless, false, false)))) return E_FAIL;

        // Read From dat
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Monster02"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Monsters/Monster02/Monster02.dat", PreTransformMatrix, Useless, false, true)))) return E_FAIL;

    }


    // Monster03
    {
        // 기본 활 쏘는 몬스터 (파란색)
        vector<_int> Useless = { -1 };
        _matrix PreTransformMatrix = XMMatrixIdentity();

        // CConverter::GetInstance()->ExportAssetFile("../Bin/resources/Models/GF_Monsters/Monster03/Monster03.fbx", "../Bin/resources/Models/GF_Monsters/Monster03/Monster03.dat", CModel::TYPE_ANIM, PreTransformMatrix);

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Monster03"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Monsters/Monster03/Monster03.dat", PreTransformMatrix, Useless, false, true)))) return E_FAIL;
    }


    // Monster04
    {
        // 기본 정병, 부메랑 던지는 애
        vector<_int> Useless = { -1 };
        _matrix PreTransformMatrix = XMMatrixIdentity();


        // CConverter::GetInstance()->ExportAssetFile("../Bin/resources/Models/GF_Monsters/Monster04/Monster04.fbx", "../Bin/resources/Models/GF_Monsters/Monster04/Monster04.dat", CModel::TYPE_ANIM, PreTransformMatrix);


        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Monster04"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Monsters/Monster04/Monster04.dat", PreTransformMatrix, Useless, false, true)))) return E_FAIL;
    }

    // Monster05
    {
        _matrix PreTransformMatrix = XMMatrixIdentity();
        // CConverter::GetInstance()->ExportAssetFile("../Bin/resources/Models/GF_Monsters/Monster05/Monster05.fbx", "../Bin/resources/Models/GF_Monsters/Monster05/Monster05.dat", CModel::TYPE_ANIM, PreTransformMatrix);


        // 말 몬스터
        vector<_int> Useless = { -1 };
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Monster05"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Monsters/Monster05/Monster05.dat", PreTransformMatrix, Useless, false, true)))) return E_FAIL;
    }

    // Monster06
    {
        // 딱정 벌레 몬스터
        vector<_int> Useless = { -1 };
        _matrix PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));

        // Monster06도 바이너리화 하면 깨짐. CBone문제인듯
        // CConverter::GetInstance()->ExportAssetFile("../Bin/resources/Models/GF_Monsters/Monster06/Monster06.fbx", "../Bin/resources/Models/GF_Monsters/Monster06/Monster06.dat", CModel::TYPE_ANIM, PreTransformMatrix);


        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Monster06"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Monsters/Monster06/Monster06.fbx", PreTransformMatrix, Useless, false, false)))) return E_FAIL;
    }

    // Monster07
    {
        // 폭탄 던지는 애
        vector<_int> Useless = { -1 };
        _matrix PreTransformMatrix = XMMatrixIdentity();


        // Monster07 바이너리화 하면 깨짐 아마 CBone문제인듯
        // CConverter::GetInstance()->ExportAssetFile("../Bin/resources/Models/GF_Monsters/Monster07/Monster07.fbx", "../Bin/resources/Models/GF_Monsters/Monster07/Monster07.dat", CModel::TYPE_ANIM, PreTransformMatrix);

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Monster07"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Monsters/Monster07/Monster07.dat", PreTransformMatrix, Useless, false, true)))) return E_FAIL;


        // if (FAILED(m_pGameInst->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Monster07"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Monsters/Monster07/Monster07.fbx", PreTransformMatrix, Useless, false, false)))) return E_FAIL;
    }



    // Monster08 : 겅은말
    {
        vector<_int> Useless = { -1 };
        _matrix PreTransformMatrix = XMMatrixIdentity();
        CConverter::GetInstance()->ExportAssetFile("../Bin/resources/Models/GF_Monsters/Monster08/Monster08.fbx", "../Bin/resources/Models/GF_Monsters/Monster08/Monster08.dat", CModel::TYPE_ANIM, PreTransformMatrix);

        // if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Monster08"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Monsters/Monster08/Monster08.dat", PreTransformMatrix, Useless, false, true))))  return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Monster08"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Monsters/Monster08/Monster08.fbx", PreTransformMatrix, Useless, false, false)))) return E_FAIL;
    }

    // Monster09 : 키큰 화살 쏘는애
    {
        vector<_int> Useless = { -1 };
        _matrix PreTransformMatrix = XMMatrixIdentity();
        // CConverter::GetInstance()->ExportAssetFile("../Bin/resources/Models/GF_Monsters/Monster09/Monster09.fbx", "../Bin/resources/Models/GF_Monsters/Monster09/Monster09.dat", CModel::TYPE_ANIM, PreTransformMatrix);

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Monster09"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Monsters/Monster09/Monster09.dat", PreTransformMatrix, Useless, false, true))))
            return E_FAIL;
    }
   
    return S_OK;
}

HRESULT CLoader::ReadyTerrain()
{
    // Terrain Textures
    {
        // 241025 Mask를 위해 dds파일 두장을 로드해준다.
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Terrain"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Textures/Terrain/Tile%d.dds"), 2)))) return E_FAIL;

        // Terrain Mask Texture
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Terrain_Mask"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.dds"), 1)))) return E_FAIL;

        /* For.Prototype_Component_Texture_Terrain_Brush */
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Terrain_Brush"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Textures/Terrain/Brush.png"), 1)))) return E_FAIL;

        // Terrain 높이맵 로드
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Textures/Terrain/Height.bmp"))))) return E_FAIL;
    }

    // Terrain Shader
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxNorTex_Terrain"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex_Terrain.hlsl"), VTX_NORTEX::Elements, VTX_NORTEX::iNumElements)))) return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxNorTex_Terrain_Brush"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/02_Shader_VtxNorTex_Terrain_Brush.hlsl"), VTX_NORTEX::Elements, VTX_NORTEX::iNumElements)))) return E_FAIL;
    }
    

    // Terrain Proto
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Terrain"), CTool_Terrain::Create(m_pDevice, m_pContext)))) return E_FAIL;
    }

    return S_OK;
}

HRESULT CLoader::ReadyTestModel()
{
    _matrix PreTransformMatrix;

    // Fiona
    vector<_int> Useless = { -1 };
    PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Fiona"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/Fiona/Fiona.dat", PreTransformMatrix, Useless, false, true, "Fiona")))) return E_FAIL;


    PreTransformMatrix = XMMatrixIdentity();

    // Weapon
    vector<_int> vec = { -1 };
    PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_ForkLift"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/resources/Models/ForkLift/ForkLift.fbx", PreTransformMatrix, vec, false, false))))
        return E_FAIL;


    // Test Model 로드
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_TestModel"), CTestModel::Create(m_pDevice, m_pContext)))) return E_FAIL;

    // Test Model 로드
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_TestModelBody"), CTestModelBody::Create(m_pDevice, m_pContext)))) return E_FAIL;

    // Test Model 로드
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_TestModelWeapon"), CTestModelWeapon::Create(m_pDevice, m_pContext)))) return E_FAIL;

    return S_OK;
}

HRESULT CLoader::ReadyBuffers()
{
    // For. Prototype_Component_VIBuffer_TempWater
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_TempWater"), CVIBuffer_TempWater::Create(m_pDevice, m_pContext))))
        return E_FAIL;


    // For. Prototype_Component_VIBuffer_TempCube
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_TempCube"), CVIBuffer_TempCube::Create(m_pDevice, m_pContext))))
        return E_FAIL;


    // For. Prototype_Component_VIBuffer_ColCube
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_ColCube"), CVIBuffer_ColCube::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLoader::ReadyCompBoundingBox()
{
    // 컴포넌트 바운딩 박스 로드
    {
        // For. Prototype_Component_BoundingBox
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_BoundingBox"), CColBoundingBox::Create(m_pDevice, m_pContext, CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosColor.hlsl"), VTX_POSCOLOR::Elements, VTX_POSCOLOR::iNumElements), CVIBuffer_ColCube::Create(m_pDevice, m_pContext)))))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CLoader::ReadyCollider()
{
    /* For.Prototype_Component_Collider_AABB */
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Collider_AABB"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
        return E_FAIL;

    /* For.Prototype_Component_Collider_OBB */
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Collider_OBB"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
        return E_FAIL;

    /* For.Prototype_Component_Collider_Sphere */
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Collider_Sphere"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
        return E_FAIL;
    

    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_AABB"), CAABB::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    
    return S_OK;
}

HRESULT CLoader::ReadyCellLine()
{
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_CellLine"), CCellLine::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    
    return S_OK;
}

HRESULT CLoader::ReadyParticles()
{
    // Snow
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Snow"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxRectInstance"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectInstance.hlsl") , VTX_RECT_INSTANCE::Elements, VTX_RECT_INSTANCE::iNumElements))))
            return E_FAIL;

        CVIBufferParticleInstance::PARTICLE_DESC			SnowDesc{};

        SnowDesc.iNumInstances = 2000;
        SnowDesc.vCenter = _float3(64.f, 20.f, 64.f);
        SnowDesc.vRange = _float3(128.f, 1.f, 128.f);
        SnowDesc.vSpeed = _float2(4.f, 10.f);
        SnowDesc.vLifeTime = _float2(2.f, 4.f);
        SnowDesc.vSize = _float2(0.2f, 0.5f);
        SnowDesc.bIsLoop = true;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Particle_Snow"), CVIBufferParticleRect::Create(m_pDevice, m_pContext, SnowDesc))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Particle_Snow"), CParticleSnow::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }


    return S_OK;
}

HRESULT CLoader::ReadySpawnTrigger()
{
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_SpawnTrigger"), CSpawnTrigger::Create(m_pDevice, m_pContext)))) return E_FAIL;
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Spawner"), CSpawner::Create(m_pDevice, m_pContext)))) return E_FAIL;

    return S_OK;
}

HRESULT CLoader::ReadyInterMove()
{
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_InterMove"), CInterMoveTool::Create(m_pDevice, m_pContext)))) return E_FAIL;
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_InterMoveEndPos"), CInterMoveEndPos::Create(m_pDevice, m_pContext)))) return E_FAIL;

    return S_OK;

}

HRESULT CLoader::ReadyNavMesh()
{
    /* For.Prototype_Component_Navigation */
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Navigation"), CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/NavMesh.dat")))))
        return E_FAIL;

    return S_OK;

}

HRESULT CLoader::LoadingLevelTool()
{
    lstrcpy(m_szLoadingText, TEXT("Loading Textures..."));
    {
        // TempWater Tex
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Water"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Textures/water.dds")))))
            return E_FAIL;


        // 로고 텍스쳐 로드
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Logo"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Textures/Collider/Collider.png")))))
            return E_FAIL;
    }

    lstrcpy(m_szLoadingText, TEXT("Loading Sounds"));
    // TODO

    lstrcpy(m_szLoadingText, TEXT("Loading Shader"));
    {
        // 실제 Shader 로드 코드
        {
            // TempWater
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxNorTex_TempWater"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex_TempWater.hlsl"), VTX_NORTEX::Elements, VTX_NORTEX::iNumElements))))
                return E_FAIL;

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_00_Shader_VtxNorTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/00_Shader_VtxNorTex.hlsl"), VTX_NORTEX::Elements, VTX_NORTEX::iNumElements))))
                return E_FAIL;

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_01_Shader_VtxNorTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/01_Shader_VtxNorTex.hlsl"), VTX_NORTEX::Elements, VTX_NORTEX::iNumElements))))
                return E_FAIL;

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_02_Shader_VtxNorTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/02_Shader_VtxNorTex.hlsl"), VTX_NORTEX::Elements, VTX_NORTEX::iNumElements))))
                return E_FAIL;

            // 모델 테스트용 쉐이더 파일(241010 직접 만든 쉐이더 파일)
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_00_Shader_VtxMesh"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/00_Shader_VtxMesh.hlsl"), VTX_MESH::Elements, VTX_MESH::iNumElements))))
                return E_FAIL;

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_00_Shader_VtxNorTex_Cube"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/00_Shader_VtxNorTex_Cube.hlsl"), VTX_NORTEX::Elements, VTX_NORTEX::iNumElements))))
                return E_FAIL;

            // ColCube 전용 쉐이더
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxPosColor"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosColor.hlsl"), VTX_POSCOLOR::Elements, VTX_POSCOLOR::iNumElements))))
                return E_FAIL;

            // AnimMesh 쉐이더
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxAnimMesh"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/00_Shader_VtxAnimMesh.hlsl"), VTX_ANIMMESH::Elements, VTX_ANIMMESH::iNumElements))))
                return E_FAIL;
        }
    }

    lstrcpy(m_szLoadingText, TEXT("Loading Models"));
    if (FAILED(ReadyModelPlayer()))
        return E_FAIL;

    if (FAILED(ReadyModelMonsters()))
        return E_FAIL;

    if (FAILED(ReadyTerrain()))
        return E_FAIL;

    if (FAILED(ReadyTestModel()))
        return E_FAIL;

    if (FAILED(ReadyBuffers()))
        return E_FAIL;

    if (FAILED(LoadStage()))
        return E_FAIL;

    if (FAILED(ReadyCompBoundingBox()))
        return E_FAIL;
    
    if (FAILED(ReadyCollider()))
        return E_FAIL;

    if (FAILED(ReadyCellLine()))
        return E_FAIL;
    
    if (FAILED(ReadyParticles()))
        return E_FAIL;
    
    if (FAILED(ReadySpawnTrigger()))
        return E_FAIL;

    if (FAILED(ReadyInterMove()))
        return E_FAIL;

    if (FAILED(ReadyNavMesh()))
        return E_FAIL;


    

    lstrcpy(m_szLoadingText, TEXT("Loading Buffer"));


    _matrix		PreTransformMatrix = XMMatrixIdentity();

    lstrcpy(m_szLoadingText, TEXT("Loading Objects"));

    PreTransformMatrix = XMMatrixIdentity();
    // 실제 객체 원형 로드
    {
        // Camera
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Camera"), CTool_Main_Camera::Create(m_pDevice, m_pContext))))
            return E_FAIL;

        // Water
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_TempWater"), CTempWater::Create(m_pDevice, m_pContext))))
            return E_FAIL;

        


        // BoundingBox를 가지고 있는 객체들 로드
        {
            // CColFloor
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_ColFloor"), CColFloor::Create(m_pDevice, m_pContext))))
                return E_FAIL;

            // CColCube
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_ColCube"), CColCube::Create(m_pDevice, m_pContext))))
                return E_FAIL;

            // TempCube
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_TempCube"), CTempCube::Create(m_pDevice, m_pContext))))
                return E_FAIL;


            // Tool_Player
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Tool_Player"), CTool_Player::Create(m_pDevice, m_pContext))))
                return E_FAIL;



            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Boss"), CBossTool::Create(m_pDevice, m_pContext)))) return E_FAIL;


            // Prototype_GameObject_Monster01
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Monster01"), CTool_Default_Monster_01::Create(m_pDevice, m_pContext)))) return E_FAIL;

            // Prototype_GameObject_Monster02
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Monster02"), CMonster02::Create(m_pDevice, m_pContext)))) return E_FAIL;

            // Prototype_GameObject_Monster03
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Monster03"), CMonster03::Create(m_pDevice, m_pContext)))) return E_FAIL;

            // Prototype_GameObject_Monster04
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Monster04"), CMonster04::Create(m_pDevice, m_pContext)))) return E_FAIL;

            //// Prototype_GameObject_Monster05
            // 말 몬스터
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Monster05"), CMonster05::Create(m_pDevice, m_pContext)))) return E_FAIL;

            //// Prototype_GameObject_Monster06
            // 딱정 벌레
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Monster06"), CMonster06::Create(m_pDevice, m_pContext)))) return E_FAIL;

            //// Prototype_GameObject_Monster07
            // 폭탄 던지는 몬스터
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Monster07"), CMonster07::Create(m_pDevice, m_pContext)))) return E_FAIL;

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Monster08"), CMonster08::Create(m_pDevice, m_pContext)))) return E_FAIL;

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Monster09"), CMonster08::Create(m_pDevice, m_pContext)))) return E_FAIL;


        }
    }



    lstrcpy(m_szLoadingText, TEXT("Finished Loading"));

    m_bIsFinished = true;

    return S_OK;
}



HRESULT CLoader::LoadStage()
{
     _matrix		PreTransformMatrix = XMMatrixIdentity();



     // 둘중 하나 선택해서 주석 해제 및 선택 level_tool에서도 주석 해제 및 설정

    // Stage01 컴포넌트 원형 추가
    {
        _matrix PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(-19.f, 0.f, 3.f);
        // _matrix PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
        // CConverter::GetInstance()->ExportAssetFile("../Bin/Resources/Models/GF_Stages/Stage01/Stage01.fbx", "../Bin/Resources/Models/GF_Stages/Stage01/Stage01.dat", CModel::TYPE_ANIM, PreTransformMatrix);


        // 스테이지 띄우고 싶으면 아래 주석만 해제하면됨
        {
            vector<_int> Useless = { -1 };
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Stage01"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/GF_Stages/Stage01/Stage01.fbx", PreTransformMatrix, Useless, false)))) 
                return E_FAIL;

            // 사용할 실 객체 추가
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Stage01"), CTool_Stage_01::Create(m_pDevice, m_pContext))))
                return E_FAIL;
        }
    }

    // 보스 스테이지 컴포넌트 원형 추가.
    // 로드 안할꺼면 그냥 주석 처리, 사용할 꺼면 주석 해제
    {
        //vector<_int> Useless = { -1 };
        //_matrix PreTransformMatrix = XMMatrixIdentity();
        //if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Boss_Stage"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/GF_Stages/Boss_Stage/Boss_Stage.dat", PreTransformMatrix, Useless, false, true))))
        //    return E_FAIL;

        //// Prototype_GameObject_Boss_Stage
        //if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Boss_Stage"), CTool_Boss_Stage::Create(m_pDevice, m_pContext))))
        //    return E_FAIL;

    }

    return S_OK;
}



CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID)
{
    CLoader* pInst = new CLoader(pDevice, pContext);

    if (FAILED(pInst->Initialize(eNextLevelID)))
    {
        MSG_BOX("Failed to Created : CLoader");
        SafeRelease(pInst);
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

    SafeRelease(m_pDevice);
    SafeRelease(m_pContext);
    SafeRelease(m_pGameInst);
}
