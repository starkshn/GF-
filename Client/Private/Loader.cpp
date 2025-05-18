#include "pch.h"
#include "Loader.h"
#include "GameInstance.h"
#include "BackGround.h"
#include "Terrain.h"
#include "Shader.h"
#include "Camera_Free.h"
#include "Model.h"


// ================================================================================
// LEVEL_GAMEPLAY


// ========================================
// Components
// ========================================
#include "Navigation.h"

// ========================================
// TestPlayer
// ========================================
#include "TestPlayer.h"
#include "TestPlayerBody.h"
#include "TestPlayerWeapon.h"


// ========================================
// Player
// ========================================
#include "Player.h"
#include "PlayerBody.h"
#include "PlayerWeapon.h"

// ========================================
// Stages
// ========================================
#include "Stage01.h"

// ========================================
// Monsters
// ========================================
#include "Monster01.h"
#include "MonsterBody01.h"
#include "Monster02.h"
#include "MonsterBody02.h"
#include "MonsterWeapon01.h"
#include "MonsterWeapon02.h"
#include "Monster03.h"
#include "MonsterBody03.h"
#include "MonsterWeapon03.h"
#include "Monster04.h"
#include "MonsterBody04.h"
#include "MonsterWeapon04.h"
#include "Monster05.h"
#include "MonsterBody05.h"
#include "MonsterWeapon05.h"
#include "Monster06.h"
#include "MonsterBody06.h"
#include "MonsterWeapon06.h"

#include "Boss.h"
#include "BossBody.h"
#include "BossWeapon.h"

// ========================================
// SkyBox
// ========================================
#include "VIBufferSkyBox.h"
#include "SkyBox.h"


// ========================================
// Trigger
// ========================================
#include "SpawnTrigger.h"
#include "Spawner.h"
#include "InterMove.h"
#include "InterMoveEndPos.h"
#include "UITrigger.h"

// ========================================
// Projectile
// ========================================
#include "Bullet.h"
#include "Arrow.h"
#include "PlayerBullet.h"
#include "MonsterBullet.h"

// ========================================
// Particles
// ========================================
#include "VIBufferParticleRect.h"
#include "VIBufferParticlePoint.h"
#include "ParticlePlayerGunFire.h"
#include "ParticleMonsterHit.h"
#include "ParticleWall.h"
#include "ParticleMonsterSpawn.h"
#include "ParticleProjectileHit.h"
#include "ParticleBossRock.h"
#include "ParticleBossRockDust.h"
#include "ParticleBossSmash.h"
#include "ParticleBossSmashDust.h"
#include "ParticleBossWalk.h"
#include "ParticleMonsterDead.h"
#include "ParticleBossRockCol.h"
#include "ParticleBossRockColDust.h"
#include "ParticleBossRockColDustLighting.h"
#include "ParticleFollowLightingDust.h"
#include "ParticleMonsterChase.h"

// ========================================
// Effect
// ========================================
#include "Effect.h"
#include "EffectProjectileWallHit.h"
#include "EffectProjectileWallHitFront.h"
#include "EffectProjectileMonsterHit.h"
#include "EffectProjectileWallHitSpot.h"
#include "EffectBullet.h"
#include "EffectBulletTrail.h"
#include "EffectBossRockAlert.h"
#include "EffectBossRockAlertFinal.h"
#include "EffectFollowLighting.h"
#include "EffectCircle1004.h"
#include "EffectHit1004.h"
#include "EffectSphere1004.h"
#include "TestEffect.h"

#include "TestTrail.h"
#include "SwordTrail.h" // 휘두를 때
#include "PiercingTrail.h" // 찌를 때 때
#include "PiercingSwingTrail.h" // Red가 창 휘두를 때
#include "ChaseTrail.h" // 쫒을 때

#include "Monster03Fire.h"
#include "Monster04Fire.h"

#include "ProjectileTrail.h"
#include "EffectMuzzle.h"
#include "BulletHit.h"

#include "BossEyeTrail.h"
#include "TorchFire.h"
#include "DoorEffect.h"
#include "EffectRoarNoise.h"

// ========================================
// Wall
// ========================================
#include "WallAABB.h"


// ========================================
// UI
// ========================================
#include "UIParent.h"
#include "TestPanel.h"
#include "UIPlayerInfoBG.h"
#include "UIPlayerTex.h"
#include "UIPlayerHPBarBG.h"
#include "UIFont.h"
#include "UIHPBar.h"
#include "UIBG.h"
#include "UITex.h"
#include "UIDamageFont.h"
#include "UIObject.h"
#include "LoadingColor.h"
#include "UITexPerspective.h"
#include "Torch.h"
#include "UICrossHair.h"
#include "UIHPVertical.h"

// LEVEL_GAMEPLAY
// ================================================================================



// ================================================================================
// LEVEL_BOSS
#include "BossStage.h"
#include "DebugCamera.h"
#include "BossRock.h"
#include "Meteor.h"
#include "BossEye.h"






// LEVEL_BOSS
// ================================================================================





CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
    , m_pGameInst{CGameInstance::GetInstance()}
{
    SafeAddRef(m_pDevice);
    SafeAddRef(m_pContext);
    SafeAddRef(m_pGameInst);
}


//typedef unsigned(__stdcall* _beginthreadex_proc_type)(void*);
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

    // 시스템 프로그래밍 P411
    // 다수의 스레드가 동시에 동일 메모리에 접근하는 것을 방지(동기화, 순서를 지킨다)하기 위한 함수
    // 유저모드 동기화 기법 중 하나, 열쇠를 얻는다.
    InitializeCriticalSection(&m_Critical_Section);
    
    /* 내 코드를 읽어나갈수 있는 하나의 스레드를 생성한다. */
    /* 생성한 스레드가 호출해야할 진입점함수를 지정해준다. */
    // _beginthreadex 호출시 각각의 스레드를 위해서 메모리를 할당한다고 했다.
    // - 따라서 스레드 종료시 할당한 메모리를 반환 해야한다. 이 역할을 하는 함수가 바로 _endThreadex이다.
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
    /*auto pObj = m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, TEXT("Layer_Loading_UI"));
    pObj->SetActive(true);*/

    EnterCriticalSection(&m_Critical_Section);

    CoInitializeEx(nullptr, 0);

    HRESULT hr = {};

 
    

    switch (m_eNextLevelID)
    {
    case Client::LEVEL_LOGO:
        hr = LoadingLevelLogo();
        break;
    case Client::LEVEL_GAMEPLAY:
        hr = LoadingLevelGamePlay();
        break;
    case Client::LEVEL_BOSS :
        hr = LoadingLevelBoss();
        break;
    case Client::LEVEL_END:
        break;
    default:
        break;
    }




    LeaveCriticalSection(&m_Critical_Section);

    // pObj->SetActive(false);


    return S_OK;
}

void CLoader::Render()
{
    
}

HRESULT CLoader::LoaReadyLoad()
{
    return S_OK;
}

HRESULT CLoader::LoadingLevelLogo()
{
    lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));
    //if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Logo"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Textures/Default_BG.png")))))
    //    return E_FAIL;

    // Shader 로드
    {
        // InputLayout을 만들기 위한 DESC를 일단 임시적으로 이곳에서 만들어 준다.
        // 아래 부분 수정 예정(240930) -> 이제 Engine에서 D3D11_INPUT_ELEMENT_DESC를 만들어서 가져다가 쓴다.

        // ID3D11_INPUT_ELEMENT_DESC 선언 어쩌구 이제 안해도됨.
        {
            //if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTX_POSTEX::Elements, VTX_POSTEX::iNumElements))))
            //    return E_FAIL;
        }

    }

    lstrcpy(m_szLoadingText, TEXT("모델(을)를 로딩중입니다."));
    // 컴포넌트로 들어갈 VIBuffer_Rect 로드
    {
        // For. Prototype_Component_VIBuffer_Rect
        //if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Rect"), CVIBufferRect::Create(m_pDevice, m_pContext))))
        //    return E_FAIL;
    }


    lstrcpy(m_szLoadingText, TEXT("객체원형(을)를 로딩중입니다."));
    // 실제 객체 원형 로드
    {
        // Prototype_GameObject_BackGround
        // 프로토 타입 객체르 만들어준다. 즉, origin 객체를 만든다.
    /*    if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pContext))))
            return E_FAIL;*/
    }

    lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

    m_bIsFinished = true;

    return S_OK;
}

