#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CShader;
class CModel;

END

BEGIN(Tool)

class CTool_Stage_01 final : public CGameObject
{
private:
	CTool_Stage_01(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTool_Stage_01(const CTool_Stage_01& Prototype);
	virtual ~CTool_Stage_01() override = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	_vector					FindHitPos(_fvector vOriginPos, _fvector vRayDir, _float vDist);

private:
	HRESULT					ReadyComponents();
	HRESULT					BindShaderResources();

private:
	CShader* m_pShaderCom = nullptr;
	CModel* m_pModelCom = nullptr;

public:
	static CTool_Stage_01*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END