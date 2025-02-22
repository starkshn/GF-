#include "pch.h"
#include "Level_GamePlay.h"
#include "GameInstance.h"
#include "Camera_Free.h"


#include "Player.h"
#include "PlayerBody.h"
#include "Model.h"
#include "Transform.h"
#include "WallAABB.h"

#include "ParticleManager.h"
#include "EffectManager.h"
#include "MonsterManager.h"

#ifdef _DEBUG
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#endif

#include "Trigger.h"
#include "SpawnTrigger.h"
#include "Spawner.h"
#include "InterMove.h"
#include "Level_Loading.h"
#include "Effect.h"

#include "ProjectileTrail.h"
#include "EffectMuzzle.h"
#include "BulletHit.h"

#include "TorchFire.h"

// =================================
// UI
// =================================
#include "UIParent.h"
#include "UIPanel.h"
#include "UIElement.h"
#include "UIFont.h"
#include "UIHPBar.h"
#include "UIBG.h"
#include "UITex.h"
#include "UIDamageFont.h"
#include "LoadingColor.h"
#include "UITexPerspective.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{

}

HRESULT CLevel_GamePlay::Initialize()
{
    auto pObj = m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, TEXT("Layer_Loading_UI"));
    pObj->SetActive(false);


    m_pGameInstance->PlayBGM(TEXT("MainBGM.mp3"), 0.2f);

    // 로딩바 준비
    {
        auto pObj = static_cast<CUIObject*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, TEXT("Layer_LoadingBar")));
        pObj->SetActive(false);
    }

    // 로딩바 안의 색 준비
    {
        auto pObj = static_cast<CUIObject*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, TEXT("Layer_LoadingColor")));
        pObj->SetActive(false);
    }

    if (FAILED(ReadyLights()))
        return E_FAIL;

    if (FAILED(ReadyLayerParticlesFromGlobal(TEXT("Layer_Particles"))))
        return E_FAIL;

    if (FAILED(ReadyPlayerInfoUI()))
    {
        MSG_BOX("ReadyPlayerInfoUI Error");
        return E_FAIL;
    }


    if (FAILED(ReadyPlayerGunUI()))
    {
        MSG_BOX("ReadyPlayerGunUI Error");
        return E_FAIL;
    }

    if (FAILED(ReadyTestS()))
    {
        MSG_BOX("ReadyTestS Error");
        return E_FAIL;
    }
    
    if (FAILED(ReadyUIVertical()))
    {
        MSG_BOX("eadyUIVertical Error");
        return E_FAIL;
    }

    if (FAILED(ReadyLayerFromGlobalPlayer(TEXT("A")))) 
        return E_FAIL;

    if (FAILED(ReadyLayerCameraFromGlobalFree(TEXT("B"))))
        return E_FAIL;

    if (FAILED(ReadyShadowCamera()))
        return E_FAIL;

    if (FAILED(ReadyLayerTerrain(TEXT("Layer_Terrain"))))
        return E_FAIL;

    if (FAILED(ReadyLayerFromGlobalMonsters(TEXT("Layer_Monster01"))))
        return E_FAIL;

    if (FAILED(ReadyLayerSkyBox(TEXT("Layer_SkyBox"))))
        return E_FAIL;
    
    if (FAILED(ReadyLayerStages(TEXT("Layer_Stage01"))))
        return E_FAIL;


    if (FAILED(ReadyLayerSpawnTriggersFromGlobal(TEXT("Layer_SpawnTrigger"))))
        return E_FAIL;

    // ==============================================================================================
    // Projectile
    // ==============================================================================================
    {
        if (FAILED(ReadyLayerProjectilesFromGlobal(TEXT("Layer_Projectile"))))
            return E_FAIL;
    }

    // ==============================================================================================
    // Particles
    // ==============================================================================================
    {
        if (FAILED(ReadyLayerParticleWallHitFromGlobal(TEXT("Layer_Particles_WallHit"))))
            return E_FAIL;

        if (FAILED(ReadyLayerParticleMonsterSpawnFromGlobal(TEXT("Layer_Particles_MonsterSpawn"))))
            return E_FAIL;

        if (FAILED(ReadyLayerParticleMonsterHitFromGlobal(TEXT("Layer_Particles_MonsterHit"))))
            return E_FAIL;

        if (FAILED(ReadyLayerParticleProjectileHitFromGlobal(TEXT("Layer_Particles_ProjectileHit"))))
            return E_FAIL;

        if (FAILED(ReadyLayerParticleBossRockFromBlobal(TEXT("Layer_Particles_BossRock"))))
            return E_FAIL;

        if (FAILED(ReadyLayerParticleBossRockDustFromBlobal(TEXT("Layer_Particles_BossRockDust"))))
            return E_FAIL;

        if (FAILED(ReadyLayerParticleBossRockColFromBlobal(TEXT("Layer_Particles_BossRockCol"))))
            return E_FAIL;

        if (FAILED(ReadyLayerParticleBossRockColDustFromBlobal(TEXT("Layer_Particles_BossRockColDust"))))
            return E_FAIL;

        if (FAILED(ReadyLayerParticleBossRockColDustLightingFromBlobal(TEXT("Layer_Particles_BossRockColDustLighting"))))
            return E_FAIL;

        if (FAILED(ReadyLayerParticleBossSmashFromBlobal(TEXT("Layer_Particles_BossSmash"))))
            return E_FAIL;

        if (FAILED(ReadyLayerParticleBossSmashDustFromBlobal(TEXT("Layer_Particles_BossSmashDust"))))
            return E_FAIL;

        if (FAILED(ReadyLayerParticleMonsterDeadFromBlobal(TEXT("Layer_Particles_MonsterDead"))))
            return E_FAIL;

        if (FAILED(ReadyLayerParticleBossWalkFromBlobal(TEXT("Layer_Particles_BossWalk"))))
            return E_FAIL;

        if (FAILED(ReadyLayerParticleFollowLightingDustFromBlobal(TEXT("Layer_Particles_FollowLightingDust"))))
            return E_FAIL;

        if (FAILED(ReadyLayerParticleMonsterChase(TEXT("Layer_Particles_MonsterChase"))))
            return E_FAIL;
    }

    // ==============================================================================================
    // Effects
    // ==============================================================================================
    {
        if (FAILED(ReadyLayerEffectProjectileWallHitFromGlobal(TEXT("Layer_Effect_ProjectileWallHit"))))
            return E_FAIL;

        if (FAILED(ReadyLayerEffectProjectileWallHitFrontFromGlobal(TEXT("Layer_Effect_ProjectileWallHitFront"))))
            return E_FAIL;

        if (FAILED(ReadyLayerEffectProjectileMonsterHitFromGlobal(TEXT("Layer_Effect_ProjectileMonsterHit"))))
            return E_FAIL;

        if (FAILED(ReadyLayerEffectProjectileWallHitSpotFromGlobal(TEXT("Layer_Effect_ProjectileWallHitSpot"))))
            return E_FAIL;

        if (FAILED(ReadyLayerEffectBulletFromGlobal(TEXT("Layer_Effect_Bullet"))))
            return E_FAIL;

        if (FAILED(ReadyLayerEffectBulletTrailFromGlobal(TEXT("Layer_Effect_BulletTrail"))))
            return E_FAIL;

        if (FAILED(ReadyLayerEffectBossRockAlertFromGlobal(TEXT("Layer_Effect_BossRockAlert"))))
            return E_FAIL;

        if (FAILED(ReadyLayerEffectBossRockAlertFinalFromGlobal(TEXT("Layer_Effect_BossRockAlertFinal"))))
            return E_FAIL;


        if (FAILED(ReadyLayerEffectFollowLightingFromGlobal(TEXT("Layer_Effect_FollowLighting"))))
            return E_FAIL;

        if (FAILED(ReadyLayerEffectCircle1004(TEXT("Layer_Effect_Circle1004"))))
            return E_FAIL;

        if (FAILED(ReadyLayerEffectHit1004(TEXT("Layer_Effect_Hit1004"))))
            return E_FAIL;

        if (FAILED(ReadyLayerEffectSphere(TEXT("Layer_Effect_Sphere1004"))))
            return E_FAIL;

        if (FAILED(ReadyLayerEffectTrail(TEXT("Layer_Effect_TestTrail"))))
            return E_FAIL;

        if (FAILED(ReadyLayerEffectSwordTrail(TEXT("Layer_Effect_SwordTrail"))))
            return E_FAIL;

        if (FAILED(ReadyLayerEffectPiercingTrail(TEXT("Layer_Effect_PiercingTrail"))))
            return E_FAIL;

        if (FAILED(ReadyLayerEffectPiercingSwingTrail(TEXT("Layer_Effect_PiercingSwingTrail"))))
            return E_FAIL;

        if (FAILED(ReadyLayerEffectChaseTrail(TEXT("Layer_Effect_ChaseTrail"))))
            return E_FAIL;

        if (FAILED(ReadyLayerEffectMonster03Fire(TEXT("Layer_Effect_Monster03Fire"))))
        {
            MSG_BOX("ReadyLayerEffectMonster03Fire Error");
            return E_FAIL;
        }

        if (FAILED(ReadyLayerEffectMonster04Fire(TEXT("Layer_Effect_Monster04Fire"))))
        {
            MSG_BOX("ReadyLayerEffectMonster04Fire Error");
            return E_FAIL;
        }

        if (FAILED(ReadyLayerEffectPlayer01Trail(TEXT("Layer_Effect_Player01Trail"))))
        {
            MSG_BOX("ReadyLayerEffectPlayer01Trail Error");
            return E_FAIL;
        }

        if (FAILED(ReadyLayerEffectPlayer02Trail(TEXT("Layer_Effect_Player02Trail"))))
        {
            MSG_BOX("ReadyLayerEffectPlayer02Trail Error");
            return E_FAIL;
        }

        if (FAILED(ReadyLayerEffectPlayer03Trail(TEXT("Layer_Effect_Player03Trail"))))
        {
            MSG_BOX("ReadyLayerEffectPlayer03Trail Error");
            return E_FAIL;
        }

        if (FAILED(ReadyLayerEffectPlayer04Trail(TEXT("Layer_Effect_Player04Trail"))))
        {
            MSG_BOX("ReadyLayerEffectPlayer04Trail Error");
            return E_FAIL;
        }

        if (FAILED(ReadyLayerEffectMonster03Trail(TEXT("Layer_Effect_Monster03Trail"))))
        {
            MSG_BOX("ReadyLayerEffectMonster03Trail Error");
            return E_FAIL;
        }

        if (FAILED(ReadyLayerEffectMonster04Trail(TEXT("Layer_Effect_Monster04Trail"))))
        {
            MSG_BOX("ReadyLayerEffectMonster04Trail Error");
            return E_FAIL;
        }

        if (FAILED(ReadyLayerEffectBossEye(TEXT("Layer_Effect_BossEye"))))
        {
            MSG_BOX("ReadyLayerEffectBossEye Error");
            return E_FAIL;
        }

        if (FAILED(ReadyLayerEffectPlayer01muzzle(TEXT("Layer_Effect_Player01Muzzle"))))
        {
            MSG_BOX("ReadyLayerEffectPlayer01muzzle Error");
            return E_FAIL;
        }


        if (FAILED(ReadyLayerEffectPlayer02muzzle(TEXT("Layer_Effect_Player02Muzzle"))))
        {
            MSG_BOX("ReadyLayerEffectPlayer02muzzle Error");
            return E_FAIL;
        }


        if (FAILED(ReadyLayerEffectPlayer03muzzle(TEXT("Layer_Effect_Player03Muzzle"))))
        {
            MSG_BOX("ReadyLayerEffectPlayer03muzzle Error");
            return E_FAIL;
        }


        if (FAILED(ReadyLayerEffectPlayer04muzzle(TEXT("Layer_Effect_Player04Muzzle"))))
        {
            MSG_BOX("ReadyLayerEffectPlayer04muzzle Error");
            return E_FAIL;
        }
        
        if (FAILED(ReadyLayerEffectBulletHit03(TEXT("Layer_Effect_BulletHit03"))))
        {
            MSG_BOX("ReadyLayerEffectBulletHit03 Error");
            return E_FAIL;
        }

        if (FAILED(ReadyLayerEffectBulletHit04(TEXT("Layer_Effect_BulletHit04"))))
        {
            MSG_BOX("ReadyLayerEffectBulletHit04 Error");
            return E_FAIL;
        }

        if (FAILED(ReadyLayerEffectDoor(TEXT("Layer_Effect_Door"))))
        {
            MSG_BOX("ReadyLayerEffectBulletHit04 Error");
            return E_FAIL;
        }

        if (FAILED(ReadyLayerEffectRoarNoise(TEXT("Layer_Effect_RoarNoise"))))
        {
            MSG_BOX("ReadyLayerEffectRoarNoise Error");
            return E_FAIL;
        }
    }

    // UI
    {
        if (FAILED(ReadyTestPanel(TEXT("Layer_UI_TestPanel"))))
        {
            MSG_BOX("Layer_UI_TestPanel Error");
            return E_FAIL;
        }
        

        if (FAILED(ReadyMiniMap()))
        {
            MSG_BOX("ReadyMiniMap Error");
            return E_FAIL;
        }
        
        if (FAILED(ReadyPlayerSkillEffectUI()))
        {
            MSG_BOX("ReadyPlayerSkillEffectUI Error");
            return E_FAIL;
        }

        if (FAILED(ReadyPlayerKeyUI()))
        {
            MSG_BOX("ReadyPlayerKeyUI Error");
            return E_FAIL;
        }

        if (FAILED(ReadyDamageUIFont()))
        {
            MSG_BOX("ReadyDamageUIFont Error");
            return E_FAIL;
        }

        if (FAILED(ReadyDoorUI()))
        {
            MSG_BOX("ReadyDoorUI Error");
            return E_FAIL;
        }
    }

    if (FAILED(LoadAABB()))
        return E_FAIL;

    if (FAILED(LoadSpawnTriggers()))
        return E_FAIL;

    if (FAILED(LoadInterMove()))
        return E_FAIL;

    if (FAILED(ReadyManagers()))
        return E_FAIL;

    if (FAILED(LoadPointLights()))
        return E_FAIL;
    
    if (FAILED(LoadUITrigger()))
        return E_FAIL;
    
