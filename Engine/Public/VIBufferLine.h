#pragma once
#include "VIBuffer.h"


BEGIN(Engine)

class CVIBufferLine final : public CVIBuffer
{
private:
	CVIBufferLine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBufferLine(const CVIBufferLine& Prototype);
	virtual ~CVIBufferLine() = default;

public:
	virtual HRESULT			InitializePrototype();
	virtual HRESULT			Initialize(void* pArg) override;

public:
	HRESULT					UpdateLineVertices(const _fvector& vStart, const _fvector& vEnd);

public:
	void					SetStartEnd(_fvector vStart, _fvector vEnd)
	{
		XMStoreFloat3(&m_vStart, vStart);
		XMStoreFloat3(&m_vEnd, vEnd);

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		// 새로운 정점 데이터 설정
		VTX_POS* pVertices = reinterpret_cast<VTX_POS*>(mappedResource.pData);
		pVertices[0].vPosition = m_vStart;
		pVertices[1].vPosition = m_vEnd;

		m_pContext->Unmap(m_pVB, 0);
	}

private:
	_float3					m_vStart;
	_float3					m_vEnd;

public:
	static CVIBufferLine*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
};

END