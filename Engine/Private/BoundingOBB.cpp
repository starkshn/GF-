#include "BoundingOBB.h"
#include "BoundingAABB.h"
#include "BoundingSphere.h"


CBoundingOBB::CBoundingOBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CBounding{ pDevice, pContext }
{
}

HRESULT CBoundingOBB::Initialize(const BOUND_DESC* pBoundDesc)
{
	const BOUND_OBB_DESC* pDesc = static_cast<const BOUND_OBB_DESC*>(pBoundDesc);

	_float4		vRotation;
	XMStoreFloat4(&vRotation, XMQuaternionRotationRollPitchYaw(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z));

	m_pOriginBoundingBox = new BoundingOrientedBox(pDesc->vCenter, pDesc->vExtents, vRotation);
	m_pBoundingBox = new BoundingOrientedBox(*m_pOriginBoundingBox);

	return S_OK;
}

void CBoundingOBB::Update(_fmatrix WorldMatrix)
{
	m_pOriginBoundingBox->Transform(*m_pBoundingBox, WorldMatrix);
}

void CBoundingOBB::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	DX::Draw(pBatch, *m_pBoundingBox, m_bIsCollsionEnter == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));
}

_bool CBoundingOBB::Intersect(CCollider::COLLIDER_TYPE eType, CBounding* pOtherBounding)
{
	m_bIsCollsionEnter = false;

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		m_bIsCollsionEnter = m_pBoundingBox->Intersects(*static_cast<CBoundingAABB*>(pOtherBounding)->GetBounding());
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

_bool CBoundingOBB::Intersect(CCollider::COLLIDER_TYPE eType, CBounding* pOtherBounding, HIT_RESULT& HitResult)
{
	m_bIsCollsionEnter = false;

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		m_bIsCollsionEnter = m_pBoundingBox->Intersects(*static_cast<CBoundingAABB*>(pOtherBounding)->GetBounding());
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

_bool CBoundingOBB::Intersect(_fvector vRayOrigin, _fvector vRayDir, _float& fDist, _float4& vPickingPos)
{
	return _bool();
}

_bool CBoundingOBB::IntersectToOBB(CBoundingOBB* pTarget)
{
	m_bIsCollsionEnter = false;

	OBBDESC		OBBDesc[2];

	OBBDesc[0] = ComputeOBB();
	OBBDesc[1] = pTarget->ComputeOBB();

	_float		fLength[3] = {};

	for (size_t i = 0; i < 2; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			_vector	vCenterDir = XMLoadFloat3(&OBBDesc[1].vCenter) - XMLoadFloat3(&OBBDesc[0].vCenter);

			fLength[0] = fabs(XMVectorGetX(XMVector3Dot(vCenterDir, XMLoadFloat3(&OBBDesc[i].vAlignDir[j]))));

			fLength[1] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vExtentDir[0]), XMLoadFloat3(&OBBDesc[i].vAlignDir[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vExtentDir[1]), XMLoadFloat3(&OBBDesc[i].vAlignDir[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vExtentDir[2]), XMLoadFloat3(&OBBDesc[i].vAlignDir[j]))));

			fLength[2] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vExtentDir[0]), XMLoadFloat3(&OBBDesc[i].vAlignDir[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vExtentDir[1]), XMLoadFloat3(&OBBDesc[i].vAlignDir[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vExtentDir[2]), XMLoadFloat3(&OBBDesc[i].vAlignDir[j]))));

			if (fLength[0] > fLength[1] + fLength[2])
				return m_bIsCollsionEnter;
		}
	}

	return m_bIsCollsionEnter = true;
}

CBoundingOBB::OBBDESC CBoundingOBB::ComputeOBB()
{
	OBBDESC		OBBDesc{};

	_float3		vPoints[8];

	m_pBoundingBox->GetCorners(vPoints);

	OBBDesc.vCenter = m_pBoundingBox->Center;

	_float3		vAxis[3] = {};

	XMStoreFloat3(&vAxis[0], XMLoadFloat3(&vPoints[5]) - XMLoadFloat3(&vPoints[4]));
	XMStoreFloat3(&vAxis[1], XMLoadFloat3(&vPoints[7]) - XMLoadFloat3(&vPoints[4]));
	XMStoreFloat3(&vAxis[2], XMLoadFloat3(&vPoints[0]) - XMLoadFloat3(&vPoints[4]));

	for (size_t i = 0; i < 3; i++)
		XMStoreFloat3(&OBBDesc.vExtentDir[i], XMLoadFloat3(&vAxis[i]) * 0.5f);

	for (size_t i = 0; i < 3; i++)
		XMStoreFloat3(&OBBDesc.vAlignDir[i], XMVector3Normalize(XMLoadFloat3(&vAxis[i])));

	return OBBDesc;
}

CBoundingOBB* CBoundingOBB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const BOUND_DESC* pBoundDesc)
{
	CBoundingOBB* pInstance = new CBoundingOBB(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pBoundDesc)))
	{
		MSG_BOX("Failed to Created : CBoundingOBB");
		SafeRelease(pInstance);
	}
	return pInstance;
}

void CBoundingOBB::Free()
{
	__super::Free();

	SafeDelete(m_pOriginBoundingBox);
	SafeDelete(m_pBoundingBox);
}
