#include "VIBufferTrail.h"
#include "GlobalFuncs.h"

CVIBufferTrail::CVIBufferTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CVIBuffer(pDevice, pContext)
{
}

CVIBufferTrail::CVIBufferTrail(const CVIBufferTrail& Prototype)
	:
	CVIBuffer(Prototype)
{
}

HRESULT CVIBufferTrail::InitializePrototype()
{
	return S_OK;
}

HRESULT CVIBufferTrail::Initialize(void* pArg)
{
	TRAIL_DESC* pDesc = static_cast<TRAIL_DESC*>(pArg);
	m_fTrailLength = pDesc->fTrailLength;
	m_iNumMaxVertices = pDesc->iNumMaxVertices;
	m_fOffset = pDesc->fOffset;
	m_fLifeTime = pDesc->fLifeTime;
	
	if (0.f == pDesc->vLocalHigh.x && 0.f == pDesc->vLocalHigh.y && 0.f == pDesc->vLocalHigh.z)
		m_vLocalTrailHigh = m_vLocalTrailHigh;
	else
	{

	}

	if (0.f == pDesc->vLocalLow.x && 0.f == pDesc->vLocalLow.y && 0.f == pDesc->vLocalLow.z)
		m_vLocalTrailLow = m_vLocalTrailLow;
	else
	{

	}
	


	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof(VTX_POSTEX_TIME);
	m_iNumVertices = m_iNumMaxVertices;
	m_iIndexStride = sizeof(_ushort);
	m_iNumIndices = 3 * 2 * (m_iNumVertices / 2 - 1);
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 정점버퍼를 몇 바이트 할당할까요? */
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	VTX_POSTEX_TIME* pVertices = new VTX_POSTEX_TIME[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTX_POSTEX_TIME) * m_iNumVertices);
	ZeroMemory(&m_SubResourceDesc, sizeof m_SubResourceDesc);
	m_SubResourceDesc.pSysMem = pVertices;
	if (FAILED(__super::CreateBuffer(&(this->m_pVB))))
		return E_FAIL;

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

	_uint iNowVertexIndex = 0;
	_uint iNumTriCnt = m_iNumIndices / 3;
	for (_uint i = 0; i < iNumTriCnt; i += 2)
	{
		pIndices[iNowVertexIndex++] = i;
		pIndices[iNowVertexIndex++] = i + 2;
		pIndices[iNowVertexIndex++] = i + 3;

		pIndices[iNowVertexIndex++] = i;
		pIndices[iNowVertexIndex++] = i + 3;
		pIndices[iNowVertexIndex++] = i + 1;
	}

	ZeroMemory(&m_SubResourceDesc, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceDesc.pSysMem = pIndices;

	if (FAILED(__super::CreateBuffer(&(this->m_pIB))))
		return E_FAIL;

#pragma endregion

	SafeDeleteArray(pVertices);
	SafeDeleteArray(pIndices);

	m_TrailVertices.resize(m_iNumMaxVertices);

	return S_OK;
}


