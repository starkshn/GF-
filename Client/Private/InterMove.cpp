#include "pch.h"
#include "InterMove.h"
#include "GameInstance.h"
#include "Collider.h"
#include "Stage01.h"
#include "Player.h"
#include "StateObjectContainer.h"
#include "MonsterManager.h"
#include "Level_Loading.h"
#include "Level_GamePlay.h"
#include "UITex.h"
#include "Layer.h"


CInterMove::CInterMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CInterTrigger(pDevice, pContext)
{

}

CInterMove::CInterMove(const CInterMove& Prototype)
    :
    CInterTrigger(Prototype)
{

}

HRESULT CInterMove::InitializePrototype()
{
    return S_OK;
}

HRESULT CInterMove::Initialize(void* pArg)
{
    if (nullptr != pArg)
    {
        INTER_MOVE_DESC* pDesc = static_cast<INTER_MOVE_DESC*>(pArg);
        m_vMoveEndPos = pDesc->vMoveEndPos;
    }

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents(pArg)))
        return E_FAIL;

    return S_OK;
}

void CInterMove::PriorityUpdate(_float fTimeDelta)
{

}

void CInterMove::Update(_float fTimeDelta)
{
    if (m_pColliderCom)
        m_pColliderCom->Update(m_pTransformCom->GetWorldMatrix());
}

// 
void CInterMove::LateUpdate(_float fTimeDelta)
{
    m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);

    if (m_pColliderCom->Intersect(GetTargetCollider()))
    {
        m_bInterWithPlayer = true;
        ShowUI();
    }
    else
    {
        if (m_bInterWithPlayer)
            CloseUI();

        m_bInterWithPlayer = false;
    }

    if (m_bInterWithPlayer)
    {
        if (m_pGameInst->KeyDown(KEY::F))
        {
            MovePlayerToEndPos();
            CloseUI();
            DeActivePointLightsAndTorchEffect();
            m_bInterWithPlayer = false;

            m_pGameInst->PlayEffect(TEXT("InterMove.mp3"), 10.f);
        }
    }
}

HRESULT CInterMove::Render()
{
    return S_OK;
}

void CInterMove::MovePlayerToEndPos()
{
    if (m_eInterMoveIdx == IDX_END)
        return;

    CGameObject* pObj = m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, LEVEL_GAMEPLAY, TEXT("A"));
    if (pObj)
    {
        CTransform* pTC = static_cast<CTransform*>(pObj->GetComponent(TEXT("Com_Transform")));
        if (pTC)
        {
            _vector vPos = XMVectorSet(m_vMoveEndPos.x, m_vMoveEndPos.y, m_vMoveEndPos.z, 1.f);
            pTC->SetTransformState(CTransform::STATE_POS, vPos);
            
            switch (m_eInterMoveIdx)
            {
            case Client::CInterMove::S1_TO_S2:
                static_cast<CStateObjectContainer*>(pObj)->SetNavMeshIdx(1);
                break;
            case Client::CInterMove::S2_TO_S3:
                static_cast<CStateObjectContainer*>(pObj)->SetNavMeshIdx(2);
                break;
            case Client::CInterMove::S_BOSS:
                static_cast<CLevel_GamePlay*>(m_pGameInst->GetCurrentLevel())->GoBossLevel();
                break;
            case Client::CInterMove::IDX_END:
                break;
            default:
                break;
            }
        }
    }
}

// UITexPlayerKeyF
void CInterMove::ShowUI()
{
    auto pUI = static_cast<CUITex*>(m_pGameInst->GetEleUI("UITexPlayerKeyF"));
    if (pUI)
    {
        pUI->ActiveRender();
    }
    
}

void CInterMove::CloseUI()
{
    auto pUI = static_cast<CUITex*>(m_pGameInst->GetEleUI("UITexPlayerKeyF"));
    if (pUI)
    {
        pUI->DeActiveRender();
    }
}

void CInterMove::DeActivePointLightsAndTorchEffect()
{
    if (GetObjectTag() == "InterMove0")
    {
        for (_int i = 0; i < 8; ++i)
        {
            string strLightName = "S1TorchLight";
            m_pGameInst->DeActivePointLight(strLightName + to_string(i));
        }

        auto pLayer = m_pGameInst->FindLayer(m_iLevelIndex, TEXT("Layer_S1TorchEffect"));
        if (pLayer)
        {
            auto List = pLayer->GetGameObjectList();
            for (auto pLightEffect : List)
                if (pLightEffect)
                    static_cast<CEffect*>(pLightEffect)->DeActive();
        }
    }
    else if (GetObjectTag() == "InterMove1")
    {
        for (_int i = 0; i < 3; ++i)
        {
            string strLightName = "S2TorchLight";
            m_pGameInst->DeActivePointLight(strLightName + to_string(i));
        }

        auto pLayer = m_pGameInst->FindLayer(m_iLevelIndex, TEXT("Layer_S2TorchEffect"));
        if (pLayer)
        {
            auto List = pLayer->GetGameObjectList();
            for (auto pLightEffect : List)
                if (pLightEffect)
                    static_cast<CEffect*>(pLightEffect)->DeActive();
        }
    }
    else if (GetObjectTag() == "InterMove2")
    {
        for (_int i = 0; i < 6; ++i)
        {
            string strLightName = "S3TorchLight";
            m_pGameInst->DeActivePointLight(strLightName + to_string(i));
        }

        auto pLayer = m_pGameInst->FindLayer(m_iLevelIndex, TEXT("Layer_S3TorchEffect"));
        if (pLayer)
        {
            auto List = pLayer->GetGameObjectList();
            for (auto pLightEffect : List)
                if (pLightEffect)
                    static_cast<CEffect*>(pLightEffect)->DeActive();
        }
    }
}

HRESULT CInterMove::ReadyComponents()
{
    return S_OK;
}

HRESULT CInterMove::ReadyComponents(void* pArg)
{
    if (nullptr != pArg)
    {
        TRIGGER_DESC* pDesc = static_cast<CTrigger::TRIGGER_DESC*>(pArg);
        CBoundingAABB::BOUND_AABB_DESC		ColliderDesc{};
        ColliderDesc.vExtents = pDesc->vExtents;
        if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
            return E_FAIL;
    }

    return S_OK;
}

CInterMove* CInterMove::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInterMove* pInstance = new CInterMove(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CInterMove");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CInterMove::Clone(void* pArg)
{
    CInterMove* pInstance = new CInterMove(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CInterMove");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CInterMove::Free()
{
    __super::Free();
}
