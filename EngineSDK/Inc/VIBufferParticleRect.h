#pragma once
#include "VIBufferParticleInstance.h"

BEGIN(Engine)

class ENGINE_DLL CVIBufferParticleRect final : public CVIBufferParticleInstance
{
private:
	CVIBufferParticleRect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBufferParticleRect(const CVIBufferParticleRect& Prototype);
	virtual ~CVIBufferParticleRect() = default;

public:
	virtual HRESULT					InitializePrototype(const PARTICLE_DESC& ParticleDesc);
	virtual HRESULT					Initialize(void* pArg) override;

public:
	static CVIBufferParticleRect*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const PARTICLE_DESC& ParticleDesc);
	virtual CComponent*				Clone(void* pArg) override;
	virtual void					Free() override;
};

END