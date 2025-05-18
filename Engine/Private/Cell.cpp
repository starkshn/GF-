#include "Cell.h"
#include "VIBufferCell.h"
#include "GlobalFuncs.h"

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	m_pDevice(pDevice),
	m_pContext(pContext)
{
	SafeAddRef(m_pContext);
	SafeAddRef(m_pDevice);
}

HRESULT CCell::Initialize(const _float3* pPoints, _int iIndex)
{
	memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);
	m_iIndex = iIndex;

#ifdef _DEBUG

	m_pVIBuffer = CVIBufferCell::Create(m_pDevice, m_pContext, m_vPoints);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

#endif

	return S_OK;
}

_bool CCell::IsInCell(_fvector vLocalPos, _int& iNeighborCellIndex, _float3& vOutNormal)
{
	for (_int i = 0; i < LINE_END; ++i)
	{
		// POINT_END : 3
		// 1번 정점 - 0번 정점
		// 2번 정점 - 1번 정점
		// 3번 정점 - 2번 정점
		_vector vLine = XMLoadFloat3(&m_vPoints[(i + 1) % POINT_END]) - XMLoadFloat3(&m_vPoints[i]);
		_vector vLineNormal = XMVectorSet(vLine.m128_f32[2] * -1.f, 0.f, vLine.m128_f32[0], 0.f);
		_vector vDir = vLocalPos - XMLoadFloat3(&m_vPoints[i]);

		// 내적한다.
		// - 내적의 결과값이 양수라면 현재 셀 밖으로 나간 것이라 간주한다.
		if (0.f <= XMVectorGetX(XMVector3Dot(XMVector3Normalize(vLineNormal), XMVector3Normalize(vDir))))
		{		
			iNeighborCellIndex = m_iNeighborIndices[i];
			XMStoreFloat3(&vOutNormal, vLineNormal);
			return false;
		}
	}

	// 내적의 결과값이 양수인 부분에 걸리지 않았다면 현재 셀안에 있는 것이므로 true를 리턴한다.
	return true;
}

_bool CCell::IsInCell(_fvector vLocalPos, _int& iNeighborCellIndex)
{
	for (_int i = 0; i < LINE_END; ++i)
	{
		_vector vLine = XMLoadFloat3(&m_vPoints[(i + 1) % POINT_END]) - XMLoadFloat3(&m_vPoints[i]);
		_vector vNormal = XMVectorSet(vLine.m128_f32[2] * -1.f, 0.f, vLine.m128_f32[0], 0.f);
		_vector vDir = vLocalPos - XMLoadFloat3(&m_vPoints[i]);

		// 내적한다.
		// - 내적의 결과값이 양수라면 현재 셀 밖으로 나간 것이라 간주한다.
		if (0.f < XMVectorGetX(XMVector3Dot(XMVector3Normalize(vNormal), XMVector3Normalize(vDir))))
		{
			iNeighborCellIndex = m_iNeighborIndices[i];
			return false;
		}
	}

	return true;
}

_int CCell::IsInCell(_fvector vLocalPos)
{
	_bool fOuts[3] = { false, false, false };

	for (_int i = 0; i < LINE_END; ++i)
	{
		_vector vLine = XMLoadFloat3(&m_vPoints[(i + 1) % POINT_END]) - XMLoadFloat3(&m_vPoints[i]);
		_vector vNormal = XMVectorSet(vLine.m128_f32[2] * -1.f, 0.f, vLine.m128_f32[0], 0.f);
		_vector vDir = vLocalPos - XMLoadFloat3(&m_vPoints[i]);

		// 내적한다.
		// - 내적의 결과값이 양수라면 현재 셀 밖으로 나간 것이라 간주한다.
		if (0.f < XMVectorGetX(XMVector3Dot(XMVector3Normalize(vNormal), XMVector3Normalize(vDir))))
		{
			fOuts[i] = true;
		}
	}

	_int iSum = 0;
	for (_int i = 0; i < 3; ++i)
	{
		iSum += (_int)fOuts[i];
	}

	if (0 == iSum)
		return GetCellIndex();
	else
		return -1;
}

