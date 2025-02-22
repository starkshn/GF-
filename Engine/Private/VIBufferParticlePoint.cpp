#include "VIBufferParticlePoint.h"
#include "GameInstance.h"

CVIBufferParticlePoint::CVIBufferParticlePoint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CVIBufferParticleInstance(pDevice, pContext)
{
}

CVIBufferParticlePoint::CVIBufferParticlePoint(const CVIBufferParticlePoint& Prototype)
    :
    CVIBufferParticleInstance(Prototype)
{
}


HRESULT CVIBufferParticlePoint::InitializePrototype(const PARTICLE_DESC& ParticleDesc)
{
    m_iNumVertexBuffers = 2;
    m_iNumInstances = ParticleDesc.iNumInstances;

    m_iVertexStride = sizeof(VTX_POINT);
    m_iNumVertices = 1;

    m_iIndexStride = sizeof(_ushort);
    m_iNumIndices = m_iNumInstances;
    m_eIndexFormat = DXGI_FORMAT_R16_UINT;
    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

#pragma region VERTEX_BUFFER

    ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
    
    m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.StructureByteStride = m_iVertexStride;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;

    VTX_POINT* pVertices = new VTX_POINT[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTX_POINT) * m_iNumVertices);

    pVertices->vPosition = XMFLOAT3(0.f, 0.f, 0.f);
    pVertices->vSize = XMFLOAT2(1.f, 1.f);

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

    ZeroMemory(&m_SubResourceDesc, sizeof(D3D11_SUBRESOURCE_DATA));
    m_SubResourceDesc.pSysMem = pIndices;

    if (FAILED(__super::CreateBuffer(&m_pIB)))
        return E_FAIL;

    SafeDeleteArray(pIndices);

#pragma endregion

#pragma region INSTANCE_BUFFER
    m_iInstanceStride = sizeof(VTX_INSTANCE);
    m_iNumIndexPerInstance = 6;

    ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

    m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstances;
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

    for (_uint i = 0; i < m_iNumInstances; i++)
    {
        m_pSpeeds[i] = m_pGameInst->ComputeRandom(ParticleDesc.vSpeed.x, ParticleDesc.vSpeed.y);

        _float		fSize = m_pGameInst->ComputeRandom(ParticleDesc.vSize.x, ParticleDesc.vSize.y);

        m_pInstanceVertices[i].vRight = _float4(fSize, 0.f, 0.f, 0.f);
        m_pInstanceVertices[i].vUp = _float4(0.f, fSize, 0.f, 0.f);
        m_pInstanceVertices[i].vLook = _float4(0.f, 0.f, fSize, 0.f);
        m_pInstanceVertices[i].vTranslation =
            _float4(
                m_pGameInst->ComputeRandom(ParticleDesc.vCenter.x - ParticleDesc.vRange.x * 0.5f, ParticleDesc.vCenter.x + ParticleDesc.vRange.x * 0.5f),
                m_pGameInst->ComputeRandom(ParticleDesc.vCenter.y - ParticleDesc.vRange.y * 0.5f, ParticleDesc.vCenter.y + ParticleDesc.vRange.y * 0.5f),
                m_pGameInst->ComputeRandom(ParticleDesc.vCenter.z - ParticleDesc.vRange.z * 0.5f, ParticleDesc.vCenter.z + ParticleDesc.vRange.z * 0.5f),
                1.f);

        m_pInstanceVertices[i].vLifeTime = _float2(
            m_pGameInst->ComputeRandom(ParticleDesc.vLifeTime.x, ParticleDesc.vLifeTime.y),
            0.f
        );
    }

    for (_uint i = 0; i < m_iNumInstances; ++i)
    {
        if (m_fMaxLifeTime < m_pInstanceVertices[i].vLifeTime.x)
            m_fMaxLifeTime = m_pInstanceVertices[i].vLifeTime.x;
    }

    ZeroMemory(&m_InstanceInitialDesc, sizeof m_InstanceInitialDesc);
    m_InstanceInitialDesc.pSysMem = m_pInstanceVertices;
    

#pragma endregion

    return S_OK;
}

HRESULT CVIBufferParticlePoint::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

CVIBufferParticlePoint* CVIBufferParticlePoint::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const PARTICLE_DESC& ParticleDesc)
{
    CVIBufferParticlePoint* pInstance = new CVIBufferParticlePoint(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype(ParticleDesc)))
    {
        MSG_BOX("Failed to Created : CVIBufferParticlePoint");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CComponent* CVIBufferParticlePoint::Clone(void* pArg)
{
    CVIBufferParticlePoint* pInstance = new CVIBufferParticlePoint(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CVIBufferParticlePoint");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CVIBufferParticlePoint::Free()
{
    __super::Free();
}
