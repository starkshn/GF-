#include "pch.h"
#include "InterMoveEndPos.h"
#include "BoundingAABB.h"
#include "GameInstance.h"

CInterMoveEndPos::CInterMoveEndPos(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CGameObject(pDevice, pContext)
{
}

CInterMoveEndPos::CInterMoveEndPos(const CInterMoveEndPos& Prototype)
    :
    CGameObject(Prototype)
{

}

HRESULT CInterMoveEndPos::InitializePrototype()
{
    return S_OK;
}

HRESULT CInterMoveEndPos::Initialize(void* pArg)
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

void CInterMoveEndPos::PriorityUpdate(_float fTimeDelta)
{

}

void CInterMoveEndPos::Update(_float fTimeDelta)
{
    if (m_pColliderCom)
        m_pColliderCom->Update(m_pTransformCom->GetWorldMatrix());
}

void CInterMoveEndPos::LateUpdate(_float fTimeDelta)
{
    m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CInterMoveEndPos::Render()
{
    // m_pColliderCom->Render(_fvector{ 1.f, 0.f, 0.f, 1.f });

    return S_OK;
}

#ifdef _DEBUG

HRESULT CInterMoveEndPos::ReadyComponents()
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



CInterMoveEndPos* CInterMoveEndPos::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInterMoveEndPos* pInstance = new CInterMoveEndPos(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CInterMoveEndPos");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CInterMoveEndPos::Clone(void* pArg)
{
    CInterMoveEndPos* pInstance = new CInterMoveEndPos(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CInterMoveEndPos");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CInterMoveEndPos::Free()
{
    __super::Free();

    SafeRelease(m_pColliderCom);
}
