#pragma once
#include "Client_Defines.h"
#include "PartObject.h"
#include "Model.h"

BEGIN(Engine)

class CShader;

END

BEGIN(Client)

class CTestPlayerBody final: public CPartObject
{
public:
	typedef struct testplayerbody_decs : public PARTOBJECT_DESC
	{
		_uint*		pState;

	} TESTPLAYERBODY_DESC;

private:
	CTestPlayerBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTestPlayerBody(const CTestPlayerBody& Prototype);
	virtual ~CTestPlayerBody() = default;

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
	CShader*					m_pShaderCom = { nullptr };
	CModel*						m_pModelCom = { nullptr };

private:
	_uint*						m_pState = { nullptr };

private:
	HRESULT						ReadyComponents();
	HRESULT						BindShaderResources();

public:
	static CTestPlayerBody*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END