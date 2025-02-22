#include "pch.h"
#include "StateObjectContainer.h"
#include "GameInstance.h"
#include "Collider.h"
#include "Navigation.h"

CStateObjectContainer::CStateObjectContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CContainerObject(pDevice, pContext)
{
}

CStateObjectContainer::CStateObjectContainer(const CStateObjectContainer& Prototype)
    :
    CContainerObject(Prototype)
{
}

HRESULT CStateObjectContainer::InitializePrototype()
{
    return S_OK;
}

HRESULT CStateObjectContainer::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;


   

    return S_OK;
}

void CStateObjectContainer::PriorityUpdate(_float fTimeDelta)
{
    __super::PriorityUpdate(fTimeDelta);
}

void CStateObjectContainer::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CStateObjectContainer::LateUpdate(_float fTimeDelta)
{
    __super::LateUpdate(fTimeDelta);
}

HRESULT CStateObjectContainer::Render()
{
    return S_OK;
}

CModel* CStateObjectContainer::GetModelByModelName(string strName)
{
    return static_cast<CStateObjectBody*>(m_PartObjects[PART_BODY])->GetModelByName(strName);
}

const CStateObjectContainer::OBJECT_STATE CStateObjectContainer::GetState(_uint iState)
{
    if (OBJECT_STATE::STATE_IDLE & iState) return OBJECT_STATE::STATE_IDLE;
    else if (OBJECT_STATE::STATE_WALK & iState) return OBJECT_STATE::STATE_WALK;
    else if (OBJECT_STATE::STATE_JUMP & iState) return OBJECT_STATE::STATE_JUMP;
    else if (OBJECT_STATE::STATE_FIRE & iState) return OBJECT_STATE::STATE_FIRE;
    else if (OBJECT_STATE::STATE_RELOAD & iState) return OBJECT_STATE::STATE_RELOAD;
    else if (OBJECT_STATE::STATE_PUT & iState) return OBJECT_STATE::STATE_PUT;
    else if (OBJECT_STATE::STATE_DRAW & iState) return OBJECT_STATE::STATE_DRAW;


    return OBJECT_STATE::STATE_IDLE;
}

void CStateObjectContainer::SetNavMeshIdx(_int iIdx)
{
    m_iNavMeshIdx = iIdx;
    

    CNavigation::NAVIGATION_DESC		NaviDesc{};
    NaviDesc.iStartIndex = -1;
    XMStoreFloat3(&NaviDesc.vWorldPos, m_pTransformCom->GetTransformState(CTransform::STATE_POS));


    if (0 == m_iNavMeshIdx)
    {
        if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationComs[NAVMESH_IDX::S1]), &NaviDesc)))
            return;
    }
    else if (1 == m_iNavMeshIdx)
    {

        if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Navigation_S2"), TEXT("Com_Navigation_S2"), reinterpret_cast<CComponent**>(&m_pNavigationComs[NAVMESH_IDX::S2]), &NaviDesc)))
            return;
    }
    else if (2 == m_iNavMeshIdx)
    {
        if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Navigation_S3"), TEXT("Com_Navigation_S3"), reinterpret_cast<CComponent**>(&m_pNavigationComs[NAVMESH_IDX::S3]), &NaviDesc)))
            return;
    }
    else if (3 == m_iNavMeshIdx)
    {
        if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Navigation_Boss"), TEXT("Com_Navigation_BOSS"), reinterpret_cast<CComponent**>(&m_pNavigationComs[NAVMESH_IDX::S_BOSS]), &NaviDesc)))
            return;
    }

}

void CStateObjectContainer::CalNotAccuarteHitPosAndNormal(_fvector vDstPos, _fvector vNotAccuarteFromPos, _fvector vFromDir, _float3& vOutPos, _float3& vOutNormal)
{
    _vector vDir = vNotAccuarteFromPos - vDstPos;
    vDir = XMVector3Normalize(vDir);

    CBoundingAABB* pAABB = static_cast<CBoundingAABB*>(m_pColliderCom->GetBoundingRaw());
    if (nullptr == pAABB)
        return;

    _float fDist = 0.f;
    _vector vHitPos = {0.f, 0.f, 0.f, 0.f};
    if (pAABB->GetBounding()->Intersects(vDstPos, vDir, fDist))
    {
        vHitPos = vDstPos + vDir * fDist;
        XMStoreFloat3(&vOutPos, vHitPos);
    }

    _float3 vCenter = const_cast<CBoundingAABB*>(pAABB)->GetBounding()->Center;
    _float3 vExtents = const_cast<CBoundingAABB*>(pAABB)->GetBounding()->Extents;
    _float3 vMin = const_cast<CBoundingAABB*>(pAABB)->GetMin();
    _float3 vMax = const_cast<CBoundingAABB*>(pAABB)->GetMax();

    // 충돌 지점과 AABB 중심의 상대 위치 계산
    _float3 vTempHitPos;
    XMStoreFloat3(&vTempHitPos, vHitPos);
    _float3 vRelativePos = { vTempHitPos.x - vCenter.x, vTempHitPos.y - vCenter.y, vTempHitPos.z - vCenter.z };

    // 노말 벡터 초기화
    _float3 vNormal = { 0.0f, 0.0f, 0.0f };

    // 충돌 방향을 기준으로 가장 가까운 축 계산
    if (abs(vRelativePos.x) / vExtents.x > abs(vRelativePos.y) / vExtents.y && abs(vRelativePos.x) / vExtents.x > abs(vRelativePos.z) / vExtents.z)
    {
        // X축 방향
        vNormal = (vRelativePos.x > 0) ? _float3(1, 0, 0) : _float3(-1, 0, 0);
    }
    else if (abs(vRelativePos.y) / vExtents.y > abs(vRelativePos.z) / vExtents.z)
    {
        // Y축 방향
        vNormal = (vRelativePos.y > 0) ? _float3(0, 1, 0) : _float3(0, -1, 0);
    }
    else
    {
        // Z축 방향
        vNormal = (vRelativePos.z > 0) ? _float3(0, 0, 1) : _float3(0, 0, -1);
    }

    vOutNormal = vNormal;
}

