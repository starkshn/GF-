#include "SpawnTrigger.h"
#include "BoundingAABB.h"
#include "GameInstance.h"

CSpawnTrigger::CSpawnTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CGameObject(pDevice, pContext)
{
}

CSpawnTrigger::CSpawnTrigger(const CSpawnTrigger& Prototype)
    :
    CGameObject(Prototype)
{

}

HRESULT CSpawnTrigger::InitializePrototype()
{
    return S_OK;
}

HRESULT CSpawnTrigger::Initialize(void* pArg)
{
    if (__super::Initialize(pArg))
        return E_FAIL;

    if (ReadyComponents())
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
}

HRESULT CSpawnTrigger::Render()
{
    m_pColliderCom->Render();

    return S_OK;
}

HRESULT CSpawnTrigger::ReadyComponents()
{
    // Com_Collider
    {
        CBoundingAABB::BOUND_AABB_DESC		ColliderDesc{};
        ColliderDesc.vExtents = _float3(0.5f, 0.5f, 0.5f);
        ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

        if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
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

    SafeRelease(m_pColliderCom);
}
