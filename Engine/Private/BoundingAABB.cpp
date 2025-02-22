#include "BoundingAABB.h"
#include "BoundingOBB.h"
#include "BoundingSphere.h"

#include <DirectXCollision.h> // BoundingBox 정의
#include <DirectXMath.h>      // FXMVECTOR 및 XMVector 관련 함수

CBoundingAABB::CBoundingAABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: 
	CBounding(pDevice, pContext)
{
}

HRESULT CBoundingAABB::Initialize(const BOUND_DESC* pBoundDesc)
{
	const BOUND_AABB_DESC* pDesc = static_cast<const BOUND_AABB_DESC*>(pBoundDesc);

	m_pOriginBoundingBox = new BoundingBox(pDesc->vCenter, pDesc->vExtents);
	m_pBoundingBox = new BoundingBox(*m_pOriginBoundingBox);

    return S_OK;
}

void CBoundingAABB::Update(_fmatrix WorldMatrix)
{
	_matrix			TransformMatrix = WorldMatrix;

	TransformMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVector3Length(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVector3Length(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVector3Length(TransformMatrix.r[2]);

	m_pOriginBoundingBox->Transform(*m_pBoundingBox, TransformMatrix);
}

void CBoundingAABB::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	DX::Draw(pBatch, *m_pBoundingBox, m_bIsCollsionEnter == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));
}

_bool CBoundingAABB::Intersect(CCollider::COLLIDER_TYPE eType, CBounding* pOtherBounding)
{
	m_bIsCollsionEnter = false;

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		m_bIsCollsionEnter = m_pBoundingBox->Intersects(*static_cast<CBoundingAABB*>(pOtherBounding)->GetBounding());
		// m_bIsCollsionEnter = IntersectToAABB(static_cast<CBoundingAABB*>(pOtherBounding)->GetBounding());
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

_bool CBoundingAABB::Intersect(CCollider::COLLIDER_TYPE eType, CBounding* pOtherBounding, HIT_RESULT& HitResult)
{
	m_bIsCollsionEnter = false;

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		m_bIsCollsionEnter = m_pBoundingBox->Intersects(*static_cast<CBoundingAABB*>(pOtherBounding)->GetBounding());
		// m_bIsCollsionEnter = IntersectToAABB(static_cast<CBoundingAABB*>(pOtherBounding)->GetBounding());
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

_bool CBoundingAABB::Intersect(_fvector vRayOrigin, _fvector vRayDir, _float& fDist, _float4& vPickingPos)
{
	// 방향 벡터 정규화 (필수)
	_vector vRayNorDir = XMVector3Normalize(vRayDir);	

	// Ray ray;
	if (m_pBoundingBox->Intersects(vRayOrigin, vRayNorDir, fDist))
		return true;
	return false;
}

_bool CBoundingAABB::IntersectToAABB(CBoundingAABB* pOther)
{
	_float3		vSrcMin, vSrcMax, vDstMin, vDstMax;

	vSrcMin = _float3(m_pBoundingBox->Center.x - m_pBoundingBox->Extents.x, m_pBoundingBox->Center.y - m_pBoundingBox->Extents.y, m_pBoundingBox->Center.z - m_pBoundingBox->Extents.z);
	vSrcMax = _float3(m_pBoundingBox->Center.x + m_pBoundingBox->Extents.x, m_pBoundingBox->Center.y + m_pBoundingBox->Extents.y, m_pBoundingBox->Center.z + m_pBoundingBox->Extents.z);

	vDstMin = _float3(pOther->m_pBoundingBox->Center.x - pOther->m_pBoundingBox->Extents.x, pOther->m_pBoundingBox->Center.y - pOther->m_pBoundingBox->Extents.y, pOther->m_pBoundingBox->Center.z - pOther->m_pBoundingBox->Extents.z);
	vDstMax = _float3(pOther->m_pBoundingBox->Center.x + pOther->m_pBoundingBox->Extents.x, pOther->m_pBoundingBox->Center.y = pOther->m_pBoundingBox->Extents.y, pOther->m_pBoundingBox->Center.z + pOther->m_pBoundingBox->Extents.z);

	// 너비가 곂치지 않았나?
	if (min(vSrcMax.x, vDstMax.x) < max(vSrcMin.x, vDstMin.x))
		return false;

	// 높이가 곂치지 않았나?
	if (min(vSrcMax.y, vDstMax.y) < max(vSrcMin.y, vDstMin.y))
		return false;

	// 깊이가 곂치지 않았나?
	if (min(vSrcMax.z, vDstMax.z) < max(vSrcMin.z, vDstMin.z))
		return false;

	return true;
}

CBoundingAABB* CBoundingAABB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const BOUND_DESC* pBoundDesc)
{
	CBoundingAABB* pInstance = new CBoundingAABB(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pBoundDesc)))
	{
		MSG_BOX("Failed to Created : CBoundingAABB");
		SafeRelease(pInstance);
	}
	return pInstance;
}

void CBoundingAABB::Free()
{
    __super::Free();

    SafeDelete(m_pOriginBoundingBox);
    SafeDelete(m_pBoundingBox);
}