void CStateObjectContainer::SetDamage(_int iDamage)
{
    m_iHP -= iDamage;
    if (m_iHP <= 0)
    {
        m_bDead = true;
    }
}

HRESULT CStateObjectContainer::ReadyComponents()
{
    return S_OK;
}

HRESULT CStateObjectContainer::ReadyPartObjects()
{
    return S_OK;
}

HRESULT CStateObjectContainer::ReadyNavMesh()
{
    _int eType = (_int)GetNavMeshIdx();

    CNavigation::NAVIGATION_DESC		NaviDesc{};
    NaviDesc.iStartIndex = -1;
    XMStoreFloat3(&NaviDesc.vWorldPos, m_pTransformCom->GetTransformState(CTransform::STATE_POS));

    switch (eType)
    {
    case Client::CStateObjectContainer::S1:
    {
        if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationComs[CStateObjectContainer::NAVMESH_IDX::S1]), &NaviDesc)))
            return E_FAIL;
    }
        break;
    case Client::CStateObjectContainer::S2:
    {
        if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Navigation_S2"), TEXT("Com_Navigation_S2"), reinterpret_cast<CComponent**>(&m_pNavigationComs[CStateObjectContainer::NAVMESH_IDX::S2]), &NaviDesc)))
            return E_FAIL;

    }
        break;
    case Client::CStateObjectContainer::S3:
    {
        if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Navigation_S3"), TEXT("Com_Navigation_S3"), reinterpret_cast<CComponent**>(&m_pNavigationComs[CStateObjectContainer::NAVMESH_IDX::S3]), &NaviDesc)))
            return E_FAIL;
    }
        break;
    case Client::CStateObjectContainer::S_BOSS:
    {
        if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Navigation_Boss"), TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationComs[CStateObjectContainer::NAVMESH_IDX::S_BOSS]), &NaviDesc)))
            return E_FAIL;
    }
        break;
    case Client::CStateObjectContainer::S_END:
        break;
    default:
        break;
    }
    
    return S_OK;
}

CGameObject* CStateObjectContainer::Clone(void* pArg)
{
    CStateObjectContainer* pInstance = new CStateObjectContainer(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CStateObjectContainer");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CStateObjectContainer::Free()
{
    __super::Free();
}


#ifdef _DEBUG



string CStateObjectContainer::GetStateDebug()
{
    if (OBJECT_STATE::STATE_IDLE & m_iState) return "IDLE";
    else if (OBJECT_STATE::STATE_WALK & m_iState) return "WALK";
    else if (OBJECT_STATE::STATE_JUMP & m_iState) return "JUMP";
    else if (OBJECT_STATE::STATE_FIRE & m_iState) return "FIRE";
    else if (OBJECT_STATE::STATE_RELOAD & m_iState) return "RELOAD";
    else if (OBJECT_STATE::STATE_PUT & m_iState) return "PUT";
    else if (OBJECT_STATE::STATE_DRAW & m_iState) return "DRAW";

    return "NONE";
}

string CStateObjectContainer::GetPrevStateDebug()
{
    if (OBJECT_STATE::STATE_IDLE & m_iPrevState) return "IDLE";
    else if (OBJECT_STATE::STATE_WALK & m_iPrevState) return "WALK";
    else if (OBJECT_STATE::STATE_JUMP & m_iPrevState) return "JUMP";
    else if (OBJECT_STATE::STATE_FIRE & m_iPrevState) return "FIRE";
    else if (OBJECT_STATE::STATE_RELOAD & m_iPrevState) return "RELOAD";
    else if (OBJECT_STATE::STATE_PUT & m_iPrevState) return "PUT";
    else if (OBJECT_STATE::STATE_DRAW & m_iPrevState) return "DRAW";


    return "NONE";
}

#endif 
