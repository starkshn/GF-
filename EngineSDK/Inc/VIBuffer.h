#pragma once
#include "Component.h"

// 정점 -> 폴리곤 -> 메쉬
// 여러 메쉬(사각형, 지형, 큐브)를 그려내기 위한 컴포넌트의 부모 클래슨
// 메쉬를 구성하기 위해서는 정점, 인덱스가 필요하고 이를 저장한다.

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	// 외부에서 CVIBuffer의 생성을 막기 위해서 protected로 설정함.
	CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer(const CVIBuffer& Prototype);
	virtual ~CVIBuffer() override = default;

public:
	virtual HRESULT				InitializePrototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual HRESULT				Render();

public:
	HRESULT						BindBufferDesc();

protected:
	ID3D11Buffer*				m_pVB = nullptr;			// 정점 버퍼
	ID3D11Buffer*				m_pIB = nullptr;			// 인덱스 버퍼

protected:
	D3D11_BUFFER_DESC			m_BufferDesc;				// 버퍼 디스크립션
	D3D11_SUBRESOURCE_DATA		m_SubResourceDesc;			// 서브리소스 디스크립션

protected:
	_uint						m_iVertexStride = 0;		// 정점 하나의 크기
	_uint						m_iNumVertices = 0;			// 정점 개수
	_uint						m_iNumVertexBuffers = 0;	// 정점 버퍼 개수

	_uint						m_iIndexStride = 0;			// 인덱스 버퍼 크기
	_uint						m_iNumIndices = 0;			// 인덱스 버퍼 개수 
	DXGI_FORMAT					m_eIndexFormat = {};		// 인덱스 버퍼 포맷
	D3D11_PRIMITIVE_TOPOLOGY	m_ePrimitiveTopology = {};	// 인덱스 버퍼 그릴 포맷?

	// 쿼드 트리 수업하면서 추가함 241211
protected:
	_float3*					m_pVertexPos = nullptr;;

protected:
	// 결과물로 ppOut에 받아주도록 하겠다.
	HRESULT						CreateBuffer(ID3D11Buffer** ppOut);

public:
	virtual CComponent*			Clone(void* pArg) abstract;
	virtual void				Free() override;
};

END