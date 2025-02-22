#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Shader.h"
#include "VIBufferParticleRect.h"
#include "VIBufferParticlePoint.h"

BEGIN(Engine)

class CVIBufferParticleRect;
class CTexture;
class CShader;

END

BEGIN(Client)

class CParticle abstract : public CGameObject
{
protected:
	CParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle(const CParticle& Prototype);
	virtual ~CParticle() = default;

public:
	virtual HRESULT				InitializePrototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				PriorityUpdate(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				LateUpdate(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

public:
	_bool						IsActivate() { return m_bActivate; }
	void						Activate();
	void						DeActivate() { m_bActivate = false; }

public:
	void						ResetParticles();
	void						UpdateParticleWorldMatrix(_fmatrix WorldMatrix);
	void						UpdateParticleWorldPos(_fvector vWorldPos);

public:
	void						SetPivot(_fvector vPivot);

protected:
	CShader*					m_pShaderCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	CVIBufferParticlePoint*		m_pVIBufferCom = nullptr;

private:
	_bool						m_bActivate = false;
	_bool						m_bIsLoop = false;

private:
	virtual HRESULT				ReadyComponents()		abstract;
	virtual HRESULT				BindShaderResources()	abstract;

public:
	virtual void				Free() override;
};

END