HRESULT CLoader::LoadingLevelGamePlay()
{
    _int iTot = 17;
    _int iCur = 0;

    auto pObj = static_cast<CLoadingColor*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, TEXT("Layer_LoadingColor")));

    {
        pObj->SetTotLoadCnt(iTot);
        pObj->SetcurLoadCnt(iCur);
    }


    // Shader
    lstrcpy(m_szLoadingText, TEXT("Loading Shaders..."));
    if (FAILED(LoadLevelGlobalShaders()))
        return E_FAIL;
    pObj->SetcurLoadCnt(iCur++);
    cout << iCur << endl;


    // Components
    lstrcpy(m_szLoadingText, TEXT("Loading Components..."));
    if (FAILED(LoadLevelGlobalComponents()))
        return E_FAIL;
    pObj->SetcurLoadCnt(++iCur);
    cout << iCur << endl;


    lstrcpy(m_szLoadingText, TEXT("Loading Player..."));
    if (FAILED(LoadLevelGlobalPlayer()))
        return E_FAIL;
    pObj->SetcurLoadCnt(++iCur);
    cout << iCur << endl;


    lstrcpy(m_szLoadingText, TEXT("Loading Stages..."));
    if (FAILED(LoadGamePlayStages()))
        return E_FAIL;
    pObj->SetcurLoadCnt(++iCur);
    cout << iCur << endl;

    // Camera
    lstrcpy(m_szLoadingText, TEXT("Loading Camera..."));
    if (FAILED(LoadLevelGlobalCamera()))
        return E_FAIL;
    pObj->SetcurLoadCnt(++iCur);
    cout << iCur << endl;

    lstrcpy(m_szLoadingText, TEXT("Loading Terrain..."));
    if (FAILED(LoadGamePlayTerrain()))
        return E_FAIL;
    pObj->SetcurLoadCnt(++iCur);
    cout << iCur << endl;

    lstrcpy(m_szLoadingText, TEXT("Loading SkyBox..."));
    if (FAILED(LoadGamePlaySkyBox()))
        return E_FAIL;
    pObj->SetcurLoadCnt(++iCur);
    cout << iCur << endl;


    lstrcpy(m_szLoadingText, TEXT("Loading Torch..."));
    if (FAILED(LoadLevelGlobalTorch()))
        return E_FAIL;
    pObj->SetcurLoadCnt(++iCur);

    lstrcpy(m_szLoadingText, TEXT("Loading Monster..."));
    if (FAILED(LoadLevelGlobalMonsters()))
        return E_FAIL;
    pObj->SetcurLoadCnt(++iCur);
    cout << iCur << endl;


    lstrcpy(m_szLoadingText, TEXT("Loading Triggers..."));
    if (FAILED(LoadLevelGlobalTriggers()))
        return E_FAIL;
    pObj->SetcurLoadCnt(++iCur);
    cout << iCur << endl;

    lstrcpy(m_szLoadingText, TEXT("Loading Projectiles..."));
    if (FAILED(LoadLevelGlobalProjectiles()))
        return E_FAIL;
    pObj->SetcurLoadCnt(++iCur);

    lstrcpy(m_szLoadingText, TEXT("Loading Effects..."));
    if (FAILED(LoadLevelGlobalEffect()))
        return E_FAIL;
    pObj->SetcurLoadCnt(++iCur);


    lstrcpy(m_szLoadingText, TEXT("Loading Particles..."));
    if (FAILED(LoadLevelGlobalParticles()))
        return E_FAIL;
    pObj->SetcurLoadCnt(++iCur);

    lstrcpy(m_szLoadingText, TEXT("Loading WallAABB..."));
    if (FAILED(LoadGamePlayWallAABB()))
        return E_FAIL;
    pObj->SetcurLoadCnt(++iCur);

    lstrcpy(m_szLoadingText, TEXT("Loading Effect Models..."));
    if (FAILED(LoadLevelGlobalEffectModel()))
        return E_FAIL;
    pObj->SetcurLoadCnt(++iCur);

    lstrcpy(m_szLoadingText, TEXT("Loading Effect Trails..."));
    if (FAILED(LoadLevelGlobalEffectTrail()))
        return E_FAIL;
    pObj->SetcurLoadCnt(++iCur);

    lstrcpy(m_szLoadingText, TEXT("Loading UIs..."));
    if (FAILED(LoadLevelGlobalUI()))
        return E_FAIL;
    pObj->SetcurLoadCnt(++iCur);



    



    // TestPlayer
    {
        _matrix PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
        vector<_int> vec = { -1 };
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/Fiona/Fiona.fbx", PreTransformMatrix, vec, false, false))))
            return E_FAIL;

        /* For.Prototype_GameObject_TestPlayer */
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_TestPlayer"), CTestPlayer::Create(m_pDevice, m_pContext)))) return E_FAIL;

        /* For.Prototype_GameObject_TestPlayerBody */
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_TestPlayerBody"), CTestPlayerBody::Create(m_pDevice, m_pContext)))) return E_FAIL;

        /* For.Prototype_GameObject_TestPlayerWeapon */
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_TestPlayerWeapon"), CTestPlayerWeapon::Create(m_pDevice, m_pContext)))) return E_FAIL;
    }

    // ForkLift
    {
        vector<_int> vec = { -1 };
        _matrix PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ForkLift"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/resources/Models/ForkLift/ForkLift.fbx", PreTransformMatrix, vec, false, false))))
            return E_FAIL;
    }

    lstrcpy(m_szLoadingText, TEXT("Loading Compolete!!!"));

    pObj->SetcurLoadCnt(iTot);

    m_bIsFinished = true;

    return S_OK;
}

HRESULT CLoader::LoadingLevelBoss()
{

    _int iTot = 16;
    _int iCur = 0;

    auto pObj = static_cast<CLoadingColor*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, TEXT("Layer_LoadingColor")));

    {
        pObj->SetTotLoadCnt(iTot);
        pObj->SetcurLoadCnt(iCur);
    }





    lstrcpy(m_szLoadingText, TEXT("Loading Level Boss Camera..."));
    if (FAILED(LoadLevelBossCamera()))
        return E_FAIL;
    pObj->SetcurLoadCnt(++iCur);

    lstrcpy(m_szLoadingText, TEXT("Loading Level Boss Player..."));
    if (FAILED(LoadLevelBossPlayer()))
        return E_FAIL;
    pObj->SetcurLoadCnt(++iCur);

    lstrcpy(m_szLoadingText, TEXT("Loading Level Boss Stages..."));
    if (FAILED(LoadLevelBossStages()))
        return E_FAIL;
    pObj->SetcurLoadCnt(++iCur);

    lstrcpy(m_szLoadingText, TEXT("Loading Level Boss Boss..."));
    if (FAILED(LoadLevelBossBoss()))
        return E_FAIL;
    pObj->SetcurLoadCnt(++iCur);

    lstrcpy(m_szLoadingText, TEXT("Loading Level SkyBox..."));
    if (FAILED(LoadLevelBossSkyBox()))
        return E_FAIL;
    pObj->SetcurLoadCnt(++iCur);

    lstrcpy(m_szLoadingText, TEXT("Loading Level Projectile..."));
    if (FAILED(LoadLevelBossProjectile()))
        return E_FAIL;
    pObj->SetcurLoadCnt(++iCur);

    lstrcpy(m_szLoadingText, TEXT("Loading Level Boss Compolete!!!"));

    m_bIsFinished = true;

    pObj->SetcurLoadCnt(iTot);

    return S_OK;
}

