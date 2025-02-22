#include "VIBuffer_TempWater.h"

CVIBuffer_TempWater::CVIBuffer_TempWater(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_TempWater::CVIBuffer_TempWater(const CVIBuffer_TempWater& Prototype)
    :
    CVIBuffer(Prototype)
{
    m_iCntX = Prototype.m_iCntX;
    m_iCntZ = Prototype.m_iCntZ;
	m_iNumVertices = Prototype.m_iNumVertices;
}

HRESULT CVIBuffer_TempWater::InitializePrototype()
{
	// 버텍스 버퍼 개수
	m_iNumVertexBuffers = 1;

	const _uint iWidth = 30;
	const _uint iHeight = 30;

	m_iCntX = iWidth;
	m_iCntZ = iHeight;

	_uint triCnt = iWidth * iHeight * 2;

	m_iVertexStride = sizeof(VTX_NORTEX);
	m_iNumVertices = (iWidth + 1) * (iHeight + 1);


	m_iIndexStride = sizeof(_uint);
	m_iNumIndices = triCnt * 3;									// 인덱스 개수
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;					// 용도 : 동적(아임 구이로 수정해야함)
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;		// CPU의 접근 방식, 쓰기 가능.
	m_BufferDesc.MiscFlags = 0;									// 아직 안 씀

	VTX_NORTEX* pVertices = new VTX_NORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTX_NORTEX) * m_iNumVertices);

	_uint idx = 0;
	for (_uint i = 0; i < iWidth + 1; ++i)
	{
		for (_uint j = 0; j < iHeight + 1; ++j)
		{
			idx = i * (iHeight + 1) + j;

			pVertices[idx].vPosition = { static_cast<_float>(j), 0.f, static_cast<_float>(i) };
			pVertices[idx].vNormal = _float3(0.f, 1.f, 0.f);
			pVertices[idx].vTexcoord = { static_cast<_float>(j), static_cast<_float>(i) };
		}
	}

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
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	idx = 0;
	for (_int i = 0; i < iWidth; ++i)
	{
		for (_int j = 0; j < iHeight; ++j)
		{
			_uint v0 = i * (iHeight + 1) + j + (iHeight + 1);
			_uint v1 = v0 + 1;
			_uint v2 = i * (iHeight + 1) + j + 1;
			_uint v3 = i * (iHeight + 1) + j;

			pIndices[idx++] = v0;
			pIndices[idx++] = v1;
			pIndices[idx++] = v2;

			pIndices[idx++] = v0;
			pIndices[idx++] = v2;
			pIndices[idx++] = v3;
		}
	}

	ZeroMemory(&m_SubResourceDesc, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceDesc.pSysMem = pIndices;

	if (FAILED(__super::CreateBuffer(&m_pIB)))
		return E_FAIL;

	SafeDeleteArray(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_TempWater::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_TempWater::ResetVtxCnt(_int iCntX, _int iCntZ)
{
	m_pVB->Release();
	m_pIB->Release();

	// D3D11_MAPPED_SUBRESOURCE mappedData;
	// HRESULT hr = m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	// for (UINT i = 0; i < mWaves.VertexCount(); ++i)
	// {
	// 	v[i].Pos = mWaves[i];
	// 	v[i].Color = XMFLOAT4(O.Of, O.Of, O.Of, l.Of);
	// }
	// m_pContext->Unmap(mWavesVB, 0);

	const _uint iWidth = iCntX;
	const _uint iHeight = iCntZ;

	m_iCntX = iWidth;
	m_iCntZ = iHeight;

	_uint triCnt = iWidth * iHeight * 2;

	m_iVertexStride = sizeof(VTX_NORTEX);
	m_iNumVertices = (iWidth + 1) * (iHeight + 1);

	m_iIndexStride = sizeof(_uint);
	m_iNumIndices = triCnt * 3;									// 인덱스 개수
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;					// 용도 : 동적(아임 구이로 수정해야함)
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;		// CPU의 접근 방식, 쓰기 가능.
	m_BufferDesc.MiscFlags = 0;									// 아직 안 씀

	VTX_NORTEX* pVertices = new VTX_NORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTX_NORTEX) * m_iNumVertices);

	_uint idx = 0;
	for (_uint i = 0; i < iWidth + 1; ++i)
	{
		for (_uint j = 0; j < iHeight + 1; ++j)
		{
			idx = i * (iHeight + 1) + j;

			pVertices[idx].vPosition = { static_cast<_float>(j), 0.f, static_cast<_float>(i) };
			pVertices[idx].vTexcoord = { static_cast<_float>(j), static_cast<_float>(i) };
			pVertices[idx].vNormal = _float3(0.f, 1.f, 0.f);
		}
	}

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
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	idx = 0;
	for (_uint i = 0; i < iWidth; ++i)
	{
		for (_uint j = 0; j < iHeight; ++j)
		{
			_uint v0 = i * (iHeight + 1) + j + (iHeight + 1);
			_uint v1 = v0 + 1;
			_uint v2 = i * (iHeight + 1) + j + 1;
			_uint v3 = i * (iHeight + 1) + j;

			pIndices[idx++] = v0;
			pIndices[idx++] = v1;
			pIndices[idx++] = v2;

			pIndices[idx++] = v0;
			pIndices[idx++] = v2;
			pIndices[idx++] = v3;
		}
	}


	ZeroMemory(&m_SubResourceDesc, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceDesc.pSysMem = pIndices;

	if (FAILED(__super::CreateBuffer(&m_pIB)))
		return E_FAIL;

	SafeDeleteArray(pIndices);

#pragma endregion


	return S_OK;
}

void CVIBuffer_TempWater::UpdateYPos(_float fDT, _float fSP)
{
	// 1. 정점 버퍼를 맵핑 (CPU에서 접근 가능하게 만듬)
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);
	if (FAILED(hr))
	{
		// 에러 처리
		return;
	}

	_uint idx = 0;

	// 2. 맵핑된 버퍼의 데이터 포인터 얻기
	VTX_NORTEX* vertices = reinterpret_cast<VTX_NORTEX*>(mappedResource.pData);

	for (_uint i = 0; i < m_iCntZ; ++i)
	{
		for (_uint j = 0; j < m_iCntX; ++j)
		{
			if (i % 3 == 0)
			{
				if (j % 2 == 1)
				{
					idx = i * (m_iCntX + 1) + j;

					if (m_bUp)
					{
						vertices[idx].vPosition.y += fDT * fSP;

						if (vertices[idx].vPosition.y >= 0.6f)
							m_bUp = false;
					}
					else
					{
						vertices[idx].vPosition.y -= fDT * fSP;
						if (vertices[idx].vPosition.y <= 0.f)
							m_bUp = true;
					}
				}
			}
			else if (i % 5 == 0)
			{
				if (j % 2 == 0)
				{
					idx = i * (m_iCntX + 1) + j;

					if (m_bUp)
					{
						vertices[idx].vPosition.y += fDT * fSP;

						if (vertices[idx].vPosition.y >= 0.8f)
							m_bUp = false;
					}
					else
					{
						vertices[idx].vPosition.y -= fDT * fSP;
						if (vertices[idx].vPosition.y <= 0.f)
							m_bUp = true;
					}
				}
			}
		}
	}

	for (_uint i = 0; i < m_iCntZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iCntX - 1; ++j)
		{
			_uint		iIndex = i * m_iCntX + j;
			_uint		iIndices[] = { iIndex + m_iCntX, iIndex + m_iCntX + 1, iIndex + 1, iIndex, };


			_vector		vSrc, vDst, vNormal;

			// 0 1 2 정점에서 (1 - 0), (2 - 1)의 벡터를 구하고 이를 외적한 뒤 정규화한 벡터를 각 정점에 누적한다.
			{
				vSrc = XMLoadFloat3(&vertices[iIndices[1]].vPosition) - XMLoadFloat3(&vertices[iIndices[0]].vPosition);
				vDst = XMLoadFloat3(&vertices[iIndices[2]].vPosition) - XMLoadFloat3(&vertices[iIndices[1]].vPosition);
				vNormal = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

				XMStoreFloat3(&vertices[iIndices[0]].vNormal, XMLoadFloat3(&vertices[iIndices[0]].vNormal) + vNormal);
				XMStoreFloat3(&vertices[iIndices[1]].vNormal, XMLoadFloat3(&vertices[iIndices[1]].vNormal) + vNormal);
				XMStoreFloat3(&vertices[iIndices[2]].vNormal, XMLoadFloat3(&vertices[iIndices[2]].vNormal) + vNormal);
			}

			// 0 2 3 정점에서 (2 - 0), (3 - 2)의 벡터를 구하고 이를 외적한 뒤 정규화한 벡터를 각 정점에 누적한다.
			{
				vSrc = XMLoadFloat3(&vertices[iIndices[2]].vPosition) - XMLoadFloat3(&vertices[iIndices[0]].vPosition);
				vDst = XMLoadFloat3(&vertices[iIndices[3]].vPosition) - XMLoadFloat3(&vertices[iIndices[2]].vPosition);
				vNormal = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

				XMStoreFloat3(&vertices[iIndices[0]].vNormal, XMLoadFloat3(&vertices[iIndices[0]].vNormal) + vNormal);
				XMStoreFloat3(&vertices[iIndices[2]].vNormal, XMLoadFloat3(&vertices[iIndices[2]].vNormal) + vNormal);
				XMStoreFloat3(&vertices[iIndices[3]].vNormal, XMLoadFloat3(&vertices[iIndices[3]].vNormal) + vNormal);
			}
		}
	}

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		XMStoreFloat3(&vertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&vertices[i].vNormal)));
	}

	m_pContext->Unmap(m_pVB, 0);
}

CVIBuffer_TempWater* CVIBuffer_TempWater::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_TempWater* pInstance = new CVIBuffer_TempWater(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_TempWater");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_TempWater::Clone(void* pArg)
{
	CVIBuffer_TempWater* pInstance = new CVIBuffer_TempWater(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_TempWater");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CVIBuffer_TempWater::Free()
{
	__super::Free();

	
}
