#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"


BEGIN(Engine)

class CVIBufferParticleRect;
class CTexture;
class CShader;

END


BEGIN(Tool)

class CParticleExplosion final : public CGameObject
{
private:
	CParticleExplosion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticleExplosion(const CParticleExplosion& Prototype);
	virtual ~CParticleExplosion() = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CVIBufferParticleRect*	m_pVIBufferCom = nullptr;

private:
	HRESULT					ReadyComponents();
	HRESULT					BindShaderResources();

public:
	static CParticleExplosion*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END