#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Client)

class CEffectBossRockAlertFinal final : public CEffect
{
private:
	CEffectBossRockAlertFinal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffectBossRockAlertFinal(const CEffectBossRockAlertFinal& Prototype);
	virtual ~CEffectBossRockAlertFinal() = default;

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
	_float	m_fMaxScale = 12.f;
	_float	m_fAccScale = 0.f;
	_float	m_fSP = 10.f;

public:
	static CEffectBossRockAlertFinal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void				Free() override;
};

END