HRESULT CLoader::LoadLevelGlobalPlayer()
{

    // Load Models...
    {
        // 1202_Right, 기본 딱총
        // 애니매이션 1, 3번 못 씀
        {
            _matrix PreTransformMatrix = XMMatrixIdentity();
            vector<_int> UselessAnimIndex = { 1, 3 };
            PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.f, 0.f, 0.f);
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Model_Player_1201"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Player/1201_Right/1201_Right.dat", PreTransformMatrix, UselessAnimIndex, true, true, false, "1201_Right"))))
                return E_FAIL;
        }



        // 1004_Right, 불꽃 비늘
        // 애니매이션 0, 2, 3번 못 씀
        {
            vector<_int> Useless = { 0, 2, 3 };
            _matrix PreTransformMatrix = XMMatrixIdentity();
            PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.08f, 0.0f, 0.08f);
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Model_Player_1004"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Player/1004_Right/1004_Right.dat", PreTransformMatrix, Useless, true, true, true, "1004_Right"))))
                return E_FAIL;
        }


        // 1503_Right
        // 애니매이션 : 0번 못씀, 1번 RELOAD, 2번 못씀, 3번 FIRE, 4, 5번 못 씀
        {
            // _matrix PreTransformMatrixO = XMMatrixIdentity();
            // CConverter::GetInstance()->ExportAssetFile("../Bin/resources/Models/GF_Player/1503_Right/1503_Right.fbx", "../Bin/resources/Models/GF_Player/1503_Right/1503_Right.dat", CModel::TYPE_ANIM, PreTransformMatrixO);

            vector<_int> Useless = { -1 };
            _matrix PreTransformMatrix = XMMatrixIdentity();
            PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Model_Player_1503"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Player/1503_Right/1503_Right.dat", PreTransformMatrix, Useless, true, true, true, "1503_Right"))))
                return E_FAIL;
        }


        // 1508_Right
        // 
        {
            vector<_int> Useless = { -1 };
            _matrix PreTransformMatrix = XMMatrixIdentity();
            PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.0f, 0.0f, -0.3f);;
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Model_Player_1508"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Player/1508_Right/1508_Right.dat", PreTransformMatrix, Useless, true, true, true, "1508_Right"))))
                return E_FAIL;
        }


    }
   
    // Load Objects...
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Player"), CPlayer::Create(m_pDevice, m_pContext)))) return E_FAIL;

        /* For.Prototype_GameObject_TestPlayerBody */
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_PlayerBody"), CPlayerBody::Create(m_pDevice, m_pContext)))) return E_FAIL;
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_PlayerWeapon"), CPlayerWeapon::Create(m_pDevice, m_pContext)))) return E_FAIL;
    }
    
    return S_OK;
}

HRESULT CLoader::LoadGamePlayStages()
{
    // _matrix PreTransformMatrix = XMMatrixIdentity();

    _matrix PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(-19.f, 0.f, 3.f);
    // _matrix PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));

    // CConverter::GetInstance()->ExportAssetFile("../Bin/Resources/Models/GF_Stages/Stage01/Stage01.fbx", "../Bin/Resources/Models/GF_Stages/Stage01/Stage01.dat", CModel::TYPE_ANIM, PreTransformMatrix);

    vector<_int> Useless = { -1 };
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Stage01"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/GF_Stages/Stage01/Stage01.fbx", PreTransformMatrix, Useless, false))))
        return E_FAIL;

    if (FAILED(m_pGameInst->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Stage01"), CStage01::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLoader::LoadLevelGlobalMonsters()
{
    // Monster01
    {
        // Prototype_Component_Model_Monster01
        vector<_int> Useless = { -1 };
        _matrix PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Model_Monster01"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Monsters/Monster01/Monster01.dat", PreTransformMatrix, Useless, false, true))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_MonsterBody01"), CMonsterBody01::Create(m_pDevice, m_pContext))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_MonsterWeapon01"), CMonsterWeapon01::Create(m_pDevice, m_pContext))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Monster01"), CMonster01::Create(m_pDevice, m_pContext))))
            return E_FAIL;

        
    }


    // Monster02
    {
        // Prototype_Component_Model_Monster01
        vector<_int> Useless = { -1 };
        _matrix PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Model_Monster02"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Monsters/Monster02/Monster02.dat", PreTransformMatrix, Useless, false, true))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_MonsterBody02"), CMonsterBody02::Create(m_pDevice, m_pContext))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_MonsterWeapon02"), CMonsterWeapon02::Create(m_pDevice, m_pContext))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Monster02"), CMonster02::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }

    // Monster03
    {
        vector<_int> Useless = { -1 };
        _matrix PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Model_Monster03"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Monsters/Monster03/Monster03.dat", PreTransformMatrix, Useless, false, true))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_MonsterBody03"), CMonsterBody03::Create(m_pDevice, m_pContext))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_MonsterWeapon03"), CMonsterWeapon03::Create(m_pDevice, m_pContext))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Monster03"), CMonster03::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }

    // Monster04
    {
        vector<_int> Useless = { -1 };
        _matrix PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Model_Monster04"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Monsters/Monster04/Monster04.dat", PreTransformMatrix, Useless, false, true))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_MonsterBody04"), CMonsterBody04::Create(m_pDevice, m_pContext))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_MonsterWeapon04"), CMonsterWeapon04::Create(m_pDevice, m_pContext))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Monster04"), CMonster04::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }

    // Monster05
    {
        vector<_int> Useless = { -1 };
        _matrix PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Model_Monster05"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Monsters/Monster05/Monster05.dat", PreTransformMatrix, Useless, false, true))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_MonsterBody05"), CMonsterBody05::Create(m_pDevice, m_pContext))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_MonsterWeapon05"), CMonsterWeapon05::Create(m_pDevice, m_pContext))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Monster05"), CMonster05::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }


    // Monster06
    {
        vector<_int> Useless = { -1 };
        _matrix PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Model_Monster06"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/GF_Monsters/Monster08/Monster08.dat", PreTransformMatrix, Useless, false, true))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_MonsterBody06"), CMonsterBody06::Create(m_pDevice, m_pContext))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_MonsterWeapon06"), CMonsterWeapon06::Create(m_pDevice, m_pContext))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Monster06"), CMonster06::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CLoader::LoadGamePlayTerrain()
{
    // Terrain Textures
    {
        // 241025 Mask를 위해 dds파일 두장을 로드해준다.
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2)))) return E_FAIL;

        // Terrain Mask Texture
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain_Mask"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.dds"), 1)))) return E_FAIL;

        /* For.Prototype_Component_Texture_Terrain_Brush */
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain_Brush"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1)))) return E_FAIL;

        // Terrain 높이맵 로드
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp"))))) return E_FAIL;
    }

    /* For.Prototype_GameObject_Terrain */
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Terrain"), CTerrain::Create(m_pDevice, m_pContext)))) 
        return E_FAIL;

    return S_OK;
}

