#pragma once
#include "Bounding.h"

BEGIN(Engine)

class CBoundingSphere final : public CBounding
{
public:
	typedef struct bound_sphere_desc : public CBounding::BOUND_DESC
	{
		_float		fRadius;
	}BOUND_SPHERE_DESC;

private:
	CBoundingSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBoundingSphere() = default;

public:
	virtual HRESULT			Initialize(const BOUND_DESC* pBoundDesc);
	virtual void			Update(_fmatrix WorldMatrix) override;
	virtual void			Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;
	virtual _bool			Intersect(CCollider::COLLIDER_TYPE eType, CBounding* pOtherBounding) override;
	virtual _bool			Intersect(CCollider::COLLIDER_TYPE eType, CBounding* pOtherBounding, HIT_RESULT& HitResult) override;
	virtual _bool			Intersect(_fvector vRayOrigin, _fvector vRayDir, _float& fDist, _float4& vPickingPos) override;



public:
	const BoundingSphere*	GetBounding() const { return m_pBoundingBox; }

private:
	BoundingSphere* m_pOriginBoundingBox = {};
	BoundingSphere* m_pBoundingBox = {};

public:
	static CBoundingSphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const BOUND_DESC* pBoundDesc);
	virtual void Free() override;
};

END