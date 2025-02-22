#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Client)

class CEffectMuzzle final : public CEffect
{
public:
	typedef struct muzzle_effect_desc : public EFFECT_DESC
	{

		MUZZLE_EFFECT_TYPE eMuzzleType;


	} MUZZLE_EFFECT_DESC;
private:
	CEffectMuzzle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffectMuzzle(const CEffectMuzzle& Prototype);
	virtual ~CEffectMuzzle() = default;

public:
	virtual HRESULT				InitializePrototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				PriorityUpdate(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				LateUpdate(_float fTimeDelta) override;
	virtual HRESULT				Render() override;


private:
	MUZZLE_EFFECT_TYPE			m_eMuzzleType = MUZZLE_EFFECT_TYPE::TYPE_END;


private:
	virtual HRESULT				ReadyComponents() override;
	virtual HRESULT				BindShaderResources() override;

public:
	static CEffectMuzzle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void				Free() override;
};

END