#include "VIBuffer_Terrain.h"
#include "QuadTree.h"
#include "GameInstance.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: 
	CVIBuffer(pDevice, pContext)
{

}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& Prototype)
	:
	CVIBuffer(Prototype),
	m_iNumVerticesX(Prototype.m_iNumVerticesX),
	m_iNumVerticesZ(Prototype.m_iNumVerticesZ),
	m_vecVertices(Prototype.m_vecVertices),
	m_pQuadTree(Prototype.m_pQuadTree)
{
	SafeAddRef(m_pQuadTree);
}

HRESULT CVIBuffer_Terrain::InitializePrototype(const _tchar* pHeightMapFilePath)
{
	_ulong dwByte = 0;
	HANDLE	hFile = CreateFile(pHeightMapFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	
	BITMAPFILEHEADER	fileHeader = {};
	BITMAPINFOHEADER	infoHeader = {};
	_uint*				pPixels = nullptr;

	ReadFile(hFile, &fileHeader, sizeof fileHeader, &dwByte, nullptr);
	ReadFile(hFile, &infoHeader, sizeof infoHeader, &dwByte, nullptr);

	m_iNumVerticesX = infoHeader.biWidth;
	m_iNumVerticesZ = infoHeader.biHeight;

	// 동적할당
	pPixels = new _uint[m_iNumVerticesX * m_iNumVerticesZ];

	ReadFile(hFile, pPixels, sizeof(_uint) * m_iNumVerticesX * m_iNumVerticesZ, &dwByte, nullptr);
	
	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof(VTX_NORTEX);						// 한 정점당 크기
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	// 정점의 개수가 2바이트를 넘어가는 경우 삼중 연산자로 일일히 확인했어야 했는데 그냥 4바이트로 통일 시키겠다. m_iNumVertices >= 65534 ? sizeof(_uint) : sizeof(_ushort)
	m_iIndexStride = sizeof(_uint);
	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2/*삼각형 개수*/ * 3/*인덱스 개수*/;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	
	// vPosition
	// A R G B
	// 00000000 01001100 00000000 00000000
	// 여기서 B가 아닌 A R G중에 비트 연산을 하게 되면 그만 큼 비트 쉬프트 연산을 수행 해주어야 한다. 원하는 값은 0~255 사이의 값이기 때문에.

	// vTexcoord
	// 하나의 텍스쳐에 해당하는 필셀을 찾아서 각 정점의 UV에다가 매핑 시켜도 된다.
	// 다만 이렇게 할 경우 텍스쳐 자체의 화질이 떨어지는 경우 품질이 떨어져 보인다. VS 단계에서 UV에 특정 상수 값을 곱하여 완화 시켜줄 수 있다.

	VTX_NORTEX* pVertices = new VTX_NORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTX_NORTEX) * m_iNumVertices);


	m_pVertexPos = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPos, sizeof(_float3) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVerticesZ; i++)
	{
		for (_uint j = 0; j < m_iNumVerticesX; j++)
		{
			const static _float fInterpolVal = 10.f;
			_uint idx = i * m_iNumVerticesX + j;
			
			pVertices[idx].vPosition = m_pVertexPos[idx] = _float3(static_cast<_float>(j), (pPixels[idx] & 0x000000ff) / fInterpolVal /*높이값 보정을 위한 값*/, static_cast<_float>(i));
			pVertices[idx].vNormal = _float3(0.f, 0.f, 0.f); // <- 우선 0 0 0으로 채운다.
			pVertices[idx].vTexcoord = _float2(static_cast<_float>(j) / (m_iNumVerticesX - 1.f), static_cast<_float>(i) / (m_iNumVerticesZ - 1.f));
			// pVertices[idx].vTexcoord = _float2(j, i); // <- 이렇게 해도 AddressU, V가 wrap으로 되어 있다면 아무 상관없다. UV가 1이 넘어가면 다시 0부터 반복되기 때문이다.
		}
	}

