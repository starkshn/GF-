#include "VIBuffer.h"

CVIBuffer::CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CComponent(pDevice, pContext)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& Prototype)
    :
    CComponent(Prototype),
    m_pVB(Prototype.m_pVB),
    m_pIB(Prototype.m_pIB),
    m_iVertexStride{ Prototype.m_iVertexStride },
    m_iNumVertices{ Prototype.m_iNumVertices },
    m_iNumVertexBuffers{ Prototype.m_iNumVertexBuffers },
    m_iIndexStride{ Prototype.m_iIndexStride },
    m_iNumIndices{ Prototype.m_iNumIndices },
    m_eIndexFormat{ Prototype.m_eIndexFormat },
    m_ePrimitiveTopology{ Prototype.m_ePrimitiveTopology },
    m_pVertexPos(Prototype.m_pVertexPos)
{
    SafeAddRef(m_pVB);
    SafeAddRef(m_pIB);
}


HRESULT CVIBuffer::InitializePrototype()
{
    return S_OK;
}

HRESULT CVIBuffer::Initialize(void* pArg)
{
    return S_OK;
}

// 24.09.25
HRESULT CVIBuffer::Render()
{
    // 레이아웃을 먼저 만들어서 쉐이더에 적용되는지 먼저 검증한다음에 DrawIndexed를 하면 쉐이더 코드가 동작하면서 그려지기 시작한다.
    m_pContext->DrawIndexed(m_iNumIndices, 0, 0);

    return S_OK;
}

HRESULT CVIBuffer::BindBufferDesc()
{
    // 버텍스 버퍼는 여러개 가질 수 있기 때문에 배열의 형태로 저장해준다.
    // (나중에 파티클 같은거 구현할때 버텍스 버퍼 여러개 쓴다)
    ID3D11Buffer*   pVertexBuffers[] = { m_pVB, };

    // 각 버퍼 마다 정점의 크기가 다를 수 있으니 배열로 Stride를 저장해준다.
    _uint           iVertexStrides[] = { m_iVertexStride, };

    // 데이터를 읽을 오프셋이다(보통 0번부터 읽으니 건드릴 일이 없다)
    _uint           iOffsets[] = { 0, };

    // IA(Input Assembler)는 렌더링 파이프 라인중에서 그릴 때 사용되는 공간으로 알고 있는게 좋다.
    // IA라는 공간을 통해서 그린다고 생각하면 좋다.
    // 1. StartSlot : IA의 정점 버퍼 슬롯들 중 자원을 연결 할 첫 슬롯의 인덱스 번호
    // 2. NumBffers : 슬롯들에 연결할 버퍼들의 전체 개수
    // 3. ppVertexBuffers : 연결할 버퍼 자원들을 가리키는 포인터들을 담은 연속적인 배열을 지정
    // 4. Stride(보폭) : 각 정점 버퍼마다(한 정점에서 그 다음 정점까지의 거리)와 버퍼에서 첨으로 쓰일 정점의 위치를 지정(Offset)
    m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);

    // 1. 인덱스 버퍼, 2. 인덱스 포맷, 3. 오프셋 (0)
    m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

    // 삼각형 단위로 그리겠다는 의미이다.
    m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

    // 이렇게 까지만 설정하고 DrawIndexed호출시 그릴 수 없다.
    // 빛 연산, TEXCOORD같은 부분들 사용자가 직접 해야하기 때문이다. -> 추가적인 부분이 필요하다(Shader)
    // m_pContext->DrawIndexed(m_iNumIndices, 0, 0);


    // D3D11_INPUT_ELEMENT_DESC 인자 설명
    // POSITION의 의미는 VTXPOSTEX 가 가지고 있는 정점의 멤버 변수 하나를 의미한다. (TEXCOORD도 마찬가지)
    // 첫번째 인자 : 니가 가지고 있는 정점의 sementic을 ElementDesc에다가 문자열로 저장해야한다.
    // 두번째 인자 : POSITION은 현재 1개이니까 0을 넣어준다. 
    // - TEXCOORD의 경우 8개(7개?)까지 가질 수 있는데 2개면 1, 3개면 2 이런식으로 넣어준다.
    // 세번째 인자 : FORMAT을 넣어준다.
    // 네번째 인자 : 0 부터 시작해서 읽는다.
    // 다섯번째 인자 : 오프셋이다. POSITION의 경우 0부터 시작한다(이후 4byte, 4byte, 4byte니가 다음 오프셋은 12이다)
    // 뒤에 두개는 인스턴스일때 유효한 인자이다.
   /* D3D11_INPUT_ELEMENT_DESC ElementDesc[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };*/
    // 위에 DESC 240927 로 Loader로 옮겨감

    // 아래 pInputLayOut는 컴객체인데 매 프레임 생성할 수 없으니 나중에 다른곳으로 이전 시키겠다.
    // ID3D11InputLayout는 쉐이더에 입력되는 정점 정보를 입력해주어야 한다. (말 그대로 레이아웃(청사진)을 제공해야한다)
    // DrawIndexed로 바로 그리는게 아니라 LayOut을 통해 쉐이더 에 적용되는지 검증을 하고 Draw를 한다.
    // ID3D11InputLayout* pInputLayOut;
    // 
    // 아래 3, 4번째 인자는 shader 파일로 만든 객체로 부터 얻어와서 채워야한다. 즉 shader를 만들고 빌드 해야한다.
    // m_pDevice->CreateInputLayout(ElementDesc, 2, 쉐이더에서 받는 정점의 정의, 쉐이더에서 받는 정점의 크기);
    // m_pContext->IASetInputLayout(pInputLayOut);

    return S_OK;
}

HRESULT CVIBuffer::CreateBuffer(ID3D11Buffer** ppOut)
{
    // CreateBuffer에 들어갈 m_BuuferDesc, m_SubResourceDesc 들은 자식에서 각 용도에 맞게 DESC들을 채운다음에 인자로 전달받아서 채울 것이다.
    if (FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceDesc, ppOut)))
        return E_FAIL;
    
    return S_OK;
}

void CVIBuffer::Free()
{
    __super::Free();

    if (false == m_bIsCloned)
        SafeDeleteArray(m_pVertexPos);

    SafeRelease(m_pVB);
    SafeRelease(m_pIB);
}
