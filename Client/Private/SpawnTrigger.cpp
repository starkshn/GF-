#include "pch.h"
#include "SpawnTrigger.h"
#include "GameInstance.h"
#include "Spawner.h"
#include "Layer.h"
#include "Effect.h"

CSpawnTrigger::CSpawnTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CTrigger(pDevice, pContext)
{

}

CSpawnTrigger::CSpawnTrigger(const CSpawnTrigger& Prototype)
    :
    CTrigger(Prototype)
{
}

HRESULT CSpawnTrigger::InitializePrototype()
{
    return S_OK;
}

HRESULT CSpawnTrigger::Initialize(void* pArg)
{
    // TRIGGER_DESC Desc{};
    // Desc.eType = CTrigger::TRIGGER_TYPE::TYPE_SPAWN;
    // Desc.bUseSpawnPos = true;
    // Desc.fRotSpeedPerSec = 0.f;
    // Desc.fSpeedPerSec = 0.f;
    // Desc.strObjectTag = "SPAWNTRIGGER";
    // Desc.vSpawnPos = _float3{-5.f, 0.f, 0.f};

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    // if (FAILED(ReadyComponents())) return E_FAIL;

    if (FAILED(ReadyComponents(pArg)))
        return E_FAIL;

    return S_OK;
}

void CSpawnTrigger::PriorityUpdate(_float fTimeDelta)
{

}

void CSpawnTrigger::Update(_float fTimeDelta)
{
    m_pColliderCom->Update(m_pTransformCom->GetWorldMatrix());
}

void CSpawnTrigger::LateUpdate(_float fTimeDelta)
{
    m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);
    
    if (m_pColliderCom->Intersect(GetTargetCollider()))
    {
        if (false == GetIsTriggered())
        {
            SetIsTriggered(true);

            for (auto& pSpawner : m_Spawners)
            {
                if (FAILED(pSpawner->SpawnMonster()))
                {
                    MSG_BOX("Spawn Monster Error!");
                }
            }

            for (auto& pSpawner : m_Spawners)
                m_pGameInst->ActiveMonsters(m_iLevelIndex, pSpawner->GetLayerTag(), 0);
        }

        ActiveLightAndEffect();
        
    }
}

HRESULT CSpawnTrigger::Render()
{
    return S_OK;
}

void CSpawnTrigger::ActiveSpawnTrigger()
{
    if (false == GetIsTriggered())
    {
        for (auto& pSpawner : m_Spawners)
        {
            if (FAILED(pSpawner->SpawnMonster()))
            {
                MSG_BOX("Spawn Monster Error!");
            }
        }

        for (auto& pSpawner : m_Spawners)
        {
            m_pGameInst->ActiveMonsters(m_iLevelIndex, pSpawner->GetLayerTag(), 0);
        }
    }
}

void CSpawnTrigger::ActiveLightAndEffect()
{
    if ("FirstSpawnTrigger" == GetObjectTag())
    {
        for (_int i = 0; i < 8; ++i)
        {
            string strLightName = "S1TorchLight";
            m_pGameInst->ActivePointLight(strLightName + to_string(i));
        }

        auto pLayer = m_pGameInst->FindLayer(m_iLevelIndex, TEXT("Layer_S1TorchEffect"));
        if (pLayer)
        {
            auto List = pLayer->GetGameObjectList();
            for (auto pLightEffect : List)
                if (pLightEffect)
                    static_cast<CEffect*>(pLightEffect)->Active();
        }
    }
    else if ("FourSpawnTrigger" == GetObjectTag())
    {
        // 라이트 : 3개
        for (_int i = 0; i < 3; ++i)
        {
            string strLightName = "S2TorchLight";
            m_pGameInst->ActivePointLight(strLightName + to_string(i));
        }

        auto pLayer = m_pGameInst->FindLayer(m_iLevelIndex, TEXT("Layer_S2TorchEffect"));
        if (pLayer)
        {
            auto List = pLayer->GetGameObjectList();
            for (auto pLightEffect : List)
                if (pLightEffect)
                    static_cast<CEffect*>(pLightEffect)->Active();
        }
    }
    else if ("SixSpawnTrigger" == GetObjectTag())
    {
        // Light : 6
        for (_int i = 0; i < 6; ++i)
        {
            string strLightName = "S3TorchLight";
            m_pGameInst->ActivePointLight(strLightName + to_string(i));
        }

        auto pLayer = m_pGameInst->FindLayer(m_iLevelIndex, TEXT("Layer_S3TorchEffect"));
        if (pLayer)
        {
            auto List = pLayer->GetGameObjectList();
            for (auto pLightEffect : List)
                if (pLightEffect)
                    static_cast<CEffect*>(pLightEffect)->Active();
        }
    }
}

HRESULT CSpawnTrigger::ReadyComponents()
{
    /* Com_Collider */
    CBoundingAABB::BOUND_AABB_DESC		ColliderDesc{};
    ColliderDesc.vExtents = _float3(0.6f, 1.3f, 5.f);

    _vector vPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);
    ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CSpawnTrigger::ReadyComponents(void* pArg)
{
    if (nullptr != pArg)
    {
        TRIGGER_DESC* pDesc = static_cast<CTrigger::TRIGGER_DESC*>(pArg);
        CBoundingAABB::BOUND_AABB_DESC		ColliderDesc{};
        // ColliderDesc.vCenter = pDesc->vSpawnPos;
        ColliderDesc.vExtents = pDesc->vExtents;
        if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
            return E_FAIL;
    }

    return S_OK;
}

CSpawnTrigger* CSpawnTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSpawnTrigger* pInstance = new CSpawnTrigger(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CSpawnTrigger");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CSpawnTrigger::Clone(void* pArg)
{
    CSpawnTrigger* pInstance = new CSpawnTrigger(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CSpawnTrigger");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CSpawnTrigger::Free()
{
    __super::Free();
}
