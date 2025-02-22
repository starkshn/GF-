#pragma once

#include "PartObject.h"
#include "Model.h"
BEGIN(Engine)

class CShader;
class CCollider;

END

BEGIN(Tool)

class CTestModelBody final : public CPartObject
{
public:
	typedef struct testmodelbody_desc : public PARTOBJECT_DESC
	{

		_uint* pState;

	} TESTMODELBODY_DESC;

private:
	CTestModelBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTestModelBody(const CTestModelBody& Prototype);
	virtual ~CTestModelBody() = default;



public:
	virtual HRESULT				InitializePrototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				PriorityUpdate(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				LateUpdate(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

public:
	const _float4x4*			GetBoneCombiedMatrix(const _char* pBoneName) const { return m_pModelCom->GetBoneCombinedMatrix(pBoneName); }

private:
	CShader*					m_pShaderCom = nullptr;
	CModel*						m_pModelCom = nullptr ;
	CCollider*					m_pColliderCom = nullptr;

private:
	_uint*						m_pState = { nullptr };

private:
	HRESULT						ReadyComponents();
	HRESULT						BindShaderResources();

public:
	static CTestModelBody*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END