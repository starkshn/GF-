#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "ParticleMonsterHit.h"
#include "Projectile.h"

BEGIN(Engine)

class CCollider;

END


BEGIN(Client)

class CWallAABB final : public CGameObject
{
public:
	typedef struct wallAABB_desc : public GAMEOBJECT_DESC
	{
		_float3 vCenter;
		_float3 vExtents;
	} WALL_AABB_DESC;

private:
	CWallAABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWallAABB(const CWallAABB& Prototype);
	virtual ~CWallAABB() override = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual	void			CalNotAccuarteHitPosAndNormal(_fvector vPlayerPos, _fvector vNotAccuarteFromPos, _fvector vFromDir, _float3& vOutPos, _float3& vOutNormal) override;

	virtual	void			CalHitPosAndNormal(_fvector vPlayerPos, _fvector vPlayertoBulletDir) override;
	void					CalHitPosAndNormalWithType(_fvector vPlayerPos, _fvector vPlayertoBulletDir, PROJECTILE_TYPE eType);
	
private:
	void					ActiveWallHitEP(PROJECTILE_TYPE eType, HIT_RESULT HitRet);

private:
	HRESULT					ReadyComponents(void* pArg);

private:
	CCollider*				m_pColliderCom = nullptr;

public:
	static CWallAABB*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END