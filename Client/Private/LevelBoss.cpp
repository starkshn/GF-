#include "pch.h"
#include "LevelBoss.h"
#include "GameInstance.h"
#include "Camera_Free.h"
#include "StateObjectContainer.h"
#include "GameObject.h"

#include "Boss.h"
#include "BossBody.h"
#include "BossWeapon.h"

#include "ParticleManager.h"
#include "EffectManager.h"
#include "MonsterManager.h"

#include "WallAABB.h"

#include "Trigger.h"
#include "Spawner.h"
#include "SpawnTrigger.h"

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
#include "LoadingColor.h"


#include "Torch.h"
#include "TorchFire.h"

CLevelBoss::CLevelBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CLevel(pDevice, pContext)
{
}

HRESULT CLevelBoss::Initialize()
{
    auto pObj = m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, TEXT("Layer_Loading_UI"));
    pObj->SetActive(false);

    {
        {
            auto pObj = static_cast<CUIObject*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, TEXT("Layer_LoadingBar")));
            pObj->SetActive(false);
        }

        {
            auto pObj = static_cast<CUIObject*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GLOBAL, TEXT("Layer_LoadingColor")));
            pObj->SetActive(false);

            static_cast<CLoadingColor*>(pObj)->SetTotLoadCnt(0);
            static_cast<CLoadingColor*>(pObj)->SetcurLoadCnt(0);
        }
    }


	if (FAILED(ReadyLights()))
		return E_FAIL;

    if (FAILED(ReadyLayerParticles(TEXT("Layer_Particles"))))
        return E_FAIL;

	if (FAILED(ReadyLayerPlayer(TEXT("A"))))
		return E_FAIL;

	if (FAILED(ReadyLayerStages(TEXT("Layer_BossStage"))))
		return E_FAIL;

    if (FAILED(ReadyLayerCameraFree(TEXT("B"))))
        return E_FAIL;

    if (FAILED(ReadyBossRock(TEXT("Layer_BossRock"))))
        return E_FAIL;

    if (FAILED(ReadyMeteors(TEXT("Layer_Meteor"))))
        return E_FAIL;

    if (FAILED(ReadyLayerBoss(TEXT("Layer_Boss"))))
        return E_FAIL;

    if (FAILED(ReadySkyBox(TEXT("Layer_SkyBox"))))
        return E_FAIL;

    // ===============================================================================
    // Read .dat Files...
    // ===============================================================================
    {
        if (FAILED(LoadAABB()))
            return E_FAIL;

        if (FAILED(LoadSpawnTriggers()))
            return E_FAIL;

        if (FAILED(LoadPointLights()))
            return E_FAIL;
    }
    
    CMonsterManager::GetInstance()->SetUpdateDoor(false);

    m_pGameInstance->PlayBGM(TEXT("BossBGM.mp3"), 0.2f);

	return S_OK;
}

void CLevelBoss::Update(const _float& fTimeDelta)
{
    CParticleManager::GetInstance()->Update(fTimeDelta);
    CEffectManager::GetInstance()->Update(fTimeDelta);
    CMonsterManager::GetInstance()->Update(fTimeDelta);
}

HRESULT CLevelBoss::Render()
{
    SetWindowText(g_hWnd, TEXT("Boss 레벨입니다."));
	return S_OK;
}