// 정점을 월드 상에 생성하는 함수이다.
void CVIBufferTrail::AddVertices(_fmatrix WorldMatrix, _float fTimeDelta)
{
	// m_TrailVertices 사이즈 + 2가 더 크면 2개 빼낸다.
	// 오래된 정점 두개를 뺀다.
	if (m_iNumMaxVertices < m_TrailVertices.size() + 2)
	{
		m_TrailVertices.pop_back();
		m_TrailVertices.pop_back();
	}
	
	// { 0.0f,  0.5f, 0.f };
	// { 0.0f, -0.5f, 0.f };
	// 로컬 좌표계
	_float fHighY = m_vLocalTrailHigh.y * m_fOffset;
	_float fLowY = m_vLocalTrailLow.y * m_fOffset;
	_vector vLocalTrailHigh = XMVectorSet(m_vLocalTrailHigh.x, fHighY, m_vLocalTrailHigh.z, 1.f);
	_vector vLocalTrailLow  = XMVectorSet(m_vLocalTrailLow.x, fLowY, m_vLocalTrailLow.z, 1.f);

	// 새로운 정점 생성.
	VTX_POSTEX_TIME VtxTrailHigh = {}, VtxTrailLow = {};
	_vector vVtxTrailHighPos = {}, vVtxTrailLowPos = {};
	
	// 로컬 좌표계에서 월드 좌표계로 변환한다.
	vVtxTrailHighPos = XMVector3TransformCoord(vLocalTrailHigh, WorldMatrix);
	vVtxTrailLowPos = XMVector3TransformCoord(vLocalTrailLow, WorldMatrix);

	// LifeTime.x 에 Max를 y에 현재 시간을 저장한다.
	VtxTrailHigh.fLifeTime.x = m_fLifeTime;
	VtxTrailLow.fLifeTime.x = m_fLifeTime;
	VtxTrailHigh.fLifeTime.y = 0.f;
	VtxTrailLow.fLifeTime.y = 0.f;

	// 월드상의 위치를 저장한다.
	XMStoreFloat3(&VtxTrailHigh.vPosition, vVtxTrailHighPos);
	XMStoreFloat3(&VtxTrailLow.vPosition, vVtxTrailLowPos);

	// 트레일 버퍼에 새로운 정점들을 추가.
	m_TrailVertices.push_front(VtxTrailHigh);
	m_TrailVertices.push_front(VtxTrailLow);

	// 현재 사용중인 개수를 2개 누적시킨다.
	m_iUsingCnt += 2;
	m_iUsingCnt = min(m_iUsingCnt, m_iNumMaxVertices);

	D3D11_MAPPED_SUBRESOURCE SubResource;
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	// Map, UnMap을 통해 정점의 위치를 변경한다.
	_float fAccDist = 0.f;
	VTX_POSTEX_TIME* pVertices = (VTX_POSTEX_TIME*)SubResource.pData;
	for (_uint i = 0; i < m_iUsingCnt; i += 2)
	{
		pVertices[i].vPosition = m_TrailVertices[i].vPosition;
		pVertices[i + 1].vPosition = m_TrailVertices[i + 1].vPosition;

		// x 값
		pVertices[i].fLifeTime.x = m_TrailVertices[i].fLifeTime.x;
		pVertices[i + 1].fLifeTime.x = m_TrailVertices[i + 1].fLifeTime.x;

		// y 값
		pVertices[i].fLifeTime.y = m_TrailVertices[i].fLifeTime.y;
		pVertices[i + 1].fLifeTime.y = m_TrailVertices[i + 1].fLifeTime.y;

		if (pVertices[i].fLifeTime.y >= pVertices[i].fLifeTime.x)
			pVertices[i].fLifeTime.y = 0.f;

		if (pVertices[i + 1].fLifeTime.y >= pVertices[i + 1].fLifeTime.x)
			pVertices[i + 1].fLifeTime.y = 0.f;

		// UV 좌표 설정
		_float fTot = (_float)(m_iUsingCnt / 2 - 1); // 전체 사각형의 개수 (정점 쌍)
		_float fRatio = ((i / 2) / fTot);            // 현재 사각형의 비율 (0 ~ 1)

		pVertices[i].vTexcoord = { fRatio, 0.0f };     // 상단 (U, V)
		pVertices[i + 1].vTexcoord = { fRatio, 1.0f }; // 하단 (U, V)
	}

	m_pContext->Unmap(m_pVB, 0);

	// m_TrailVertices의 정점이 4개라면 Indices는 6개이다.
	// m_iNumIndices = 3 * 2 * (m_TrailVertices.size() / 2 - 1);

	m_iNumIndices = 3 * 2 * (m_iUsingCnt / 2 - 1);

	for (_int i = 0; i < m_iUsingCnt; ++i)
		m_TrailVertices[i].fLifeTime.y += fTimeDelta;
}

void CVIBufferTrail::ResetTrailBuffers()
{
	for (_uint i = 0; i < m_iUsingCnt; i++)
		m_TrailVertices[i].fLifeTime.y = 0.f;

	m_iUsingCnt = 0;
}


CVIBufferTrail* CVIBufferTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBufferTrail* pInstance = new CVIBufferTrail(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CVIBufferTrail");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CComponent* CVIBufferTrail::Clone(void* pArg)
{
	CVIBufferTrail* pInstance = new CVIBufferTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBufferTrail");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CVIBufferTrail::Free()
{
	__super::Free();
}