HRESULT CLoader::LoadGamePlaySkyBox()
{
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_SkyBox"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
        return E_FAIL;

    if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_SkyBox"), CVIBufferSkyBox::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_SkyBox */
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_SkyBox"), CSkyBox::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLoader::LoadLevelGlobalTriggers()
{
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_SpawnTrigger"), CSpawnTrigger::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Spawner"), CSpawner::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_InterMove"), CInterMove::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UITrigger"), CUITrigger::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_InterMoveEndPos"), CInterMoveEndPos::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLoader::LoadLevelGlobalProjectiles()
{
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Arrow"), CArrow::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Bullet"), CBullet::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_PlayerBullet"), CPlayerBullet::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_MonsterBullet"), CMonsterBullet::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLoader::LoadLevelGlobalParticles()
{
    // Player Gun Fire
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Explosion"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayerEffect/GunFire/muzzle_orange.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Snow"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPointInstance"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTX_POINT_INSTANCE::Elements, VTX_POINT_INSTANCE::iNumElements))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPointInstance_PlayerGunFire"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance_PlayerGunFire.hlsl"), VTX_POINT_INSTANCE::Elements, VTX_POINT_INSTANCE::iNumElements))))
            return E_FAIL;

        CVIBufferParticleInstance::PARTICLE_DESC			ExplosionDesc{};

        ExplosionDesc.iNumInstances = 1;
        ExplosionDesc.vCenter = _float3(0.f, 0.f, 0.f);
        ExplosionDesc.vRange = _float3(0.1f, 0.1f, 0.1f);
        ExplosionDesc.vSpeed = _float2(0.f, 0.f);
        ExplosionDesc.vLifeTime = _float2(0.2f, 0.21f);
        ExplosionDesc.vSize = _float2(0.7f, 0.71f);
        ExplosionDesc.vPivot = _float3(0.f, 0.0f, -1.f);
        ExplosionDesc.bIsLoop = false;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Particle_Explosion"), CVIBufferParticlePoint::Create(m_pDevice, m_pContext, ExplosionDesc))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_PlayerGunFire"), CParticlePlayerGunFire::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }

    // Monster Hit Particle
    {
        CVIBufferParticleInstance::PARTICLE_DESC			MonsterHitDesc{};

        MonsterHitDesc.iNumInstances = 22;
        MonsterHitDesc.vCenter = _float3(0.f, 0.f, 0.f);
        MonsterHitDesc.vRange = _float3(1.0f, 0.3f, 1.0f);
        MonsterHitDesc.vSpeed = _float2(9.f, 12.f);
        MonsterHitDesc.vLifeTime = _float2(0.25f, 0.5f);
        MonsterHitDesc.vSize = _float2(0.2f, 0.3f);
        MonsterHitDesc.vPivot = _float3(0.f, -1.0f, 0.f);
        MonsterHitDesc.bIsLoop = false;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_MonsterHit"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayerEffect/ProjectileHit/particle_blue_02.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Particle_MonsterHit"), CVIBufferParticlePoint::Create(m_pDevice, m_pContext, MonsterHitDesc))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), CParticleMonsterHit::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }

    // Wall
    {
        CVIBufferParticleInstance::PARTICLE_DESC			SpawnDesc{};

        SpawnDesc.iNumInstances = 4;
        SpawnDesc.vCenter = _float3(0.f, 0.f, 0.f);
        SpawnDesc.vRange = _float3(1.f, 0.3f, 1.f);
        SpawnDesc.vSpeed = _float2(5.f, 20.f);
        SpawnDesc.vLifeTime = _float2(0.25f, 0.35f);
        SpawnDesc.vSize = _float2(0.1f, 0.4f);
        SpawnDesc.vPivot = _float3(0.f, -1.0f, 0.f);
        SpawnDesc.bIsLoop = false;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Particle_WallHit"), CVIBufferParticlePoint::Create(m_pDevice, m_pContext, SpawnDesc))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_WallHit"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/WallHitEffect/GW_2202_glow_006.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPointInstance_WallHit"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance_WallHit.hlsl"), VTX_POINT_INSTANCE::Elements, VTX_POINT_INSTANCE::iNumElements))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_WallHit"), CParticleWall::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }

    // MonsterSpawn
    {
        CVIBufferParticleInstance::PARTICLE_DESC			SpawnDesc{};

        SpawnDesc.iNumInstances = 50;
        SpawnDesc.vCenter = _float3(0.f, 0.f, 0.f);
        SpawnDesc.vRange = _float3(2.5f, 1.f, 2.5f);
        SpawnDesc.vSpeed = _float2(1.f, 2.f);
        SpawnDesc.vLifeTime = _float2(1.0f, 1.5f);
        SpawnDesc.vSize = _float2(1.f, 1.5f);
        SpawnDesc.vPivot = _float3(0.f, 0.f, 0.f);
        SpawnDesc.bIsLoop = false;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Particle_MonsterSpawn"), CVIBufferParticlePoint::Create(m_pDevice, m_pContext, SpawnDesc))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_MonsterSpawn"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MonsterSpawn/GW_2281_smoke_001.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPointInstance_MonsterSpawn"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance_MonsterSpawn.hlsl"), VTX_POINT_INSTANCE::Elements, VTX_POINT_INSTANCE::iNumElements))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterSpawn"), CParticleMonsterSpawn::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }

    // ProjectileHit
    {
        CVIBufferParticleInstance::PARTICLE_DESC			ProjectileHitDesc{};

        ProjectileHitDesc.iNumInstances = 1;
        ProjectileHitDesc.vCenter = _float3(0.f, 0.f, 0.f);
        ProjectileHitDesc.vRange = _float3(0.01f, 0.01f, 0.01f);
        ProjectileHitDesc.vSpeed = _float2(0.f, 0.f);
        ProjectileHitDesc.vLifeTime = _float2(0.3f, 0.31f);
        ProjectileHitDesc.vSize = _float2(3.f, 1.f);
        ProjectileHitDesc.vPivot = _float3(0.f, 0.0f, -1.f);
        ProjectileHitDesc.bIsLoop = false;


        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_ProjectileHit"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayerEffect/ProjectileHit/speak_hit_05.png"), 1))))
            return E_FAIL;


        {
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Particle_ProjectileHit"), CVIBufferParticlePoint::Create(m_pDevice, m_pContext, ProjectileHitDesc))))
                return E_FAIL;


            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPointInstance_ProjectileHit"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance_ProjectileHit.hlsl"), VTX_POINT_INSTANCE::Elements, VTX_POINT_INSTANCE::iNumElements))))
                return E_FAIL;
        }

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), CParticleProjectileHit::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }


    // ParticleBossRock
    {
        CVIBufferParticleInstance::PARTICLE_DESC ParticleBossRockDesc = {};

        ParticleBossRockDesc.iNumInstances = 100;
        ParticleBossRockDesc.vCenter = _float3(0.f, 0.f, 0.f);
        ParticleBossRockDesc.vRange = _float3(10.f, 10.f, 10.f);
        ParticleBossRockDesc.vSpeed = _float2(15.f, 25.f);
        ParticleBossRockDesc.vLifeTime = _float2(0.3f, 0.5f);
        ParticleBossRockDesc.vSize = _float2(0.2f, 0.3f);
        ParticleBossRockDesc.vPivot = _float3(0.f, 0.0f, 0.f);
        ParticleBossRockDesc.bIsLoop = true;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Particle_BossRock"), CVIBufferParticlePoint::Create(m_pDevice, m_pContext, ParticleBossRockDesc))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_BossRock"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Boss/glow_blue.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRock"), CParticleBossRock::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }

    // ParticleBossRockDust
    // 보스 몬스터의 메테오 모을 때 생기는 파티클
    {
        CVIBufferParticleInstance::PARTICLE_DESC ParticleBossRockDesc = {};

        ParticleBossRockDesc.iNumInstances = 300;
        ParticleBossRockDesc.vCenter = _float3(0.f, 0.f, 0.f);
        ParticleBossRockDesc.vRange = _float3(20.f, 20.f, 20.f);
        ParticleBossRockDesc.vSpeed = _float2(25.f, 35.f);
        ParticleBossRockDesc.vLifeTime = _float2(0.3f, 0.5f);
        ParticleBossRockDesc.vSize = _float2(0.1f, 0.15f);
        ParticleBossRockDesc.vPivot = _float3(0.f, 0.0f, 0.f);
        ParticleBossRockDesc.bIsLoop = true;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Particle_BossRockDust"), CVIBufferParticlePoint::Create(m_pDevice, m_pContext, ParticleBossRockDesc))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_BossRockDust"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Boss/7138_glow_01.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockDust"), CParticleBossRockDust::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }

    // ParticleBossRockCol
    {
        CVIBufferParticleInstance::PARTICLE_DESC ParticleBossRockDesc = {};

        ParticleBossRockDesc.iNumInstances = 150;
        ParticleBossRockDesc.vCenter = _float3(0.f, 0.f, 0.f);
        ParticleBossRockDesc.vRange = _float3(2.f, 1.f, 2.f);
        ParticleBossRockDesc.vSpeed = _float2(25.f, 35.f);
        ParticleBossRockDesc.vLifeTime = _float2(1.5f, 2.f);
        ParticleBossRockDesc.vSize = _float2(0.25f, 1.5f);
        ParticleBossRockDesc.vPivot = _float3(0.f, 0.f, 0.f);
        ParticleBossRockDesc.bIsLoop = false;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Particle_BossRockCol"), CVIBufferParticlePoint::Create(m_pDevice, m_pContext, ParticleBossRockDesc))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_BossRockCol"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Boss/stone_03.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockCol"), CParticleBossRockCol::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }

    // ParticleBossRockColDust
    {
        CVIBufferParticleInstance::PARTICLE_DESC ParticleBossRockDesc = {};

        ParticleBossRockDesc.iNumInstances = 50;
        ParticleBossRockDesc.vCenter = _float3(0.f, 0.f, 0.f);
        ParticleBossRockDesc.vRange = _float3(1.5f, 3.f, 1.5f);
        ParticleBossRockDesc.vSpeed = _float2(5.f, 10.f);
        ParticleBossRockDesc.vLifeTime = _float2(1.5f, 1.51f);
        ParticleBossRockDesc.vSize = _float2(2.f, 5.f);
        ParticleBossRockDesc.vPivot = _float3(0.f, -3.f, 0.f);
        ParticleBossRockDesc.bIsLoop = false;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Particle_BossRockColDust"), CVIBufferParticlePoint::Create(m_pDevice, m_pContext, ParticleBossRockDesc))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_BossRockColDust"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Boss/fire_39.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockColDust"), CParticleBossRockColDust::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }


    // ParticleBossRockColDustLighting
    {
        CVIBufferParticleInstance::PARTICLE_DESC ParticleBossRockDesc = {};

        ParticleBossRockDesc.iNumInstances = 100;
        ParticleBossRockDesc.vCenter = _float3(0.f, 0.f, 0.f);
        ParticleBossRockDesc.vRange = _float3(3.f, 3.f, 3.f);
        ParticleBossRockDesc.vSpeed = _float2(10.f, 15.f);
        ParticleBossRockDesc.vLifeTime = _float2(1.f, 1.25f);
        ParticleBossRockDesc.vSize = _float2(1.5f, 2.f);
        ParticleBossRockDesc.vPivot = _float3(0.f, 0.f, 0.f);
        ParticleBossRockDesc.bIsLoop = false;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Particle_BossRockColDustLighting"), CVIBufferParticlePoint::Create(m_pDevice, m_pContext, ParticleBossRockDesc))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_BossRockColDustLighting"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Boss/lightning_05.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockColDustLighting"), CParticleBossRockColDustLighting::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }


    // ParticleBossSmash
    {
        CVIBufferParticleInstance::PARTICLE_DESC ParticleBossSmashDesc = {};
        ParticleBossSmashDesc.iNumInstances = 30;
        ParticleBossSmashDesc.vCenter = _float3(0.f, 0.f, 0.f);
        ParticleBossSmashDesc.vRange = _float3(10.f, 5.f, 10.f);
        ParticleBossSmashDesc.vSpeed = _float2(2.f, 3.f);
        ParticleBossSmashDesc.vLifeTime = _float2(0.25f, 0.3f);
        ParticleBossSmashDesc.vSize = _float2(2.5f, 3.f);
        ParticleBossSmashDesc.vPivot = _float3(0.f, -1.f, 0.f);
        ParticleBossSmashDesc.bIsLoop = false;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Particle_BossSmash"), CVIBufferParticlePoint::Create(m_pDevice, m_pContext, ParticleBossSmashDesc))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_BossSmash"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Boss/spark_cp_041.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossSmash"), CParticleBossSmash::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }


    // ParticleBossSmashDust
    {
        CVIBufferParticleInstance::PARTICLE_DESC ParticleBossSmashDesc = {};
        ParticleBossSmashDesc.iNumInstances = 200;
        ParticleBossSmashDesc.vCenter = _float3(0.f, 0.f, 0.f);
        ParticleBossSmashDesc.vRange = _float3(1.f, 1.f, 1.f);
        ParticleBossSmashDesc.vSpeed = _float2(15.f, 20.f);
        ParticleBossSmashDesc.vLifeTime = _float2(5.f, 6.f);
        ParticleBossSmashDesc.vSize = _float2(0.1f, 0.2f);
        ParticleBossSmashDesc.vPivot = _float3(0.f, 0.f, 0.f);
        ParticleBossSmashDesc.bIsLoop = false;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Particle_BossSmashDust"), CVIBufferParticlePoint::Create(m_pDevice, m_pContext, ParticleBossSmashDesc))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_BossSmashDust"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Boss/stone_03.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossSmashDust"), CParticleBossSmashDust::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }


    // ParticleBossWalk
    {
        CVIBufferParticleInstance::PARTICLE_DESC ParticleBossWalkDesc = {};
        ParticleBossWalkDesc.iNumInstances = 50;
        ParticleBossWalkDesc.vCenter = _float3(0.f, 0.f, 0.f);
        ParticleBossWalkDesc.vRange = _float3(3.f, 1.f, 3.f);
        ParticleBossWalkDesc.vSpeed = _float2(0.1f, 0.3f);
        ParticleBossWalkDesc.vLifeTime = _float2(0.25f, 0.5f);
        ParticleBossWalkDesc.vSize = _float2(1.f, 1.5f);
        ParticleBossWalkDesc.vPivot = _float3(0.f, -1.f, 0.f);
        ParticleBossWalkDesc.bIsLoop = true;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Particle_BossWalk"), CVIBufferParticlePoint::Create(m_pDevice, m_pContext, ParticleBossWalkDesc))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_BossWalk"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Boss/spark_cp_041.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossWalk"), CParticleBossWalk::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }


    // MonsterDead
    {
        CVIBufferParticleInstance::PARTICLE_DESC			SpawnDesc{};

        SpawnDesc.iNumInstances = 30;
        SpawnDesc.vCenter = _float3(0.f, 0.f, 0.f);
        SpawnDesc.vRange = _float3(2.5f, 2.f, 2.5f);
        SpawnDesc.vSpeed = _float2(1.f, 2.f);
        SpawnDesc.vLifeTime = _float2(1.0f, 1.5f);
        SpawnDesc.vSize = _float2(1.f, 1.5f);
        SpawnDesc.vPivot = _float3(0.f, 0.f, 0.f);
        SpawnDesc.bIsLoop = false;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Particle_MonsterDead"), CVIBufferParticlePoint::Create(m_pDevice, m_pContext, SpawnDesc))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_MonsterDead"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MonsterSpawn/GW_2281_smoke_001.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterDead"), CParticleMonsterDead::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }

    // FollowLightingDust
    {
        CVIBufferParticleInstance::PARTICLE_DESC			SpawnDesc{};

        SpawnDesc.iNumInstances = 30;
        SpawnDesc.vCenter = _float3(0.f, 0.f, 0.f);
        SpawnDesc.vRange = _float3(5.f, 2.f, 5.f);
        SpawnDesc.vSpeed = _float2(2.f, 2.5f);
        SpawnDesc.vLifeTime = _float2(0.75f, 0.76f);
        SpawnDesc.vSize = _float2(1.f, 1.5f);
        SpawnDesc.vPivot = _float3(0.f, 0.f, 0.f);
        SpawnDesc.bIsLoop = false;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Particle_FollowLightingDust"), CVIBufferParticlePoint::Create(m_pDevice, m_pContext, SpawnDesc))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_FollowLightingDust"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Boss/fire_33.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_FollowLightingDust"), CParticleFollowLightingDust::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }

    // MonsterChase
    {
        CVIBufferParticleInstance::PARTICLE_DESC			SpawnDesc{};

        SpawnDesc.iNumInstances = 5;
        SpawnDesc.vCenter = _float3(0.f, 0.f, 0.f);
        SpawnDesc.vRange = _float3(0.5f, 0.f, 0.5f);
        SpawnDesc.vSpeed = _float2(0.5f, 0.6f);
        SpawnDesc.vLifeTime = _float2(0.5f, 0.51f);
        SpawnDesc.vSize = _float2(0.7f, 0.71f);
        SpawnDesc.vPivot = _float3(0.f, 0.f, 0.f);
        SpawnDesc.bIsLoop = false;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Particle_MonsterChase"), CVIBufferParticlePoint::Create(m_pDevice, m_pContext, SpawnDesc))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_MonsterChase"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Monster/ChaseParticle/spark_cp_031.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterChase"), CParticleMonsterChase::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }


    return S_OK;
}

HRESULT CLoader::LoadGamePlayWallAABB()
{
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_WallAABB"), CWallAABB::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLoader::LoadLevelBossStages()
{
    vector<_int> Useless = { -1 };
    _matrix PreTransformMatrix = XMMatrixIdentity();
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_BOSS, TEXT("Prototype_Component_Model_BossStage"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/GF_Stages/Boss_Stage/Boss_Stage.dat", PreTransformMatrix, Useless, false, true))))
        return E_FAIL;

    if (FAILED(m_pGameInst->AddPrototype(LEVEL_BOSS, TEXT("Prototype_GameObject_BossStage"), CBossStage::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    return S_OK;
}



HRESULT CLoader::LoadLevelBossCamera()
{
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_BOSS, TEXT("Prototype_GameObject_Camera"), CCamera_Free::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_DebugCamera"), CDebugCamera::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLoader::LoadLevelBossPlayer()
{
    // Load Models...
    {
        // 1202_Right, 기본 딱총
        // 애니매이션 1, 3번 못 씀
        {
            _matrix PreTransformMatrix = XMMatrixIdentity();
            vector<_int> UselessAnimIndex = { 1, 3 };
            PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.f, 0.f, 0.f);
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_BOSS, TEXT("Prototype_Component_Model_Player_1201"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Player/1201_Right/1201_Right.dat", PreTransformMatrix, UselessAnimIndex, true, true, false, "1201_Right"))))
                return E_FAIL;
        }



        // 1004_Right, 불꽃 비늘
        // 애니매이션 0, 2, 3번 못 씀
        {
            vector<_int> Useless = { 0, 2, 3 };
            _matrix PreTransformMatrix = XMMatrixIdentity();
            PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.11f, 0.0f, 0.1f);
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_BOSS, TEXT("Prototype_Component_Model_Player_1004"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Player/1004_Right/1004_Right.dat", PreTransformMatrix, Useless, true, true, true, "1004_Right"))))
                return E_FAIL;
        }



        // 1503_Right
        // 애니매이션 : 0번 못씀, 1번 RELOAD, 2번 못씀, 3번 FIRE, 4, 5번 못 씀
        {
            // _matrix PreTransformMatrixO = XMMatrixIdentity();
            // CConverter::GetInstance()->ExportAssetFile("../Bin/resources/Models/GF_Player/1503_Right/1503_Right.fbx", "../Bin/resources/Models/GF_Player/1503_Right/1503_Right.dat", CModel::TYPE_ANIM, PreTransformMatrixO);

            vector<_int> Useless = { -1 };
            _matrix PreTransformMatrix = XMMatrixIdentity();
            PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_BOSS, TEXT("Prototype_Component_Model_Player_1503"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Player/1503_Right/1503_Right.dat", PreTransformMatrix, Useless, true, true, true, "1503_Right"))))
                return E_FAIL;
        }


        // 1508_Right
        // 
        {
            vector<_int> Useless = { -1 };
            _matrix PreTransformMatrix = XMMatrixIdentity();
            PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_BOSS, TEXT("Prototype_Component_Model_Player_1508"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/GF_Player/1508_Right/1508_Right.dat", PreTransformMatrix, Useless, true, true, true, "1508_Right"))))
                return E_FAIL;
        }


    }

    // Load Objects...
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_BOSS, TEXT("Prototype_GameObject_Player"), CPlayer::Create(m_pDevice, m_pContext)))) return E_FAIL;
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_BOSS, TEXT("Prototype_GameObject_PlayerBody"), CPlayerBody::Create(m_pDevice, m_pContext)))) return E_FAIL;
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_BOSS, TEXT("Prototype_GameObject_PlayerWeapon"), CPlayerWeapon::Create(m_pDevice, m_pContext)))) return E_FAIL;
    }




    return S_OK;
}

HRESULT CLoader::LoadLevelBossBoss()
{
    vector<_int> Useless = { -1 };
    _matrix PreTransformMatrix = XMMatrixIdentity();
    PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_BOSS, TEXT("Prototype_Component_Model_Boss"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/GF_Boss/BossModel/3909_temp.fbx", PreTransformMatrix, Useless, false, false, false,  "3909_temp"))))
        return E_FAIL;


    if (FAILED(m_pGameInst->AddPrototype(LEVEL_BOSS, TEXT("Prototype_GameObject_Boss"), CBoss::Create(m_pDevice, m_pContext)))) return E_FAIL;
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_BOSS, TEXT("Prototype_GameObject_BossBody"), CBossBody::Create(m_pDevice, m_pContext)))) return E_FAIL;
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_BOSS, TEXT("Prototype_GameObject_BossWeapon"), CBossWeapon::Create(m_pDevice, m_pContext)))) return E_FAIL;

    if (FAILED(m_pGameInst->AddPrototype(LEVEL_BOSS, TEXT("Prototype_GameObject_BossEye"), CBossEye::Create(m_pDevice, m_pContext)))) return E_FAIL;


    if (FAILED(m_pGameInst->AddPrototype(LEVEL_BOSS, TEXT("Prototype_Component_Texture_BossDeadNoise"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Models/GF_Boss/BossDead/NoiseFire01.png"), 1))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLoader::LoadLevelBossSkyBox()
{
  /*  if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_SkyBox"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
        return E_FAIL;*/

    if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Back"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Back.dds"), 1))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLoader::LoadLevelBossProjectile()
{
    vector<_int> Useless = { -1 };
    _matrix PreTransformMatrix = XMMatrixIdentity();
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_BOSS, TEXT("Prototype_Component_Model_BossRock"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/GF_Boss/BossRock/rock.fbx", PreTransformMatrix, Useless, false, false, false, "rock"))))
        return E_FAIL;

    if (FAILED(m_pGameInst->AddPrototype(LEVEL_BOSS, TEXT("Prototype_GameObject_BossRock"), CBossRock::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInst->AddPrototype(LEVEL_BOSS, TEXT("Prototype_GameObject_Meteor"), CMeteor::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLoader::LoadLevelGlobalShaders()
{
    // PosTex Shader
    {
        //if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTX_POSTEX::Elements, VTX_POSTEX::iNumElements))))
        //    return E_FAIL;
    }

    // VtxNorTex Shader
    {
        /* For.Prototype_Component_Shader_VtxNorTex*/
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxNorTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTX_NORTEX::Elements, VTX_NORTEX::iNumElements))))
            return E_FAIL;
    }

    // Model Shader
    {
        // NoneAnim Shader
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxMesh"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTX_MESH::Elements, VTX_MESH::iNumElements)))) return E_FAIL;

        // AnimMesh 쉐이더
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxAnimMesh"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTX_ANIMMESH::Elements, VTX_ANIMMESH::iNumElements))))
            return E_FAIL;
    }

    // SkyBox Shader
    {
        /* For.Prototype_Component_Shader_VtxCube */
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxCube_SkyBox"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/shaderFiles/Shader_VtxCube_SkyBox.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
            return E_FAIL;
    }

    // Terrain Shader
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxNorTex_Terrain_Brush"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex_Terrain_Brush.hlsl"), VTX_NORTEX::Elements, VTX_NORTEX::iNumElements))))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CLoader::LoadLevelGlobalComponents()
{
    // Colliders
    {
        /* For.Prototype_Component_Collider_AABB */
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Collider_AABB"),
            CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
            return E_FAIL;

        /* For.Prototype_Component_Collider_OBB */
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Collider_OBB"),
            CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
            return E_FAIL;

        /* For.Prototype_Component_Collider_Sphere */
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Collider_Sphere"),
            CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
            return E_FAIL;
    }
    

    {
        /* For.Prototype_Component_Navigation */
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Navigation"), CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/NavMesh_Scene01.dat")))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Navigation_Boss"), CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/NavMeshBoss.dat")))))
            return E_FAIL;


        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Navigation_S2"), CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/NavMesh_S2.dat")))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Navigation_S3"), CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/NavMesh_S3.dat")))))
            return E_FAIL;
    }


    return S_OK;
}

