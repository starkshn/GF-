#pragma once
#include "Base.h"


BEGIN(Engine)

class CGameInstance;

class CQuadTree final : public CBase
{
public:
	enum CORNER { COR_LT, COR_RT, COR_RB, COR_LB, COR_END};
	enum NEIGHBOR { NEIGHBOR_LEFT, NEIGHBOR_TOP, NEIGHBOR_RIGHT, NEIGHBOR_BOTTOM, NEIGHBOR_END };

private:
	CQuadTree();
	virtual ~CQuadTree() = default;

public:
	HRESULT		Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB);
	void		Culling(const _float3* pVertexPositions, _uint* pIndices, _uint* pNumIndices, _fmatrix WorldMatrixInv);
	void		MakeNeighbors();

private:
	_uint							m_iCorners[COR_END] = {};
	_uint							m_iCenter = {};
	CQuadTree*						m_pChildren[COR_END] = { nullptr };
	CGameInstance*					m_pGameInst = nullptr;

	CQuadTree*						m_pNeighbors[NEIGHBOR_END];

private:
	_bool							IsDraw(const _float3* pVertexPos, _fmatrix WorldMatrixInv);

public:
	static CQuadTree* Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB);
	virtual void Free() override;
};

END