#ifdef _DEBUG
    
    m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GAMEPLAY, LEVEL_GAMEPLAY, TEXT("A")));
    m_pPlayerTransformCom = static_cast<CTransform*>(m_pPlayer->GetComponent(TEXT("Com_Transform")));

    if (nullptr != m_pPlayer)
    {
        CModel* pModel = m_pPlayer->GetModelByModelName("1201_Right");
        if (nullptr != pModel) m_pPlayerModels.push_back(pModel);

        pModel = m_pPlayer->GetModelByModelName("1004_Right");
        if (nullptr != pModel) m_pPlayerModels.push_back(pModel);

        pModel = m_pPlayer->GetModelByModelName("1503_Right");
        if (nullptr != pModel) m_pPlayerModels.push_back(pModel);

        pModel = m_pPlayer->GetModelByModelName("1058_Right");
        if (nullptr != pModel) m_pPlayerModels.push_back(pModel);

        // TODO 다른 모델들도 추가
    }

    if (nullptr == m_pPlayerTransformCom)
        return E_FAIL;

    m_pCurCamera = dynamic_cast<CCamera_Free*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GAMEPLAY, LEVEL_GAMEPLAY, TEXT("B")));
    if (nullptr == m_pCurCamera)
        return E_FAIL;

    InitializeImgui();

#endif

    return S_OK;
}

void CLevel_GamePlay::Update(const _float& fTimeDelta)
{
    CParticleManager::GetInstance()->Update(fTimeDelta);
    CEffectManager::GetInstance()->Update(fTimeDelta);
    CMonsterManager::GetInstance()->Update(fTimeDelta);

    static _bool temp = false;
    if (GetKeyState(VK_RETURN) & 0x8000)
    {
        auto pObj = m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, TEXT("Layer_Loading_UI"));
        pObj->SetActive(true);
        m_pGameInstance->StopBGM();

        {
            {
                auto pObj = static_cast<CUIObject*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, TEXT("Layer_LoadingBar")));
                pObj->SetActive(true);
            }

            {
                auto pObj = static_cast<CUIObject*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, TEXT("Layer_LoadingColor")));
                pObj->SetActive(true);
            }
        }

        m_pGameInstance->RenderOffPlayerUI();

        CMonsterManager::GetInstance()->Clear();
        CEffectManager::GetInstance()->DeActiveEffect(CEffectManager::EFFECT_DOOR);

        m_pGameInstance->OpenLevel(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_BOSS));

        temp = true;


    }
    else if (m_bMoveBossLevel)
    {
        auto pObj = m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, TEXT("Layer_Loading_UI"));
        pObj->SetActive(true);
        m_pGameInstance->StopBGM();


        {
            {
                auto pObj = static_cast<CUIObject*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, TEXT("Layer_LoadingBar")));
                pObj->SetActive(true);
            }

            {
                auto pObj = static_cast<CUIObject*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, TEXT("Layer_LoadingColor")));
                pObj->SetActive(true);

                static_cast<CLoadingColor*>(pObj)->SetTotLoadCnt(0);
                static_cast<CLoadingColor*>(pObj)->SetcurLoadCnt(0);
            }
        }




        m_pGameInstance->RenderOffPlayerUI();


        CMonsterManager::GetInstance()->Clear();
        CEffectManager::GetInstance()->DeActiveEffect(CEffectManager::EFFECT_DOOR);

        m_pGameInstance->OpenLevel(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_BOSS));
        temp = true;


    }

#ifdef  _DEBUG

    if (false == temp)
    {
        if (m_bCameraAttach)
        {
            m_pGameInstance->GetTransformMatrix(CPipeLine::DTS::D3DTS_VIEW);
        }

        if (m_bPlayerAttach)
        {
            m_pPlayerTransformCom->SetTransformState(CTransform::STATE_POS, XMLoadFloat4(m_pGameInstance->GetCameraPosition()));
            m_pPlayerTransformCom->SetTransformState(CTransform::STATE_LOOK, XMLoadFloat4(m_pGameInstance->GetCameraState(Engine::CAMERA_STATE::LOOK)));
            m_pPlayerTransformCom->SetTransformState(CTransform::STATE_UP, XMLoadFloat4(m_pGameInstance->GetCameraState(Engine::CAMERA_STATE::UP)));
            m_pPlayerTransformCom->SetTransformState(CTransform::STATE_RIGHT, XMLoadFloat4(m_pGameInstance->GetCameraState(Engine::CAMERA_STATE::RIGHT)));
        }
    }
   

#endif


}

