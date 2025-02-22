#pragma once
#include "Base.h"
#include "DebugDraw.h"
#include "Collider.h"

//	1. 각 충돌체 클래스들의 부모 클래스다.
//	2. 원랜 구조체를 쓰면되는 구조체의 부모 구조체가 없더라.

BEGIN(Engine)

class CBounding abstract : public CBase
{
public:
	typedef struct bound_desc
	{

		_float3		vCenter;

	} BOUND_DESC;

protected:
	CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding() = default;

public:
	virtual HRESULT				Initialize();
	virtual void				Update(_fmatrix WorldMatrix) abstract;
	virtual void				Render(PrimitiveBatch<VertexPositionColor>* pBatchs) abstract;
	virtual _bool				Intersect(CCollider::COLLIDER_TYPE eType, CBounding* pOtherBounding) abstract;
	virtual _bool				Intersect(CCollider::COLLIDER_TYPE eType, CBounding* pOtherBounding, HIT_RESULT& HitResult) abstract;
	virtual _bool				Intersect(_fvector vRayOrigin, _fvector vRayDir, _float& fDist, _float4& vPickingPos) abstract;


protected:
	void						FillHitResult(CCollider::COLLIDER_TYPE eTypeSrc, CCollider::COLLIDER_TYPE eTypeDst, CBounding* pSrc, CBounding* pDst, HIT_RESULT& HitResult);

private:
	void						ComputeHitPointSphereAABB(CBounding* pSrc, CBounding* pDst, HIT_RESULT& HitResult);

protected:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;
	_bool						m_bIsCollsionEnter = false;

public:
	virtual void				Free() override;
};

END