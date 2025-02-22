#pragma once

#include "PartObject.h"
#include "Model.h"

BEGIN(Engine)

class CShader;
class CModel;
class CCollider;

END

BEGIN(Tool)

class CTestModelWeapon final : public CPartObject
{
public:
	typedef struct testmodelweapon_desc : CPartObject::PARTOBJECT_DESC
	{
		_uint* pState;
		const _float4x4* pSocketMatrix;
	}	TESTMODELWEAPON_DESC;

private:
	CTestModelWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTestModelWeapon(const CTestModelWeapon& Prototype);
	virtual ~CTestModelWeapon() = default;
	
public:
	virtual HRESULT					InitializePrototype() override;
	virtual HRESULT					Initialize(void* pArg) override;
	virtual void					PriorityUpdate(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					LateUpdate(_float fTimeDelta) override;
	virtual HRESULT					Render() override;

private:
	CShader*						m_pShaderCom = nullptr;
	CModel*							m_pModelCom = nullptr;
	CCollider*						m_pColliderCom = nullptr;

private:
	_uint*							m_pState = { nullptr };
	const _float4x4*				m_pSocketMatrix = { nullptr };

private:
	HRESULT							ReadyComponents();
	HRESULT							BindShaderResources();

public:
	static CTestModelWeapon*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg);
	virtual void					Free() override;
};

END