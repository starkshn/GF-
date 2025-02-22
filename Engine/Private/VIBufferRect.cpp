#include "VIBufferRect.h"

CVIBufferRect::CVIBufferRect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CVIBuffer(pDevice, pContext)
{
}

CVIBufferRect::CVIBufferRect(const CVIBufferRect& Prototype)
	:
	CVIBuffer(Prototype)
{
}

HRESULT CVIBufferRect::InitializePrototype()
{
	// 버텍스 버퍼 개수
	m_iNumVertexBuffers = 1;

	// Stride는 '크기'를 나타낸다. 즉, 한 정점의 크기가 몇인지 m_iVertexStride에 저장한다.
	m_iVertexStride = sizeof(VTX_POSTEX);
	m_iNumVertices = 4;

	m_iIndexStride = sizeof(_ushort);
	m_iNumIndices = 6;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	
#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	// 24.09.24
	// 수업에서 위처럼 되어있는데 뭘까?
	// ZeroMemory(&m_BufferDesc, sizeof(m_BufferDesc));
	// 미묘하게 다른 문법을 사용하지만 동작의 차이는 없다고 한다.
	// sizeof는 연산자 이므로 괄호가 필수가 아니다.
	// 식이 들어가거나 의도를 명확하게 하고싶으면 () 를 붙여주자

	// 정점 버퍼를 몇 바이트 할당 할까요?
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;

	// 연결(바인드) 플래그
	// - 자원을 파이프 라인의 어디에 연결할 수 있을지를 나타낸다. (OR 비트 플래그 사용가능)
	// - VERTEX_BUFFER, INDEX_BUFFER는 파이프라인에 기하구조 자료를 공급하기 위한 입력 조립기 단계(Input Assemble)에 부착할 자원에 쓰인다.
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	
	// 자원 용도 플래그
	// - 해당 필드는 응용 프로그램이 자원을 어떤 용도로 쓸 것인지를 나타낸다.
	// - 여기서 자원은 컴퓨터안의 어딘가의 메모리 블록이다. 그런데 자원이 존재하는 메모리는 비디오 카드의 메모리일 수도 있고(GPU), 시스템 메모리(메인 메모리)일 수도 있다. 또한 비디오 -> 시스템, 시스템 -> 비디오 방향 이 있을 수 있다.
	// - 실행 시점 모듈/구동기가 자원을 최적의 장소에 두고 내부적으로 자원을 효울적으로 처리할 수 있게 하기 위해 반드시 Usage를 설정하여 '의도'를 나타내주어야 한다.
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;					// 용도 : Default(정적) GPU : Read, CPU : Read

	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = 0;					// CPU의 접근 방식, 기본 용도는 CPU의 접근을 막는다(0으로 설정)
	m_BufferDesc.MiscFlags = 0;							// 아직 안 씀
	
	VTX_POSTEX* pVertices = new VTX_POSTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTX_POSTEX) * m_iNumVertices);

	// 0 1
	// 3 2
	// NDC 상의 위치를 잡아준다.
	pVertices[0].vPosition = XMFLOAT3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexcoord = XMFLOAT2(0.f, 0.f);

	pVertices[1].vPosition = XMFLOAT3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexcoord = XMFLOAT2(1.f, 0.f);

	pVertices[2].vPosition = XMFLOAT3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexcoord = XMFLOAT2(1.f, 1.f);

	pVertices[3].vPosition = XMFLOAT3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexcoord = XMFLOAT2(0.f, 1.f);

	// 24.09.24
	// D3D11_SUBRESOURCE_DATA
	// GPU로 데이터를 전송할 때 쓰이는 구조체이다.
	// GPU가 화면에 그래픽을 그리기 위해 필요한 데이터를 어떻게 보내야 할지 설명하는 역할을 한다.
	// 
	// 왜 필요한가?
	// GPU는 메모리에 저장된 데이터를 사용해 화면에 그래픽을 표시한다. 이때 CPU가 이 데이터를 준비해주고 GPU가 그 데이터를 읽어서 처리하는 방식이다. GPU가 데이터를 잘 이해할 수 있게 데이터의 위치와 형태를 알려줘야 하는데 이때 D3D11_SUBRESOURCE_DATA가 필요하다.
	// 용도가 뭔가?
	// - 버퍼나 텍스쳐에 데이터를 초기화할 때 쓴다.
	// - GPU가 사용할 데이터를 CPU에서 미리 준비해서 GPU로 전송할때 , GPU 메모리에 어떻게 데이터를 채울지 알려주는 역할을 한다.
	ZeroMemory(&m_SubResourceDesc, sizeof m_SubResourceDesc);
	m_SubResourceDesc.pSysMem = pVertices;

	if (FAILED(__super::CreateBuffer(&m_pVB)))
		return E_FAIL;

	// GPU에게 데이터는 넘겨주었으니 pVertices는 필요없다. 동적할당한 메모리를 해제해주자.
	SafeDeleteArray(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	// 24.09.25
	// 정점 버퍼를 몇 바이트 할당할까요?
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = 0;//m_iIndexStride; // 0 으로 설정해도 무방하다. 나중에 이를 지정하는 함수가 따로 있어서 무방하다고 한다.
	m_BufferDesc.CPUAccessFlags = 0; // 접근 X
	m_BufferDesc.MiscFlags = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;

	pIndices[3] = 0;
	pIndices[4] = 2;
	pIndices[5] = 3;

	ZeroMemory(&m_SubResourceDesc, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceDesc.pSysMem = pIndices;

	if (FAILED(__super::CreateBuffer(&m_pIB)))
		return E_FAIL;

	SafeDeleteArray(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CVIBufferRect::Initialize(void* pArg)
{
	return S_OK;
}

CVIBufferRect* CVIBufferRect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBufferRect* pInstance = new CVIBufferRect(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CVIBufferRect");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CComponent* CVIBufferRect::Clone(void* pArg)
{
	CVIBufferRect* pInstance = new CVIBufferRect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBufferRect");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CVIBufferRect::Free()
{
	__super::Free();
}
