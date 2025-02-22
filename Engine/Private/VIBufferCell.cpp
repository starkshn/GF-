#include "VIBufferCell.h"

CVIBufferCell::CVIBufferCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : 
    CVIBuffer(pDevice, pContext)
{
}

CVIBufferCell::CVIBufferCell(const CVIBufferCell& Prototype)
    :
    CVIBuffer(Prototype)
{
}

HRESULT CVIBufferCell::InitializePrototype(const _float3* pPoints)
{
    m_iNumVertexBuffers = 1;
    m_iVertexStride = sizeof(VTX_POS);
    m_iNumVertices = 3;                 // 정점은 3개

    m_iIndexStride = sizeof(_ushort);
    m_iNumIndices = 4; //  0 1 2 0 순으로 LINESTRIP으로 그릴 것이다.
    m_eIndexFormat = DXGI_FORMAT_R16_UINT;
    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;


#pragma region VERTEX_BUFFER	

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTX_POS* pVertices = new VTX_POS[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTX_POS) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
		pVertices[i].vPosition = pPoints[i];

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
	m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 0;

	ZeroMemory(&m_SubResourceDesc, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceDesc.pSysMem = pIndices;

	if (FAILED(__super::CreateBuffer(&m_pIB)))
		return E_FAIL;

	SafeDeleteArray(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CVIBufferCell::Initialize(void* pArg)
{
    return S_OK;
}

CVIBufferCell* CVIBufferCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints)
{
	CVIBufferCell* pInstance = new CVIBufferCell(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype(pPoints)))
	{
		MSG_BOX("Failed to Created : CVIBufferCell");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CComponent* CVIBufferCell::Clone(void* pArg)
{
	CVIBufferCell* pInstance = new CVIBufferCell(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBufferCell");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CVIBufferCell::Free()
{
	__super::Free();
}
