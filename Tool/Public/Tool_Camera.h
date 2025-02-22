#pragma once
#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)

class CShader;

END

BEGIN(Tool)

class CTool_Camera final : public CGameObject
{
private:
	CTool_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTool_Camera(const CTool_Camera& Prototype);
	virtual ~CTool_Camera() override = default;

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

public:
	_vector					Get_Pos() { return m_pTransformCom->GetTransformState(CTransform::STATE_POS); }
	
private:
	CShader*				m_pShaderCom = nullptr;
	CShader*				m_pShaderTempWaterCom = nullptr;
	_float4x4				m_ViewMatrix{}, m_ProjMatrix{};

public:
	static CTool_Camera*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END