HRESULT CLevel_GamePlay::Render()
{


#ifdef _DEBUG
    SetWindowText(g_hWnd, TEXT("게임 플레이 레벨입니다."));

    RenderImgui();

#endif

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLights()
{
    LIGHT_DESC	LightDesc{};


    ZeroMemory(&LightDesc, sizeof LightDesc);

    LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
    LightDesc.vDirection = _float4{ 1.f, -1.f, 1.f, 0.f };
    LightDesc.vDiffuse = _float4(0.7f, 0.7f, 0.7f, 1.f);
    LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
    LightDesc.vSpecular = _float4(0.1f, 0.1f, 0.1f, 0.0f);

    if (FAILED(m_pGameInstance->AddLight(LightDesc)))
        return E_FAIL;
    

   /* ZeroMemory(&LightDesc, sizeof LightDesc);

    LightDesc.eType = LIGHT_DESC::TYPE_POINT;
    LightDesc.vPosition = _float4(0.f, 3.f, 0.f, 0.f);
    LightDesc.fRange = 3.f;
    LightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
    LightDesc.vAmbient = _float4(0.4f, 0.1f, 0.1f, 1.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;

    if (FAILED(m_pGameInstance->AddLight(LightDesc)))
        return E_FAIL;

    ZeroMemory(&LightDesc, sizeof LightDesc);

    LightDesc.eType = LIGHT_DESC::TYPE_POINT;
    LightDesc.vPosition = _float4(-5.f, 3.f, 0.f, 0.f);
    LightDesc.fRange = 3.f;
    LightDesc.vDiffuse = _float4(0.f, 1.f, 0.f, 1.f);
    LightDesc.vAmbient = _float4(0.1f, 0.4f, 0.1f, 1.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;

    if (FAILED(m_pGameInstance->AddLight(LightDesc)))
        return E_FAIL;*/





    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerFromGlobalPlayer(const _wstring& strLayerTag)
{
    CContainerObject::CONTAINER_OBJECT_DESC Desc = {};
    Desc.iNumPartObjects = CStateObjectContainer::PARTOBJECT::PART_END;
    Desc.fSpeedPerSec = 5.f;
    Desc.fRotSpeedPerSec = XMConvertToRadians(90.f);
    Desc.strObjectTag = "Player";
    Desc.iLevelIndex = LEVEL_GAMEPLAY;


    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Player"), LEVEL_GAMEPLAY, strLayerTag, &Desc))) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerStages(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Stage01"), LEVEL_GAMEPLAY, strLayerTag)))  return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerTerrain(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Terrain"), LEVEL_GAMEPLAY, strLayerTag, nullptr))) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerCameraFromGlobalFree(const _wstring& strLayerTag)
{
    CCamera_Free::CAMERA_FREE_DESC		Desc{};

    Desc.fSpeedPerSec       = 10.f;
    Desc.fRotSpeedPerSec    = XMConvertToRadians(180.f);
    Desc.fMouseSensor       = 0.1f;
    Desc.fFovY              = XMConvertToRadians(60.f);
    Desc.fAspectRatio       = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;

    // near의 값은 0이 되면 안된다. 0보다큰 값으로 설정해야 원근 투영시 제대로 계산된다. 241128
    Desc.fNear              = 0.1f;
    Desc.fFar               = 1000.f;
    Desc.vEye               = _float3(0.f, 2.f, -7.f);
    Desc.vAt                = _float3(0.f, 0.f, 0.f);
    Desc.iLevelIndex        = LEVEL_GAMEPLAY;


    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Camera_Free"), LEVEL_GAMEPLAY, strLayerTag, &Desc))) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyShadowCamera()
{
    m_pGameInstance->SetUpShadowTransformMatrix(CShadow::D3DTS_VIEW, XMMatrixLookAtLH(XMVectorSet(-30.f, 50.f, 20.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));

    m_pGameInstance->SetUpShadowTransformMatrix(CShadow::D3DTS_PROJ, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), static_cast<_float>(g_iWinSizeX) / g_iWinSizeY, 0.1f, 1000.f));

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerFromGlobalMonsters(const _wstring& strLayerTag)
{
    // if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Monster01"), LEVEL_GAMEPLAY, strLayerTag, nullptr))) return E_FAIL;
    // if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Monster02"), LEVEL_GAMEPLAY, strLayerTag, nullptr))) return E_FAIL;
    // if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Monster03"), LEVEL_GAMEPLAY, strLayerTag, nullptr))) return E_FAIL;
    // if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Monster04"), LEVEL_GAMEPLAY, strLayerTag, nullptr))) return E_FAIL;
    // if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Monster05"), LEVEL_GAMEPLAY, strLayerTag, nullptr))) return E_FAIL;
    // if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Monster06"), LEVEL_GAMEPLAY, strLayerTag, nullptr))) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerSkyBox(const _wstring& strLayerTag)
{
    CGameObject::GAMEOBJECT_DESC Desc = {};
    Desc.iLevelIndex = LEVEL_GAMEPLAY;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_SkyBox"), LEVEL_GAMEPLAY, strLayerTag, &Desc)))
        return E_FAIL;
    
    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerSpawnTriggersFromGlobal(const _wstring& strLayerTag)
{
    // if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_SpawnTrigger"), LEVEL_GAMEPLAY, strLayerTag, nullptr))) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerParticlesFromGlobal(const _wstring& strLayerTag)
{
    // PLAYER GUN FIRE
    {
        CGameObject::GAMEOBJECT_DESC Desc = {};
        Desc.strObjectTag = "PlayerGunFire";

        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_PlayerGunFire"), LEVEL_GLOBAL, strLayerTag, &Desc)))
            return E_FAIL;
    }

    // MONSTER HIT
    {
        CGameObject::GAMEOBJECT_DESC Desc = {};
        Desc.strObjectTag = "MonsterHit";

        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), LEVEL_GLOBAL, strLayerTag, &Desc)))
            return E_FAIL;
    }
    
    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerParticleWallHitFromGlobal(const _wstring& strLayerTag)
{
    {
        CGameObject::GAMEOBJECT_DESC Desc = {};
        Desc.strObjectTag = "WallHit";

        // 30
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_WallHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_WallHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_WallHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_WallHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_WallHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_WallHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_WallHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_WallHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_WallHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_WallHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;

        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_WallHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_WallHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_WallHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_WallHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_WallHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_WallHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_WallHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_WallHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_WallHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_WallHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;

        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_WallHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_WallHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_WallHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_WallHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_WallHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_WallHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_WallHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_WallHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_WallHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_WallHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    }

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerParticleMonsterSpawnFromGlobal(const _wstring& strLayerTag)
{
    {
        CGameObject::GAMEOBJECT_DESC Desc = {};
        Desc.strObjectTag = "ParticleMonsterSpawn";

        // 20
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterSpawn"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterSpawn"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterSpawn"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterSpawn"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterSpawn"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterSpawn"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterSpawn"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterSpawn"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterSpawn"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterSpawn"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;

        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterSpawn"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterSpawn"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterSpawn"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterSpawn"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterSpawn"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterSpawn"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterSpawn"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterSpawn"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterSpawn"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterSpawn"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    }

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerParticleMonsterHitFromGlobal(const _wstring& strLayerTag)
{
    {
        CGameObject::GAMEOBJECT_DESC Desc = {};
        Desc.strObjectTag = "ParticleMonsterHit";

        // 30
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;

        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;

        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;

    }

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerParticleProjectileHitFromGlobal(const _wstring& strLayerTag)
{
    CGameObject::GAMEOBJECT_DESC Desc = {};
    Desc.strObjectTag = "ParticleProjectileHit";

    // 50
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_ProjectileHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerParticleBossRockFromBlobal(const _wstring& strLayerTag)
{
    CGameObject::GAMEOBJECT_DESC Desc = {};
    Desc.strObjectTag = "ParticleBossRock";

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRock"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerParticleBossRockDustFromBlobal(const _wstring& strLayerTag)
{
    CGameObject::GAMEOBJECT_DESC Desc = {};
    Desc.strObjectTag = "ParticleBossRockDust";

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockDust"), LEVEL_GLOBAL, strLayerTag, &Desc))) 
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerParticleBossRockColFromBlobal(const _wstring& strLayerTag)
{
    CGameObject::GAMEOBJECT_DESC Desc = {};
    Desc.strObjectTag = "ParticleBossRockCol";

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockCol"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockCol"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockCol"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockCol"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockCol"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockCol"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockCol"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockCol"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockCol"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockCol"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockCol"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;


    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerParticleBossRockColDustFromBlobal(const _wstring& strLayerTag)
{
    CGameObject::GAMEOBJECT_DESC Desc = {};
    Desc.strObjectTag = "ParticleBossRockColDust";

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockColDust"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockColDust"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockColDust"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockColDust"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockColDust"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockColDust"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockColDust"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockColDust"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockColDust"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockColDust"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockColDust"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;


    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerParticleBossRockColDustLightingFromBlobal(const _wstring& strLayerTag)
{
    CGameObject::GAMEOBJECT_DESC Desc = {};
    Desc.strObjectTag = "ParticleBossRockColDustLighting";

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockColDustLighting"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockColDustLighting"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockColDustLighting"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockColDustLighting"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockColDustLighting"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockColDustLighting"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockColDustLighting"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockColDustLighting"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockColDustLighting"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockColDustLighting"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossRockColDustLighting"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;


    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerParticleBossSmashFromBlobal(const _wstring& strLayerTag)
{
    CGameObject::GAMEOBJECT_DESC Desc = {};
    Desc.strObjectTag = "ParticleBossSmash";

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossSmash"), LEVEL_GLOBAL, strLayerTag, &Desc))) 
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerParticleBossSmashDustFromBlobal(const _wstring& strLayerTag)
{
    CGameObject::GAMEOBJECT_DESC Desc = {};
    Desc.strObjectTag = "ParticleBossSmashDust";

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossSmashDust"), LEVEL_GLOBAL, strLayerTag, &Desc)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossSmashDust"), LEVEL_GLOBAL, strLayerTag, &Desc)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossSmashDust"), LEVEL_GLOBAL, strLayerTag, &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerParticleBossWalkFromBlobal(const _wstring& strLayerTag)
{
    CGameObject::GAMEOBJECT_DESC Desc = {};
    Desc.strObjectTag = "ParticleBossWalk";

    // 10개
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_BossWalk"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerParticleMonsterDeadFromBlobal(const _wstring& strLayerTag)
{
    CGameObject::GAMEOBJECT_DESC Desc = {};
    Desc.strObjectTag = "ParticleMonsterDead";

    // 10개
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterDead"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterDead"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterDead"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterDead"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterDead"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterDead"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterDead"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterDead"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterDead"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterDead"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerParticleFollowLightingDustFromBlobal(const _wstring& strLayerTag)
{
    CGameObject::GAMEOBJECT_DESC Desc = {};
    Desc.strObjectTag = "ParticleFollowLightingDust";

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_FollowLightingDust"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_FollowLightingDust"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_FollowLightingDust"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_FollowLightingDust"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_FollowLightingDust"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_FollowLightingDust"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_FollowLightingDust"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_FollowLightingDust"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_FollowLightingDust"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_FollowLightingDust"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_FollowLightingDust"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_FollowLightingDust"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_FollowLightingDust"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_FollowLightingDust"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_FollowLightingDust"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_FollowLightingDust"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_FollowLightingDust"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_FollowLightingDust"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_FollowLightingDust"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_FollowLightingDust"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerParticleMonsterChase(const _wstring& strLayerTag)
{
    CGameObject::GAMEOBJECT_DESC Desc = {};
    Desc.strObjectTag = "ParticleMonsterChase";

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterChase"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterChase"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterChase"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterChase"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterChase"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterChase"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterChase"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterChase"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterChase"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterChase"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterChase"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterChase"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterChase"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterChase"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterChase"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterChase"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterChase"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterChase"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterChase"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterChase"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterChase"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterChase"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterChase"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterChase"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterChase"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterChase"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterChase"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterChase"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterChase"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterChase"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectProjectileWallHitFromGlobal(const _wstring& strLayerTag)
{
    CEffect::EFFECT_DESC Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 0.25f;
    Desc.strObjectTag = "EffectProjectileWallHit";
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;
    
    // 30
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;


    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectProjectileWallHitFrontFromGlobal(const _wstring& strLayerTag)
{
    {
        CEffect::EFFECT_DESC Desc = {};
        Desc.bUseFrameEffect = false;
        Desc.fMaxLifeTime = 0.15f;
        Desc.strObjectTag = "EffectProjectileWallHitFront";
        Desc.fSpeedPerSec = 0.f;
        Desc.fRotSpeedPerSec = 0.f;
        Desc.bUseSpawnPos = false;
        Desc.iLevelIndex = LEVEL_GLOBAL;

        // 30
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitFront"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitFront"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitFront"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitFront"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitFront"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitFront"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitFront"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitFront"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitFront"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitFront"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;

        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitFront"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitFront"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitFront"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitFront"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitFront"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitFront"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitFront"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitFront"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitFront"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitFront"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;

        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitFront"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitFront"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitFront"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitFront"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitFront"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitFront"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitFront"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitFront"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitFront"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitFront"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    }

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectProjectileWallHitSpotFromGlobal(const _wstring& strLayerTag)
{
    CEffect::EFFECT_DESC Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 1.5f;
    Desc.strObjectTag = "EffectProjectileWallHitSpot";
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileWallHitSpot"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;



    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectProjectileMonsterHitFromGlobal(const _wstring& strLayerTag)
{
    CEffect::EFFECT_DESC Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 0.25f;
    Desc.strObjectTag = "EffectProjectileMonsterHit";
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileMonsterHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileMonsterHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileMonsterHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileMonsterHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileMonsterHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileMonsterHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileMonsterHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileMonsterHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileMonsterHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileMonsterHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileMonsterHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileMonsterHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileMonsterHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileMonsterHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileMonsterHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileMonsterHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileMonsterHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileMonsterHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileMonsterHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_ProjectileMonsterHit"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectBulletFromGlobal(const _wstring& strLayerTag)
{
    CEffect::EFFECT_DESC Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 5.f;
    Desc.strObjectTag = "EffectBullet";
    Desc.fSpeedPerSec = 80.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;


    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;


    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;


    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;


    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Bullet"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectBulletTrailFromGlobal(const _wstring& strLayerTag)
{
    CEffect::EFFECT_DESC Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 5.f;
    Desc.strObjectTag = "EffectBulletTrail";
    Desc.fSpeedPerSec = 60.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;


    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectBossRockAlertFromGlobal(const _wstring& strLayerTag)
{
    CEffect::EFFECT_DESC Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 3.5f;
    Desc.strObjectTag = "EffectBossRockAlert";
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BossRockAlert"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BossRockAlert"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BossRockAlert"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectBossRockAlertFinalFromGlobal(const _wstring& strLayerTag)
{
    CEffect::EFFECT_DESC Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 3.5f;
    Desc.strObjectTag = "EffectBossRockAlertFinal";
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BossRockAlertFinal"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BossRockAlertFinal"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BossRockAlertFinal"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectFollowLightingFromGlobal(const _wstring& strLayerTag)
{
    CEffect::EFFECT_DESC Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 1.5f;
    Desc.strObjectTag = "EffectFollowLighting";
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;
    
    // 20개
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_FollowLighting"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_FollowLighting"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_FollowLighting"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_FollowLighting"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_FollowLighting"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_FollowLighting"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_FollowLighting"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_FollowLighting"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_FollowLighting"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_FollowLighting"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_FollowLighting"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_FollowLighting"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_FollowLighting"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_FollowLighting"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_FollowLighting"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_FollowLighting"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_FollowLighting"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_FollowLighting"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_FollowLighting"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_FollowLighting"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectTrail(const _wstring& strLayerTag)
{
    CEffect::EFFECT_DESC Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 1.5f;
    Desc.strObjectTag = "EffectTestTrail";
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_TestTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectSwordTrail(const _wstring& strLayerTag)
{
    CEffect::EFFECT_DESC Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 0.5f;
    Desc.strObjectTag = "EffectSwordTrail";
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;

    // Sword
    {
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_SwordTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_SwordTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_SwordTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_SwordTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_SwordTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_SwordTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_SwordTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_SwordTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_SwordTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_SwordTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    }

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectPiercingTrail(const _wstring& strLayerTag)
{
    CEffect::EFFECT_DESC Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 0.5f;
    Desc.strObjectTag = "EffectPiercingTrail";
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;


    {
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_PiercingTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_PiercingTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_PiercingTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_PiercingTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_PiercingTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_PiercingTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_PiercingTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_PiercingTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_PiercingTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_PiercingTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;

    }

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectPiercingSwingTrail(const _wstring& strLayerTag)
{
    CEffect::EFFECT_DESC Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 3.f;
    Desc.strObjectTag = "EffectPiercingSwingTrail";
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;

    {
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_PiercingSwingTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_PiercingSwingTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_PiercingSwingTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_PiercingSwingTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_PiercingSwingTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_PiercingSwingTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_PiercingSwingTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_PiercingSwingTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_PiercingSwingTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_PiercingSwingTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    }

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectChaseTrail(const _wstring& strLayerTag)
{
    /*CEffect::EFFECT_DESC Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 1.5f;
    Desc.strObjectTag = "EffectChaseTrail";
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;

    {
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ChaseTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ChaseTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ChaseTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ChaseTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ChaseTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ChaseTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ChaseTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ChaseTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ChaseTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ChaseTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    }*/


    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectBossEye(const _wstring& starLayerTag)
{
    CEffect::EFFECT_DESC Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 999999.f;
    Desc.strObjectTag = "EffectBossEyeTrail";
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_BossEyeTrail"), Desc.iLevelIndex, starLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_BossEyeTrail"), Desc.iLevelIndex, starLayerTag, &Desc))) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectMonster03Fire(const _wstring& strLayerTag)
{
    CEffect::EFFECT_DESC Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 0.3f;
    Desc.strObjectTag = "EffectMonster03Fire";
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster03Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster03Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster03Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster03Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster03Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster03Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster03Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster03Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster03Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster03Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster03Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster03Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster03Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster03Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster03Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster03Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster03Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster03Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster03Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster03Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;


    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectMonster04Fire(const _wstring& strLayerTag)
{
    CEffect::EFFECT_DESC Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 0.2f;
    Desc.strObjectTag = "EffectMonster04Fire";
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster04Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster04Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster04Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster04Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster04Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster04Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster04Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster04Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster04Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster04Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;


    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster04Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster04Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster04Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster04Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster04Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;


    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster04Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster04Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster04Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster04Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Monster04Fire"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectPlayer01Trail(const _wstring& strLayerTag)
{
    CProjectileTrail::PROJECTILE_TRAIL_DESC  Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 2.f;
    Desc.strObjectTag = "EffectPlayer01Trail";
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;
    Desc.eTrailType = PROJECTILE_TRAIL_TYPE::PLAYER01;


    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectPlayer02Trail(const _wstring& strLayerTag)
{
    CProjectileTrail::PROJECTILE_TRAIL_DESC  Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 2.f;
    Desc.strObjectTag = "EffectPlayer02Trail";
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;
    Desc.eTrailType = PROJECTILE_TRAIL_TYPE::PLAYER02;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectPlayer03Trail(const _wstring& strLayerTag)
{
    CProjectileTrail::PROJECTILE_TRAIL_DESC  Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 2.f;
    Desc.strObjectTag = "EffectPlayer03Trail";
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;
    Desc.eTrailType = PROJECTILE_TRAIL_TYPE::PLAYER03;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectPlayer04Trail(const _wstring& strLayerTag)
{
    CProjectileTrail::PROJECTILE_TRAIL_DESC  Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 2.f;
    Desc.strObjectTag = "EffectPlayer04Trail";
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;
    Desc.eTrailType = PROJECTILE_TRAIL_TYPE::PLAYER04;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectMonster03Trail(const _wstring& strLayerTag)
{
    CProjectileTrail::PROJECTILE_TRAIL_DESC  Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 2.f;
    Desc.strObjectTag = "EffectMonster03Trail";
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;

    Desc.eTrailType = PROJECTILE_TRAIL_TYPE::MONSTER03;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectMonster04Trail(const _wstring& strLayerTag)
{
    CProjectileTrail::PROJECTILE_TRAIL_DESC  Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 2.f;
    Desc.strObjectTag = "EffectMonster04Trail";
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;
    Desc.eTrailType = PROJECTILE_TRAIL_TYPE::MONSTER04;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_ProjectileTrail"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectPlayer01muzzle(const _wstring& strLayerTag)
{
    CEffectMuzzle::MUZZLE_EFFECT_DESC Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 0.1f;
    Desc.strObjectTag = "EffectMuzzlePlayer01";
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;
    Desc.eMuzzleType = MUZZLE_EFFECT_TYPE::PLAYER01;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Muzzle"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Muzzle"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Muzzle"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Muzzle"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Muzzle"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectPlayer02muzzle(const _wstring& strLayerTag)
{
    CEffectMuzzle::MUZZLE_EFFECT_DESC Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 0.1f;
    Desc.strObjectTag = "EffectMuzzlePlayer02";
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;
    Desc.eMuzzleType = MUZZLE_EFFECT_TYPE::PLAYER02;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Muzzle"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Muzzle"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Muzzle"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Muzzle"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Muzzle"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectPlayer03muzzle(const _wstring& strLayerTag)
{
    CEffectMuzzle::MUZZLE_EFFECT_DESC Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 0.1f;
    Desc.strObjectTag = "EffectMuzzlePlayer03";
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;
    Desc.eMuzzleType = MUZZLE_EFFECT_TYPE::PLAYER03;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Muzzle"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Muzzle"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Muzzle"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Muzzle"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Muzzle"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectPlayer04muzzle(const _wstring& strLayerTag)
{
    CEffectMuzzle::MUZZLE_EFFECT_DESC Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 0.1f;
    Desc.strObjectTag = "EffectMuzzlePlayer04";
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;
    Desc.eMuzzleType = MUZZLE_EFFECT_TYPE::PLAYER04;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Muzzle"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Muzzle"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Muzzle"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Muzzle"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Muzzle"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectBulletHit03(const _wstring& strLayerTag)
{
    CBulletHit::BULLET_HIT_DESC  Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 0.3f;
    Desc.strObjectTag = "EffectBulletHit03";
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;
    Desc.eHitType = BULLET_HIT_TYPE::PLAYER03;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;


    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectBulletHit04(const _wstring& strLayerTag)
{
    CBulletHit::BULLET_HIT_DESC  Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 0.20f;
    Desc.strObjectTag = "EffectBulletHit04";
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;
    Desc.eHitType = BULLET_HIT_TYPE::PLAYER04;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_BulletHit"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;


    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectCircle1004(const _wstring& strLayerTag)
{

    CEffect::EFFECT_DESC Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 0.25f;
    Desc.strObjectTag = "EffectCircle1004";
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;

    // 20개
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Circle1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Circle1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Circle1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Circle1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Circle1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Circle1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Circle1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Circle1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Circle1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Circle1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Circle1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Circle1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Circle1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Circle1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Circle1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Circle1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Circle1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Circle1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Circle1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Circle1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectHit1004(const _wstring& strLayerTag)
{
    CEffect::EFFECT_DESC Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 0.25f;
    Desc.strObjectTag = "EffectHit1004";
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;

    // 20개
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Hit1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Hit1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Hit1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Hit1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Hit1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Hit1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Hit1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Hit1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Hit1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Hit1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Hit1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Hit1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Hit1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Hit1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Hit1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Hit1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Hit1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Hit1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Hit1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_Hit1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectSphere(const _wstring& strLayerTag)
{
    CEffect::EFFECT_DESC Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 0.25f;
    Desc.strObjectTag = "EffectSphere1004";
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_EffectSphere1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_EffectSphere1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_EffectSphere1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_EffectSphere1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_EffectSphere1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_EffectSphere1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_EffectSphere1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_EffectSphere1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_EffectSphere1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_EffectSphere1004"), Desc.iLevelIndex, strLayerTag, &Desc))) return E_FAIL;



    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectDoor(const _wstring& strLayerTag)
{
    CEffect::EFFECT_DESC Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 2.f;
    Desc.strObjectTag = "EffectDoor";
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_EffectDoor"), Desc.iLevelIndex, strLayerTag, &Desc))) 
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerEffectRoarNoise(const _wstring& strLayerTag)
{
    CEffect::EFFECT_DESC Desc = {};
    Desc.bUseFrameEffect = false;
    Desc.fMaxLifeTime = 1.75f;
    Desc.strObjectTag = "EffectRoarNoise";
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.iLevelIndex = LEVEL_GLOBAL;
    
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_RoarNoise"), Desc.iLevelIndex, strLayerTag, &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyLayerProjectilesFromGlobal(const _wstring& strLayerTag)
{
    // if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_PlayerBullet"), LEVEL_GAMEPLAY, strLayerTag, nullptr))) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyManagers()
{
    if (FAILED(CParticleManager::GetInstance()->Initialize()))
        return E_FAIL;

    if (FAILED(CEffectManager::GetInstance()->Initialize()))
        return E_FAIL;

    if (FAILED(CMonsterManager::GetInstance()->Initialize()))
        return E_FAIL;

    return S_OK;
}

// Layer : Layer_UI_TestPanel
HRESULT CLevel_GamePlay::ReadyTestPanel(const _wstring& strLayerTag)
{
    //CUIParent::UI_DEFAULT_INFO Desc = {};
    //Desc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
    //Desc.eUIType = UI_TYPE::UI_PANEL;

    //Desc.fSSizeX = 400;
    //Desc.fSSizeY = 100;

    //Desc.fSX = (Desc.fSSizeX * 0.5f);
    //Desc.fSY = g_iWinSizeY - Desc.fSSizeY;

    //Desc.bUseSpawnPos = false;
    //Desc.fSpeedPerSec = 0.f;
    //Desc.fSpeedPerSec = 0.f;
    //Desc.strObjectTag = "TestPanel";

    //if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_TestPanel"), LEVEL_GAMEPLAY, strLayerTag, &Desc))) return E_FAIL;
    //CUIPanel* pObj = static_cast<CUIPanel*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GAMEPLAY, TEXT("Layer_UI_TestPanel")));
    //if (pObj) m_pGameInstance->AddPanelUIToManager(Desc.strObjectTag, pObj);
    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyPlayerInfoUI()
{
    // PlayerInfoBG
    _wstring strLayerTag = TEXT("Layer_UI_PlayerInfoBG");

    CUIParent::UI_DEFAULT_INFO PanelDesc = {};
    PanelDesc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
    PanelDesc.eUIType = UI_TYPE::UI_PANEL;
    PanelDesc.fSSizeX = 500;
    PanelDesc.fSSizeY = 120;
    PanelDesc.fSX = (PanelDesc.fSSizeX * 0.5f);
    PanelDesc.fSY = g_iWinSizeY - PanelDesc.fSSizeY + 50.f;
    PanelDesc.bUseSpawnPos = false;
    PanelDesc.fSpeedPerSec = 0.f;
    PanelDesc.fSpeedPerSec = 0.f;
    PanelDesc.strObjectTag = "UIPlayerInfoBG";
    PanelDesc.iDepth = 5;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_PlayerInfoBG"), LEVEL_GLOBAL, strLayerTag, &PanelDesc))) return E_FAIL;
    CUIPanel* pObj = static_cast<CUIPanel*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
    if (pObj)
        m_pGameInstance->AddPanelUIToManager(PanelDesc.strObjectTag, pObj);
    else
        return E_FAIL;

    m_pGameInstance->AddPlayerUI(pObj);

    // 250, 600

    // PlayerTex
    {
        _wstring strLayerTag = TEXT("Layer_UI_PlayerTex");

        CUIParent::UI_DEFAULT_INFO Desc = {};
        Desc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
        Desc.eUIType = UI_TYPE::UI_TEX;
        Desc.fSSizeX = 200;
        Desc.fSSizeY = 200;
        Desc.fSX = 0;
        Desc.fSY = 0;
        Desc.bUseSpawnPos = false;
        Desc.fSpeedPerSec = 0.f;
        Desc.fSpeedPerSec = 0.f;
        Desc.strObjectTag = "UIPlayerTex";
        Desc.iDepth = 0;

        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_PlayerTex"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
        if (pObj)
            m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
        else
            return E_FAIL;

        m_pGameInstance->AddPlayerUI(pObj);


        m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, -Desc.fSSizeX * 0.5f - 50.f, -70.f);
    }

    // PlayerHPBarBGBlue
    {
        _wstring strLayerTag = TEXT("Layer_UI_PlayerBPBarBG");
        CUIParent::UI_DEFAULT_INFO Desc = {};
        Desc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
        Desc.eUIType = UI_TYPE::UI_PANEL;
        Desc.fSSizeX = 230;
        Desc.fSSizeY = 25;
        Desc.fSX = 0;
        Desc.fSY = 0;
        Desc.bUseSpawnPos = false;
        Desc.fSpeedPerSec = 0.f;
        Desc.fSpeedPerSec = 0.f;
        Desc.strObjectTag = "UIPlayerBPBarBGBlue";
        Desc.iDepth = 1;

        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_PlayerHPBarBG"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
        if (pObj)
            m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
        else
            return E_FAIL;
        m_pGameInstance->AddPlayerUI(pObj);


        m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, 65.f, -15.f);
    }

    // PlayerHPBarBGGreen
    {
        _wstring strLayerTag = TEXT("Layer_UI_PlayerBPBarBG");
        CUIParent::UI_DEFAULT_INFO Desc = {};
        Desc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
        Desc.eUIType = UI_TYPE::UI_PANEL;
        Desc.fSSizeX = 230;
        Desc.fSSizeY = 25;
        Desc.fSX = 0;
        Desc.fSY = 0;
        Desc.bUseSpawnPos = false;
        Desc.fSpeedPerSec = 0.f;
        Desc.fSpeedPerSec = 0.f;
        Desc.strObjectTag = "UIPlayerBPBarBGGreen";
        Desc.iDepth = 1;

        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_PlayerHPBarBG"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
        CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
        if (pObj)
            m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
        else
            return E_FAIL;

        m_pGameInstance->AddPlayerUI(pObj);


        m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, 45.f, 0.f);
    }

    // 크로스헤어
    {
        auto ViewDesc = m_pGameInstance->GetViewPortDesc();

        CUIParent::UI_DEFAULT_INFO Desc = {};
        Desc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
        Desc.eUIType = UI_TYPE::UI_TEX;
        Desc.fSSizeX = 50;
        Desc.fSSizeY = 50;
        Desc.fSX = ViewDesc.Width / 2;
        Desc.fSY = ViewDesc.Height / 2;
        Desc.bUseSpawnPos = false;
        Desc.fSpeedPerSec = 0.f;
        Desc.fSpeedPerSec = 0.f;
        Desc.strObjectTag = "UIPlayerCrossHair";
        Desc.iDepth = 0;

        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_CrossHair"), LEVEL_GLOBAL, strLayerTag, &Desc)))
            return E_FAIL;

        auto pCur = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));

        m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pCur);
        m_pGameInstance->AddPlayerUI(pCur);
    }


#pragma region "UIPlayerHPFontBlue"
    // PlayerUIFont
    {
        _wstring strLayerTag = TEXT("Layer_UI_PlayerHPBlue");
        CUIFont::UI_FONT_DESC Desc = {};
        Desc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
        Desc.eUIType = UI_TYPE::UI_PANEL;
        Desc.fSSizeX = 15;
        Desc.fSSizeY = 15;
        Desc.fSX = 0;
        Desc.fSY = 0;
        Desc.bUseSpawnPos = false;
        Desc.fSpeedPerSec = 0.f;
        Desc.fSpeedPerSec = 0.f;
        Desc.iDepth = 1;
        Desc.eFontColor = CUIFont::FONT_BLUE;

        _float fWidth = 190.f;
        _float fHeight = -23.f;
        _float fOffset = 12.f;

        // UIPlayerHPBlueOne
        {
            Desc.strObjectTag = "UIPlayerHPBlueFirst";

            if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Font"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
            CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
            if (pObj)
                m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
            else
                return E_FAIL;

            m_pGameInstance->AddPlayerUI(pObj);

            m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, fWidth, fHeight);
        }
       
        // UIPlayerHPBlueTwo
        {
            Desc.strObjectTag = "UIPlayerHPBlueTwo";

            if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Font"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
            CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
            if (pObj)
                m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
            else
                return E_FAIL;
            m_pGameInstance->AddPlayerUI(pObj);

            m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, fWidth += fOffset, fHeight);
        }

        // UIPlayerHPBlueThree
        {
            Desc.strObjectTag = "UIPlayerHPBlueThree";

            if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Font"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
            CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
            if (pObj)
                m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
            else
                return E_FAIL;
            m_pGameInstance->AddPlayerUI(pObj);

            m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, fWidth += fOffset, fHeight);

        }


        // UIPlayerHPBlueFour
        {
            Desc.strObjectTag = "UIPlayerHPBlueFour";

            if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Font"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
            CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
            if (pObj)
                m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
            else
                return E_FAIL;
            m_pGameInstance->AddPlayerUI(pObj);

            m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, fWidth += fOffset, fHeight);
        }
    
        // UIPlayerHPBlueFive
        {
            Desc.strObjectTag = "UIPlayerHPBlueFive";

            if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Font"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
            CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
            if (pObj)
                m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
            else
                return E_FAIL;
            m_pGameInstance->AddPlayerUI(pObj);

            m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, fWidth += fOffset, fHeight);
        }


        // UIPlayerHPBlueSix
        {
            Desc.strObjectTag = "UIPlayerHPBlueSix";

            if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Font"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
            CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
            if (pObj)
                m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
            else
                return E_FAIL;
            m_pGameInstance->AddPlayerUI(pObj);

            m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, fWidth += fOffset, fHeight);
        }

        // UIPlayerHPBlueSeven
        {
            Desc.strObjectTag = "UIPlayerHPBlueSeven";

            if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Font"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
            CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
            if (pObj)
                m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
            else
                return E_FAIL;
            m_pGameInstance->AddPlayerUI(pObj);

            m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, fWidth += fOffset, fHeight);
        }
    }
#pragma endregion

#pragma region "UIPlayerHPFontGreen"

    // PlayerUIFontGreen
    {
        _wstring strLayerTag = TEXT("Layer_UI_PlayerHPGreen");
        CUIFont::UI_FONT_DESC Desc = {};
        Desc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
        Desc.eUIType = UI_TYPE::UI_PANEL;
        Desc.fSSizeX = 15;
        Desc.fSSizeY = 15;
        Desc.fSX = 0;
        Desc.fSY = 0;
        Desc.bUseSpawnPos = false;
        Desc.fSpeedPerSec = 0.f;
        Desc.fSpeedPerSec = 0.f;
        Desc.iDepth = 1;
        Desc.eFontColor = CUIFont::FONT_GREEN;


        _float fWidth = 170.f;
        _float fHeight = -5.f;
        _float fOffset = 12.f;


        // UIPlayerHPBlueOne
        {
            Desc.strObjectTag = "UIPlayerHPGreenFirst";

            if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Font"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
            CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
            if (pObj)
                m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
            else
                return E_FAIL;
            m_pGameInstance->AddPlayerUI(pObj);

            m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, fWidth, fHeight);
        }

        // UIPlayerHPBlueTwo
        {
            Desc.strObjectTag = "UIPlayerHPGreenTwo";

            if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Font"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
            CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
            if (pObj)
                m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
            else
                return E_FAIL;
            m_pGameInstance->AddPlayerUI(pObj);

            m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, fWidth += fOffset, fHeight);
        }

        // UIPlayerHPBlueThree
        {
            Desc.strObjectTag = "UIPlayerHPGreenThree";

            if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Font"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
            CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
            if (pObj)
                m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
            else
                return E_FAIL;
            m_pGameInstance->AddPlayerUI(pObj);

            m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, fWidth += fOffset, fHeight);

        }


        // UIPlayerHPBlueFour
        {
            Desc.strObjectTag = "UIPlayerHPGreenFour";

            if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Font"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
            CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
            if (pObj)
                m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
            else
                return E_FAIL;
            m_pGameInstance->AddPlayerUI(pObj);

            m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, fWidth += fOffset, fHeight);
        }

        // UIPlayerHPBlueFive
        {
            Desc.strObjectTag = "UIPlayerHPGreenFive";

            if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Font"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
            CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
            if (pObj)
                m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
            else
                return E_FAIL;
            m_pGameInstance->AddPlayerUI(pObj);

            m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, fWidth += fOffset, fHeight);
        }


        // UIPlayerHPBlueSix
        {
            Desc.strObjectTag = "UIPlayerHPGreenSix";

            if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Font"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
            CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
            if (pObj)
                m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
            else
                return E_FAIL;
            m_pGameInstance->AddPlayerUI(pObj);

            m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, fWidth += fOffset, fHeight);
        }

        // UIPlayerHPBlueSeven
        {
            Desc.strObjectTag = "UIPlayerHPGreenSeven";

            if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Font"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
            CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
            if (pObj)
                m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
            else
                return E_FAIL;
            m_pGameInstance->AddPlayerUI(pObj);

            m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, fWidth += fOffset, fHeight);
        }
    }
#pragma endregion

#pragma region "PlayerHpBar"
    // PlayerHpBar
    {
        _wstring strLayerTag = TEXT("Layer_UI_PlayerBPBar");

        CUIHPBar::UI_HP_BAR_DESC Desc = {};
        Desc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
        Desc.eUIType = UI_TYPE::UI_PANEL;
        Desc.fSX = 0;
        Desc.fSY = 0;
        Desc.bUseSpawnPos = false;
        Desc.fSpeedPerSec = 0.f;
        Desc.fSpeedPerSec = 0.f;
        Desc.iDepth = 0;
        Desc.eHPType = CUIHPBar::HP_PLAYER;
        Desc.fMaxHPBarVal = 100.f;

        // PlayerHPBarBlue
        {
            Desc.fSSizeX = 230;
            Desc.fSSizeY = 25;
            Desc.strObjectTag = "UIPlayerBPBarBlue";
            Desc.eHPBarColor = CUIHPBar::HP_BLUE;

            if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_HPBar"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
            CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
            if (pObj)
                m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
            else
                return E_FAIL;
            m_pGameInstance->AddPlayerUI(pObj);


            m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, 65.f, -15.f);
        }

        // PlayerHPBarGreen
        {
            Desc.fSSizeX = 230;
            Desc.fSSizeY = 25;
            Desc.strObjectTag = "UIPlayerBPBarGreen";
            Desc.eHPBarColor = CUIHPBar::HP_GREEN;

            if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_HPBar"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
            CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
            if (pObj)
                m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
            else
                return E_FAIL;

            m_pGameInstance->AddPlayerUI(pObj);

            m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, 45.f, 0.f);
        }
    }
#pragma endregion



  
    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyPlayerGunUI()
{

    _uint iNumViewPorts = 1;
    D3D11_VIEWPORT ViewportDesc = {};
    m_pContext->RSGetViewports(&iNumViewPorts, &ViewportDesc);

    _float fBGY = ViewportDesc.Height - 100.f;
    _float fBGX = ViewportDesc.Width;

    // GunInfoBG
    {
        _wstring strLayerTag = TEXT("Layer_UI_PlayerGunInfoBG");

        CUIBG::UI_BG_DESC PanelDesc = {};
        PanelDesc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
        PanelDesc.eUIType = UI_TYPE::UI_PANEL;

        PanelDesc.bUseSpawnPos = false;
        PanelDesc.fSpeedPerSec = 0.f;
        PanelDesc.fSpeedPerSec = 0.f;
        PanelDesc.iDepth = 5;
        PanelDesc.eBGType = CUIBG::UI_BG_TYPE::PLAYER_GUN_INFO;

        PanelDesc.fSSizeX = 250.f;
        PanelDesc.fSSizeY = 100.f;

        PanelDesc.fSX = fBGX - PanelDesc.fSSizeX * 0.5f - 30.f;
        PanelDesc.fSY = fBGY;

        PanelDesc.strObjectTag = "UIPlayerGunInfoBG";

        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_BG"), LEVEL_GLOBAL, strLayerTag, &PanelDesc))) return E_FAIL;

        CUIPanel* pObj = static_cast<CUIPanel*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
        if (pObj)
            m_pGameInstance->AddPanelUIToManager(PanelDesc.strObjectTag, pObj);
        else
            return E_FAIL;

        m_pGameInstance->AddPlayerUI(pObj);


        {
            
        }


        // 1 2 3
        {
            _float fSizeX = 50.f;
            _float fSizeY = 50.f;
            _float fOffSetY = 60.f;

            CUITex::UI_TEX_DESC Desc = {};
            Desc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
            Desc.eUIType = UI_TYPE::UI_TEX;
            Desc.fSSizeX = fSizeX;
            Desc.fSSizeY = fSizeY;
            Desc.fSX = 0.f;
            Desc.fSY = 0.f;
            Desc.bUseSpawnPos = false;
            Desc.fSpeedPerSec = 0.f;
            Desc.fSpeedPerSec = 0.f;
            Desc.iDepth = 0;

            // Number1
            {

                Desc.eUITexType = CUITex::UI_TEX_TYPE::PLAYER_GUN_KEY_NUMBER_1;
                Desc.strObjectTag = "UIPlayerGunNumber1";

                if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Tex"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
                CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
                if (pObj)
                    m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
                else
                    return E_FAIL;
                m_pGameInstance->AddPlayerUI(pObj);

                m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, 0.f, fOffSetY);
            }

            // Number2
            {
                Desc.eUITexType = CUITex::UI_TEX_TYPE::PLAYER_GUN_KEY_NUMBER_2;
                Desc.strObjectTag = "UIPlayerGunNumber2";

                if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Tex"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
                CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
                if (pObj)
                    m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
                else
                    return E_FAIL;
                m_pGameInstance->AddPlayerUI(pObj);

                m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, 50.f, fOffSetY);
            }

            // Number3
            {
                Desc.eUITexType = CUITex::UI_TEX_TYPE::PLAYER_GUN_KEY_NUMBER_3;
                Desc.strObjectTag = "UIPlayerGunNumber3";

                if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Tex"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
                CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
                if (pObj)
                    m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
                else
                    return E_FAIL;
                m_pGameInstance->AddPlayerUI(pObj);

                m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, 100.f, fOffSetY);
            }
        }

        // WeaponImg
        {
            CUITex::UI_TEX_DESC Desc = {};
            Desc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
            Desc.eUIType = UI_TYPE::UI_TEX;
            Desc.fSSizeX = 120;
            Desc.fSSizeY = 120;
            Desc.fSX = 0.f;
            Desc.fSY = 0.f;
            Desc.bUseSpawnPos = false;
            Desc.fSpeedPerSec = 0.f;
            Desc.fSpeedPerSec = 0.f;
            Desc.iDepth = 0;

            // 1202
            {
                Desc.eUITexType = CUITex::UI_TEX_TYPE::PLAYER_GUN_TEX;
                Desc.strObjectTag = "UIPlayerGunTex";

                if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Tex"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
                CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
                if (pObj)
                    m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
                else
                    return E_FAIL;
                m_pGameInstance->AddPlayerUI(pObj);

                m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, 0.f, 0.f);
            }
        }

        // Gun Bullet Font
        {
            CUIFont::UI_FONT_DESC Desc = {};
            Desc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
            Desc.eUIType = UI_TYPE::UI_PANEL;

            Desc.fSX = 0;
            Desc.fSY = 0;
            Desc.bUseSpawnPos = false;
            Desc.fSpeedPerSec = 0.f;
            Desc.fSpeedPerSec = 0.f;
            Desc.iDepth = 1;
            Desc.eFontColor = CUIFont::FONT_DEFAULT;
            Desc.fFontScalingSP = 100.f;

            _float fSX = 0.f;
            _float fSY = -50.f;
            _float fSXAcc = -50.f;
            _float Acc = 30.f;

            // UIPlayerBulletFont
            {
                // First
                {
                    Desc.fSSizeX = 30;
                    Desc.fSSizeY = 30;
                    Desc.strObjectTag = "UIPlayerBulletFontFirst";

                    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Font"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
                    CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
                    if (pObj)
                        m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
                    else
                        return E_FAIL;
                    m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, fSXAcc, fSY);

                    m_pGameInstance->AddPlayerUI(pObj);

                }

                // Two
                {
                    Desc.fSSizeX = 30;
                    Desc.fSSizeY = 30;
                    Desc.strObjectTag = "UIPlayerBulletFontTwo";

                    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Font"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
                    CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
                    if (pObj)
                        m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
                    else
                        return E_FAIL;

                    m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, fSXAcc += Acc, fSY);

                    m_pGameInstance->AddPlayerUI(pObj);
                }

                _float fAccThree = 20.f;
                // Three // Slash
                {
                    Desc.fSSizeX = 25;
                    Desc.fSSizeY = 25;
                    Desc.strObjectTag = "UIPlayerBulletFontThree";

                    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Font"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
                    CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
                    if (pObj)
                        m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
                    else
                        return E_FAIL;
                    m_pGameInstance->AddPlayerUI(pObj);

                    m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, fSXAcc += 25.f, fSY);
                }

                // Four
                {
                    Desc.fSSizeX = 25;
                    Desc.fSSizeY = 25;
                    Desc.strObjectTag = "UIPlayerBulletFontFour";

                    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Font"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
                    CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
                    if (pObj)
                        m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
                    else
                        return E_FAIL;
                    m_pGameInstance->AddPlayerUI(pObj);

                    m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, fSXAcc += fAccThree, fSY);
                }

                // Five
                {
                    Desc.fSSizeX = 25;
                    Desc.fSSizeY = 25;
                    Desc.strObjectTag = "UIPlayerBulletFontFive";

                    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Font"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
                    CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
                    if (pObj)
                        m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
                    else
                        return E_FAIL;
                    m_pGameInstance->AddPlayerUI(pObj);

                    m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, fSXAcc += fAccThree, fSY);
                }

                // Six
                {
                    Desc.fSSizeX = 25;
                    Desc.fSSizeY = 25;
                    Desc.strObjectTag = "UIPlayerBulletFontSix";

                    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Font"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
                    CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
                    if (pObj)
                        m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
                    else
                        return E_FAIL;

                    m_pGameInstance->AddPlayerUI(pObj);

                    m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, fSXAcc += fAccThree, fSY);
                }
            }
        }
    }

    // SkillInfoBG
    {
        _wstring strLayerTag = TEXT("Layer_UI_PlayerSkillBG");

        CUIBG::UI_BG_DESC PanelDesc = {};
        PanelDesc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
        PanelDesc.eUIType = UI_TYPE::UI_PANEL;
        PanelDesc.bUseSpawnPos = false;
        PanelDesc.fSpeedPerSec = 0.f;
        PanelDesc.fSpeedPerSec = 0.f;
        PanelDesc.iDepth = 5;
        PanelDesc.eBGType = CUIBG::UI_BG_TYPE::PLAYER_SKILL;
        PanelDesc.fSSizeX = 100.f;
        PanelDesc.fSSizeY = 100.f;
        PanelDesc.fSX = fBGX - 320.f;
        PanelDesc.fSY = fBGY;
        PanelDesc.strObjectTag = "UIPlayerSkillBG";

        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_BG"), LEVEL_GLOBAL, strLayerTag, &PanelDesc))) return E_FAIL;

        CUIPanel* pObj = static_cast<CUIPanel*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
        if (pObj)
            m_pGameInstance->AddPanelUIToManager(PanelDesc.strObjectTag, pObj);
        else
            return E_FAIL;
        m_pGameInstance->AddPlayerUI(pObj);


        // Tex
        {
            CUITex::UI_TEX_DESC Desc = {};
            Desc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
            Desc.eUIType = UI_TYPE::UI_TEX;
            Desc.fSSizeX = 70;
            Desc.fSSizeY = 70;
            Desc.fSX = 0.f;
            Desc.fSY = 0.f;
            Desc.bUseSpawnPos = false;
            Desc.fSpeedPerSec = 0.f;
            Desc.fSpeedPerSec = 0.f;
            Desc.iDepth = 1;

            // Dash BG
            {
                Desc.eUITexType = CUITex::UI_TEX_TYPE::PLAYER_SKILL_DASH_BG;
                Desc.strObjectTag = "UIPlayerSkillDashBG";

                if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Tex"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
                CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
                if (pObj)
                    m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
                else
                    return E_FAIL;
                m_pGameInstance->AddPlayerUI(pObj);

                m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, 0.f, 0.f);
            }

            // Dash
            {
                Desc.iDepth = 0;
                Desc.eUITexType = CUITex::UI_TEX_TYPE::PLAYER_SKILL_DASH;
                Desc.strObjectTag = "UIPlayerSkillDash";

                if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Tex"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
                CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
                if (pObj)
                    m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
                else
                    return E_FAIL;
                m_pGameInstance->AddPlayerUI(pObj);

                m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, 0.f, 0.f);
            }

            // DashKey
            {
                Desc.fSSizeX = 70;
                Desc.fSSizeY = 30;
                Desc.iDepth = 0;
                Desc.eUITexType = CUITex::UI_TEX_TYPE::PLAYER_KEY_SHIFT;
                Desc.strObjectTag = "UIPlayerKeyShift";

                if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Tex"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
                CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
                if (pObj)
                    m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
                else
                    return E_FAIL;
                m_pGameInstance->AddPlayerUI(pObj);

                m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, +40.f, 40.f);
            }
        }
    }

    // SkillMid
    {
        _wstring strLayerTag = TEXT("Layer_UI_PlayerSkillMidBG");

        CUIBG::UI_BG_DESC PanelDesc = {};
        PanelDesc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
        PanelDesc.eUIType = UI_TYPE::UI_PANEL;
        PanelDesc.bUseSpawnPos = false;
        PanelDesc.fSpeedPerSec = 0.f;
        PanelDesc.fSpeedPerSec = 0.f;
        PanelDesc.iDepth = 5;
        PanelDesc.eBGType = CUIBG::UI_BG_TYPE::PLAYER_SKILL_MID;
        PanelDesc.fSSizeX = 100.f;
        PanelDesc.fSSizeY = 100.f;
        PanelDesc.fSX = fBGX * 0.5f;
        PanelDesc.fSY = fBGY;
        PanelDesc.strObjectTag = "UIPlayerSkillMidBG";

        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_BG"), LEVEL_GLOBAL, strLayerTag, &PanelDesc))) return E_FAIL;

        CUIPanel* pObj = static_cast<CUIPanel*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
        if (pObj)
            m_pGameInstance->AddPanelUIToManager(PanelDesc.strObjectTag, pObj);
        else
            return E_FAIL;
        m_pGameInstance->AddPlayerUI(pObj);

        // Tex
        {
            CUITex::UI_TEX_DESC Desc = {};
            Desc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
            Desc.eUIType = UI_TYPE::UI_TEX;
            Desc.fSSizeX = 100.f;
            Desc.fSSizeY = 100.f;
            Desc.fSX = 0.f;
            Desc.fSY = 0.f;
            Desc.bUseSpawnPos = false;
            Desc.fSpeedPerSec = 0.f;
            Desc.fSpeedPerSec = 0.f;
            Desc.iDepth = 0;
            Desc.eUITexType = CUITex::UI_TEX_TYPE::PLAYER_SKILL_MID;
            Desc.strObjectTag = "UIPlayerSkillMid";

           /* Desc.bUseAnim = true;
            Desc.iAnimCol = 4;
            Desc.iAnimRow = 3;
            Desc.fAnimMaxTime = 1.f;*/

            if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Tex"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
            CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
            if (pObj)
                m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
            else
                return E_FAIL;
            m_pGameInstance->AddPlayerUI(pObj);

            m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, 0.f, 0.f);
        }


        {
            CUITex::UI_TEX_DESC Desc = {};
            Desc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
            Desc.eUIType = UI_TYPE::UI_TEX;
            Desc.fSSizeX = 70.f;
            Desc.fSSizeY = 70.f;
            Desc.fSX = 0.f;
            Desc.fSY = 0.f;
            Desc.bUseSpawnPos = false;
            Desc.fSpeedPerSec = 0.f;
            Desc.fSpeedPerSec = 0.f;
            Desc.iDepth = 0;
            Desc.eUITexType = CUITex::UI_TEX_TYPE::PLAYER_SKILL_MID_ANIM;
            Desc.strObjectTag = "UIPlayerSkillMidAnim";

            Desc.bUseAnim = true;
            Desc.iAnimCol = 4;
            Desc.iAnimRow = 3;
            Desc.fAnimMaxTime = 0.25f;

            if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Tex"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
            CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
            if (pObj)
                m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
            else
                return E_FAIL;

            m_pGameInstance->AddPlayerUI(pObj);

            m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, 0.f, -10.f);
        }


        // Q
        {
            CUITex::UI_TEX_DESC Desc = {};
            Desc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
            Desc.eUIType = UI_TYPE::UI_TEX;
            Desc.fSSizeX = 30.f;
            Desc.fSSizeY = 30.f;
            Desc.fSX = 0.f;
            Desc.fSY = 0.f;
            Desc.bUseSpawnPos = false;
            Desc.fSpeedPerSec = 0.f;
            Desc.fSpeedPerSec = 0.f;
            Desc.iDepth = 0;
            Desc.eUITexType = CUITex::UI_TEX_TYPE::PLAYER_KEY_Q;
            Desc.strObjectTag = "UIPlayerKeyQ";

            /* Desc.bUseAnim = true;
             Desc.iAnimCol = 4;
             Desc.iAnimRow = 3;
             Desc.fAnimMaxTime = 1.f;*/

            if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Tex"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
            CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
            if (pObj)
                m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
            else
                return E_FAIL;

            m_pGameInstance->AddPlayerUI(pObj);

            m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, 30.f, 40.f);
        }
    }


    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyMiniMap()
{
    _uint iNumViewPorts = 1;
    D3D11_VIEWPORT ViewportDesc = {};
    m_pContext->RSGetViewports(&iNumViewPorts, &ViewportDesc);

    // GunInfoBG
    {
        _wstring strLayerTag = TEXT("Layer_UI_MiniMap");

        CUIBG::UI_BG_DESC PanelDesc = {};
        PanelDesc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
        PanelDesc.eUIType = UI_TYPE::UI_PANEL;

        PanelDesc.bUseSpawnPos = false;
        PanelDesc.fSpeedPerSec = 0.f;
        PanelDesc.fSpeedPerSec = 0.f;
        PanelDesc.iDepth = 5;
        PanelDesc.eBGType = CUIBG::UI_BG_TYPE::MINI_MAP;

        PanelDesc.fSSizeX = 250.f;
        PanelDesc.fSSizeY = 250.f;

        PanelDesc.fSX = PanelDesc.fSSizeX * 0.5f + 30.f;
        PanelDesc.fSY = PanelDesc.fSSizeY * 0.5f + 30.f;

        PanelDesc.strObjectTag = "UIMiniMapBG";

        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_BG"), LEVEL_GLOBAL, strLayerTag, &PanelDesc))) return E_FAIL;

        CUIPanel* pObj = static_cast<CUIPanel*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
        if (pObj)
            m_pGameInstance->AddPanelUIToManager(PanelDesc.strObjectTag, pObj);
        else
            return E_FAIL;

        m_pGameInstance->AddPlayerUI(pObj);


        // PlayerPos
        {
            CUITex::UI_TEX_DESC Desc = {};
            Desc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
            Desc.eUIType = UI_TYPE::UI_TEX;
            Desc.fSSizeX = 30;
            Desc.fSSizeY = 30;
            Desc.fSX = 0.f;
            Desc.fSY = 0.f;
            Desc.bUseSpawnPos = false;
            Desc.fSpeedPerSec = 0.f;
            Desc.fSpeedPerSec = 0.f;
            Desc.iDepth = 0;

            // UIMiniMapPlayerPos
            {
                Desc.eUITexType = CUITex::UI_TEX_TYPE::MINI_MAP_PLAYER_POS;
                Desc.strObjectTag = "UIMiniMapPlayerPos";

                if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Tex"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
                CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
                if (pObj)
                    m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
                else
                    return E_FAIL;

                m_pGameInstance->AddPlayerUI(pObj);

                m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, 0.f, 0.f);
            }
        }

        // Monster Pos
        {
            CUITex::UI_TEX_DESC Desc = {};
            Desc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
            Desc.eUIType = UI_TYPE::UI_TEX;
            Desc.fSSizeX = 15;
            Desc.fSSizeY = 15;
            Desc.fSX = 0.f;
            Desc.fSY = 0.f;
            Desc.bUseSpawnPos = false;
            Desc.fSpeedPerSec = 0.f;
            Desc.fSpeedPerSec = 0.f;
            Desc.iDepth = 0;
            Desc.eUITexType = CUITex::UI_TEX_TYPE::MINI_MAP_MONSTER_POS;

            static _int iNum = 0;


            // UIMiniMapMonsterPos
            {
                for (_int i = 0; i < 20; ++i)
                {
                    Desc.strObjectTag = "UIMiniMapMonterPos" + to_string(iNum++);

                    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Tex"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
                    CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
                    if (pObj)
                        m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
                    else
                        return E_FAIL;

                    m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, 0.f, 0.f);

                    pObj->DeActiveRender();
                }
            }
        }

        // Boss
        {
            CUITex::UI_TEX_DESC Desc = {};
            Desc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
            Desc.eUIType = UI_TYPE::UI_TEX;
            Desc.fSSizeX = 30;
            Desc.fSSizeY = 30;
            Desc.fSX = 0.f;
            Desc.fSY = 0.f;
            Desc.bUseSpawnPos = false;
            Desc.fSpeedPerSec = 0.f;
            Desc.fSpeedPerSec = 0.f;
            Desc.iDepth = 0;
            Desc.eUITexType = CUITex::UI_TEX_TYPE::MINI_MAP_BOSS;
            Desc.strObjectTag = "UIMiniMapBossPos";

            if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Tex"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
            CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
            if (pObj)
                m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
            else
                return E_FAIL;

            m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, 0.f, 0.f);

            pObj->DeActiveRender();
        }


        // Door
        {
            CUITex::UI_TEX_DESC Desc = {};
            Desc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
            Desc.eUIType = UI_TYPE::UI_TEX;
            Desc.fSSizeX = 30;
            Desc.fSSizeY = 30;
            Desc.fSX = 0.f;
            Desc.fSY = 0.f;
            Desc.bUseSpawnPos = false;
            Desc.fSpeedPerSec = 0.f;
            Desc.fSpeedPerSec = 0.f;
            Desc.iDepth = 0;

            // UIMiniMapPlayerPos
            {
                Desc.eUITexType = CUITex::UI_TEX_TYPE::MINI_DOOR;
                Desc.strObjectTag = "UIMiniMapDoor";

                if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Tex"), LEVEL_GLOBAL, strLayerTag, &Desc))) return E_FAIL;
                CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
                if (pObj)
                    m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
                else
                    return E_FAIL;

                m_pGameInstance->AddPlayerUI(pObj);
                m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, 0.f, 0.f);
                pObj->DeActiveRender();

            }
        }
    }
   
    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyPlayerSkillEffectUI()
{
    _wstring strLayerTag = TEXT("Layer_UI_Tex_PlayerEffect");

    auto ViewDesc = m_pGameInstance->GetViewPortDesc();

    {
        CUITex::UI_TEX_DESC Desc = {};
        Desc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
        Desc.eUIType = UI_TYPE::UI_TEX;
        Desc.fSSizeX = ViewDesc.Width + 1400;
        Desc.fSSizeY = ViewDesc.Height + 800;
        Desc.fSX = ViewDesc.Width * 0.5f;
        Desc.fSY = ViewDesc.Height * 0.5f;
        Desc.bUseSpawnPos = false;
        Desc.fSpeedPerSec = 0.f;
        Desc.fSpeedPerSec = 0.f;
        Desc.iDepth = 0;
        Desc.eUITexType = CUITex::UI_TEX_TYPE::PLAYER_SKILL_DASH_EFFECT;
        Desc.eUITexEffectType = CUITex::UI_TEX_EFFECT_ALPHA;
        Desc.strObjectTag = "UITexPlayerDashEffect";

        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Tex"), LEVEL_GLOBAL, strLayerTag, &Desc)))
            return E_FAIL;

        CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
        m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
        pObj->DeActiveRender();


    }


    {
        CUITex::UI_TEX_DESC Desc = {};
        Desc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
        Desc.eUIType = UI_TYPE::UI_TEX;
        Desc.fSSizeX = ViewDesc.Width;
        Desc.fSSizeY = ViewDesc.Height;
        Desc.fSX = ViewDesc.Width * 0.5f;
        Desc.fSY = ViewDesc.Height * 0.5f;
        Desc.bUseSpawnPos = false;
        Desc.fSpeedPerSec = 0.f;
        Desc.fSpeedPerSec = 0.f;
        Desc.iDepth = 0;
        Desc.eUITexType = CUITex::UI_TEX_TYPE::PLAYER_DAMAGED_EFFECT;
        Desc.eUITexEffectType = CUITex::UI_TEX_EFFECT_ALPHA;
        Desc.strObjectTag = "UITexPlayerDamagedEffect";

        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Tex"), LEVEL_GLOBAL, strLayerTag, &Desc)))
            return E_FAIL;

        CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
        m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
        pObj->DeActiveRender();

    }

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyPlayerKeyUI()
{
    _wstring strLayerTag = TEXT("Layer_UI_Tex_PlayerKey");
    auto ViewDesc = m_pGameInstance->GetViewPortDesc();

    CUITex::UI_TEX_DESC Desc = {};
    Desc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
    Desc.eUIType = UI_TYPE::UI_TEX;
    Desc.fSSizeX = 50.f;
    Desc.fSSizeY = 50.f;
    Desc.fSX = ViewDesc.Width * 0.5f;
    Desc.fSY = ViewDesc.Height * 0.5f + 100.f;
    Desc.bUseSpawnPos = false;
    Desc.fSpeedPerSec = 0.f;
    Desc.fSpeedPerSec = 0.f;
    Desc.iDepth = 0;
    Desc.eUITexType = CUITex::UI_TEX_TYPE::PLAYER_KEY_F;
    Desc.eUITexEffectType = CUITex::UI_TEX_EFFECT_DEFAULT;
    Desc.strObjectTag = "UITexPlayerKeyF";

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Tex"), LEVEL_GLOBAL, strLayerTag, &Desc)))
        return E_FAIL;

    CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
    m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
    pObj->DeActiveRender();

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyDamageUIFont()
{
    _wstring strLayerTag = TEXT("Layer_UI_Damage_Font");
    auto ViewDesc = m_pGameInstance->GetViewPortDesc();

    CUIDamageFont::UI_DAMAGE_FONT_DESC Desc = {};
    Desc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
    Desc.eUIType = UI_TYPE::UI_DAMAGE_FONT;
    Desc.fSSizeX = 300.f;
    Desc.fSSizeY = 300.f;
    Desc.fSX = 0.f;
    Desc.fSY = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.fSpeedPerSec = 0.f;
    Desc.fSpeedPerSec = 0.f;
    Desc.iDepth = 0;

    Desc.eUIDamageFontColor = CUIDamageFont::UI_DAMAGE_FONT_DEFAULT;
    Desc.fEndSacle = 20.f;
    Desc.fStartScale = 150.f;
    Desc.fScalingSP = 500.f;
    
    for (_int i = 0; i < 30; ++i)
    {
        Desc.strObjectTag = "UIDamageFont" + to_string(i);

        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_DamageFont"), LEVEL_GLOBAL, strLayerTag, &Desc)))
            return E_FAIL;

        CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
        m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
        pObj->DeActiveRender();

    }

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyUIVertical()
{
    _wstring strLayerTag = TEXT("Layer_UI_HPVertical");
    auto ViewDesc = m_pGameInstance->GetViewPortDesc();

    CUIParent::UI_DEFAULT_INFO Desc = {};
    Desc.eShowType = UI_SHOW_TYPE::UI_PERSPECTIVE;
    Desc.eUIType = UI_TYPE::UI_BAR;
    Desc.fSSizeX = 50.f;
    Desc.fSSizeY = 50.f;
    Desc.fSX = 0.f;
    Desc.fSY = 0.f;
    Desc.bUseSpawnPos = false;
    Desc.fSpeedPerSec = 0.f;
    Desc.fSpeedPerSec = 0.f;
    Desc.iDepth = 0;

    for (_int i = 0; i < 30; ++i)
    {
        Desc.strObjectTag = "HPVertical" + to_string(i);

        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_HPVertical"), LEVEL_GLOBAL, strLayerTag, &Desc)))
            return E_FAIL;

        CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, strLayerTag));
        m_pGameInstance->AddElementUIToManagerVertical(Desc.strObjectTag, pObj);
        pObj->DeActiveRender();
    }


    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyDoorUI()
{
    
    CUITexPerspective::UI_TEX_P_DESC Desc = {};
    Desc.bUseSpawnPos = false;
    Desc.eShowType = UI_SHOW_TYPE::UI_PERSPECTIVE;
    Desc.eUITexPType = CUITexPerspective::UI_TEX_P_DOOR;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.fSpeedPerSec = 0.f;
    Desc.fSSizeX = 50.f;
    Desc.fSSizeY = 50.f;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Tex_Perspective"), LEVEL_GLOBAL, TEXT("Layer_UI_Perspetive"), &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadyTestS()
{
    //_wstring strLayerTag = TEXT("Layer_UI_Voctory");

    //auto ViewDesc = m_pGameInstance->GetViewPortDesc();

    //CUITex::UI_TEX_DESC Desc = {};
    //Desc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
    //Desc.eUIType = UI_TYPE::UI_TEX;
    //Desc.fSSizeX = ViewDesc.Width;
    //Desc.fSSizeY = ViewDesc.Height;
    //Desc.fSX = ViewDesc.Width * 0.5f;
    //Desc.fSY = ViewDesc.Height * 0.5f;
    //Desc.bUseSpawnPos = false;
    //Desc.fSpeedPerSec = 0.f;
    //Desc.fSpeedPerSec = 0.f;
    //Desc.iDepth = 0;
    //Desc.eUITexType = CUITex::UI_TEX_TYPE::VICTORY;
    //Desc.eUITexEffectType = CUITex::UI_TEX_EFFECT_DEFAULT;
    //Desc.strObjectTag = "UITexVictory";

    //if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Tex"), LEVEL_GAMEPLAY, strLayerTag, &Desc)))
    //    return E_FAIL;
    //CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GAMEPLAY, strLayerTag));
    //m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);


    return S_OK;

}

#ifdef _DEBUG
void CLevel_GamePlay::GoBossLevel()
{
    m_bMoveBossLevel = true;
}
void CLevel_GamePlay::InitializeImgui()
{
    // ImGui 컨텍스트 생성
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& IO = ImGui::GetIO();
    (void)IO; // 사용하지 않는 변수 경고 방지

    // ImGui 설정
    IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;			// 키보드 내비게이션 활성화
    IO.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;			// 게임패드 내비게이션 활성화
    IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;				 // 도킹 기능 활성화
    IO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;				 // 멀티 뷰포트 활성화

    // ImGui 스타일 설정 (옵션)
    ImGui::StyleColorsDark(); // 기본 어두운 테마
    // ImGui::StyleColorsClassic(); // 클래식 테마
    // ImGui::StyleColorsLight(); // 밝은 테마

    // ImGui Win32 백엔드 초기화
    ImGui_ImplWin32_Init(g_hWnd);

    // ImGui DirectX 11 백엔드 초기화
    ImGui_ImplDX11_Init(m_pDevice, m_pContext);
}

void CLevel_GamePlay::RenderImgui()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Debug");

    if (ImGui::CollapsingHeader("Player"))
    {
        _uint iState = m_pPlayer->GetState();
        string strCurPlayerState = m_pPlayer->GetStateDebug();
        string strPrevPlayerState = m_pPlayer->GetPrevStateDebug();
        string strCurModelName = m_pPlayerModels[m_iCurrentModelIndex]->GetModelName();

        ImGui::Text("Player Model Name : %s", strCurModelName.c_str());
        ImGui::Text("Player State : %s", strCurPlayerState.c_str());
        ImGui::Text("Player Prev State : %s", strPrevPlayerState.c_str());

        _uint iCurAnimIndex = m_pPlayerModels[m_iCurrentModelIndex]->GetCurrentAnimationIndex();
        string strCurAnimName = m_pPlayerModels[m_iCurrentModelIndex]->GetAnimationName(iCurAnimIndex);
        ImGui::Text("Player Anim Name %s", strCurAnimName.c_str());
        
        _float4 vPos;
        XMStoreFloat4(&vPos, m_pPlayerTransformCom->GetTransformState(CTransform::STATE_POS));
        ImGui::Text("Player Pos X : %.2f, Y : %.2f, Z : %.2f", vPos.x, vPos.y, vPos.z);
    }


    if (ImGui::CollapsingHeader("Camera"))
    {
        if (ImGui::Button("Attach Camera To Player"))
        {
            m_bCameraAttach = true;
            m_pCurCamera->SetAttachToPlayer(m_bCameraAttach);
        }

        if (ImGui::Button("Detach Camera From Player"))
        {
            m_bCameraAttach = false;
            m_pCurCamera->SetAttachToPlayer(m_bCameraAttach);
        }

        if (ImGui::Button("Attach Player To Camera"))
        {
            m_bPlayerAttach = true;
        }
        
        if (ImGui::Button("Detach Player From Camera"))
        {
            m_bPlayerAttach = false;
        }
    }

    ImGui::End();

    // ImGui 프레임 완료
    ImGui::Render();

    // ImGui 그리기
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
}
#endif

HRESULT CLevel_GamePlay::LoadAABB()
{
    ReadAABB("../Bin/DataFiles/AABB.dat");
    ReadAABB("../Bin/DataFiles/AABB_Scene01_Second.dat");
    ReadAABB("../Bin/DataFiles/AABBS01Floor.dat");
    ReadAABB("../Bin/DataFiles/AABBS01Ceil.dat");

    // S2
    ReadAABB("../Bin/DataFiles/AABB_S2_FloorCeil.dat");
    ReadAABB("../Bin/DataFiles/AABB_S2_Wall.dat");


    // S3
    ReadAABB("../Bin/DataFiles/AABB_S3_Base.dat");
    ReadAABB("../Bin/DataFiles/AABB_S3_Detail.dat");

    return S_OK;
}

void CLevel_GamePlay::ReadAABB(string strFilePath)
{
    ifstream InFile(strFilePath.c_str(), ios::in | ios_base::binary);
    if (!InFile.is_open())
    {
        MSG_BOX("ReadSpawnTriggers ReadAABB Error!");
        return;
    }

    _int iSize;
    InFile.read((_char*)&iSize, sizeof _int);

    for (_int i = 0; i < iSize; ++i)
    {
        _float3 vPos;
        _float3 vEx;

        InFile.read((_char*)&vPos, sizeof _float3);
        InFile.read((_char*)&vEx, sizeof _float3);

        CWallAABB::WALL_AABB_DESC Desc = {};
        Desc.bUseSpawnPos = true;
        Desc.fRotSpeedPerSec = 0.f;
        Desc.fSpeedPerSec = 0.f;
        Desc.strObjectTag = "WallAABB";
        // Desc.vCenter = _float3{ vPos.x , vPos.y, vPos.z };
        Desc.vSpawnPos = vPos;
        Desc.vExtents = vEx;
        Desc.iLevelIndex = LEVEL_GAMEPLAY;

        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_WallAABB"), LEVEL_GAMEPLAY, TEXT("Layer_WallAABB"), &Desc)))
        {
            MSG_BOX("ReadSpawnTriggers Prototype_GameObject_WallAABB Error!");
            return;
        }
    }
}

HRESULT CLevel_GamePlay::LoadSpawnTriggers()
{
    //// S1
    //ReadSpawnTriggers("../Bin/DataFiles/SpawnTrigger_Scene01_First.dat", 0, "FirstSpawnTrigger");
    //// ReadSpawnTriggers("../Bin/DataFiles/SpawnTrigger_Scene01_Second.dat");
    //ReadSpawnTriggers("../Bin/DataFiles/ST_01_Chang1.dat", 0, "SecondSpawnTrigger");
    //ReadSpawnTriggers("../Bin/DataFiles/ST_01_ArrowRed_2.dat", 0, "ThridSpawnTrigger");

    //// S2
    //ReadSpawnTriggers("../Bin/DataFiles/SpawnTrigger_S2_ArrowYellowRed.dat", 1, "FourSpawnTrigger", 3);
    //ReadSpawnTriggers("../Bin/DataFiles/SpawnTrigger_S2_ChangRedHorse.dat",  1, "FivSpawnTrigger");

    //// S3
    //ReadSpawnTriggers("../Bin/DataFiles/SpawnTrigger_S3_4.dat", 2, "SixSpawnTrigger");
    //ReadSpawnTriggers("../Bin/DataFiles/SpawnTrigger_S3_4_2.dat", 2, "SevenSpawnTrigger", 1);
    //ReadSpawnTriggers("../Bin/DataFiles/SpawnTrigger_S3_4_3.dat", 2, "EightSpawnTrigger", 1);
    


    // S1
    ReadSpawnTriggers("../Bin/DataFiles/SpawnTrigger_Scene01_First.dat", "FirstSpawnTrigger", 0);
    // ReadSpawnTriggers("../Bin/DataFiles/SpawnTrigger_Scene01_Second.dat");
    ReadSpawnTriggers("../Bin/DataFiles/ST_01_Chang1.dat", "SecondSpawnTrigger", 0);
    ReadSpawnTriggers("../Bin/DataFiles/ST_01_ArrowRed_2.dat", "ThridSpawnTrigger", 0);

    // S2
    ReadSpawnTriggers("../Bin/DataFiles/SpawnTrigger_S2_ArrowYellowRed.dat", "FourSpawnTrigger", 1, 3);
    ReadSpawnTriggers("../Bin/DataFiles/SpawnTrigger_S2_ChangRedHorse.dat", "FivSpawnTrigger", 1);

    // S3
    ReadSpawnTriggers("../Bin/DataFiles/SpawnTrigger_S3_4.dat", "SixSpawnTrigger", 2);
    ReadSpawnTriggers("../Bin/DataFiles/SpawnTrigger_S3_4_2.dat", "SevenSpawnTrigger",2, 1);
    ReadSpawnTriggers("../Bin/DataFiles/SpawnTrigger_S3_4_3.dat", "EightSpawnTrigger", 2, 1);

    return S_OK;
}

void CLevel_GamePlay::ReadSpawnTriggers(string strFilePath, string strSpawnTriggerTag, _int iNavMeshIdx, _int iDir)
{
    static _int g_iNum = 0;

    ifstream InFile(strFilePath.c_str(), ios::in | ios_base::binary);
    if (!InFile.is_open())
    {
        MSG_BOX("ReadSpawnTriggers Error!");
        return;
    }

    _int iSpawnerCnt;
    _int iRotType;
    _vector vCenter;
    _vector vExtents;

    InFile.read((_char*)&iSpawnerCnt, sizeof _int);
    InFile.read((_char*)&iRotType, sizeof _int);
    InFile.read((_char*)&vCenter, sizeof _vector);
    InFile.read((_char*)&vExtents, sizeof _vector);

    CTrigger::TRIGGER_DESC SpawnTriggerDesc{};
    SpawnTriggerDesc.eTriggerType = CTrigger::TRIGGER_TYPE::TYPE_SPAWN_TRIGGER;
    SpawnTriggerDesc.bUseSpawnPos = true;
    SpawnTriggerDesc.fRotSpeedPerSec = 0.f;
    SpawnTriggerDesc.fSpeedPerSec = 0.f;
    SpawnTriggerDesc.strObjectTag = strSpawnTriggerTag;
    SpawnTriggerDesc.iLevelIndex = LEVEL_GAMEPLAY;

    XMStoreFloat3(&SpawnTriggerDesc.vSpawnPos, vCenter);
    XMStoreFloat3(&SpawnTriggerDesc.vExtents, vExtents);

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_SpawnTrigger"), LEVEL_GAMEPLAY, TEXT("SpawnTrigger"), &SpawnTriggerDesc)))
    {
        MSG_BOX("ReadSpawnTriggers Prototype_GameObject_SpawnTrigger Error!");
        return;

    }

    CSpawnTrigger* pCurSpawnTrigger = static_cast<CSpawnTrigger*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GAMEPLAY, TEXT("SpawnTrigger")));

    vector<pair<_vector, _int>> SpawnerInfos;
    vector<CSpawner::SPAWNER_DESC> Descs;

    for (_int i = 0; i < iSpawnerCnt; ++i)
    {
        _vector vSpawnerPos;
        _int iSpawnerType;
        InFile.read((_char*)&vSpawnerPos, sizeof _vector);
        InFile.read((_char*)&iSpawnerType, sizeof _int);

        SpawnerInfos.push_back({ vSpawnerPos, iSpawnerType });

        CSpawner::SPAWNER_DESC desc;
        desc.bUseSpawnPos = true;
        desc.fRotSpeedPerSec = 0.f;
        desc.fSpeedPerSec = 0.f;
        desc.strObjectTag = "Spawner";

        // enum ROT_TYPE { ROT_LEFT, ROT_BACK, ROT_RIGHT, ROT_FRONT, ROT_NONE, };
        if (iDir == -1)
        {
            desc.iInitRotType = iRotType;
        }
        else
        {
            desc.iInitRotType = iDir;
        }


        desc.iLevelIndex = LEVEL_GAMEPLAY;
        XMStoreFloat3(&desc.vSpawnPos, SpawnerInfos[i].first);
        Descs.push_back(desc);
    }

    for (_int i = 0; i < Descs.size(); ++i)
    {
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Spawner"), LEVEL_GAMEPLAY, TEXT("Layer_Spawner"), &Descs[i])))
        {
            MSG_BOX("ReadSpawnTriggers Prototype_GameObject_Spawner Error!");
            return;
        }

        CSpawner* pSpawner = static_cast<CSpawner*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GAMEPLAY, TEXT("Layer_Spawner")));
        pSpawner->SetSpawnMonsterType(static_cast<SPAWN_MONSTER_TYPE>(SpawnerInfos[i].second));
        pSpawner->SetSpawnNavMeshIdx(iNavMeshIdx);

        pCurSpawnTrigger->AddOwnSpawner(pSpawner);
    }
}

HRESULT CLevel_GamePlay::LoadInterMove()
{
    // S1 -> S2
    ReadInterMoves("../Bin/DataFiles/InterMove_S1_To_S2.dat", 0);

    ReadInterMoves("../Bin/DataFiles/InterMove_S2_To_S3.dat", 1);

    ReadInterMoves("../Bin/DataFiles/InterMove_S3_To_Boss.dat", 2);

    return S_OK;
}

void CLevel_GamePlay::ReadInterMoves(string strFilePath, _int iIdx)
{
    static _int g_Num = 0;

    ifstream InFile(strFilePath.c_str(), ios::in | ios_base::binary);
    if (!InFile.is_open())
    {
        MSG_BOX("Read InterMove Error!");
        return;
    }

    _int iSpawnerCnt;
    _int iRotType;
    _vector vCenter;
    _vector vExtents;

    InFile.read((_char*)&iSpawnerCnt, sizeof _int); // <- not use
    InFile.read((_char*)&iRotType, sizeof _int);
    InFile.read((_char*)&vCenter, sizeof _vector);
    InFile.read((_char*)&vExtents, sizeof _vector);


    CInterMove::INTER_MOVE_DESC InterMoveDesc;
    InterMoveDesc.eTriggerType = CTrigger::TYPE_INTER_MOVE;
    InterMoveDesc.bUseSpawnPos = true;
    InterMoveDesc.fRotSpeedPerSec = 0.f;
    InterMoveDesc.fSpeedPerSec = 0.f;
    InterMoveDesc.strObjectTag = "InterMove" + to_string(g_Num++);
    InterMoveDesc.iLevelIndex = LEVEL_GAMEPLAY;


    XMStoreFloat3(&InterMoveDesc.vSpawnPos, vCenter);
    XMStoreFloat3(&InterMoveDesc.vExtents, vExtents);


    vector<CSpawner::SPAWNER_DESC> Descs;
    _vector vInterMoveEndPos;
    _int iDummy;

    InFile.read((_char*)&vInterMoveEndPos, sizeof _vector);
    InFile.read((_char*)&iDummy, sizeof _int); // <- not use



    XMStoreFloat3(&InterMoveDesc.vMoveEndPos, vInterMoveEndPos);

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_InterMove"), LEVEL_GAMEPLAY, TEXT("Layer_InterMove"), &InterMoveDesc)))
    {
        MSG_BOX("Read Prototype_GameObject_InterMove Error!");
        return;

    }

    CInterMove* pInterMove = static_cast<CInterMove*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GAMEPLAY, TEXT("Layer_InterMove")));
    pInterMove->SetInterMoveIdx(CInterMove::INTER_MOVE_IDX(iIdx));

    CSpawner::SPAWNER_DESC desc;
    desc.bUseSpawnPos = true;
    desc.fRotSpeedPerSec = 0.f;
    desc.fSpeedPerSec = 0.f;
    desc.strObjectTag = "Spawner";
    desc.iInitRotType = iRotType;
    XMStoreFloat3(&desc.vSpawnPos, vInterMoveEndPos);
    Descs.push_back(desc);

    for (_int i = 0; i < Descs.size(); ++i)
    {
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_InterMoveEndPos"), LEVEL_GAMEPLAY, TEXT("Layer_InterMoveEndPos"), &Descs[i])))
        {
            MSG_BOX("Read Prototype_GameObject_InterMoveEndPos Error!");
            return;
        }

        CInterMoveEndPos* pInterMoveEndPos = static_cast<CInterMoveEndPos*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GAMEPLAY, TEXT("Layer_InterMoveEndPos")));
        pInterMove->SetInterMoveEndPos(pInterMoveEndPos);
    }
}

HRESULT CLevel_GamePlay::LoadPointLights()
{
    if (FAILED(ReadPointlights("../Bin/DataFiles/S1PointLight.dat", TEXT("Layer_S1Torch"), TEXT("Layer_S1TorchEffect"))))
        return E_FAIL;

    if (FAILED(ReadPointlights("../Bin/DataFiles/S2PointLight.dat", TEXT("Layer_S2Torch"), TEXT("Layer_S2TorchEffect"))))
        return E_FAIL;

    if (FAILED(ReadPointlights("../Bin/DataFiles/S3PointLight.dat", TEXT("Layer_S3Torch"), TEXT("Layer_S3TorchEffect"))))
        return E_FAIL;

    return S_OK;
}

#include <string>
#include <codecvt>
#include <locale>


HRESULT CLevel_GamePlay::ReadPointlights(string strFilePath, const _wstring& strLayerTagTorch, const _wstring& strLayerTagTorchEffect)
{
    ifstream InFile(strFilePath.c_str(), ios::in | ios_base::binary);
    if (!InFile.is_open())
    {
        MSG_BOX("PointLights Error!");
        return E_FAIL;
    }

    _int iSize;
    InFile.read((_char*)&iSize, sizeof _int);

    for (_int i = 0; i < iSize; ++i)
    {
        _float3 vPos;
        _float3 vEx;

        InFile.read((_char*)&vPos, sizeof _float3);
        InFile.read((_char*)&vEx, sizeof _float3);

        LIGHT_DESC	LightDesc{};
        LightDesc.eType = LIGHT_DESC::TYPE_POINT;
        LightDesc.vDiffuse = _float4(1.0f, 0.5f, 0.0f, 1.0f);
        LightDesc.vAmbient = _float4(0.5f, 0.2f, 0.1f, 1.0f);
        LightDesc.vSpecular = _float4({ 1.0f, 0.6f, 0.3f, 1.0f });
        LightDesc.fRange = 5.f;
        LightDesc.vPosition = { vPos.x, vPos.y, vPos.z, 1.f };

        auto iter = find(strFilePath.begin(), strFilePath.end(), 'S');
        string strPre = strFilePath.substr(iter - strFilePath.begin(), 2);

        LightDesc.strLightName = strPre + "TorchLight" + to_string(i);

        if (FAILED(m_pGameInstance->AddLight(LightDesc)))
            return E_FAIL;

        m_pGameInstance->DeActivePointLight(LightDesc.strLightName);

        CGameObject::GAMEOBJECT_DESC Desc = {};
        Desc.bUseSpawnPos = true;
        Desc.fRotSpeedPerSec = 0.f;
        Desc.fSpeedPerSec = 0.f;
        Desc.iLevelIndex = LEVEL_GAMEPLAY;
        Desc.strObjectTag = "Torch" + to_string(i);
        Desc.vSpawnPos = vPos;
        
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Torch"), LEVEL_GAMEPLAY, strLayerTagTorch, &Desc)))
            return E_FAIL;

        // TorchFire
        {
            CTorchFire::TORCH_FIRE_DESC Desc = {};
            Desc.bUseSpawnPos = true;
            Desc.strObjectTag = "TorchFire" + to_string(i);
            Desc.fSpeedPerSec = 0.f;
            Desc.fRotSpeedPerSec = 0.f;
            Desc.iLevelIndex = LEVEL_GAMEPLAY;
            Desc.vSpawnPos = vPos;
            Desc.fNoiseMax = 3.f;

            if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_TorchFire"), LEVEL_GAMEPLAY, strLayerTagTorchEffect, &Desc)))
                return E_FAIL;

    /*        static_cast<CTorchFire*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, Desc.iLevelIndex, TEXT("Layer_Torch")))->SetActive(1);*/
        }
    }

    return S_OK;
}

HRESULT CLevel_GamePlay::LoadUITrigger()
{
    if (FAILED(ReadUITrigger("../Bin/DataFiles/S1UITrigger.dat", TEXT("Layer_S1UITrigger"), "S1UITrigger")))
        return E_FAIL;

    if (FAILED(ReadUITrigger("../Bin/DataFiles/S2UITrigger.dat", TEXT("Layer_S2UITrigger"), "S2UITrigger")))
        return E_FAIL;

    if (FAILED(ReadUITrigger("../Bin/DataFiles/S3UITrigger.dat", TEXT("Layer_S3UITrigger"), "S3UITrigger")))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::ReadUITrigger(string strFilePath, const _wstring& strUITriggerLayerTag, string strUITriggerObjTag)
{
    static _int g_iNum = 0;

    ifstream InFile(strFilePath.c_str(), ios::in | ios_base::binary);
    if (!InFile.is_open())
    {
        MSG_BOX("PointLights Error!");
        return E_FAIL;
    }

    _int iSize;
    InFile.read((_char*)&iSize, sizeof _int);

    for (_int i = 0; i < iSize; ++i)
    {
        _float3 vPos;
        _float3 vEx;

        InFile.read((_char*)&vPos, sizeof _float3);
        InFile.read((_char*)&vEx, sizeof _float3);

        CTrigger::TRIGGER_DESC Desc = {};
        Desc.bUseSpawnPos = true;
        Desc.fRotSpeedPerSec = 0.f;
        Desc.fSpeedPerSec = 0.f;
        Desc.iLevelIndex = LEVEL_GAMEPLAY;
        Desc.strObjectTag = "UITrigger" + to_string(g_iNum++);
        Desc.vSpawnPos = vPos;
        Desc.vCenter = vPos;
        Desc.vExtents = vEx;

        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UITrigger"), Desc.iLevelIndex, strUITriggerLayerTag, &Desc)))
            return E_FAIL;
    }

    return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_GamePlay* pInst = new CLevel_GamePlay(pDevice, pContext);

    if (FAILED(pInst->Initialize()))
    {
        MSG_BOX("Failed to Created : CLevel_GamePlay");
        SafeRelease(pInst);
    }

    return pInst;
}

void CLevel_GamePlay::Free()
{
    __super::Free();
}

