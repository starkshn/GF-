#include "pch.h"
#include "Spawner.h"
#include "BoundingAABB.h"
#include "GameInstance.h"
#include "MonsterBase.h"
#include "ParticleManager.h"
#include "UIParent.h"
#include "UIPanel.h"
#include "UIElement.h"
#include "UIFont.h"
#include "UIHPBar.h"
#include "UIBG.h"
#include "UITex.h"


CSpawner::CSpawner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CGameObject(pDevice, pContext)
{
}

CSpawner::CSpawner(const CSpawner& Prototype)
    :
    CGameObject(Prototype)
{

}

HRESULT CSpawner::InitializePrototype()
{
    return S_OK;
}

HRESULT CSpawner::Initialize(void* pArg)
{
    if (nullptr != pArg)
    {
        SPAWNER_DESC*  pDesc = static_cast<SPAWNER_DESC*>(pArg);
        m_iInitRotType = pDesc->iInitRotType;
    }

    if (__super::Initialize(pArg))
        return E_FAIL;

#ifdef _DEBUG
    if (ReadyComponents())
        return E_FAIL;
#endif

    return S_OK;
}

void CSpawner::PriorityUpdate(_float fTimeDelta)
{

}

static _float fAcc = 0.f;
static _bool bOnce = 1;
static _bool bOnce2 = true;


void CSpawner::Update(_float fTimeDelta)
{
    m_pColliderCom->Update(m_pTransformCom->GetWorldMatrix());


    if (!bOnce)
    {
        fAcc += fTimeDelta;
        if (fAcc > 1.f)
        {
            bOnce = true;
            bOnce2 = true;
            fAcc = 0.f;
        }

        if (fAcc >= 0.25f && bOnce2 == true)
        {   
            bOnce2 = false;
            m_pGameInst->PlayEffect(TEXT("MonsterSpawn2.mp3"), 1.f);
        }
    }
}