HRESULT CLoader::LoadLevelGlobalCamera()
{
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Camera_Free"), CCamera_Free::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLoader::LoadLevelGlobalEffect()
{
    // 수업에서 쓰는거
    {
        // Texutre
        /*if (FAILED(m_pGameInst->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Textures/Explosion/Explosion%d.png"), 90)))) 
            return E_FAIL;*/
    }

    {
        // Texutre
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_ProjectileWallHit"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Textures/PlayerEffect/ProjectileHit/muzzle_11.png"), 1)))) 
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHit"), CEffectProjectileWallHit::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }

    // WallhitFront
    {
        // Texutre
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_ProjectileWallHitFront"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Textures/PlayerEffect/ProjectileHit/DJS_hit_x_01.png"), 1))))
            return E_FAIL;


        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitFront"), CEffectProjectileWallHitFront::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }


    // MonsterHit
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_ProjectileMonsterHit"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Textures/PlayerEffect/ProjectileHit/J_H_004.png"), 1)))) return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileMonsterHit"), CEffectProjectileMonsterHit::Create(m_pDevice, m_pContext)))) return E_FAIL;
    }

    // WallHitSpot
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_ProjectileWallHitSpot"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Textures/PlayerEffect/ProjectileHit/earth_01.png"), 1)))) 
            return E_FAIL;
        

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), CEffectProjectileWallHitSpot::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }

    // Bullet
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_Bullet"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Textures/PlayerEffect/ProjectileMove/glow_big_yellow.png"), 1))))
            return E_FAIL;


        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), CEffectBullet::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }


    // BulletTrail
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_BulletTrail"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Textures/PlayerEffect/ProjectileMove/trail_36.png"), 1))))
            return E_FAIL;


        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), CEffectBulletTrail::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }

    // BossRockAlert
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_BossRockAlert"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Textures/Boss/img_salva_2.png"), 1))))
            return E_FAIL;


        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BossRockAlert"), CEffectBossRockAlert::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }

    // BossRockAlertFinal
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_BossRockAlertFinal"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Textures/Boss/skill_loop_01_ring01.png"), 1))))
            return E_FAIL;


        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BossRockAlertFinal"), CEffectBossRockAlertFinal::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }



    // FollowLighting
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_FollowLighting"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Textures/Boss/ligtning_18.png"), 1))))
            return E_FAIL;


        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_FollowLighting"), CEffectFollowLighting::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }


    // Circle1004
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_Circle1004"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Textures/PlayerEffect/1004/water_boom_02.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_Circle1004_Noise"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Textures/PlayerEffect/1004/BUFF_noise_020_04.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_Circle1004_Color"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Textures/PlayerEffect/1004/uvanim_01_purple.png"), 1))))
            return E_FAIL;


        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Circle1004"), CEffectCircle1004::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }


    // Hit1004
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_Hit1004Fire"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayerEffect/1004/210_fire08.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_Hit1004Diffuse"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayerEffect/1004/GW_2202_object_002.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_Hit1004Noise"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayerEffect/1004/NoiseFire01.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Hit1004"), CEffectHit1004::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }

    // Monster03Fire
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_Monster03Fire"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Monster/muzzle_flash_02.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster03Fire"), CMonster03Fire::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }

    // Monster04Fire
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_Monster04Fire"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Monster/GW_2301_ray_003.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster04Fire"), CMonster04Fire::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }





    // Plater Muzzles
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Muzzle"), CEffectMuzzle::Create(m_pDevice, m_pContext))))
            return E_FAIL;

        // 01
        {
            // Tex
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_Player01Muzzle"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayerEffect/Muzzle/muzzle_09.png"), 1))))
                return E_FAIL;
        }

        // 02
        {
            // Tex
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_Player02Muzzle"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayerEffect/Muzzle/Flare16.png"), 1))))
                return E_FAIL;
        }

        // 03
        {
            // Tex
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_Player03Muzzle"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayerEffect/Muzzle/muzzle_09.png"), 1))))    
                return E_FAIL;
        }

        // 04
        {
            // Tex
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_Player04Muzzle"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayerEffect/Muzzle/muzzle_06.png"), 1))))
                return E_FAIL;
        }
    }

    // Bullet Hit
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletHit"), CBulletHit::Create(m_pDevice, m_pContext))))
            return E_FAIL;

        // 03
        {
            // Tex
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_BulletHit03"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayerEffect/BulletHitEffect/fire_cp_003.png"), 1))))
                return E_FAIL;
        }

        // 04
        {
            // Tex
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_BulletHit04"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayerEffect/BulletHitEffect/muzzle_green_05.png"), 1))))
                return E_FAIL;
        }

    }

    // TorchFire
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_TorchFire"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Torch/fire_cp_008.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_TorchFireNoise"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Torch/NoiseFire01.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_TorchFire"), CTorchFire::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }

    // Boss Roar Noise
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_RoarNoise"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Boss/ring_noise_01.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_RoarNoise"), CEffectRoarNoise::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CLoader::LoadLevelGlobalEffectModel()
{
    _matrix PreTransformMatrix = XMMatrixIdentity();
    vector<_int> Useless = { -1 };
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Model_EffectSphere"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect/TestEffect/9408_trail.fbx", PreTransformMatrix, Useless, false))))
        return E_FAIL;
    
    // 번개 모델 테스트
    //if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Model_EffectTest"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect/Lighting/lightning_all.fbx", PreTransformMatrix, Useless, false))))
    //    return E_FAIL;

    //if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Sphere1004"), CEffectSphere1004::Create(m_pDevice, m_pContext))))
    //    return E_FAIL;

    if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_Test"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Models/Effect/TestEffect/NoiseElectro01.png"), 1))))
        return E_FAIL;

    if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_TestEffectColor"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Models/Effect/TestEffect/noise_010.png"), 1))))
        return E_FAIL;

    if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_EffectTest"), CTestEffect::Create(m_pDevice, m_pContext))))
        return E_FAIL;



    // EffectSphere1004
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Diffuse_EffectSphere1004"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Models/Effect/EffectSphere1004/GW_2202_ray_002.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Noise_EffectSphere1004"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Models/Effect/EffectSphere1004/NoiseElectro01.png"), 1))))
            return E_FAIL;



        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_EffectSphere1004"), CEffectSphere1004::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }

    // DoorEffect
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_EffectDoor"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Textures/Effect/DoorEffect/target_ring_01.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_EffectDoor"), CDoorEffect::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }


    return S_OK;
}

