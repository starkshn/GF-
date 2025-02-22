#include "Frustum.h"
#include "GameInstance.h"

CFrustum::CFrustum()
	:
	m_pGameInst(CGameInstance::GetInstance())
{
	SafeAddRef(m_pGameInst);
}

HRESULT CFrustum::Initialize()
{
	// 절두체를 구성하기 위한 8개의 점을 만들어준다.
	// NDC 상에 존재한다.
	m_vPoints[0] = _float3(-1.f, 1.f, 0.f);
	m_vPoints[1] = _float3(1.f, 1.f, 0.f);
	m_vPoints[2] = _float3(1.f, -1.f, 0.f);
	m_vPoints[3] = _float3(-1.f, -1.f, 0.f);

	m_vPoints[4] = _float3(-1.f, 1.f, 1.f);
	m_vPoints[5] = _float3(1.f, 1.f, 1.f);
	m_vPoints[6] = _float3(1.f, -1.f, 1.f);
	m_vPoints[7] = _float3(-1.f, -1.f, 1.f);

	return S_OK;
}

void CFrustum::Update()
{
	// 8개의 점을 월드까지 변환 시킨 후, 평면을 구성한다.
	// 모든 오브젝트들으 공통의 영역은 월드이기때문이다.

	_matrix ViewInv = m_pGameInst->GetTransformMatrixInverse(CPipeLine::D3DTS_VIEW);
	_matrix ProjInv = m_pGameInst->GetTransformMatrixInverse(CPipeLine::D3DTS_PROJ);

	for (_int i = 0; i < 8; ++i)
	{
		// View까지 이동
		// Coord함수의 경우 역행렬을 곱하면 w곱하기를 수행해준다. Shader의 mul함수와 다르다.
		XMStoreFloat3(&m_vPointsInWorld[i], XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[i]), ProjInv));

		// 월드까지 이동
		XMStoreFloat3(&m_vPointsInWorld[i], XMVector3TransformCoord(XMLoadFloat3(&m_vPointsInWorld[i]), ViewInv));
	}
	
	MakePlanes(m_vPointsInWorld, m_vPlanesInWorld);
}

void CFrustum::TransformToLocalSpace(_matrix WorldMatrix)
{
	_matrix WorldMatrixInv = XMMatrixInverse(nullptr, WorldMatrix);
	_float3 vLocalPoints[8] = {};
	for (_int i = 0; i < 8; ++i)
	{
		XMStoreFloat3(&vLocalPoints[i], XMVector3TransformCoord(XMLoadFloat3(&m_vPointsInWorld[i]), WorldMatrixInv));
	}

	MakePlanes(vLocalPoints, m_vPlanesInLocal);
}

_bool CFrustum::IsInWorldSpace(_fvector vWorldPos, _float fRange)
{
	for (_int i = 0; i < 6; ++i)
	{
		// a b c d
		// x y z w(1)
		// ax + by + cz + d = ? => 벡터의 dot 연산과 같다.
		// - w 값 신용할 수 없다면 XMPlaneDot 를 쓴다.
		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vPlanesInWorld[i]), vWorldPos)))
			return false;
	}
	return true;
}

_bool CFrustum::IsInLocalSpace(_fvector vLocalPos, _float fRange)
{
	for (_int i = 0; i < 6; i++)
	{
		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vPlanesInLocal[i]), vLocalPos)))
			return false;
	}

	return true;
}

void CFrustum::MakePlanes(const _float3* pPoints, _float4* pPlanes)
{
	/* +x */
	XMStoreFloat4(&pPlanes[0], XMPlaneFromPoints(
		XMLoadFloat3(&pPoints[1]),
		XMLoadFloat3(&pPoints[5]),
		XMLoadFloat3(&pPoints[6])));
	/* -x */
	XMStoreFloat4(&pPlanes[1], XMPlaneFromPoints(
		XMLoadFloat3(&pPoints[4]),
		XMLoadFloat3(&pPoints[0]),
		XMLoadFloat3(&pPoints[3])));

	/* +y */
	XMStoreFloat4(&pPlanes[2], XMPlaneFromPoints(
		XMLoadFloat3(&pPoints[4]),
		XMLoadFloat3(&pPoints[5]),
		XMLoadFloat3(&pPoints[1])));
	/* -y */
	XMStoreFloat4(&pPlanes[3], XMPlaneFromPoints(
		XMLoadFloat3(&pPoints[3]),
		XMLoadFloat3(&pPoints[2]),
		XMLoadFloat3(&pPoints[6])));

	/* +z */
	XMStoreFloat4(&pPlanes[4], XMPlaneFromPoints(
		XMLoadFloat3(&pPoints[5]),
		XMLoadFloat3(&pPoints[4]),
		XMLoadFloat3(&pPoints[7])));
	/* -z */
	XMStoreFloat4(&pPlanes[5], XMPlaneFromPoints(
		XMLoadFloat3(&pPoints[0]),
		XMLoadFloat3(&pPoints[1]),
		XMLoadFloat3(&pPoints[2])));
}

CFrustum* CFrustum::Create()
{
	CFrustum* pInstance = new CFrustum();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CFrustum");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CFrustum::Free()
{
	__super::Free();
	SafeRelease(m_pGameInst);
}
