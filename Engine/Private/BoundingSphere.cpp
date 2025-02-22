#include "BoundingSphere.h"
#include "BoundingAABB.h"
#include "BoundingOBB.h"
#include <algorithm>

CBoundingSphere::CBoundingSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : 
    CBounding{ pDevice, pContext }
{
}

HRESULT CBoundingSphere::Initialize(const BOUND_DESC* pBoundDesc)
{
    const BOUND_SPHERE_DESC* pDesc = static_cast<const BOUND_SPHERE_DESC*>(pBoundDesc);

    m_pOriginBoundingBox = new BoundingSphere(pDesc->vCenter, pDesc->fRadius);
    m_pBoundingBox = new BoundingSphere(*m_pOriginBoundingBox);

    return S_OK;
}

void CBoundingSphere::Update(_fmatrix WorldMatrix)
{
    m_pOriginBoundingBox->Transform(*m_pBoundingBox, WorldMatrix);
}

void CBoundingSphere::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
    DX::Draw(pBatch, *m_pBoundingBox, m_bIsCollsionEnter == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));
}

_bool CBoundingSphere::Intersect(CCollider::COLLIDER_TYPE eType, CBounding* pOtherBounding)
{
    m_bIsCollsionEnter = false;

    switch (eType)
    {
    case CCollider::TYPE_AABB:
    {
        m_bIsCollsionEnter = m_pBoundingBox->Intersects(*static_cast<CBoundingAABB*>(pOtherBounding)->GetBounding());
    }
        break;

    case CCollider::TYPE_OBB:
        m_bIsCollsionEnter = m_pBoundingBox->Intersects(*static_cast<CBoundingOBB*>(pOtherBounding)->GetBounding());
        break;

    case CCollider::TYPE_SPHERE:
    {
        m_bIsCollsionEnter = m_pBoundingBox->Intersects(*static_cast<CBoundingSphere*>(pOtherBounding)->GetBounding());
        
    }
        break;
    }

    

    return m_bIsCollsionEnter;
}

_bool CBoundingSphere::Intersect(CCollider::COLLIDER_TYPE eType, CBounding* pOtherBounding, HIT_RESULT& HitResult)
{
    m_bIsCollsionEnter = false;

    switch (eType)
    {
    case CCollider::TYPE_AABB:
    {
        m_bIsCollsionEnter = m_pBoundingBox->Intersects(*static_cast<CBoundingAABB*>(pOtherBounding)->GetBounding());

        // 충돌 했다면 
        if (m_bIsCollsionEnter)
        {
            FillHitResult(CCollider::TYPE_SPHERE, CCollider::TYPE_AABB, this, pOtherBounding, HitResult);
        }
    }
        break;

    case CCollider::TYPE_OBB:
        m_bIsCollsionEnter = m_pBoundingBox->Intersects(*static_cast<CBoundingOBB*>(pOtherBounding)->GetBounding());
        break;

    case CCollider::TYPE_SPHERE:
        m_bIsCollsionEnter = m_pBoundingBox->Intersects(*static_cast<CBoundingSphere*>(pOtherBounding)->GetBounding());
        break;
    }

    return m_bIsCollsionEnter;
}

_bool CBoundingSphere::Intersect(_fvector vRayOrigin, _fvector vRayDir, _float& fDist, _float4& vPickingPos)
{
    return _bool();
}

CBoundingSphere* CBoundingSphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const BOUND_DESC* pBoundDesc)
{
    CBoundingSphere* pInstance = new CBoundingSphere(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pBoundDesc)))
    {
        MSG_BOX("Failed to Created : CBoundingSphere");
        SafeRelease(pInstance);
    }
    return pInstance;
}

void CBoundingSphere::Free()
{
    __super::Free();

    SafeDelete(m_pOriginBoundingBox);
    SafeDelete(m_pBoundingBox);
}
