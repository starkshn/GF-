#include "pch.h"
#include "InterTrigger.h"

CInterTrigger::CInterTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CTrigger(pDevice, pContext)
{
}

CInterTrigger::CInterTrigger(const CInterTrigger& Prototype)
    :
    CTrigger(Prototype)
{

}

HRESULT CInterTrigger::InitializePrototype()
{
    return S_OK;
}

HRESULT CInterTrigger::Initialize(void* pArg)
{
    if (nullptr != pArg)
    {
        INTER_TRIGGER_DESC* pDesc = static_cast<INTER_TRIGGER_DESC*>(pArg);
        m_eInterType = pDesc->eInterType;
    }

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CInterTrigger::PriorityUpdate(_float fTimeDelta)
{
    
}

void CInterTrigger::Update(_float fTimeDelta)
{

}

void CInterTrigger::LateUpdate(_float fTimeDelta)
{
    
}

HRESULT CInterTrigger::Render()
{
    return S_OK;
}

HRESULT CInterTrigger::ReadyComponents()
{
    return S_OK;
}

HRESULT CInterTrigger::ReadyComponents(void* pArg)
{
    return S_OK;
}

void CInterTrigger::Free()
{
    __super::Free();
}