HRESULT CLevelBoss::ReadyLights()
{
    LIGHT_DESC	LightDesc{};
    LightDesc.eType = LIGHT_DESC::TYPE_POINT;
    LightDesc.vDiffuse = _float4(0.0f, 0.0f, 1.f, 1.f);
    LightDesc.vAmbient = _float4(0.0f, 0.0f, 1.f, 1.f);
    LightDesc.vSpecular = _float4(0.f, 0.f, 1.f, 1.f);
    LightDesc.fRange = 20.f;
    LightDesc.vPosition = { 0.f, 0.f, 8.f, 1.f };
    LightDesc.strLightName = "BossRock";

    if (FAILED(m_pGameInstance->AddLight(LightDesc)))
        return E_FAIL;

    m_pGameInstance->DeActivePointLight("BossRock");

    // M1
    {
        LIGHT_DESC	LightDesc{};
        LightDesc.eType = LIGHT_DESC::TYPE_POINT;
        LightDesc.vDiffuse = _float4(0.0f, 0.0f, 1.f, 1.f);
        LightDesc.vAmbient = _float4(0.0f, 0.0f, 1.f, 1.f);
        LightDesc.vSpecular = _float4(0.f, 0.f, 1.f, 1.f);
        LightDesc.fRange = 20.f;
        LightDesc.vPosition = { 0.f, 0.f, 8.f, 1.f };
        LightDesc.strLightName = "M1";

        if (FAILED(m_pGameInstance->AddLight(LightDesc)))
            return E_FAIL;

        m_pGameInstance->DeActivePointLight("M1");
    }

    // M2
    {
        LIGHT_DESC	LightDesc{};
        LightDesc.eType = LIGHT_DESC::TYPE_POINT;
        LightDesc.vDiffuse = _float4(0.0f, 0.0f, 1.f, 1.f);
        LightDesc.vAmbient = _float4(0.0f, 0.0f, 1.f, 1.f);
        LightDesc.vSpecular = _float4(0.f, 0.f, 1.f, 1.f);
        LightDesc.fRange = 20.f;
        LightDesc.vPosition = { 0.f, 0.f, 8.f, 1.f };
        LightDesc.strLightName = "M2";

        if (FAILED(m_pGameInstance->AddLight(LightDesc)))
            return E_FAIL;

        m_pGameInstance->DeActivePointLight("M2");
    }

    // M3
    {
        LIGHT_DESC	LightDesc{};
        LightDesc.eType = LIGHT_DESC::TYPE_POINT;
        LightDesc.vDiffuse = _float4(0.0f, 0.0f, 1.f, 1.f);
        LightDesc.vAmbient = _float4(0.0f, 0.0f, 1.f, 1.f);
        LightDesc.vSpecular = _float4(0.f, 0.f, 1.f, 1.f);
        LightDesc.fRange = 20.f;
        LightDesc.vPosition = { 0.f, 0.f, 8.f, 1.f };
        LightDesc.strLightName = "M3";

        if (FAILED(m_pGameInstance->AddLight(LightDesc)))
            return E_FAIL;

        m_pGameInstance->DeActivePointLight("M3");
    }

    // M4
    {
        LIGHT_DESC	LightDesc{};
        LightDesc.eType = LIGHT_DESC::TYPE_POINT;
        LightDesc.vDiffuse = _float4(0.0f, 0.0f, 1.f, 1.f);
        LightDesc.vAmbient = _float4(0.0f, 0.0f, 1.f, 1.f);
        LightDesc.vSpecular = _float4(0.f, 0.f, 1.f, 1.f);
        LightDesc.fRange = 20.f;
        LightDesc.vPosition = { 0.f, 0.f, 8.f, 1.f };
        LightDesc.strLightName = "M4";

        if (FAILED(m_pGameInstance->AddLight(LightDesc)))
            return E_FAIL;

        m_pGameInstance->DeActivePointLight("M4");
    }

    // M5
    {
        LIGHT_DESC	LightDesc{};
        LightDesc.eType = LIGHT_DESC::TYPE_POINT;
        LightDesc.vDiffuse = _float4(0.0f, 0.0f, 1.f, 1.f);
        LightDesc.vAmbient = _float4(0.0f, 0.0f, 1.f, 1.f);
        LightDesc.vSpecular = _float4(0.f, 0.f, 1.f, 1.f);
        LightDesc.fRange = 20.f;
        LightDesc.vPosition = { 0.f, 0.f, 8.f, 1.f };
        LightDesc.strLightName = "M5";

        if (FAILED(m_pGameInstance->AddLight(LightDesc)))
            return E_FAIL;

        m_pGameInstance->DeActivePointLight("M5");
    }

    // M6
    {
        LIGHT_DESC	LightDesc{};
        LightDesc.eType = LIGHT_DESC::TYPE_POINT;
        LightDesc.vDiffuse = _float4(0.0f, 0.0f, 1.f, 1.f);
        LightDesc.vAmbient = _float4(0.0f, 0.0f, 1.f, 1.f);
        LightDesc.vSpecular = _float4(0.f, 0.f, 1.f, 1.f);
        LightDesc.fRange = 20.f;
        LightDesc.vPosition = { 0.f, 0.f, 8.f, 1.f };
        LightDesc.strLightName = "M6";

        if (FAILED(m_pGameInstance->AddLight(LightDesc)))
            return E_FAIL;

        m_pGameInstance->DeActivePointLight("M6");
    }

    // M7
    {
        LIGHT_DESC	LightDesc{};
        LightDesc.eType = LIGHT_DESC::TYPE_POINT;
        LightDesc.vDiffuse = _float4(0.0f, 0.0f, 1.f, 1.f);
        LightDesc.vAmbient = _float4(0.0f, 0.0f, 1.f, 1.f);
        LightDesc.vSpecular = _float4(0.f, 0.f, 1.f, 1.f);
        LightDesc.fRange = 20.f;
        LightDesc.vPosition = { 0.f, 0.f, 8.f, 1.f };
        LightDesc.strLightName = "M7";

        if (FAILED(m_pGameInstance->AddLight(LightDesc)))
            return E_FAIL;

        m_pGameInstance->DeActivePointLight("M7");
    }

    // M8
    {
        LIGHT_DESC	LightDesc{};
        LightDesc.eType = LIGHT_DESC::TYPE_POINT;
        LightDesc.vDiffuse = _float4(0.0f, 0.0f, 1.f, 1.f);
        LightDesc.vAmbient = _float4(0.0f, 0.0f, 1.f, 1.f);
        LightDesc.vSpecular = _float4(0.f, 0.f, 1.f, 1.f);
        LightDesc.fRange = 20.f;
        LightDesc.vPosition = { 0.f, 0.f, 8.f, 1.f };
        LightDesc.strLightName = "M8";

        if (FAILED(m_pGameInstance->AddLight(LightDesc)))
            return E_FAIL;

        m_pGameInstance->DeActivePointLight("M8");
    }

    // M9
    {
        LIGHT_DESC	LightDesc{};
        LightDesc.eType = LIGHT_DESC::TYPE_POINT;
        LightDesc.vDiffuse = _float4(0.0f, 0.0f, 1.f, 1.f);
        LightDesc.vAmbient = _float4(0.0f, 0.0f, 1.f, 1.f);
        LightDesc.vSpecular = _float4(0.f, 0.f, 1.f, 1.f);
        LightDesc.fRange = 20.f;
        LightDesc.vPosition = { 0.f, 0.f, 8.f, 1.f };
        LightDesc.strLightName = "M9";

        if (FAILED(m_pGameInstance->AddLight(LightDesc)))
            return E_FAIL;

        m_pGameInstance->DeActivePointLight("M9");
    }

    // M10
    {
        LIGHT_DESC	LightDesc{};
        LightDesc.eType = LIGHT_DESC::TYPE_POINT;
        LightDesc.vDiffuse = _float4(0.0f, 0.0f, 1.f, 1.f);
        LightDesc.vAmbient = _float4(0.0f, 0.0f, 1.f, 1.f);
        LightDesc.vSpecular = _float4(0.f, 0.f, 1.f, 1.f);
        LightDesc.fRange = 20.f;
        LightDesc.vPosition = { 0.f, 0.f, 8.f, 1.f };
        LightDesc.strLightName = "M10";

        if (FAILED(m_pGameInstance->AddLight(LightDesc)))
            return E_FAIL;

        m_pGameInstance->DeActivePointLight("M10");
    }




    return S_OK;
}

