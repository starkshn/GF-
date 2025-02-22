#pragma once
#include "Client_Defines.h"
#include "StateObjectWeapon.h"
#include "MonsterBase.h"
#include "MonsterBullet.h"
#include "Player.h"

BEGIN(Engine)

class CShader;
class CModel;
class CCollider;

END

BEGIN(Client)

class CMonsterWeaponBase : public CStateObjectWeapon
{
public:
	enum MONSTER_PROJECTILE_TYPE { MPT_01, MPT_02, MPT_END};

protected:
	CMonsterWeaponBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterWeaponBase(const CMonsterWeaponBase& Prototype);
	virtual ~CMonsterWeaponBase() = default;

public:
	virtual HRESULT					InitializePrototype() override;
	virtual HRESULT					Initialize(void* pArg) override;

public:
	_uint							GetOwnerLevelID();

	HRESULT							FireProjectile(MONSTER_PROJECTILE_TYPE eType);
	HRESULT							CreateProjectile(MONSTER_PROJECTILE_TYPE eType, _fvector vBulletDir);
	HRESULT							ActiveProjectileEffect(MONSTER_PROJECTILE_TYPE eType, CMonsterBullet* pBullet);

protected:
	CCollider*						m_pOtherCol = nullptr;

public:
	virtual void					Free() override;
};

END