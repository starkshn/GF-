#include "PipeLine.h"

CPipeLine::CPipeLine()
{
}


HRESULT CPipeLine::Initialize()
{
    return S_OK;
}

void CPipeLine::Update()
{
	// Update를 돌면서 모든 행렬에 대해 역행렬을 구해 저장한다.
	for (_uint i = 0; i < DTS::D3DTS_END; ++i)
		XMStoreFloat4x4(&m_TransformInverseMatrices[i], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformMatrices[i])));

	// D3DTS_VIEW로부터 카메라의 위치를 구한다. 241004
	// - 카메라의 월드행렬의 역행렬(뷰 스페이스 변환 행렬)을 m_TransformMatrices[D3DTS_VIEW]에 저장해두었는데 m_TransformMatrices[D3DTS_VIEW]의 역행렬을 다시 m_TransformInverseMatrices[DTS::D3DTS_VIEW]에 저장해두었다.
	// - 카메라 월드 행렬의 역행렬은 뷰 스페이스 변환 행렬이고, 뷰 스페이스 변환 행렬은 카메라의 월드 변환 행렬이다.
	// - 이 카메라의 월드 변환 행렬의 3행의 성분(위치)를 m_vCamPos에 담아둔다.
	XMStoreFloat4(&m_vCamPos, XMLoadFloat4(reinterpret_cast<_float4*>(&m_TransformInverseMatrices[DTS::D3DTS_VIEW].m[3])));
}

CPipeLine* CPipeLine::Create()
{
	CPipeLine* pInstance = new CPipeLine();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CPipeLine");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CPipeLine::Free()
{
	__super::Free();
}