HRESULT CLevelBoss::ReadySkyBox(const _wstring& strLayerTag)
{
    CGameObject::GAMEOBJECT_DESC Desc = {};
    Desc.iLevelIndex = LEVEL_BOSS;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_SkyBox"), LEVEL_BOSS, strLayerTag, &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevelBoss::ReadyBossRock(const _wstring& strLayerTag)
{
    CGameObject::GAMEOBJECT_DESC     Desc = {};
    Desc.bUseSpawnPos = false;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.fSpeedPerSec = 40.f;
    Desc.iLevelIndex = LEVEL_BOSS;
    Desc.strObjectTag = "BossRock";
    Desc.vSpawnPos = _float3{ 0.f, 0.f, 0.f };
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_BOSS, TEXT("Prototype_GameObject_BossRock"), LEVEL_BOSS, strLayerTag, &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevelBoss::ReadyMeteors(const _wstring& strLayerTag)
{
    CGameObject::GAMEOBJECT_DESC     Desc = {};
    Desc.bUseSpawnPos = false;
    Desc.fRotSpeedPerSec = 0.f;
    Desc.fSpeedPerSec = 40.f;
    Desc.iLevelIndex = LEVEL_BOSS;
    Desc.strObjectTag = "Meteor";
    Desc.vSpawnPos = _float3{ 0.f, 0.f, 0.f };

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_BOSS, TEXT("Prototype_GameObject_Meteor"), LEVEL_BOSS, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_BOSS, TEXT("Prototype_GameObject_Meteor"), LEVEL_BOSS, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_BOSS, TEXT("Prototype_GameObject_Meteor"), LEVEL_BOSS, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_BOSS, TEXT("Prototype_GameObject_Meteor"), LEVEL_BOSS, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_BOSS, TEXT("Prototype_GameObject_Meteor"), LEVEL_BOSS, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_BOSS, TEXT("Prototype_GameObject_Meteor"), LEVEL_BOSS, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_BOSS, TEXT("Prototype_GameObject_Meteor"), LEVEL_BOSS, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_BOSS, TEXT("Prototype_GameObject_Meteor"), LEVEL_BOSS, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_BOSS, TEXT("Prototype_GameObject_Meteor"), LEVEL_BOSS, strLayerTag, &Desc))) return E_FAIL;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_BOSS, TEXT("Prototype_GameObject_Meteor"), LEVEL_BOSS, strLayerTag, &Desc))) return E_FAIL;
    return S_OK;
}