void CSpawner::LateUpdate(_float fTimeDelta)
{
    m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CSpawner::Render()
{
    // m_pColliderCom->Render(_fvector{ 1.f, 0.f, 0.f, 1.f });
    return S_OK;
}

#ifdef _DEBUG

HRESULT CSpawner::SpawnMonster()
{
    if (bOnce)
        m_pGameInst->PlayEffect(TEXT("MonsterSpawn.mp3"), 1.f);

    // ================================
    // ONCE
    // ================================
    bOnce = 0;


    _vector vTempSpawnPos;
    switch (m_eSpawnMonsterType)
    {
    case Engine::MONSTER01:
    {
        CMonsterBase::MONSTER_DESC Desc = {};
        Desc.iNumPartObjects = CMonsterBase::PART_END;
        Desc.fSpeedPerSec = 3.f;
        Desc.fRotSpeedPerSec = XMConvertToRadians(180.f);
        Desc.strObjectTag = "Monster";
        Desc.bUseSpawnPos = true;
        Desc.iNumAttackRanges = 2;
        Desc.iNavMeshIdx = GetspawnNavMeshIdx();
        Desc.iHP = 150;

        //_float vAttackRanges[2] = { 3.f, 3.5f };
        //Desc.pAttackRanges = vAttackRanges;

        //_float vAttackCoolTimes[2] = { 0.f, 1.5f };
        //Desc.pAttackCoolTimes = vAttackCoolTimes;



        _float vAttackRanges[1] = { 3.3f };
        Desc.pAttackRanges = vAttackRanges;

        _float vAttackCoolTimes[1] = { 0.f };
        Desc.pAttackCoolTimes = vAttackCoolTimes;

        Desc.iLevelIndex = m_iLevelIndex;

        // TODO -> 여기서 파일에서 읽어와서 값을 써야한다.
        _vector vSpawnPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);
        XMStoreFloat3(&Desc.vSpawnPos, vSpawnPos);
        vTempSpawnPos = vSpawnPos;

        if (FAILED(m_pGameInst->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Monster01"), Desc.iLevelIndex, TEXT("Layer_Monster01"), &Desc)))
            return E_FAIL;

        m_strLayerTag = TEXT("Layer_Monster01");

        // enum ROT_TYPE { ROT_LEFT, ROT_BACK, ROT_RIGHT, ROT_FRONT, ROT_NONE, };
        if (0 == m_iInitRotType)
        {
            static_cast<CTransform*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, Desc.iLevelIndex, TEXT("Layer_Monster01"))->GetComponent(TEXT("Com_Transform")))->Rotation(_vector{ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(-90.f));
        }
        else if (1 == m_iInitRotType)
        {
            static_cast<CTransform*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, Desc.iLevelIndex, TEXT("Layer_Monster01"))->GetComponent(TEXT("Com_Transform")))->Rotation(_vector{ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f));
        }
        else if (2 == m_iInitRotType)
        {
            static_cast<CTransform*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, Desc.iLevelIndex, TEXT("Layer_Monster01"))->GetComponent(TEXT("Com_Transform")))->Rotation(_vector{ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(90.f));
        }
        else if (3 == m_iInitRotType)
        {
            // static_cast<CTransform*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GAMEPLAY, LEVEL_GAMEPLAY, TEXT("Layer_Monster01"))->GetComponent(TEXT("Com_Transform")))->Rotation(_fvector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(0.f));
        }

        m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, Desc.iLevelIndex, TEXT("Layer_Monster01"))->SetActive(true);

        if (FAILED(CreateMonsterHPBar(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, Desc.iLevelIndex, TEXT("Layer_Monster01")), Desc.iLevelIndex, Desc.iHP)))
            return E_FAIL;
    }
    break;
    case Engine::MONSTER02:
    {
        CMonsterBase::MONSTER_DESC Desc = {};
        Desc.iNumPartObjects = CMonsterBase::PART_END;
        Desc.fSpeedPerSec = 5.f;
        Desc.fRotSpeedPerSec = XMConvertToRadians(180.f);
        Desc.strObjectTag = "Monster";
        Desc.bUseSpawnPos = true;
        Desc.iNavMeshIdx = GetspawnNavMeshIdx();
        Desc.iHP = 150;


        _float vAttackRanges[2] = { 4.5f, 9.f };
        Desc.pAttackRanges = vAttackRanges;
        Desc.iNumAttackRanges = 2;

        _float vAttackCoolTimes[2] = { 0.f, 7.f };
        Desc.pAttackCoolTimes = vAttackCoolTimes;
        Desc.iLevelIndex = m_iLevelIndex;


        // TODO -> 여기서 파일에서 읽어와서 값을 써야한다.
        _vector vSpawnPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);
        XMStoreFloat3(&Desc.vSpawnPos, vSpawnPos);
        vTempSpawnPos = vSpawnPos;


        if (FAILED(m_pGameInst->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Monster02"), Desc.iLevelIndex, TEXT("Layer_Monster02"), &Desc)))
            return E_FAIL;

        m_strLayerTag = TEXT("Layer_Monster02");


        // enum ROT_TYPE { ROT_LEFT, ROT_BACK, ROT_RIGHT, ROT_FRONT, ROT_NONE, };
        if (0 == m_iInitRotType)
        {
            static_cast<CTransform*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, Desc.iLevelIndex, TEXT("Layer_Monster02"))->GetComponent(TEXT("Com_Transform")))->Rotation(_vector{ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(-90.f));
        }
        else if (1 == m_iInitRotType)
        {
            static_cast<CTransform*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, Desc.iLevelIndex, TEXT("Layer_Monster02"))->GetComponent(TEXT("Com_Transform")))->Rotation(_vector{ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f));
        }
        else if (2 == m_iInitRotType)
        {
            static_cast<CTransform*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, Desc.iLevelIndex, TEXT("Layer_Monster02"))->GetComponent(TEXT("Com_Transform")))->Rotation(_vector{ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(90.f));
        }
        else if (3 == m_iInitRotType)
        {
        }

        m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, Desc.iLevelIndex, TEXT("Layer_Monster02"))->SetActive(true);

        if (FAILED(CreateMonsterHPBar(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, Desc.iLevelIndex, TEXT("Layer_Monster02")), Desc.iLevelIndex, Desc.iHP)))
            return E_FAIL;
    }

    break;
    case Engine::MONSTER03:
    {
        CMonsterBase::MONSTER_DESC Desc = {};
        Desc.iNumPartObjects = CMonsterBase::PART_END;
        Desc.fSpeedPerSec = 3.5f;
        Desc.fRotSpeedPerSec = XMConvertToRadians(180.f);
        Desc.strObjectTag = "Monster";
        Desc.bUseSpawnPos = true;
        _float vAttackRanges[1] = { 15.f };
        Desc.pAttackRanges = vAttackRanges;
        Desc.iNumAttackRanges = 1;
        _float vAttackCoolTimes[1] = { 2.f };
        Desc.pAttackCoolTimes = vAttackCoolTimes;
        Desc.iLevelIndex = m_iLevelIndex;
        Desc.iNavMeshIdx = GetspawnNavMeshIdx();
        Desc.iHP = 150;

        _vector vSpawnPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);
        XMStoreFloat3(&Desc.vSpawnPos, vSpawnPos);
        vTempSpawnPos = vSpawnPos;

        if (FAILED(m_pGameInst->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Monster03"), Desc.iLevelIndex, TEXT("Layer_Monster03"), &Desc)))
            return E_FAIL;

        m_strLayerTag = TEXT("Layer_Monster03");

        // enum ROT_TYPE { ROT_LEFT, ROT_BACK, ROT_RIGHT, ROT_FRONT, ROT_NONE, };
        if (0 == m_iInitRotType)
        {
            static_cast<CTransform*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, Desc.iLevelIndex, TEXT("Layer_Monster03"))->GetComponent(TEXT("Com_Transform")))->Rotation(_vector{ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(-90.f));
        }
        else if (1 == m_iInitRotType)
        {
            static_cast<CTransform*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, Desc.iLevelIndex, TEXT("Layer_Monster03"))->GetComponent(TEXT("Com_Transform")))->Rotation(_vector{ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f));
        }
        else if (2 == m_iInitRotType)
        {
            static_cast<CTransform*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, Desc.iLevelIndex, TEXT("Layer_Monster03"))->GetComponent(TEXT("Com_Transform")))->Rotation(_vector{ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(90.f));
        }
        else if (3 == m_iInitRotType)
        {
        }

        m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, Desc.iLevelIndex, TEXT("Layer_Monster03"))->SetActive(true);

        if (FAILED(CreateMonsterHPBar(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, Desc.iLevelIndex, TEXT("Layer_Monster03")), Desc.iLevelIndex, Desc.iHP)))
            return E_FAIL;
    }

    break;
    case Engine::MONSTER04:
    {
        CMonsterBase::MONSTER_DESC Desc = {};
        Desc.iNumPartObjects = CMonsterBase::PART_END;
        Desc.fSpeedPerSec = 4.5f;
        Desc.fRotSpeedPerSec = XMConvertToRadians(180.f);
        Desc.strObjectTag = "Monster";
        Desc.bUseSpawnPos = true;

        _float vAttackRanges[1] = { 15.f };
        Desc.pAttackRanges = vAttackRanges;
        Desc.iNumAttackRanges = 1;
        Desc.iLevelIndex = m_iLevelIndex;
        Desc.iNavMeshIdx = GetspawnNavMeshIdx();
        Desc.iHP = 150;

        _float vAttackCoolTimes[1] = { 5.f };
        Desc.pAttackCoolTimes = vAttackCoolTimes;

        _vector vSpawnPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);
        XMStoreFloat3(&Desc.vSpawnPos, vSpawnPos);
        vTempSpawnPos = vSpawnPos;


        if (FAILED(m_pGameInst->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Monster04"), Desc.iLevelIndex, TEXT("Layer_Monster04"), &Desc)))
            return E_FAIL;

        m_strLayerTag = TEXT("Layer_Monster04");


        // enum ROT_TYPE { ROT_LEFT, ROT_BACK, ROT_RIGHT, ROT_FRONT, ROT_NONE, };
        if (0 == m_iInitRotType)
        {
            static_cast<CTransform*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, Desc.iLevelIndex, TEXT("Layer_Monster04"))->GetComponent(TEXT("Com_Transform")))->Rotation(_vector{ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(-90.f));
        }
        else if (1 == m_iInitRotType)
        {
            static_cast<CTransform*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, Desc.iLevelIndex, TEXT("Layer_Monster04"))->GetComponent(TEXT("Com_Transform")))->Rotation(_vector{ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f));
        }
        else if (2 == m_iInitRotType)
        {
            static_cast<CTransform*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, Desc.iLevelIndex, TEXT("Layer_Monster04"))->GetComponent(TEXT("Com_Transform")))->Rotation(_vector{ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(90.f));
        }
        else if (3 == m_iInitRotType)
        {
        }

        m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, Desc.iLevelIndex, TEXT("Layer_Monster04"))->SetActive(true);


        if (FAILED(CreateMonsterHPBar(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, Desc.iLevelIndex, TEXT("Layer_Monster04")), Desc.iLevelIndex, Desc.iHP)))
            return E_FAIL;

    }
    break;
    case Engine::MONSTER05:
    {
        CMonsterBase::MONSTER_DESC Desc = {};
        Desc.iNumPartObjects = CMonsterBase::PART_END;
        Desc.fSpeedPerSec = 1.5f;
        Desc.fRotSpeedPerSec = XMConvertToRadians(180.f);
        Desc.strObjectTag = "Monster";
        Desc.bUseSpawnPos = true;
        Desc.iHP = 150;

        _float vAttackRanges[3] = { 5.f, 6.5f, 8.f };
        Desc.pAttackRanges = vAttackRanges;
        Desc.iNumAttackRanges = 3;

        _float vAttackCoolTimes[3] = { 0.f, 0.f, 5.f };
        Desc.pAttackCoolTimes = vAttackCoolTimes;
        Desc.iLevelIndex = m_iLevelIndex;
        Desc.iNavMeshIdx = GetspawnNavMeshIdx();



        _vector vSpawnPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);
        XMStoreFloat3(&Desc.vSpawnPos, vSpawnPos);
        vTempSpawnPos = vSpawnPos;


        if (FAILED(m_pGameInst->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Monster05"), Desc.iLevelIndex, TEXT("Layer_Monster05"), &Desc)))
            return E_FAIL;

        m_strLayerTag = TEXT("Layer_Monster05");


        // enum ROT_TYPE { ROT_LEFT, ROT_BACK, ROT_RIGHT, ROT_FRONT, ROT_NONE, };
        if (0 == m_iInitRotType)
        {
            static_cast<CTransform*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, Desc.iLevelIndex, TEXT("Layer_Monster05"))->GetComponent(TEXT("Com_Transform")))->Rotation(_vector{ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(-90.f));
        }
        else if (1 == m_iInitRotType)
        {
            static_cast<CTransform*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, Desc.iLevelIndex, TEXT("Layer_Monster05"))->GetComponent(TEXT("Com_Transform")))->Rotation(_vector{ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f));
        }
        else if (2 == m_iInitRotType)
        {
            static_cast<CTransform*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, Desc.iLevelIndex, TEXT("Layer_Monster05"))->GetComponent(TEXT("Com_Transform")))->Rotation(_vector{ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(90.f));
        }
        else if (3 == m_iInitRotType)
        {
        }

        m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, Desc.iLevelIndex, TEXT("Layer_Monster05"))->SetActive(true);

        if (FAILED(CreateMonsterHPBar(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, Desc.iLevelIndex, TEXT("Layer_Monster05")), Desc.iLevelIndex, Desc.iHP)))
            return E_FAIL;

    }
    break;
    case Engine::MONSTER06:
    {
        CMonsterBase::MONSTER_DESC Desc = {};
        Desc.iNumPartObjects = CMonsterBase::PART_END;
        Desc.fSpeedPerSec = 5.f;
        Desc.fRotSpeedPerSec = XMConvertToRadians(180.f);
        Desc.strObjectTag = "Monster";
        Desc.bUseSpawnPos = true;
        Desc.iNavMeshIdx = GetspawnNavMeshIdx();
        Desc.iHP = 150;

        
        _float vAttackRanges[1] = { 4.5f };
        Desc.pAttackRanges = vAttackRanges;
        Desc.iNumAttackRanges = 1;
        
        _float vAttackCoolTimes[2] = { 0.f, 5.f };
        Desc.pAttackCoolTimes = vAttackCoolTimes;
        Desc.iLevelIndex = m_iLevelIndex;


        _vector vSpawnPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);
        XMStoreFloat3(&Desc.vSpawnPos, vSpawnPos);
        vTempSpawnPos = vSpawnPos;

        if (FAILED(m_pGameInst->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_Monster06"), Desc.iLevelIndex, TEXT("Layer_Monster06"), &Desc)))
            return E_FAIL;

        m_strLayerTag = TEXT("Layer_Monster06");

        // enum ROT_TYPE { ROT_LEFT, ROT_BACK, ROT_RIGHT, ROT_FRONT, ROT_NONE, };
        if (0 == m_iInitRotType)
        {
            static_cast<CTransform*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, Desc.iLevelIndex, TEXT("Layer_Monster06"))->GetComponent(TEXT("Com_Transform")))->Rotation(_vector{ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(-90.f));
        }
        else if (1 == m_iInitRotType)
        {
            static_cast<CTransform*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, Desc.iLevelIndex, TEXT("Layer_Monster06"))->GetComponent(TEXT("Com_Transform")))->Rotation(_vector{ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f));
        }
        else if (2 == m_iInitRotType)
        {
            static_cast<CTransform*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, Desc.iLevelIndex, TEXT("Layer_Monster06"))->GetComponent(TEXT("Com_Transform")))->Rotation(_vector{ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(90.f));
        }
        else if (3 == m_iInitRotType)
        {

        }

        m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, Desc.iLevelIndex, TEXT("Layer_Monster06"))->SetActive(true);

        if (FAILED(CreateMonsterHPBar(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, Desc.iLevelIndex, TEXT("Layer_Monster06")), Desc.iLevelIndex, Desc.iHP)))
            return E_FAIL;
    }
    break;
    default:
        break;
    }

    HIT_RESULT Hit;
    XMStoreFloat3(&Hit.vHitPos, vTempSpawnPos);
    CParticleManager::GetInstance()->ActiveParticle(CParticleManager::MONSTER_SPAWN, Hit);
    return S_OK;
}

