#pragma once
#include "Bounding.h"

BEGIN(Engine)

class CBoundingAABB final : public CBounding
{
public:
	typedef struct bound_aabb_desc : public CBounding::BOUND_DESC
	{
		_float3 vExtents;
	} BOUND_AABB_DESC;

private:
	CBoundingAABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBoundingAABB() = default;

public:
	virtual HRESULT			Initialize(const BOUND_DESC* pBoundDesc);
	virtual void			Update(_fmatrix WorldMatrix) override;
	virtual void			Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;
	virtual _bool			Intersect(CCollider::COLLIDER_TYPE eType, CBounding* pOtherBounding) override;
	virtual _bool			Intersect(CCollider::COLLIDER_TYPE eType, CBounding* pOtherBounding, HIT_RESULT& HitResult) override;
	virtual _bool			Intersect(_fvector vRayOrigin, _fvector vRayDir, _float& fDist, _float4& vPickingPos) override;


public:
	const BoundingBox*		GetBounding() const { return m_pBoundingBox; }

public:
	_float3 GetMin()  {  return _float3(m_pBoundingBox->Center.x - m_pBoundingBox->Extents.x, m_pBoundingBox->Center.y - m_pBoundingBox->Extents.y, m_pBoundingBox->Center.z - m_pBoundingBox->Extents.z );  }
	_float3 GetMax()  {  return _float3(m_pBoundingBox->Center.x + m_pBoundingBox->Extents.x, m_pBoundingBox->Center.y + m_pBoundingBox->Extents.y, m_pBoundingBox->Center.z + m_pBoundingBox->Extents.z );  }

private:
	_bool					IntersectToAABB(CBoundingAABB* pOther);

private:
	BoundingBox*			m_pOriginBoundingBox = nullptr;
	BoundingBox*			m_pBoundingBox = nullptr;

public:
	static CBoundingAABB*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const BOUND_DESC* pBoundDesc);
	virtual void			Free() override;
};

END