HRESULT CLevelBoss::ReadySkyBox()
{
    CGameObject::GAMEOBJECT_DESC Desc = {};
    Desc.iLevelIndex = LEVEL_BOSS;
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_SkyBox"), LEVEL_BOSS, TEXT("Layer_SkyBox"), &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevelBoss::LoadAABB()
{
    ReadAABB("../Bin/DataFiles/BossAABB.dat");
    return S_OK;
}

void CLevelBoss::ReadAABB(string strFilePath)
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


        if (AlmostEqual(vPos.y, -0.2f))
        {
            _int a = 10;
            // vPos.y = 1.f;
        }


        CWallAABB::WALL_AABB_DESC Desc = {};
        Desc.bUseSpawnPos = true;
        Desc.fRotSpeedPerSec = 0.f;
        Desc.fSpeedPerSec = 0.f;
        Desc.strObjectTag = "WallAABB";
        // Desc.vCenter = _float3{ vPos.x , vPos.y, vPos.z };
        Desc.vSpawnPos = vPos;
        Desc.vExtents = vEx;
        Desc.iLevelIndex = LEVEL_BOSS;

        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_WallAABB"), Desc.iLevelIndex, TEXT("Layer_WallAABB"), &Desc)))
        {
            MSG_BOX("ReadSpawnTriggers Prototype_GameObject_WallAABB Error!");
            return;
        }
    }
}

HRESULT CLevelBoss::LoadSpawnTriggers()
{
    ReadSpawnTriggers("../Bin/DataFiles/SpawnTriggerBoss.dat", (_int)CStateObjectContainer::NAVMESH_IDX::S_BOSS);
    ReadSpawnTriggers("../Bin/DataFiles/SpawnTriggerBoss2.dat", (_int)CStateObjectContainer::NAVMESH_IDX::S_BOSS);

    return S_OK;
}