HRESULT CSpawner::CreateMonsterHPBar(CGameObject* pOwner, _int iLevelIdx, _int iMaxHP)
{
    static _int g_iAccNum = 0;
    _wstring strLayerTag = TEXT("Layer_UI_MonsterHPBG");

     CUIBG::UI_BG_DESC PanelDesc = {};
    PanelDesc.eShowType = UI_SHOW_TYPE::UI_PERSPECTIVE; // 원근
    PanelDesc.eUIType = UI_TYPE::UI_PANEL;
    PanelDesc.fSSizeX = 200;
    PanelDesc.fSSizeY = 50;
    PanelDesc.fSX = (PanelDesc.fSSizeX * 0.5f);
    PanelDesc.fSY = g_iWinSizeY - PanelDesc.fSSizeY + 50.f;
    PanelDesc.bUseSpawnPos = false;
    PanelDesc.fSpeedPerSec = 0.f;
    PanelDesc.fSpeedPerSec = 0.f;
    PanelDesc.strObjectTag = "UIMonsterHPBG" + to_string(g_iAccNum++);
    PanelDesc.iDepth = 5;
    PanelDesc.eBGType = CUIBG::UI_BG_TYPE::MOSTER_HP;


    vector<string> strNames;
    strNames.push_back(PanelDesc.strObjectTag);


    if (!pOwner)
        return E_FAIL;

    PanelDesc.pUIOwner = pOwner;
    

    if (FAILED(m_pGameInst->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_BG"), iLevelIdx, strLayerTag, &PanelDesc))) return E_FAIL;
    CUIBG* pPanel = static_cast<CUIBG*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, iLevelIdx, strLayerTag));
    if (pPanel)
        m_pGameInst->AddPanelUIToManager(PanelDesc.strObjectTag, pPanel);
    else
        return E_FAIL;

    // Child HP Bar BG
    {
        static _int g_iNum = 0;
        // 1
        {
            CUITex::UI_TEX_DESC Desc = {};
            Desc.eShowType = UI_SHOW_TYPE::UI_PERSPECTIVE; // 직교
            Desc.eUIType = UI_TYPE::UI_BAR;
            Desc.fSX = 0.f;
            Desc.fSY = -8.f;
            Desc.bUseSpawnPos = false;
            Desc.fSpeedPerSec = 0.f;
            Desc.fSpeedPerSec = 0.f;
            Desc.iDepth = 1;
            Desc.eUITexType = CUITex::UI_TEX_TYPE::MONSTER_HP_BG;
            Desc.pPerspectiveUIOwner = pPanel;
            Desc.fSSizeX = 180.f;
            Desc.fSSizeY = 10.f;
            Desc.strObjectTag = "UIMonsterHPBG" + to_string(g_iNum++);

            if (FAILED(m_pGameInst->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Tex"), iLevelIdx, strLayerTag, &Desc))) return E_FAIL;
            CUIElement* pObj = static_cast<CUIElement*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, iLevelIdx, strLayerTag));
            if (pObj)
                m_pGameInst->AddElementUIToManager(Desc.strObjectTag, pObj);
            else
                return E_FAIL;

            m_pGameInst->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, 0.f, 0.f);
        }
        
        // 2
        {
            CUITex::UI_TEX_DESC Desc = {};
            Desc.eShowType = UI_SHOW_TYPE::UI_PERSPECTIVE; // 직교
            Desc.eUIType = UI_TYPE::UI_BAR;
            Desc.fSX = 0.f;
            Desc.fSY = +8.f;
            Desc.bUseSpawnPos = false;
            Desc.fSpeedPerSec = 0.f;
            Desc.fSpeedPerSec = 0.f;
            Desc.iDepth = 1;
            Desc.eUITexType = CUITex::UI_TEX_TYPE::MONSTER_HP_BG;
            Desc.pPerspectiveUIOwner = pPanel;
            Desc.fSSizeX = 180.f;
            Desc.fSSizeY = 10.f;
            Desc.strObjectTag = "UIMonsterHPBG" + to_string(g_iNum++);

            if (FAILED(m_pGameInst->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_Tex"), iLevelIdx, strLayerTag, &Desc))) return E_FAIL;
            CUIElement* pObj = static_cast<CUIElement*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, iLevelIdx, strLayerTag));
            if (pObj)
                m_pGameInst->AddElementUIToManager(Desc.strObjectTag, pObj);
            else
                return E_FAIL;

            m_pGameInst->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, 0.f, 0.f);
        }
    }


    CMonsterBase* pMon = static_cast<CMonsterBase*>(pOwner);

    // Child HP Bar
    {
        static _int g_iBarNum = 0;
        _wstring strLayerTag = TEXT("Layer_UI_MonsterHPBar");

        // HP 1
        {
            CUIHPBar::UI_HP_BAR_DESC Desc = {};
            Desc.eShowType = UI_SHOW_TYPE::UI_PERSPECTIVE; // 직교
            Desc.eUIType = UI_TYPE::UI_BAR;
            Desc.fSX = 0;
            Desc.fSY = -8.f;
            Desc.fSSizeX = 180.f;
            Desc.fSSizeY = 10.f;
            Desc.bUseSpawnPos = false;
            Desc.fSpeedPerSec = 0.f;
            Desc.fSpeedPerSec = 0.f;
            Desc.iDepth = 0;
            Desc.eHPType = CUIHPBar::HP_MONSTER;
            Desc.pPerspectiveUIOwner = pPanel;
            Desc.strObjectTag = "UIMonsterHPBar" + to_string(g_iBarNum++);

            strNames.push_back(Desc.strObjectTag);

            Desc.eHPBarColor = CUIHPBar::HP_RED;

            Desc.fMaxHPBarVal = (_float)(iMaxHP / 2.f);

            if (FAILED(m_pGameInst->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_HPBar"), iLevelIdx, strLayerTag, &Desc))) return E_FAIL;
            CUIElement* pObj = static_cast<CUIElement*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, iLevelIdx, strLayerTag));
            if (pObj)
                m_pGameInst->AddElementUIToManager(Desc.strObjectTag, pObj);
            else
                return E_FAIL;

            m_pGameInst->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, 0.f, 0.f);
        }


        // HP 2
        {
            CUIHPBar::UI_HP_BAR_DESC Desc = {};
            Desc.eShowType = UI_SHOW_TYPE::UI_PERSPECTIVE; // 직교
            Desc.eUIType = UI_TYPE::UI_BAR;
            Desc.fSX = 0;
            Desc.fSY = 8.f;
            Desc.fSSizeX = 180.f;
            Desc.fSSizeY = 10.f;
            Desc.bUseSpawnPos = false;
            Desc.fSpeedPerSec = 0.f;
            Desc.fSpeedPerSec = 0.f;
            Desc.iDepth = 0;
            Desc.eHPType = CUIHPBar::HP_MONSTER;
            Desc.pPerspectiveUIOwner = pPanel;
            Desc.strObjectTag = "UIMonsterHPBar" + to_string(g_iBarNum++);
            strNames.push_back(Desc.strObjectTag);

            Desc.eHPBarColor = CUIHPBar::HP_RED;
            Desc.fMaxHPBarVal = (_float)(iMaxHP / 2.f);

            if (FAILED(m_pGameInst->AddGameObjectToLayer(LEVEL_GLOBAL, TEXT("Prototype_GameObject_UI_HPBar"), iLevelIdx, strLayerTag, &Desc))) return E_FAIL;
            CUIElement* pObj = static_cast<CUIElement*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, iLevelIdx, strLayerTag));
            if (pObj)
                m_pGameInst->AddElementUIToManager(Desc.strObjectTag, pObj);
            else
                return E_FAIL;

            m_pGameInst->AddElementToPanel(PanelDesc.strObjectTag, Desc.strObjectTag, 0.f, 0.f);
        }
    }
    
    pMon->SetHPBarNames(strNames);

    return S_OK;
}

