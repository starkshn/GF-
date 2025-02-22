#include "Component.h"
#include "GameInstance.h"

CComponent::CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    m_pDevice(pDevice),
    m_pContext(pContext),
    m_pGameInst(CGameInstance::GetInstance()),
    m_bIsCloned(false)
{
    SafeAddRef(m_pDevice);
    SafeAddRef(m_pContext);
    SafeAddRef(m_pGameInst);
}

CComponent::CComponent(const CComponent& Prototype)
    :
    m_pDevice(Prototype.m_pDevice),
    m_pContext(Prototype.m_pContext),
    m_pGameInst(CGameInstance::GetInstance()),
    m_bIsCloned(true)
{
    SafeAddRef(m_pDevice);
    SafeAddRef(m_pContext);
    SafeAddRef(m_pGameInst);
}

HRESULT CComponent::InitializePrototype()
{
    return S_OK;
}

HRESULT CComponent::Initialize(void* pArg)
{

    return S_OK;
}

void CComponent::Free()
{
    __super::Free();

    SafeRelease(m_pDevice);
    SafeRelease(m_pContext);
    SafeRelease(m_pGameInst);
}
