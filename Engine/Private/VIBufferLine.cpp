#include "VIBufferLine.h"

CVIBufferLine::CVIBufferLine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CVIBuffer(pDevice, pContext)
{
}

CVIBufferLine::CVIBufferLine(const CVIBufferLine& Prototype)
    :
    CVIBuffer(Prototype)
{
}

HRESULT CVIBufferLine::InitializePrototype()
{
	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof(VTX_POS);
	m_iNumVertices = 2;                 // 정점은 3개

	m_iIndexStride = sizeof(_ushort);
	m_iNumIndices = 2;						// 0 1 순
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;


#pragma region VERTEX_BUFFER	

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	VTX_POS* pVertices = new VTX_POS[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTX_POS) * m_iNumVertices);

	pVertices[0].vPosition.x = 0.f; // 첫 번째 점의 X 좌표
	pVertices[0].vPosition.y = 0.0f;  // 첫 번째 점의 Y 좌표
	pVertices[0].vPosition.z = 0.0f;  // 첫 번째 점의 Z 좌표

	pVertices[1].vPosition.x = 0.f;  // 두 번째 점의 X 좌표
	pVertices[1].vPosition.y = 0.0f;  // 두 번째 점의 Y 좌표
	pVertices[1].vPosition.z = 0.0f;  // 두 번째 점의 Z 좌표

	ZeroMemory(&m_SubResourceDesc, sizeof m_SubResourceDesc);
	m_SubResourceDesc.pSysMem = pVertices;


	if (FAILED(__super::CreateBuffer(&m_pVB)))
		return E_FAIL;

	SafeDeleteArray(pVertices);

#pragma endregion



#pragma region INDEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = sizeof(_ushort);
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	pIndices[0] = 0;
	pIndices[1] = 1;
	
	ZeroMemory(&m_SubResourceDesc, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceDesc.pSysMem = pIndices;

	if (FAILED(__super::CreateBuffer(&m_pIB)))
		return E_FAIL;

	SafeDeleteArray(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CVIBufferLine::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CVIBufferLine::UpdateLineVertices(const _fvector& vStart, const _fvector& vEnd)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		return E_FAIL;

	_float3 vStartPos, vEndPos;
	XMStoreFloat3(&vStartPos, vStart);
	XMStoreFloat3(&vEndPos, vEnd);

	// 새로운 정점 데이터 설정
	VTX_POS* pVertices = reinterpret_cast<VTX_POS*>(mappedResource.pData);
	pVertices[0].vPosition = vStartPos;
	pVertices[1].vPosition = vEndPos;

	m_pContext->Unmap(m_pVB, 0);

	return S_OK;
}

CVIBufferLine* CVIBufferLine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBufferLine* pInstance = new CVIBufferLine(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CVIBufferLine");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CComponent* CVIBufferLine::Clone(void* pArg)
{
	CVIBufferLine* pInstance = new CVIBufferLine(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBufferLine");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CVIBufferLine::Free()
{
	__super::Free();
}
