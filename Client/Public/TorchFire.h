#pragma once
#include "Client_Defines.h"	
#include "Effect.h"

BEGIN(Client)

class CTorchFire final : public CEffect
{
public:
	typedef struct torch_fire_desc : CEffect::EFFECT_DESC
	{
		_float fNoiseMax;
		
	} TORCH_FIRE_DESC;


private:
	CTorchFire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTorchFire(const CTorchFire& Prototype);
	virtual ~CTorchFire() = default;

public:
	virtual HRESULT				InitializePrototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				PriorityUpdate(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				LateUpdate(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

private:
	virtual HRESULT				ReadyComponents() override;
	virtual HRESULT				BindShaderResources() override;


private:
	_float						m_fNoiseMax = 0.f;
	_float						m_fNoiseCur = 0.f;
	_bool						m_bReverse = false;

private:
	CTexture*					m_pTextureNoise = nullptr;

public:
	static CTorchFire*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END