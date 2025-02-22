#pragma once
#include "VIBuffer.h"
#include <deque>


BEGIN(Engine)

class ENGINE_DLL CVIBufferTrail : public CVIBuffer
{
public:
	typedef struct
	{
		_float fTrailLength;
		_uint iNumMaxVertices;
		_float fOffset;
		_float fLifeTime;

		_float3 vLocalHigh;
		_float3 vLocalLow;

	}	TRAIL_DESC;


private:
	CVIBufferTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBufferTrail(const CVIBufferTrail& Prototype);
	virtual ~CVIBufferTrail() override = default;

public:
	virtual HRESULT						InitializePrototype() override;
	virtual HRESULT						Initialize(void* pArg) override;

public:
	void								AddVertices(_fmatrix WorldMatrix, _float fTimeDelta);
	void								ResetTrailBuffers();

private:

	_float3								m_vLocalTrailLow = { 0.0f, -0.5f,0.f };
	_float3								m_vLocalTrailHigh = { 0.f, 0.5f, 0.f };

	std::deque<VTX_POSTEX_TIME>			m_TrailVertices;
	_uint								m_iUsingCnt = 0; // 덱에서 사용하고 있는 실제 사이즈
	_uint								m_iNumMaxVertices = 0;
	_uint								m_fTrailLength = 0;

	_float								m_fOffset = 1.f;
	_float								m_fLifeTime = 0.f;

public:
	static			CVIBufferTrail*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual			CComponent*			Clone(void* pArg) override;
	virtual			void				Free() override;
};

END