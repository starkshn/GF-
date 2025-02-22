#pragma once
#include "Base.h"

BEGIN(Engine)

class CVIBufferCell;

class CCell final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };

private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	HRESULT					Initialize(const _float3* pPoints, _int iIndex);

public:
	_bool					IsInCell(_fvector vLocalPos, _int& iNeighborCellIndex, _float3& vNormal);
	_bool					IsInCell(_fvector vLocalPos, _int& iNeighborCellIndex);

	// Spawner에서 호출시 어떤 셀 안에 있는지 한번만 호출하는 함수
	_int					IsInCell(_fvector vLocalPos);


	_float3					GetPoint(POINT ePoint) const { return m_vPoints[ePoint]; }
	void					SetNeighbor(LINE eLine, CCell* pNeighborCell) { m_iNeighborIndices[eLine] = pNeighborCell->m_iIndex; }

public:
	_bool					ComparePoints(const _float3& SrcPoint, const _float3& DstPoint);
	_float					ComputeCellHeight(_fvector vPosition);

public:
	_int					GetCellIndex() { return m_iIndex; }

#ifdef _DEBUG
	HRESULT					Render();
#endif

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

private:
	_int					m_iIndex = { -1 };
	_float3					m_vPoints[POINT_END] = {};
	_int					m_iNeighborIndices[LINE_END] = { -1, -1, -1 };


#ifdef _DEBUG
	CVIBufferCell*			m_pVIBuffer = nullptr;
#endif

public:
	static CCell*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex);
	virtual void			Free() override;

};

END