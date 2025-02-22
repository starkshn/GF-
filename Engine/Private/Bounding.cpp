#include "Bounding.h"
#include "BoundingAABB.h"
#include "BoundingOBB.h"
#include "BoundingSphere.h"

#undef min
#undef max

CBounding::CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    m_pDevice(pDevice),
    m_pContext(pContext)
{
    SafeAddRef(m_pDevice);
    SafeAddRef(m_pContext);
}

HRESULT CBounding::Initialize()
{
    return S_OK;
}

void CBounding::FillHitResult(CCollider::COLLIDER_TYPE eTypeSrc, CCollider::COLLIDER_TYPE eTypeDst, CBounding* pSrc, CBounding* pDst, HIT_RESULT& HitResult)
{
    switch (eTypeSrc)
    {
    case Engine::CCollider::TYPE_SPHERE:
    {
        switch (eTypeDst)
        {
        case Engine::CCollider::TYPE_SPHERE:
            break;
        case Engine::CCollider::TYPE_AABB:
        {
            ComputeHitPointSphereAABB(pSrc, pDst, HitResult);
        }
            break;
        case Engine::CCollider::TYPE_OBB:
            break;
        case Engine::CCollider::TYPE_END:
            break;
        default:
            break;
        }
    }
        break;
    case Engine::CCollider::TYPE_AABB:
        break;
    case Engine::CCollider::TYPE_OBB:
        break;
    case Engine::CCollider::TYPE_END:
        break;
    default:
        break;
    }
}

void CBounding::ComputeHitPointSphereAABB(CBounding* pSrc, CBounding* pDst, HIT_RESULT& HitResult)
{
    _float3 vSphereCenter = static_cast<CBoundingSphere*>(pSrc)->GetBounding()->Center;
    _float  vRadius = static_cast<CBoundingSphere*>(pSrc)->GetBounding()->Radius;

    const BoundingBox* pAABB = static_cast<CBoundingAABB*>(pDst)->GetBounding();
    _float3 vAABBCenter = pAABB->Center;
    _float3 vAABBExtents = pAABB->Extents;

    // AABB의 최소/최대 좌표 계산
    _float3 aabbMin = 
    { vAABBCenter.x - vAABBExtents.x, vAABBCenter.y - vAABBExtents.y, vAABBCenter.z - vAABBExtents.z };
    _float3 aabbMax = 
    { vAABBCenter.x + vAABBExtents.x, vAABBCenter.y + vAABBExtents.y, vAABBCenter.z + vAABBExtents.z };

    // 충돌 지점 계산 (클램핑)
    // 구의 중심 좌표를 AABB의 범위 안으로 제한하여 구에서 AABB 표면까지 가장 가까운 점을 찾는다.
    // 구의 중심이 AABB 밖에 있다면, AABB 경계에 위치시킵니다. 구의 중심이 AABB 내부에 있다면, 구의 중심 그대로 반환됩니다.
    _float3 collisionPoint;

    collisionPoint.x = std::min(std::max(vSphereCenter.x, aabbMin.x), aabbMax.x);
    collisionPoint.y = std::min(std::max(vSphereCenter.y, aabbMin.y), aabbMax.y);
    collisionPoint.z = std::min(std::max(vSphereCenter.z, aabbMin.z), aabbMax.z);

    HitResult.vHitPos = collisionPoint;

    // 법선 벡터 계산
    _float3 vNormal = { 0.0f, 0.0f, 0.0f };

    _float distXMin = fabs(collisionPoint.x - aabbMin.x);
    _float distXMax = fabs(collisionPoint.x - aabbMax.x);
    _float distYMin = fabs(collisionPoint.y - aabbMin.y);
    _float distYMax = fabs(collisionPoint.y - aabbMax.y);
    _float distZMin = fabs(collisionPoint.z - aabbMin.z);
    _float distZMax = fabs(collisionPoint.z - aabbMax.z);

    // 가장 가까운 표면 찾기
    _float minDist = std::min({ distXMin, distXMax, distYMin, distYMax, distZMin, distZMax });

    if (minDist == distXMin) 
        vNormal = { -1.0f, 0.0f, 0.0f };
    else if (minDist == distXMax) 
        vNormal = { 1.0f, 0.0f, 0.0f };
    else if (minDist == distYMin) 
        vNormal = { 0.0f, -1.0f, 0.0f };
    else if (minDist == distYMax) 
        vNormal = { 0.0f, 1.0f, 0.0f };
    else if (minDist == distZMin) 
        vNormal = { 0.0f, 0.0f, 1.0f };
    else if (minDist == distZMax) 
        vNormal = { 0.0f, 0.0f, -1.0f };
    else
    {
        _int a = 10;
    }

    HitResult.vNormal = vNormal;
}


void CBounding::Free()
{
    __super::Free();

    SafeRelease(m_pDevice);
    SafeRelease(m_pContext);
}