HRESULT CSpawner::ReadyComponents()
{
    // Com_Collider
    {
        CBoundingAABB::BOUND_AABB_DESC		ColliderDesc{};
        ColliderDesc.vExtents = _float3(0.5f, 0.5f, 0.5f);
        ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

        if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
            return E_FAIL;
    }

    return S_OK;
}
#endif


//HRESULT CSpawner::ReadySpawnMonster()
//{
//    switch (m_eSpawnMonsterType)
//    {
//    case Engine::MONSTER01:
//    {
//        CMonsterBase::MONSTER_DESC Desc = {};
//        Desc.iNumPartObjects = CMonsterBase::PART_END;
//        Desc.fSpeedPerSec = 3.f;
//        Desc.fRotSpeedPerSec = XMConvertToRadians(180.f);
//        Desc.strObjectTag = "Monster01";
//        Desc.bUseSpawnPos = true;
//        Desc.iNumAttackRanges = 2;
//
//        _float vAttackRanges[2] = { 2.f, 3.f };
//        Desc.pAttackRanges = vAttackRanges;
//
//        _float vAttackCoolTimes[2] = { 0.f, 0.f };
//        Desc.pAttackCoolTimes = vAttackCoolTimes;
//
//        // TODO -> 여기서 파일에서 읽어와서 값을 써야한다.
//        _vector vSpawnPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);
//        XMStoreFloat3(&Desc.vSpawnPos, vSpawnPos);
//
//        if (FAILED(m_pGameInst->AddGameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Monster01"), LEVEL_GAMEPLAY, TEXT("Layer_Monster01"), &Desc)))
//            return E_FAIL;
//
//        // enum ROT_TYPE { ROT_LEFT, ROT_BACK, ROT_RIGHT, ROT_FRONT, ROT_NONE, };
//        if (0 == m_iInitRotType)
//        {
//            static_cast<CTransform*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GAMEPLAY, LEVEL_GAMEPLAY, TEXT("Layer_Monster01"))->GetComponent(TEXT("Com_Transform")))->Rotation(_vector{ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(90.f));
//        }
//        else if (1 == m_iInitRotType)
//        {
//            static_cast<CTransform*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GAMEPLAY, LEVEL_GAMEPLAY, TEXT("Layer_Monster01"))->GetComponent(TEXT("Com_Transform")))->Rotation(_vector{ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f));
//        }
//        else if (2 == m_iInitRotType)
//        {
//            static_cast<CTransform*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GAMEPLAY, LEVEL_GAMEPLAY, TEXT("Layer_Monster01"))->GetComponent(TEXT("Com_Transform")))->Rotation(_vector{ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(-90.f));
//        }
//        else if (3 == m_iInitRotType)
//        {
//            // static_cast<CTransform*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GAMEPLAY, LEVEL_GAMEPLAY, TEXT("Layer_Monster01"))->GetComponent(TEXT("Com_Transform")))->Rotation(_fvector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(0.f));
//        }
//    }
//
//        break;
//    case Engine::MONSTER02:
//    {
//        if (FAILED(m_pGameInst->AddGameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Monster01"), LEVEL_GAMEPLAY, TEXT("Layer_Monster01"), nullptr)))
//            return E_FAIL;
//    }
//
//        break;
//    case Engine::MONSTER03:
//    {
//        if (FAILED(m_pGameInst->AddGameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Monster01"), LEVEL_GAMEPLAY, TEXT("Layer_Monster01"), nullptr)))
//            return E_FAIL;
//    }
//
//        break;
//    case Engine::MONSTER04:
//    {
//        if (FAILED(m_pGameInst->AddGameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Monster01"), LEVEL_GAMEPLAY, TEXT("Layer_Monster01"), nullptr)))
//            return E_FAIL;
//    }
//        break;
//    case Engine::MONSTER05:
//    {
//        if (FAILED(m_pGameInst->AddGameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Monster01"), LEVEL_GAMEPLAY, TEXT("Layer_Monster01"), nullptr)))
//            return E_FAIL;
//    }
//        break;
//    case Engine::MONSTER06:
//    {
//        if (FAILED(m_pGameInst->AddGameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Monster01"), LEVEL_GAMEPLAY, TEXT("Layer_Monster01"), nullptr)))
//            return E_FAIL;
//    }
//        break;
//    default:
//        break;
//    }
//
//    return S_OK;
//}


CSpawner* CSpawner::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSpawner* pInstance = new CSpawner(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CSpawner");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CSpawner::Clone(void* pArg)
{
    CSpawner* pInstance = new CSpawner(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CSpawner");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CSpawner::Free()
{
    __super::Free();

    SafeRelease(m_pColliderCom);
}