_bool CCell::ComparePoints(const _float3& SrcPoint, const _float3& DstPoint)
{
	// XMVectorEqual() : x, y, z, w 성분을 비교하여 일치하면 1, 그렇지 않으면 0을 채우는 vector를 반환
	// XMVector3Equal() : x, y, z 성분을 비교하여 전부다 일치한다면 true, 그렇지 않다면 false를 반환


	// 현재 셀의 POINT_A와 Src의 점 하나가 일치하는지 확인한다.
	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(&SrcPoint)))
	{
		// POINT_A와 Src점하나가 일치하는데 현재 셀의 B점과 DstPoint가 일치한다면 true 반환.
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(&DstPoint)))
			return true;

		// POINT_A와 Src점하나가 일치하는데 현재 셀의 C점과 DstPoint가 일치한다면 true 반환.
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(&DstPoint)))
			return true;
	}

	// 현재 셀의 POINT_B와 Src의 점 하나가 일치하는지 확인한다.
	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(&SrcPoint)))
	{
		// POINT_B와 Src점하나가 일치하는데 현재 셀의 C점과 DstPoint가 일치한다면 true 반환.
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(&DstPoint)))
			return true;

		// POINT_B와 Src점하나가 일치하는데 현재 셀의 A점과 DstPoint가 일치한다면 true 반환.
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(&DstPoint)))
			return true;
	}

	// 현재 셀의 POINT_C와 Src의 점 하나가 일치하는지 확인한다.
	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(&SrcPoint)))
	{
		// POINT_C와 Src점하나가 일치하는데 현재 셀의 A점과 DstPoint가 일치한다면 true 반환.
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(&DstPoint)))
			return true;

		// POINT_C와 Src점하나가 일치하는데 현재 셀의 B점과 DstPoint가 일치한다면 true 반환.
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(&DstPoint)))
			return true;
	}

	return false;
}

_float CCell::ComputeCellHeight(_fvector vPosition)
{
	// 직선의 방정식의 연장선이 거의 평면의 방정식이다.
	// 직선의 방정식 : ax + by = 0
	// 평면의 방정식 : ax + by + cz + d = 0

	// 높이를 구해야하기 때문에
	// y = (-ax - cz - b) / d 를 하면 y값을 알 수 있다.

	// XMPlaneFromPointNormal : 평면에 해당하는 세점과 평면에 수직인 벡터를 넘겨주면 a b c d를 구해준다.
	// XMPlaneFromPoints : 평면에 해당하는 세점을 넘겨주면 평면에 방정식에 해당하는 계수 a b c d 를 반환한다.
	// 
	// - a b c는 평면의 법선 벡터 n = (a, b, c)의 x y z의 성분이다. 이 법선벡터는 평면에 수직인 방향을 가르킨다.
	// - d값은 평면의 방정식에서 평면의 위치를 결정하는 값으로, 평면이 원점을 기준으로 얼마나 떨어져있는지를 나타낸다.
	// - d값이 0이라면 평면에 있는 것이고 양수라면 법선벡터 방향쪽에 점이 존재하고 음수라면 법선 벡터 반대 방향에 d거리만큼 있다는 것을 의미한다.
	// - 이때 인자에 넣는 점의 방향에 따라 d의 값이 양수인지 음수인지가 결정된다. 
	// - 현재 셀은 시계방향으로 셋팅해두었기 때문에 해당함수의 인자 순서도 시계방향으로 넣어주어야한다.

	// 평면에 해당하는 세점 POINT_A, POINT_B, POINT_C를 넘겨주어 a, b, c, d를 구해낸다.
	_vector vPlane = XMPlaneFromPoints(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(&m_vPoints[POINT_C]));

	// 반환한 결과값을 플레이어의 Y에 넣을 것이다.
	return (-XMVectorGetX(vPlane) * XMVectorGetX(vPosition) - XMVectorGetZ(vPlane) * XMVectorGetZ(vPosition) - XMVectorGetW(vPlane)) / XMVectorGetY(vPlane);
}

#ifdef _DEBUG
HRESULT CCell::Render()
{
	m_pVIBuffer->BindBufferDesc();
	m_pVIBuffer->Render();

	return S_OK;
}
#endif

CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex)
{
	CCell* pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex)))
	{
		MSG_BOX("Failed to Created : CCell");
		SafeRelease(pInstance);
	}
	return pInstance;
}

void CCell::Free()
{
	__super::Free();

#ifdef _DEBUG

	SafeRelease(m_pVIBuffer);

#endif

	SafeRelease(m_pContext);
	SafeRelease(m_pDevice);
}
