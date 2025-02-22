#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CShader;
class CModel;

END

BEGIN(Tool)

class CTool_Boss_Stage final : public CGameObject
{
private:
	CTool_Boss_Stage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTool_Boss_Stage(const CTool_Boss_Stage& Prototype);
	virtual ~CTool_Boss_Stage() override = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	HRESULT					Ready_Components();
	HRESULT					Bind_ShaderResources();

private:
	CShader* m_pShaderCom = nullptr;
	CModel* m_pModelCom = nullptr;

public:
	static CTool_Boss_Stage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END