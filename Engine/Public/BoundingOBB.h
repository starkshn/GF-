#pragma once
#include "Bounding.h"


BEGIN(Engine)

class CBoundingOBB final : public CBounding
{
public:
	typedef struct bound_obb_desc : public CBounding::BOUND_DESC
	{
		_float3		vExtents;
		_float3		vRotation;
	}BOUND_OBB_DESC;

	typedef struct obbdesc
	{
		_float3		vCenter;
		_float3		vExtentDir[3];
		_float3		vAlignDir[3];

	}OBBDESC;

private:
	CBoundingOBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBoundingOBB() = default;


public:
	virtual HRESULT				Initialize(const BOUND_DESC* pBoundDesc);
	virtual void				Update(_fmatrix WorldMatrix) override;
	virtual void				Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;
	virtual _bool				Intersect(CCollider::COLLIDER_TYPE eType, CBounding* pOtherBounding) override;
	virtual _bool				Intersect(CCollider::COLLIDER_TYPE eType, CBounding* pOtherBounding, HIT_RESULT& HitResult) override;
	virtual _bool				Intersect(_fvector vRayOrigin, _fvector vRayDir, _float& fDist, _float4& vPickingPos) override;


public:
	const BoundingOrientedBox* GetBounding() const { return m_pBoundingBox; }

private:
	BoundingOrientedBox*		m_pOriginBoundingBox = {};
	BoundingOrientedBox*		m_pBoundingBox = {};


private:
	_bool						IntersectToOBB(CBoundingOBB* pTarget);
	OBBDESC						ComputeOBB();

public:
	static CBoundingOBB*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const BOUND_DESC* pBoundDesc);
	virtual void				Free() override;
};

END