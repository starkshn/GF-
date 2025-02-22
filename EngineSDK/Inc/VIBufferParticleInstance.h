#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBufferParticleInstance abstract : public CVIBuffer
{
public:
	typedef struct particle_desc
	{

		_uint		iNumInstances = 0;
		_float3		vCenter;
		_float3		vRange;
		_float3		vPivot;
		_float2		vSpeed;
		_float2		vLifeTime;
		_float2		vSize;
		_bool		bIsLoop;

	} PARTICLE_DESC;

protected:
	CVIBufferParticleInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBufferParticleInstance(const CVIBufferParticleInstance& Prototype);
	virtual ~CVIBufferParticleInstance() = default;

public:
	virtual HRESULT			InitializePrototype();
	virtual HRESULT			Initialize(void* pArg);
	virtual HRESULT			Render();
	virtual HRESULT			BindBufferDesc();

public:
	void					DropParticles(_float fTimeDelta);
	void					SpreadParticles(_float fTimeDelta);
	void					ReverseSpreadParticles(_float fTimeDelta);

public:
	void					ResetParticles();
	_bool					IsActivate() { return m_bActivate; }
	void					Activate() { m_bActivate = true; }
	void					DeActivate() { m_bActivate = false; }
	_bool					IsLoop() { return m_bIsLoopParticle; }

public:
	void					SetPivot(_fvector vPivotDir) { XMStoreFloat3(&m_vPivot, vPivotDir); }
	
protected:
	ID3D11Buffer*			m_pVBInstance = nullptr;
	D3D11_BUFFER_DESC		m_InstanceBufferDesc = {};
	D3D11_SUBRESOURCE_DATA	m_InstanceInitialDesc = {};
	_uint					m_iNumInstances = 0;
	_uint					m_iNumIndexPerInstance = 0;
	_uint					m_iInstanceStride = 0;
	VTX_INSTANCE*			m_pInstanceVertices = nullptr;
	_float3					m_vPivot = _float3{ 0.f, 0.f, 0.f };
	_bool					m_bIsLoopParticle = false;
	_float*					m_pSpeeds = nullptr;


	_float					m_fMaxLifeTime = 0.f;
	_bool					m_bActivate = false;

public:
	virtual CComponent*		Clone(void* pArg) abstract;
	virtual void			Free() override;
};

END