HRESULT CLoader::LoadLevelGlobalEffectTrail()
{
    // Buffer & Shader
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex_Time"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/shaderFiles/Shader_VtxPosTexTime.hlsl"), VTX_POSTEX_TIME::Elements, VTX_POSTEX_TIME::iNumElements))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Trail"), CVIBufferTrail::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }

    // Tex
    {
        // 테스트
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_TestTrail"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Trail/SwordTrail.png"), 1))))
            return E_FAIL;

        // 휘두를 때
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_SwordTrail"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Trail/SwordTrail.png"), 1))))
            return E_FAIL;

        // 찌를 때
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_PiercingTrail"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Trail/PiercingTrail.png"), 1))))
            return E_FAIL;

        // Red가 창 휘두를 때
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_PiercingSwingTrail"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Trail/bullet_pink.png"), 1))))
            return E_FAIL;
        
        // 쫒을 때
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_ChaseTrail"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Trail/ChaseTrail.png"), 1))))
            return E_FAIL;

        // Player01
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Player01Trail"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Trail/bullet_red.png"), 1))))
            return E_FAIL;

        // Player02
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Player02Trail"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Trail/bullet_purple.png"), 1))))
            return E_FAIL;

        // Player03
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Player03Trail"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Trail/bullet_pink.png"), 1))))
            return E_FAIL;

        // Player04
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Player04Trail"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Trail/bullet_green.png"), 1))))
            return E_FAIL;

        // Monster03
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Monster03Trail"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Trail/2161_Trail_02.png"), 1))))
            return E_FAIL;

        // Monster04
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Monster04Trail"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Trail/GW_2301_trail_009.png"), 1))))
            return E_FAIL;


        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_BossEye"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Trail/trail_red.png"), 1))))
            return E_FAIL;
    }

    // Trail Objects
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_TestTrail"), CTestTrail::Create(m_pDevice, m_pContext))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_SwordTrail"), CSwordTrail::Create(m_pDevice, m_pContext))))
            return E_FAIL;
        
        // 쓰고 싶으면 아래 주석 해제 -> LEVEL_GAME playe ReadyLayerEffectChaseTrail 주석 해제, EffectManager 주석 해제
        //if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ChaseTrail"), CSwordTrail::Create(m_pDevice, m_pContext))))
        //    return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_PiercingTrail"), CPiercingTrail::Create(m_pDevice, m_pContext))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_PiercingSwingTrail"), CPiercingSwingTrail::Create(m_pDevice, m_pContext))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), CProjectileTrail::Create(m_pDevice, m_pContext))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_BossEyeTrail"), CBossEyeTrail::Create(m_pDevice, m_pContext))))
            return E_FAIL;








    }

    return S_OK;
}

