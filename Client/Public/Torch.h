#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CShader;
class CModel;
class CNavigation;

END

BEGIN(Client)

class CTorch final : public CGameObject
{
private:
	CTorch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTorch(const CTorch& Prototype);
	virtual ~CTorch() override = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	HRESULT					ReadyComponents();
	HRESULT					BindShaderResources();

private:
	CShader* m_pShaderCom = nullptr;
	CModel* m_pModelCom = nullptr;

public:
	static CTorch*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END