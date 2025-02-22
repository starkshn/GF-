#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Client)

class CEffectCircle1004 final : public CEffect
{
private:
	CEffectCircle1004(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffectCircle1004(const CEffectCircle1004& Prototype);
	virtual ~CEffectCircle1004() = default;

public:
	virtual HRESULT				InitializePrototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				PriorityUpdate(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				LateUpdate(_float fTimeDelta) override;
	virtual HRESULT				Render() override;


private:
	CTexture*					m_pNoiseTex = nullptr;
	CTexture*					m_pColorTex = nullptr;

private:
	virtual HRESULT				ReadyComponents() override;
	virtual HRESULT				BindShaderResources() override;

public:
	static CEffectCircle1004* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void				Free() override;
};

END