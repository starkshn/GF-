#include "VIBufferSkyBox.h"

CVIBufferSkyBox::CVIBufferSkyBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CVIBuffer(pDevice, pContext)
{
}

CVIBufferSkyBox::CVIBufferSkyBox(const CVIBufferSkyBox& Prototype)
    :
    CVIBuffer(Prototype)
{
}

HRESULT CVIBufferSkyBox::InitializePrototype()
{
    m_iNumVertexBuffers = 1;

    m_iVertexStride = sizeof(VTXCUBE);
    m_iNumVertices = 8;

    m_iIndexStride = sizeof(_ushort);
    m_iNumIndices = 36;
    m_eIndexFormat = DXGI_FORMAT_R16_UINT;
    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER	
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 정점버퍼를 몇 바이트 할당할까요? */
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTXCUBE* pVertices = new VTXCUBE[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXCUBE) * m_iNumVertices);

	pVertices[0].vPosition = XMFLOAT3(-0.5f, 0.5f, -0.5f);
	pVertices[0].vTexcoord = pVertices[0].vPosition;

	pVertices[1].vPosition = XMFLOAT3(0.5f, 0.5f, -0.5f);
	pVertices[1].vTexcoord = pVertices[1].vPosition;

	pVertices[2].vPosition = XMFLOAT3(0.5f, -0.5f, -0.5f);
	pVertices[2].vTexcoord = pVertices[2].vPosition;

	pVertices[3].vPosition = XMFLOAT3(-0.5f, -0.5f, -0.5f);
	pVertices[3].vTexcoord = pVertices[3].vPosition;

	pVertices[4].vPosition = XMFLOAT3(-0.5f, 0.5f, 0.5f);
	pVertices[4].vTexcoord = pVertices[4].vPosition;

	pVertices[5].vPosition = XMFLOAT3(0.5f, 0.5f, 0.5f);
	pVertices[5].vTexcoord = pVertices[5].vPosition;

	pVertices[6].vPosition = XMFLOAT3(0.5f, -0.5f, 0.5f);
	pVertices[6].vTexcoord = pVertices[6].vPosition;

	pVertices[7].vPosition = XMFLOAT3(-0.5f, -0.5f, 0.5f);
	pVertices[7].vTexcoord = pVertices[7].vPosition;

	ZeroMemory(&m_SubResourceDesc, sizeof m_SubResourceDesc);
	m_SubResourceDesc.pSysMem = pVertices;


	if (FAILED(__super::CreateBuffer(&m_pVB)))
		return E_FAIL;

	SafeDeleteArray(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 정점버퍼를 몇 바이트 할당할까요? */
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	/* +x */
	pIndices[0] = 1; pIndices[1] = 5; pIndices[2] = 6;
	pIndices[3] = 1; pIndices[4] = 6; pIndices[5] = 2;

	/* -x */
	pIndices[6] = 4; pIndices[7] = 0; pIndices[8] = 3;
	pIndices[9] = 4; pIndices[10] = 3; pIndices[11] = 7;

	/* +y */
	pIndices[12] = 4; pIndices[13] = 5; pIndices[14] = 1;
	pIndices[15] = 4; pIndices[16] = 1; pIndices[17] = 0;

	/* -y */
	pIndices[18] = 3; pIndices[19] = 2; pIndices[20] = 6;
	pIndices[21] = 3; pIndices[22] = 6; pIndices[23] = 7;

	/* +z */
	pIndices[24] = 5; pIndices[25] = 4; pIndices[26] = 7;
	pIndices[27] = 5; pIndices[28] = 7; pIndices[29] = 6;

	/* -z */
	pIndices[30] = 0; pIndices[31] = 1; pIndices[32] = 2;
	pIndices[33] = 0; pIndices[34] = 2; pIndices[35] = 3;

	ZeroMemory(&m_SubResourceDesc, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceDesc.pSysMem = pIndices;

	if (FAILED(__super::CreateBuffer(&m_pIB)))
		return E_FAIL;

	SafeDeleteArray(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CVIBufferSkyBox::Initialize(void* pArg)
{
    return S_OK;
}

CVIBufferSkyBox* CVIBufferSkyBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CVIBufferSkyBox* pInstance = new CVIBufferSkyBox(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CVIBufferSkyBox");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CComponent* CVIBufferSkyBox::Clone(void* pArg)
{
    CVIBufferSkyBox* pInstance = new CVIBufferSkyBox(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CVIBufferSkyBox");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CVIBufferSkyBox::Free()
{
    __super::Free();
}