void CLevelBoss::ReadSpawnTriggers(string strFilePath, _int iNavMeshIdx, _int iDir)
{
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
    SpawnTriggerDesc.strObjectTag = "SpawnTrigger";
    SpawnTriggerDesc.iLevelIndex = LEVEL_BOSS;

    XMStoreFloat3(&SpawnTriggerDesc.vSpawnPos, vCenter);
    XMStoreFloat3(&SpawnTriggerDesc.vExtents, vExtents);

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_SpawnTrigger"), SpawnTriggerDesc.iLevelIndex, TEXT("Layer_SpawnTrigger"), &SpawnTriggerDesc)))
    {
        MSG_BOX("ReadSpawnTriggers Prototype_GameObject_SpawnTrigger Error!");
        return;

    }

    CSpawnTrigger* pCurSpawnTrigger = static_cast<CSpawnTrigger*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_BOSS, TEXT("Layer_SpawnTrigger")));

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



        desc.iInitRotType = iRotType;
        desc.iLevelIndex = LEVEL_BOSS;
        XMStoreFloat3(&desc.vSpawnPos, SpawnerInfos[i].first);
        Descs.push_back(desc);
    }

    for (_int i = 0; i < Descs.size(); ++i)
    {
        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Spawner"), LEVEL_BOSS, TEXT("Layer_Spawner"), &Descs[i])))
        {
            MSG_BOX("ReadSpawnTriggers Prototype_GameObject_Spawner Error!");
            return;
        }

        CSpawner* pSpawner = static_cast<CSpawner*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_BOSS, TEXT("Layer_Spawner")));
        pSpawner->SetSpawnMonsterType(static_cast<SPAWN_MONSTER_TYPE>(SpawnerInfos[i].second));
        pSpawner->SetSpawnNavMeshIdx(iNavMeshIdx);
        pCurSpawnTrigger->AddOwnSpawner(pSpawner);
    }
}

HRESULT CLevelBoss::LoadPointLights()
{
    if (FAILED(ReadPointlights("../Bin/DataFiles/BossPointLight.dat", TEXT("Layer_BossTorch"), TEXT("Layer_BossTorchEffect"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevelBoss::ReadPointlights(string strFilePath, const _wstring& strLayerTagTorch, const _wstring& strLayerTagTorchEffect)
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
        LightDesc.fRange = 13.f;
        LightDesc.vPosition = { vPos.x, vPos.y, vPos.z, 1.f };

        auto iter = find(strFilePath.begin(), strFilePath.end(), 'S');
        string strPre = strFilePath.substr(iter - strFilePath.begin(), 2);

        LightDesc.strLightName = strPre + "TorchLight" + to_string(i);

        if (FAILED(m_pGameInstance->AddLight(LightDesc)))
            return E_FAIL;


        CGameObject::GAMEOBJECT_DESC Desc = {};
        Desc.bUseSpawnPos = true;
        Desc.fRotSpeedPerSec = 0.f;
        Desc.fSpeedPerSec = 0.f;
        Desc.iLevelIndex = LEVEL_BOSS;
        Desc.strObjectTag = "Torch" + to_string(i);
        Desc.vSpawnPos = vPos;

        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Torch"), LEVEL_BOSS, strLayerTagTorch, &Desc)))
            return E_FAIL;

        // TorchFire
        {
            CTorchFire::TORCH_FIRE_DESC Desc = {};
            Desc.bUseSpawnPos = true;
            Desc.strObjectTag = "TorchFire" + to_string(i);
            Desc.fSpeedPerSec = 0.f;
            Desc.fRotSpeedPerSec = 0.f;
            Desc.iLevelIndex = LEVEL_BOSS;
            Desc.vSpawnPos = vPos;
            Desc.fNoiseMax = 3.f;

            if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Effect_TorchFire"), LEVEL_BOSS, strLayerTagTorchEffect, &Desc)))
                return E_FAIL;

            static_cast<CEffect*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_BOSS, strLayerTagTorchEffect))->Active();
        }
    }

    return S_OK;
}

HRESULT CLevelBoss::ReadyLayerCameraFree(const _wstring& strLayerTag)
{
    CCamera_Free::CAMERA_FREE_DESC		Desc{};

    Desc.fSpeedPerSec = 10.f;
    Desc.fRotSpeedPerSec = XMConvertToRadians(180.f);
    Desc.fMouseSensor = 0.1f;
    Desc.fFovY = XMConvertToRadians(60.f);
    Desc.fAspectRatio = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
    Desc.fNear = 0.1f;
    Desc.fFar = 1000.f;
    Desc.vEye = _float3(0.f, 2.f, -7.f);
    Desc.vAt = _float3(0.f, 0.f, 0.f);
    Desc.iLevelIndex = LEVEL_BOSS;

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Camera_Free"), LEVEL_BOSS, strLayerTag, &Desc))) 
        return E_FAIL;

    CCamera_Free* pCam = static_cast<CCamera_Free*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_BOSS, TEXT("B")));
    if (pCam)
    {
        pCam->SetCameraEffectType(CCamera_Free::EFFECT_CINEMATIC);

    }


    return S_OK;
}

