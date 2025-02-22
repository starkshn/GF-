#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Collider.h"
#include "MonsterBase.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CModel;

END

BEGIN(Client)

class CWallAABB;

class CProjectile abstract : public CGameObject
{
public:

public:
	typedef struct projectile_desc : public GAMEOBJECT_DESC
	{
		PROJECTILE_TYPE ePJType;
		_float3 vDir;
		
	} PROJECTILE_DESC;


protected:
	CProjectile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProjectile(const CProjectile& Prototype);
	virtual ~CProjectile() override = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual _bool			MoveProjectile(_float fTimeDelta);

protected:
	void					CheckColWall();
	void					CheckColMonster();
	void					CheckColPlayer();

public:
	void					SetDir(_fvector vDir) { XMStoreFloat3(&m_vDir, vDir); }

public:
	virtual void			Fire() { m_bFire = true; }
	_bool					IsFire() { return m_bFire; }

protected:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;

	_wstring				m_strMonsterLayers[7] = { TEXT("Layer_Monster01"), TEXT("Layer_Monster02") , TEXT("Layer_Monster03") ,TEXT("Layer_Monster04") , TEXT("Layer_Monster05") , TEXT("Layer_Monster06") ,TEXT("Layer_Boss") };

protected:
	_float3					m_vDir = { 0.f, 0.f, 0.f };
	_bool					m_bFire = false;

	PROJECTILE_TYPE					m_eType;

private:
	virtual HRESULT			ReadyComponents() { return S_OK; }
	virtual HRESULT			BindShaderResources() { return S_OK; }

public:
	virtual	void			Free() override;
};

END