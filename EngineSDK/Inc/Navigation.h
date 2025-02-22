#pragma once
#include "Component.h"

BEGIN(Engine)

class CCell;
class CShader;

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct nav_desc
	{
		_int iStartIndex = -1; // -1값도 사용하기 위한 목적으로 _int
		_float3 vWorldPos;
	} NAVIGATION_DESC;

private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& Prototype);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT		InitializePrototype(const _tchar* pNavigationDataFile);
	virtual HRESULT		Initialize(void* pArg);

public:
	// Call-By-Value로 월드 행렬을 매틱 받아온다.
	virtual void		Update(_fmatrix WorldMatrix);


#ifdef _DEBUG

public:
	virtual	HRESULT				Render() override;

#endif

	// 특정 오브젝트의 월드 좌표를 받아와 이를 지형의 로컬좌표로 변환시켜 계산한다.
	// Cell 벗어났다면 false, Cell안이라면 true를 리턴
	_bool				IsInCell(_fvector vWorldPos, _fvector vLook, _float3& vSlidingDir);
	_bool				IsInCell(_fvector vWorldPos);
	_bool				IsInOtherCell(_fvector vWorldPos, _fvector vWorldSlidingDir, _float3& vResultDir);
	_vector				SetUpHeightOnNavigation(_fvector vPosition);
	_int				FindCellIndex(_fvector vPos);

private:
	HRESULT				SetUpNeighbors();
	_float				ComputeCellHeight(_int iCellIndex, _fvector vPosition);	// 셀의 높이 계산하는 함수

public:
	_int				GetCurrentCellIndex() { return m_iCurrentCellIndex; }

private:
	CShader*			m_pShader = nullptr;

private:
	vector<CCell*>		m_Cells;
	static _float4x4	m_WorldMatrix;
	_int				m_iCurrentCellIndex = -1;

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFile);
	virtual CComponent* Clone(void* pArg) override;
	virtual void		Free() override;
};

END