#pragma once
#include "Client_Defines.h"
#include "Particle.h"


BEGIN(Engine)

class CVIBufferParticleRect;
class CTexture;
class CShader;

END


BEGIN(Client)
class CParticleMonsterSpawn : public CParticle
{
private:
	CParticleMonsterSpawn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticleMonsterSpawn(const CParticleMonsterSpawn& Prototype);
	virtual ~CParticleMonsterSpawn() = default;

public:
	virtual HRESULT					InitializePrototype() override;
	virtual HRESULT					Initialize(void* pArg) override;
	virtual void					PriorityUpdate(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					LateUpdate(_float fTimeDelta) override;
	virtual HRESULT					Render() override;

private:
	virtual HRESULT					ReadyComponents() override;
	virtual HRESULT					BindShaderResources() override;

public:
	static CParticleMonsterSpawn*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg);
	virtual void					Free() override;
};

END