HRESULT CLevelBoss::ReadyLayerParticles(const _wstring& strLayerTag)
{
    // PLAYER GUN FIRE
    {
        CGameObject::GAMEOBJECT_DESC Desc = {};
        Desc.strObjectTag = "PlayerGunFire";

        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_PlayerGunFire"), LEVEL_BOSS, strLayerTag, &Desc)))
            return E_FAIL;
    }
    // MONSTER HIT
    {
        CGameObject::GAMEOBJECT_DESC Desc = {};
        Desc.strObjectTag = "MonsterHit";

        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Particle_MonsterHit"), LEVEL_BOSS, strLayerTag, &Desc)))
            return E_FAIL;
    }
   

    return S_OK;
}


HRESULT CLevelBoss::ReadyLayerPlayer(const _wstring& strLayerTag)
{
    CContainerObject::CONTAINER_OBJECT_DESC Desc = {};
    Desc.iNumPartObjects = CStateObjectContainer::PARTOBJECT::PART_END;
    Desc.fSpeedPerSec = 5.f;
    Desc.fRotSpeedPerSec = XMConvertToRadians(90.f);
    Desc.strObjectTag = "Player";
    Desc.iLevelIndex = LEVEL_BOSS;

    // Desc.
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Player"), LEVEL_BOSS, strLayerTag, &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevelBoss::ReadyLayerStages(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_BOSS, TEXT("Prototype_GameObject_BossStage"), LEVEL_BOSS, strLayerTag)))  
        return E_FAIL;

    return S_OK;
}

