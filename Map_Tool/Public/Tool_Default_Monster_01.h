#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CShader;
class CModel;
class CColBoundingBox;

END

BEGIN(Tool)

class CTool_Default_Monster_01 final : public CGameObject
{
private:
	CTool_Default_Monster_01(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTool_Default_Monster_01(const CTool_Default_Monster_01& Prototype);
	virtual ~CTool_Default_Monster_01() override = default;

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
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CColBoundingBox*		m_pBoundingBox = nullptr;

public:
	static CTool_Default_Monster_01* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void			Free() override;

};

END