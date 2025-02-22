#pragma once

#include "Client_Defines.h"
#include "PartObject.h"



BEGIN(Engine)

class CShader;
class CModel;

END

BEGIN(Client)

class CTestPlayerWeapon final : public CPartObject
{
public:
	typedef struct weapon_desc : CPartObject::PARTOBJECT_DESC
	{
		_uint* pState;
		const _float4x4* pSocketMatrix;
	}	TESTWEAPON_DESC;

private:
	CTestPlayerWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTestPlayerWeapon(const CTestPlayerWeapon& Prototype);
	virtual ~CTestPlayerWeapon() = default;

public:
	virtual HRESULT					InitializePrototype() override;
	virtual HRESULT					Initialize(void* pArg) override;
	virtual void					PriorityUpdate(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					LateUpdate(_float fTimeDelta) override;
	virtual HRESULT					Render() override;

private:
	CShader*						m_pShaderCom = { nullptr };
	CModel*							m_pModelCom = { nullptr };

private:
	_uint*							m_pState = { nullptr };
	const _float4x4*				m_pSocketMatrix = { nullptr };

private:
	HRESULT							ReadyComponents();
	HRESULT							BindShaderResources();

public:
	static CTestPlayerWeapon*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg);
	virtual void					Free() override;


};

END