#pragma endregion
	
	
#pragma region INDEX_BUFFER

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = {};


	for (_uint i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;
			_uint		iIndices[] = { iIndex + m_iNumVerticesX, iIndex + m_iNumVerticesX + 1, iIndex + 1, iIndex, };

			_vector		vSrc, vDst, vNormal;

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			// 0 1 2 정점에서 (1 - 0), (2 - 1)의 벡터를 구하고 이를 외적한 뒤 정규화한 벡터를 각 정점에 누적한다.
			{
				vSrc = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
				vDst = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
				vNormal = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

				XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
				XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal);
				XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);
			}
			
			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];

			// 0 2 3 정점에서 (2 - 0), (3 - 2)의 벡터를 구하고 이를 외적한 뒤 정규화한 벡터를 각 정점에 누적한다.
			{
				vSrc = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
				vDst = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
				vNormal = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

				XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
				XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);
				XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal);
			}
		}
	}

	// 정규화
	// for (_uint i = 0; i < m_iNumVerticesZ * m_iNumVerticesX; i++)
	// {
	// 	_vector vNorVec = XMLoadFloat3(&pVertices[i].vNormal);
	// 	vNorVec = XMVector3Normalize(vNorVec);
	// 	XMStoreFloat3(&pVertices[i].vNormal, vNorVec);
	// }

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));

		m_vecVertices.push_back(pVertices[i]);
	}
	
#pragma endregion


	// Create Vertex Buffer
	{
		ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

		m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		m_BufferDesc.StructureByteStride = m_iVertexStride;
		m_BufferDesc.CPUAccessFlags = 0;
		m_BufferDesc.MiscFlags = 0;

		ZeroMemory(&m_SubResourceDesc, sizeof m_SubResourceDesc);
		m_SubResourceDesc.pSysMem = pVertices;

		if (FAILED(__super::CreateBuffer(&m_pVB)))
			return E_FAIL;
	}

	// Create Index Buffer
	{
		ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

		/* 정점버퍼를 몇 바이트 할당할까요? */
		m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
		m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;
		m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		m_BufferDesc.MiscFlags = 0;

		ZeroMemory(&m_SubResourceDesc, sizeof(D3D11_SUBRESOURCE_DATA));
		m_SubResourceDesc.pSysMem = pIndices;

		if (FAILED(__super::CreateBuffer(&m_pIB)))
			return E_FAIL;
	}

	SafeDeleteArray(pVertices);
	SafeDeleteArray(pIndices);
	SafeDeleteArray(pPixels);


	m_pQuadTree = CQuadTree::Create(m_iNumVerticesX * m_iNumVerticesZ - m_iNumVerticesX, m_iNumVerticesX * m_iNumVerticesZ - 1, m_iNumVerticesX - 1, 0);
	if (nullptr == m_pQuadTree)
		return E_FAIL;


	m_pQuadTree->MakeNeighbors();

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
	return S_OK;
}

void CVIBuffer_Terrain::Culling(_fmatrix WorldMatrix)
{
	/* 절두체는 지형 버퍼의 로컬스페이스로 변환되었다. */
	m_pGameInst->TransformFrustumToLocalSpace(WorldMatrix);

	D3D11_MAPPED_SUBRESOURCE		SubResource = {};

	m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	_uint* pIndices = static_cast<_uint*>(SubResource.pData);

	_uint		iNumIndices = {};

	m_pQuadTree->Culling(m_pVertexPos, pIndices, &iNumIndices, XMMatrixInverse(nullptr, WorldMatrix));

	/*for (size_t i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex,
			};

			_bool		isIn[4] = {
				m_pGameInstance->isIn_Frustum_InLocalSpace(XMLoadFloat3(&m_pVertexPositions[iIndices[0]])),
				m_pGameInstance->isIn_Frustum_InLocalSpace(XMLoadFloat3(&m_pVertexPositions[iIndices[1]])),
				m_pGameInstance->isIn_Frustum_InLocalSpace(XMLoadFloat3(&m_pVertexPositions[iIndices[2]])),
				m_pGameInstance->isIn_Frustum_InLocalSpace(XMLoadFloat3(&m_pVertexPositions[iIndices[3]]))
			};

			if (true == isIn[0] &&
				true == isIn[1] &&
				true == isIn[2])
			{
				pIndices[iNumIndices++] = iIndices[0];
				pIndices[iNumIndices++] = iIndices[1];
				pIndices[iNumIndices++] = iIndices[2];
			}

			if (true == isIn[0] &&
				true == isIn[2] &&
				true == isIn[3])
			{
				pIndices[iNumIndices++] = iIndices[0];
				pIndices[iNumIndices++] = iIndices[2];
				pIndices[iNumIndices++] = iIndices[3];
			}
		}
	}
*/
	m_iNumIndices = iNumIndices;

	m_pContext->Unmap(m_pIB, 0);
}

const vector<VTX_NORTEX>& CVIBuffer_Terrain::GetVerticesVector()
{
	return m_vecVertices;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFilePath)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype(pHeightMapFilePath)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Terrain");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Terrain::Clone(void* pArg)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Terrain");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();

	SafeRelease(m_pQuadTree);
}
