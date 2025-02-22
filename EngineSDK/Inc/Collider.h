#pragma once
#include "Component.h"


// 객체에 씌워야할 충돌체를 의미한다.
// Sphere, AABB, OBB


BEGIN(Engine)

class CBounding;

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum COLLIDER_TYPE {TYPE_SPHERE, TYPE_AABB, TYPE_OBB, TYPE_END};

private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& Prototype);
	virtual ~CCollider() = default;

public:
	virtual HRESULT							InitializePrototype(COLLIDER_TYPE eType);
	virtual HRESULT							Initialize(void* pArg) override;
	virtual void							Update(_fmatrix WorldMatrix);

#ifdef _DEBUG

	virtual HRESULT							Render() override;
	virtual HRESULT							Render(_fvector vColor);

#endif

public:
	_bool									Intersect(const CCollider* pOtherColiider);
	_bool									Intersect(const CCollider* pOtherColiider, HIT_RESULT& HitResult);
	_bool									IntersectOnlyProjectile(CCollider* pProjectileColiider, HIT_RESULT& HitResult);

	// AABB <-> RAY
	_bool									Intersect(_fvector vRayOrigin, _fvector vRayDir, _float& fDist, _float4& vPickingPos);

public:
	const CBounding*						GetBounding() { return m_pBounding; };
	CBounding*								GetBoundingRaw() { return m_pBounding; };
	
public:
	_bool									IsEndCollision() { return m_bEndCollision; }
	void									SetEndCollision() { m_bEndCollision = true; }

	_bool									IsActiveCol() { return m_bActiveCol; }
	void									ActiveCol() { m_bActiveCol = true; }
	void									DeActiveCol() { m_bActiveCol = false; }
	void									ResetCol() { m_bEndCollision = false; m_bActiveCol = false; }

public:
	void									SetIsCanCol(_bool bIsCanCol) { m_bIsCanCol = bIsCanCol; }
	_bool									IsCanCol() { return m_bIsCanCol; }

private:
	COLLIDER_TYPE							m_eType = TYPE_END;
	CBounding*								m_pBounding = nullptr;

	// 디버그용 멤버 변수
#ifdef _DEBUG
	PrimitiveBatch<VertexPositionColor>*	m_pBatch = nullptr;
	BasicEffect*							m_pEffect = nullptr;
	ID3D11InputLayout*						m_pInputLayout = nullptr;
#endif

private:
	_bool									m_bActiveCol = true;;
	_bool									m_bEndCollision = false;

private:
	_bool									m_bIsCanCol = false;

public:
	static CCollider*						Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER_TYPE eType);
	virtual CComponent*						Clone(void* pArg) override;
	virtual void							Free() override;
};

END