#pragma once
#include "Client_Defines.h"
#include "PartObject.h"
#include "Effect.h"
#include "EffectTrail.h"

BEGIN(Engine)

class CShader;
class CModel;
class CCollider;

END

BEGIN(Client)

class CStateObjectContainer;

class CStateObjectWeapon : public CPartObject
{
public:
	typedef struct state_object_weapon_desc : public PARTOBJECT_DESC
	{
		CStateObjectContainer*	pOwner;
		_uint*					pState;
		const _float4x4*		pSocketMatrix;

	} STATE_OBJECT_WEAPON_DESC;

protected:
	CStateObjectWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStateObjectWeapon(const CStateObjectWeapon& Prototype);
	virtual ~CStateObjectWeapon() = default;

public:
	virtual HRESULT					InitializePrototype() override;
	virtual HRESULT					Initialize(void* pArg) override;

public:
	CStateObjectContainer*			GetOwner() { return m_pOwner; }
	_uint							GetOwnerState() { return *m_pState; }

private:
	virtual HRESULT					ReadyComponents() { return S_OK; }
	virtual HRESULT					BindShaderResources() { return S_OK; }

public:
	void							SetEffectObj(CEffect* pEffect) 
	{ 
		if (pEffect)
			m_pEffect = pEffect; 
	}



protected:
	CShader*						m_pShaderCom = nullptr;
	CModel*							m_pModelCom = nullptr;
	CCollider*						m_pColliderCom = nullptr;

protected:
	_uint*							m_pState = nullptr;
	const _float4x4*				m_pSocketMatrix = nullptr;
	CStateObjectContainer*			m_pOwner = nullptr;

	CEffect*						m_pEffect = nullptr;



public:
	virtual void					Free() override;
};

END