HRESULT CLoader::LoadLevelGlobalUI()
{
    // TestPanel UI
    {
        //if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_TestPanel"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerInfo/img_bg1.png"), 1))))
        //    return E_FAIL;

        //if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_TestPanel"), CTestPanel::Create(m_pDevice, m_pContext))))
        //    return E_FAIL;
    }



    // GameObj
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_HPVertical"), CUIHPVertical::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }

    if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex_UI"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/shaderFiles/Shader_VtxPosTex_UI.hlsl"), VTX_POSTEX::Elements, VTX_POSTEX::iNumElements))))
        return E_FAIL;


    // PlayerInfoBG
    {
        // Texture
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_PlayerInfoBG"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerInfo/img_bg1.png"), 1))))
            return E_FAIL;

        // GameObj
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_PlayerInfoBG"), CUIPlayerInfoBG::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }


    // PlayerTex
    {
        // Texture
        {
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_PlayerTex"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerInfo/icon_hero2_3.png"), 1))))
                return E_FAIL;
        }

        // GameObj
        {
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_PlayerTex"), CUIPlayerTex::Create(m_pDevice, m_pContext))))
                return E_FAIL;
        }
    }



    // PlayerHPBarBG
    {
        // Texture
        {
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_PlayerHPBarBG"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerInfo/img_hpbar_bg.png"), 1))))
                return E_FAIL;
        }

        // GameObj
        {
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_PlayerHPBarBG"), CUIPlayerHPBarBG::Create(m_pDevice, m_pContext))))
                return E_FAIL;
        }
    }

    // PlayerHPBar
    {
        // Texture
        {
            // if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_PlayerTex"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerInfo/img_hpbar_bg.png"), 1)))) return E_FAIL;
        }

        // GameObj
        {
            // if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_PlayerTex"), CUIPlayerHPBar::Create(m_pDevice, m_pContext)))) return E_FAIL;
        }
    }

    // PlayerFont
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Font_Blue"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerInfo/Bar_Character_SFBar_1.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Font_Green"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerInfo/Bar_Character_HPBar_2.png"), 1))))
            return E_FAIL;

        // Texture
        {
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Font_0"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Numbers/Number_0.png"), 1))))
                return E_FAIL;

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Font_1"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Numbers/Number_1.png"), 1))))
                return E_FAIL;

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Font_2"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Numbers/Number_2.png"), 1))))
                return E_FAIL;

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Font_3"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Numbers/Number_3.png"), 1))))
                return E_FAIL;

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Font_4"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Numbers/Number_4.png"), 1))))
                return E_FAIL;

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Font_5"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Numbers/Number_5.png"), 1))))
                return E_FAIL;

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Font_6"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Numbers/Number_6.png"), 1))))
                return E_FAIL;

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Font_7"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Numbers/Number_7.png"), 1))))
                return E_FAIL;

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Font_8"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Numbers/Number_8.png"), 1))))
                return E_FAIL;

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Font_9"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Numbers/Number_9.png"), 1))))
                return E_FAIL;

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Font_10"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Numbers/Number_10.png"), 1))))
                return E_FAIL;

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Font_11"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Numbers/Number_11.png"), 1))))
                return E_FAIL;
        }


        // GameObj
        {
            if(FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Font"), CUIFont::Create(m_pDevice, m_pContext))))
                return E_FAIL;
        }
    }

    // HPBar
    {

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_HP_Red"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerInfo/Bar_Character_HPBar_1.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_HP_Green"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerInfo/Bar_Character_HPBar_2.png"), 1))))
            return E_FAIL;


        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_HP_Blue"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerInfo/Bar_Character_SFBar_1.png"), 1))))
            return E_FAIL;

        // Obj
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_HPBar"), CUIHPBar::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }





    // UIBG
    {
        // Tex
        {
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_BG_PlayerGunInfo"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerGunInfo/img_weaponbg3.png"), 1))))
                return E_FAIL;

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_BG_PlayerSkill"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerSkill/bg_Normalskill.png"), 1))))
                return E_FAIL;

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_BG_PlayerSkillMid"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerSkill/img_skillbg1.png"), 1))))
                return E_FAIL;

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_BG_MonsterHP"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Monster/bg_monsterHP_normal.png"), 1))))
                return E_FAIL;

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_MonsterHPBG"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Monster/monsterHP.png"), 1))))
                return E_FAIL;

            // VerticalHPLine
            {
                if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_VerticalHP"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Monster/img_growth_line1.png"), 1))))
                    return E_FAIL;
            }
    
            // PlayerSkill Dash
            {
                if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_PlayerSkillDashBG"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerSkill/icon_1310_2.png"), 1))))
                    return E_FAIL;

                if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_PlayerSkillDash"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerSkill/icon_1310_1.png"), 1))))
                    return E_FAIL;
            }

            // PlayerskillMid
            {
                if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_PlayerSkillMid"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerSkillMid/icon_1302_1.png"), 1))))
                    return E_FAIL;
            }

            // MiniMap
            {
                if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_BG_MiniMap"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MiniMap/bg_minimap.png"), 1))))
                    return E_FAIL;

                if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_MiniMapPlayerPos"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MiniMap/icon_own.png"), 1))))
                    return E_FAIL;

                if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_MiniMapMonsterPos"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MiniMap/icon_monster.png"), 1))))
                    return E_FAIL;


                if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_MiniMapBossPos"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MiniMap/Icon_boss1.png"), 1))))
                    return E_FAIL;
            }

            // Boss
            {
                if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_BG_BossHP"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Boss/img_boss_hpbg.png"), 1))))
                    return E_FAIL;

                if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_BossIcon"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Boss/Icon_boss1 #1375523.png"), 1))))
                    return E_FAIL;

                if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_BossHPBG"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Boss/bossHP_2.png"), 1))))
                    return E_FAIL;
            }
        }

        // Obj
        {
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_BG"), CUIBG::Create(m_pDevice, m_pContext))))
                return E_FAIL;
        }
    }

    // UITex
    {
        // Tex Number 123
        {
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_KeyNumber1"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerGunInfo/img_shine1.png"), 1))))
                return E_FAIL;

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_KeyNumber2"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerGunInfo/img_shine2.png"), 1))))
                return E_FAIL;

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_KeyNumber3"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerGunInfo/img_shine3.png"), 1))))
                return E_FAIL;
        }
       

        // Player Weapons
        {

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_1202"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerWeapon/1202.png"), 1))))
                return E_FAIL;

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_1004"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerWeapon/1004.png"), 1))))
                return E_FAIL;

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_1506"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerWeapon/1506.png"), 1))))
                return E_FAIL;

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_1508"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerWeapon/1508.png"), 1))))
                return E_FAIL;
        }

        // PlayerDashEffect
        {
            // HERO_1304_ray_003.png
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_PlayerDashEffect"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerSkill/HERO_1411_ray_002.png"), 1))))
                return E_FAIL;
        }

        // HitEffect
        {
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_PlayerDamageEffect"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerSkill/img_inkwarning.png"), 1))))
                return E_FAIL;
        }

        // PlayerSkillMidAnim
        {
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_PlayerSkillMidAnim"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerSkillMid/ligtning_10.png"), 1))))
                return E_FAIL;
        }

        // Victory
        {
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_Victory"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/victory.png"), 1))))
                return E_FAIL;
        }

        // MiniMap
        {
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_Door"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MiniMap/icon_mark_door.png"), 1))))
                return E_FAIL;

            // Obj
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Tex_Perspective"), CUITexPerspective::Create(m_pDevice, m_pContext))))
                return E_FAIL;
        }
        
        // Obj
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Tex"), CUITex::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }



    // Player Key Font
    {
        if(FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_PlayerKeyShift"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Key/sck303.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_PlayerKeyF"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Key/sck102.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_PlayerKeyE"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Key/sck101.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_PlayerKeyQ"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Key/sck113.png"), 1))))
            return E_FAIL;
    }

    // DamageFont
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_DamageFont"), CUIDamageFont::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }

    // CrossHair
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_CrossHair"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/CrossHair.png"), 1))))
            return E_FAIL;

        // Obj
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_CrossHair"), CUICrossHair::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }


    return S_OK;
}

HRESULT CLoader::LoadLevelGlobalTorch()
{

    {
        // _matrix PreTransformMatrix = XMMatrixRotationX(XMConvertToRadians(0.f));

        _matrix PreTransformMatrix = XMMatrixIdentity();
        vector<_int> Useless = { -1 };
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_Component_Model_Torch"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Torch/Torch.fbx", PreTransformMatrix, Useless, false))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Torch"), CTorch::Create(m_pDevice, m_pContext))))
            return E_FAIL;

        return S_OK;
    }



    return E_NOTIMPL;
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
