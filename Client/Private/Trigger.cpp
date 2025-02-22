#include "pch.h"
#include "Trigger.h"
#include "GameInstance.h"

CTrigger::CTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CGameObject(pDevice, pContext)
{

}

CTrigger::CTrigger(const CTrigger& Prototype)
    :
    CGameObject(Prototype)
{
}

HRESULT CTrigger::InitializePrototype()
{
    return S_OK;
}

HRESULT CTrigger::Initialize(void* pArg)
{
    if (nullptr != pArg)
    {
        TRIGGER_DESC* pDesc = static_cast<TRIGGER_DESC*>(pArg);
        m_eTriggerType = pDesc->eTriggerType;

        CCollider* pTargetCol = static_cast<CCollider*>(m_pGameInst->GetComponent(pDesc->iLevelIndex, TEXT("A"), TEXT("Com_Collider")));
        if (nullptr != pTargetCol)
            SetTargetCollider(pTargetCol);
        else
            return E_FAIL;
    }

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CTrigger::ReadyComponents()
{
    return S_OK;
}

void CTrigger::Free()
{
    __super::Free();

    if (nullptr != m_pColliderCom)
        SafeRelease(m_pColliderCom);

    if (nullptr != m_pTargetCollider)
        SafeRelease(m_pTargetCollider);
}
