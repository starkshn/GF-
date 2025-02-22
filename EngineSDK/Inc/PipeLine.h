#pragma once
#include "Base.h"

// PipeLine클래스의 목적, 용도
// - 자주 사용되는 투영(원근), 뷰 변환 행렬을 보관한다.
// - DX9과 다르게 DX11은 고정 렌더링 파이프라인을 지원하지 않는다. (장치에 행렬을 보관하는 개념이 사라짐)
// - 때문에 카메라가 뷰, 투영 행렬을 관리하긴 하지만 이에대한 결과물은 파이프라인에 저장해 줄 것이다.
// - 파이프라인을 통해서 쉽고 빠르게 뷰, 투영 행렬을 얻어 올 수 있도록 하겠다.
// - Update 함수를 통해서 뷰, 투영행렬의 역행렬 또한 매프레임 한번씩 구해둔다.

BEGIN(Engine)

class CPipeLine final : public CBase
{
public:
	enum D3D_TRANSFORM_STATE
	{
		D3DTS_VIEW, D3DTS_PROJ, D3DTS_END,
	};

	typedef D3D_TRANSFORM_STATE DTS;

private:
	CPipeLine();
	virtual ~CPipeLine() override = default;

public:
	void				SetTransformMatrix(DTS eState, _fmatrix TransformMatrix) { XMStoreFloat4x4(&m_TransformMatrices[eState], TransformMatrix); }
	_matrix				GetTransformMatrix(DTS eState) { return XMLoadFloat4x4(&m_TransformMatrices[eState]); }
	_float4x4			GetTransformMatrixFloat4x4(DTS eState) { return m_TransformMatrices[eState]; }
	const _float4*		GetCamPosition() const { return &m_vCamPos; }
	const _float4*		GetCameraState(CAMERA_STATE eState) 
	{ return reinterpret_cast<_float4*>(&m_TransformInverseMatrices[DTS::D3DTS_VIEW].m[eState]); }

	_matrix				GetTransformMatrixInverse(DTS eState)
	{ return XMLoadFloat4x4(&m_TransformInverseMatrices[eState]); }
	_float4x4				GetTransformMatrixFloat4x4Inverse(DTS eState)
	{ return m_TransformInverseMatrices[eState]; }
	
public:
	HRESULT				Initialize();
	void				Update();

private:
	// 행렬들을 배열로 정적 배열로 저장해둔다.
	_float4x4			m_TransformMatrices[D3DTS_END];
	_float4x4			m_TransformInverseMatrices[D3DTS_END];
	_float4				m_vCamPos = {};		// 카메라 위치는 얻어오는 경우가 굉장히 빈번하다.

public:
	static CPipeLine*	Create();
	virtual void		Free() override;
};

END