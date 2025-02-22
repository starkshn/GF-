#include "Spawner.h"
#include "BoundingAABB.h"
#include "GameInstance.h"

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
    if (__super::Initialize(pArg))
        return E_FAIL;

    if (ReadyComponents())
        return E_FAIL;

    return S_OK;
}

void CSpawner::PriorityUpdate(_float fTimeDelta)
{

}

void CSpawner::Update(_float fTimeDelta)
{
    m_pColliderCom->Update(m_pTransformCom->GetWorldMatrix());
}

void CSpawner::LateUpdate(_float fTimeDelta)
{
    m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CSpawner::Render()
{
    m_pColliderCom->Render(_fvector{1.f, 0.f, 0.f, 1.f});

    return S_OK;
}

HRESULT CSpawner::ReadyComponents()
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
