#include "VIBufferParticleRect.h"
#include "GameInstance.h"

CVIBufferParticleRect::CVIBufferParticleRect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CVIBufferParticleInstance(pDevice, pContext)
{

}

CVIBufferParticleRect::CVIBufferParticleRect(const CVIBufferParticleRect& Prototype)
    :
    CVIBufferParticleInstance(Prototype)
{
}

HRESULT CVIBufferParticleRect::InitializePrototype(const PARTICLE_DESC& ParticleDesc)
{
    m_iNumVertexBuffers = 2;                        // 버텍스 버퍼 개수 2개
    m_iNumInstances = ParticleDesc.iNumInstances;    // 인스턴스의 개수
    // 폭팔하는 파티클의 입자 개수가 10000개라면 10000개가 될 것이다.

    m_iVertexStride = sizeof(VTX_POSTEX);            // 정점 하나당의 사이즈
    m_iNumVertices = 4;                             // 버퍼의 정점 개수
    
    m_iIndexStride = sizeof(_ushort);               // 인덱스 버퍼 원소 하나의 사이즈
    m_iNumIndices = 6 * m_iNumInstances;            // 인덱스 버퍼의 개수
    // 사각형의 원래 인덱스 버퍼의 원소의 개수는 원래 6개이다 012 023 
    // 여기에 인스턴스 개수를 곱해주면 인덱스 버퍼의 총 원소의 개수가 나온다.

    m_eIndexFormat = DXGI_FORMAT_R16_UINT;
    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
    
    ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
    
    m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;      // 정점 버퍼 사이즈
    m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;              
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.StructureByteStride = m_iVertexStride;             // 정점 하나당 사이즈
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;

    VTX_POSTEX* pVertices = new VTX_POSTEX[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTX_POSTEX) * m_iNumVertices);

    pVertices[0].vPosition = XMFLOAT3(-0.5f, 0.5f, 0.f);
    pVertices[0].vTexcoord = XMFLOAT2(0.f, 0.f);

    pVertices[1].vPosition = XMFLOAT3(0.5f, 0.5f, 0.f);
    pVertices[1].vTexcoord = XMFLOAT2(1.f, 0.f);

    pVertices[2].vPosition = XMFLOAT3(0.5f, -0.5f, 0.f);
    pVertices[2].vTexcoord = XMFLOAT2(1.f, 1.f);

    pVertices[3].vPosition = XMFLOAT3(-0.5f, -0.5f, 0.f);
    pVertices[3].vTexcoord = XMFLOAT2(0.f, 1.f);

    ZeroMemory(&m_SubResourceDesc, sizeof m_SubResourceDesc);
    m_SubResourceDesc.pSysMem = pVertices;

    if (FAILED(__super::CreateBuffer(&m_pVB)))
        return E_FAIL;

    SafeDeleteArray(pVertices);

#pragma endregion

#pragma region INDEX_BUFFFER

    ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

    // 인덱스 버퍼의 총 바이트 수
    m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
    m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.StructureByteStride = 0;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;

    _ushort* pIndices = new _ushort[m_iNumIndices];
    ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

    _uint iNumIndices = 0;

    // 인스턴스 개수만큼 인덱스 번호를 채워준다.
    for (_uint i = 0; i < m_iNumInstances; ++i)
    {
        pIndices[iNumIndices++] = 0;
        pIndices[iNumIndices++] = 1;
        pIndices[iNumIndices++] = 2;

        pIndices[iNumIndices++] = 0;
        pIndices[iNumIndices++] = 2;
        pIndices[iNumIndices++] = 3;
    }

    ZeroMemory(&m_SubResourceDesc, sizeof(D3D11_SUBRESOURCE_DATA));
    m_SubResourceDesc.pSysMem = pIndices;

    if (FAILED(__super::CreateBuffer(&m_pIB)))
        return E_FAIL;

    SafeDeleteArray(pIndices);

#pragma endregion

#pragma region INSTANCE_BUFFER

    m_iInstanceStride = sizeof(VTX_INSTANCE);           // 인스턴싱 버퍼의 정점 하나의 사이즈
    m_iNumIndexPerInstance = 6;                         // 인스턴스당 인덱스 개수

    ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
    
    m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstances;   // 인스턴싱 버퍼 전체 사이즈
    m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;
    m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    m_InstanceBufferDesc.MiscFlags = 0;

    m_pInstanceVertices = new VTX_INSTANCE[m_iNumInstances];
    ZeroMemory(m_pInstanceVertices, m_iInstanceStride * m_iNumInstances);

    m_bIsLoopParticle = ParticleDesc.bIsLoop;
    m_pSpeeds = new _float[m_iNumInstances];
    m_vPivot = ParticleDesc.vPivot;

    for (_uint i = 0; i < m_iNumInstances; ++i)
    {
        m_pSpeeds[i] = m_pGameInst->ComputeRandom(ParticleDesc.vSpeed.x, ParticleDesc.vSpeed.y);
        _float fSize = m_pGameInst->ComputeRandom(ParticleDesc.vSize.x, ParticleDesc.vSize.y);
        m_pInstanceVertices[i].vLifeTime = _float2(m_pGameInst->ComputeRandom(ParticleDesc.vLifeTime.x, ParticleDesc.vLifeTime.y), 0.f);

        m_pInstanceVertices[i].vRight = _float4(fSize, 0.f, 0.f, 0.f);
        m_pInstanceVertices[i].vUp = _float4(0.f, fSize, 0.f, 0.f);
        m_pInstanceVertices[i].vLook = _float4(0.f, 0.f, fSize, 0.f);

        m_pInstanceVertices[i].vTranslation = 
        _float4
        (
            m_pGameInst->ComputeRandom(ParticleDesc.vCenter.x - ParticleDesc.vRange.x * 0.5f, ParticleDesc.vCenter.x + ParticleDesc.vRange.x * 0.5f),
            m_pGameInst->ComputeRandom(ParticleDesc.vCenter.y - ParticleDesc.vRange.y * 0.5f, ParticleDesc.vCenter.y + ParticleDesc.vRange.y * 0.5f),
            m_pGameInst->ComputeRandom(ParticleDesc.vCenter.z - ParticleDesc.vRange.z * 0.5f, ParticleDesc.vCenter.z + ParticleDesc.vRange.z * 0.5f),
            1.f
        );

        // m_pInstanceVertices[i].vTranslation = _float4(m_pGameInst->ComputeRandom(-5.f, 5.f), m_pGameInst->ComputeRandom(-5.f, 5.f), m_pGameInst->ComputeRandom(-5.f, 5.f), 1.f);
    }

    ZeroMemory(&m_InstanceInitialDesc, sizeof m_InstanceInitialDesc);
    m_InstanceInitialDesc.pSysMem = m_pInstanceVertices;

#pragma endregion

    return S_OK;
}

HRESULT CVIBufferParticleRect::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

CVIBufferParticleRect* CVIBufferParticleRect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const PARTICLE_DESC& ParticleDesc)
{
    CVIBufferParticleRect* pInstance = new CVIBufferParticleRect(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype(ParticleDesc)))
    {
        MSG_BOX("Failed to Created : CVIBufferParticleRect");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CComponent* CVIBufferParticleRect::Clone(void* pArg)
{
    CVIBufferParticleRect* pInstance = new CVIBufferParticleRect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CVIBufferParticleRect");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CVIBufferParticleRect::Free()
{
    __super::Free();
}
