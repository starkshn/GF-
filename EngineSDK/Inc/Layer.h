#pragma once
#include "Base.h"

// 객체들을 Layer 별로 묶어서 다수 보관한다.
// CLayer는 Object Manager가 들고 있는다.

BEGIN(Engine)

// forward declarations...
class CGameObject;
class CComponent;
class CCollider;

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() override = default;

public :
	HRESULT						AddGameObject(CGameObject* pGameObject);
	void						PriorityUpdate(const _float& fTimeDelta);
	void						Update(const _float& fTimeDelta);
	void						Late_Update(const _float& fTimeDelta);

public:
	CGameObject*				GetGameObject(const string& strObjectTag);
	CGameObject*				GetGameObjectBack();
	const list<CGameObject*>&	GetGameObjectList() { return m_GameObjects; }
	CGameObject*				GetPickedGameObject(_fvector vRayOrigin, _fvector vRayDir, _float& fDist, _float4& vPickingPos);
	CComponent*					GetComponent(const _wstring& strComponentTag, _uint iObjectIndex = 0);
	CComponent*					GetComponent(_uint iPartObjectIndex, const _wstring& strPartComponentTag, _uint iObjectIndex = 0);


	_bool						CheckIntersect(const CCollider* pSrc);
	_bool						CheckIntersect(const CCollider* pSrc, HIT_RESULT& HitResult);
	_bool						CheckIntersectOnlyProjectile(CCollider* pProjectileCol, HIT_RESULT& HitResult);

public:
	_bool						RemoveFromLayer(CGameObject* pRemoveObject);

public:
	void						ActiveMonsters();

private:
	list<CGameObject*>			m_GameObjects{};

public:
	static CLayer*				Create();
	virtual void				Free() override;
};

END