HRESULT CLevelBoss::ReadyLayerBoss(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_BOSS, TEXT("Prototype_GameObject_Boss"), LEVEL_BOSS, strLayerTag)))
        return E_FAIL;

    auto pBoss = m_pGameInstance->GetGameObjctInLayerBack(LEVEL_BOSS, LEVEL_BOSS, TEXT("Layer_Boss"));
    if (!pBoss)
        return E_FAIL;

    if (FAILED(ReadyLayerBossUI(pBoss, LEVEL_BOSS, 2000)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevelBoss::ReadyLayerBossUI(CGameObject* pOwner, _int iLevelIdx, _int iMaxHP)
{
    vector<string> strNames;

    // Boss HP
    {
        _wstring strLayerTag = TEXT("Layer_UI_BG_BossHP");
        auto ViewDesc = m_pGameInstance->GetViewPortDesc();
        _float fSX = ViewDesc.Width;
        _float fSY = ViewDesc.Height;

        CUIBG::UI_BG_DESC PanelDesc = {};
        PanelDesc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
        PanelDesc.eUIType = UI_TYPE::UI_PANEL;
        PanelDesc.bUseSpawnPos = false;
        PanelDesc.fSpeedPerSec = 0.f;
        PanelDesc.fSpeedPerSec = 0.f;
        PanelDesc.iDepth = 5;
        PanelDesc.eBGType = CUIBG::UI_BG_TYPE::BOSS_HP;
        PanelDesc.fSSizeX = 550.f;
        PanelDesc.fSSizeY = 60.f;
        PanelDesc.fSX = fSX * 0.5f;
        PanelDesc.fSY = PanelDesc.fSSizeY * 0.5f + 25.f;
        PanelDesc.strObjectTag = "UIBGBossHP";

        strNames.push_back(PanelDesc.strObjectTag);

        if (!pOwner)
            return E_FAIL;
        
        // 주인 등록
        PanelDesc.pUIOwner = pOwner;


        if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_BG"), iLevelIdx, strLayerTag, &PanelDesc))) return E_FAIL;

        CUIPanel* pPanel = static_cast<CUIPanel*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, iLevelIdx, strLayerTag));
        if (pPanel)
            m_pGameInstance->AddPanelUIToManager(PanelDesc.strObjectTag, pPanel);
        else
            return E_FAIL;

        // BossIcon
        {
            CUITex::UI_TEX_DESC Desc = {};
            Desc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
            Desc.eUIType = UI_TYPE::UI_TEX;
            Desc.fSSizeX = 100;
            Desc.fSSizeY = 100;
            Desc.fSX = 0.f;
            Desc.fSY = 0.f;
            Desc.bUseSpawnPos = false;
            Desc.fSpeedPerSec = 0.f;
            Desc.fSpeedPerSec = 0.f;
            Desc.iDepth = 0;
            Desc.eUITexType = CUITex::UI_TEX_TYPE::BOSS_ICON;
            Desc.strObjectTag = "UIBossIcon";

            if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Tex"), iLevelIdx, strLayerTag, &Desc))) return E_FAIL;
            CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, iLevelIdx, strLayerTag));
            if (pObj)
                m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
            else
                return E_FAIL;
            m_pGameInstance->AddElementToPanel
            (PanelDesc.strObjectTag, Desc.strObjectTag, -PanelDesc.fSSizeX * 0.5f, 0.f);
        }

        // BossHPBGBlue
        {
            CUITex::UI_TEX_DESC Desc = {};
            Desc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
            Desc.eUIType = UI_TYPE::UI_TEX;
            Desc.fSSizeX = 460.f;
            Desc.fSSizeY = 10.f;
            Desc.fSX = 0.f;
            Desc.fSY = 0.f;
            Desc.bUseSpawnPos = false;
            Desc.fSpeedPerSec = 0.f;
            Desc.fSpeedPerSec = 0.f;
            Desc.iDepth = 2;
            Desc.eUITexType = CUITex::UI_TEX_TYPE::BOSS_HP_BG;
            Desc.strObjectTag = "UIBossGPBGBlue";

            if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Tex"), iLevelIdx, strLayerTag, &Desc))) return E_FAIL;
            CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, iLevelIdx, strLayerTag));
            if (pObj)
                m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
            else
                return E_FAIL;
            m_pGameInstance->AddElementToPanel
            (PanelDesc.strObjectTag, Desc.strObjectTag, 0.f, -7.f);

            // HPBar
            {
                _wstring strLayerTag = TEXT("Layer_UI_BossHP");

                CUIHPBar::UI_HP_BAR_DESC Desc = {};
                Desc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
                Desc.eUIType = UI_TYPE::UI_BAR;
                Desc.fSX = 0;
                Desc.fSY = -7.f;
                Desc.fSSizeX = 460.f;
                Desc.fSSizeY = 10.f;
                Desc.bUseSpawnPos = false;
                Desc.fSpeedPerSec = 0.f;
                Desc.fSpeedPerSec = 0.f;
                Desc.iDepth = 0;
                Desc.eHPType = CUIHPBar::HP_MONSTER;
                Desc.pPerspectiveUIOwner = pPanel;
                Desc.strObjectTag = "UIBossHPBarBlue";
                Desc.eHPBarColor = CUIHPBar::HP_BLUE;
                Desc.fMaxHPBarVal = (_float)(iMaxHP / 2.f);

                strNames.push_back(Desc.strObjectTag);


                if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_HPBar"), iLevelIdx, strLayerTag, &Desc))) return E_FAIL;
                CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, iLevelIdx, strLayerTag));
                if (pObj)
                    m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
                else
                    return E_FAIL;

                m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, 0.f, -7.f);
            }
        }

        // BossHPBGRed
        {
            CUITex::UI_TEX_DESC Desc = {};
            Desc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
            Desc.eUIType = UI_TYPE::UI_TEX;
            Desc.fSSizeX = 460;
            Desc.fSSizeY = 18;
            Desc.fSX = 0.f;
            Desc.fSY = 0.f;
            Desc.bUseSpawnPos = false;
            Desc.fSpeedPerSec = 0.f;
            Desc.fSpeedPerSec = 0.f;
            Desc.iDepth = 2;
            Desc.eUITexType = CUITex::UI_TEX_TYPE::BOSS_HP_BG;
            Desc.strObjectTag = "UIBossGPBGRed";

            if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Tex"), iLevelIdx, strLayerTag, &Desc))) return E_FAIL;
            CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, iLevelIdx, strLayerTag));
            if (pObj)
                m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
            else
                return E_FAIL;
            m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, 0.f, 11.f);

            // HPBar
            {
                CUIHPBar::UI_HP_BAR_DESC Desc = {};
                Desc.eShowType = UI_SHOW_TYPE::UI_ORTHO; // 직교
                Desc.eUIType = UI_TYPE::UI_BAR;
                Desc.fSX = 0;
                Desc.fSY = 11.f;
                Desc.fSSizeX = 460;
                Desc.fSSizeY = 18.f;
                Desc.bUseSpawnPos = false;
                Desc.fSpeedPerSec = 0.f;
                Desc.fSpeedPerSec = 0.f;
                Desc.iDepth = 0;
                Desc.eHPType = CUIHPBar::HP_MONSTER;
                Desc.pPerspectiveUIOwner = pPanel;
                Desc.strObjectTag = "UIBossHPBarRed";
                strNames.push_back(Desc.strObjectTag);

                Desc.eHPBarColor = CUIHPBar::HP_RED;
                Desc.fMaxHPBarVal = (_float)(iMaxHP / 2.f);

                if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_HPBar"), iLevelIdx, strLayerTag, &Desc))) 
                    return E_FAIL;
                CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, iLevelIdx, strLayerTag));
                if (pObj)
                    m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
                else
                    return E_FAIL;

                m_pGameInstance->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, 0.f, 11.f);
            }
        }
    }

    CMonsterBase* pMon = static_cast<CMonsterBase*>(pOwner);
    pMon->SetHPBarNames(strNames);


    // Victory UI
    {
        if (FAILED(ReadyLayerUI()))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CLevelBoss::ReadyLayerUI()
{
    _wstring strLayerTag = TEXT("Layer_UI_Voctory");

    auto ViewDesc = m_pGameInstance->GetViewPortDesc();

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
    Desc.eUITexType = CUITex::UI_TEX_TYPE::VICTORY;
    Desc.eUITexEffectType = CUITex::UI_TEX_EFFECT_DEFAULT;
    Desc.strObjectTag = "UITexVictory";

    if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Tex"), LEVEL_BOSS, strLayerTag, &Desc)))
        return E_FAIL;
    CUIElement* pObj = static_cast<CUIElement*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_BOSS, strLayerTag));
    m_pGameInstance->AddElementUIToManager(Desc.strObjectTag, pObj);
    pObj->DeActiveRender();

    return S_OK;
}

CLevelBoss* CLevelBoss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevelBoss* pInst = new CLevelBoss(pDevice, pContext);

	if (FAILED(pInst->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevelBoss");
		SafeRelease(pInst);
	}

	return pInst;
}

void CLevelBoss::Free()
{
	__super::Free();

	
}
