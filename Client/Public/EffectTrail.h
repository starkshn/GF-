#pragma once
#include "Client_Defines.h"
#include "Effect.h"
#include "VIBufferTrail.h"

BEGIN(Engine)

class CVIBufferTrail;

END

BEGIN(Client)

class CEffectTrail : public CEffect
{
protected:
	CEffectTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffectTrail(const CEffectTrail& Prototype);
	virtual ~CEffectTrail() = default;

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

protected:
	CVIBufferTrail*				m_pVIBufferTrailCom = nullptr;

public:
	static CEffectTrail*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
	
};

END