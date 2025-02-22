#pragma once
#include "Client_Defines.h"
#include "StateObjectWeapon.h"
#include "ParticlePlayerGunFire.h"
#include "Projectile.h"
#include "PlayerBullet.h"

BEGIN(Engine)

class CCollider;

END

BEGIN(Client)

class CPlayer;
class CPlayerBody;
class CProjectile;

class CPlayerWeapon final : public CStateObjectWeapon
{
public:
	typedef struct player_weapon_desc : CStateObjectWeapon::STATE_OBJECT_WEAPON_DESC
	{
		_uint iNumModels;
		vector<const _float4x4*>* pSocketMatrices;

	}	PLAYER_WEAPON_DESC;

private:
	CPlayerWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerWeapon(const CPlayerWeapon& Prototype);
	virtual ~CPlayerWeapon() = default;

public:
	virtual HRESULT					InitializePrototype() override;
	virtual HRESULT					Initialize(void* pArg) override;
	virtual void					PriorityUpdate(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					LateUpdate(_float fTimeDelta) override;
	virtual HRESULT					Render() override;

public:
	void							ActivateGunMuzzleEffect(_uint iGunFireCount);
	void							Fire();

private:
	_vector							CalWorldEndPos();
	_vector							CalWorldEndPos(_float fSX, _float fSY);
	void							ActiveEffectAndParticle(_uint iModelIdx);
	void							CreateBullet(_uint iModelIdx);
	void							SpawnBullet(CProjectile::PROJECTILE_DESC Desc);

private:
	PLAYER_WEAPON_PROJECTILE_TYPE	m_eProjectileType = PLAYER_WEAPON_PROJECTILE_TYPE::NONE;
	vector<const _float4x4*>		m_pSocketMatrices;

private:
	CPlayer*						m_pPlayer = nullptr;
	CCollider*						m_pColliderCom = nullptr;

private:
	_bool							m_bActivate = false;
	CParticlePlayerGunFire*			m_pParticlePlayerGunFire = nullptr;

private:
	vector<HIT_RESULT>				m_BulletEffectInfos;
	vector<HIT_RESULT>				m_BulletTrailEffectInfos;
	vector<CProjectile*>			m_Projectiles;

private:
	HRESULT							ReadyComponents();
	HRESULT							BindShaderResources();

public:
	static CPlayerWeapon*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg);
	virtual void					Free() override;


};

END