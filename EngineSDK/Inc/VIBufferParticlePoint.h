#pragma once
#include "VIBufferParticleInstance.h"

BEGIN(Engine)

class ENGINE_DLL CVIBufferParticlePoint final : public CVIBufferParticleInstance
{
private:
	CVIBufferParticlePoint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBufferParticlePoint(const CVIBufferParticlePoint& Prototype);
	virtual ~CVIBufferParticlePoint() = default;

public:
	virtual HRESULT					InitializePrototype(const PARTICLE_DESC& ParticleDesc);
	virtual HRESULT					Initialize(void* pArg) override;

public:
	static CVIBufferParticlePoint*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const PARTICLE_DESC& ParticleDesc);
	virtual CComponent*				Clone(void* pArg) override;
	virtual void					Free() override;
};

END