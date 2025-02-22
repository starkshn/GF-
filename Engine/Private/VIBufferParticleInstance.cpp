#include "VIBufferParticleInstance.h"
#include "GlobalFuncs.h"

CVIBufferParticleInstance::CVIBufferParticleInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CVIBuffer(pDevice, pContext)
{

}

CVIBufferParticleInstance::CVIBufferParticleInstance(const CVIBufferParticleInstance& Prototype)
    :
    CVIBuffer(Prototype),
    m_pVBInstance(Prototype.m_pVBInstance) /* nullptr이 채워진다 */,
    m_InstanceBufferDesc(Prototype.m_InstanceBufferDesc),
    m_InstanceInitialDesc(Prototype.m_InstanceInitialDesc),
    m_iNumInstances(Prototype.m_iNumInstances),
    m_iNumIndexPerInstance(Prototype.m_iNumIndexPerInstance),
    m_iInstanceStride(Prototype.m_iInstanceStride),
    m_pInstanceVertices(Prototype.m_pInstanceVertices),
    m_pSpeeds(Prototype.m_pSpeeds),
    m_bIsLoopParticle(Prototype.m_bIsLoopParticle),
    m_vPivot(Prototype.m_vPivot ),
    m_fMaxLifeTime(Prototype.m_fMaxLifeTime),
    m_bActivate(Prototype.m_bActivate)
{
    SafeAddRef(m_pVBInstance);
}

HRESULT CVIBufferParticleInstance::InitializePrototype()
{
    return S_OK;
}

HRESULT CVIBufferParticleInstance::Initialize(void* pArg)
{
    return m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InstanceInitialDesc, &m_pVBInstance);
}

HRESULT CVIBufferParticleInstance::Render()
{
    m_pContext->DrawIndexedInstanced(m_iNumIndexPerInstance, m_iNumInstances, 0, 0, 0);
    return S_OK;
}

HRESULT CVIBufferParticleInstance::BindBufferDesc()
{
    ID3D11Buffer* pVertexBuffers[] = { m_pVB, m_pVBInstance };
    _uint iVertexStrides[] = { m_iVertexStride, m_iInstanceStride };
    _uint iOffsets[] = { 0, 0 };
    
    m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
    m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
    m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

    return S_OK;
}

void CVIBufferParticleInstance::DropParticles(_float fTimeDelta)
{
    D3D11_MAPPED_SUBRESOURCE SubResource;
    
    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    VTX_INSTANCE* pVertices = static_cast<VTX_INSTANCE*>(SubResource.pData);

    for (_uint i = 0; i < m_iNumInstances; ++i)
    {
        pVertices[i].vTranslation.y -= m_pSpeeds[i] * fTimeDelta;
        pVertices[i].vLifeTime.y += fTimeDelta;
        
        if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
        {
            if (m_bIsLoopParticle)
            {
                // 초기값은 이미 담아두고 있기 때문에 바로 대입해준다.
                pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
                pVertices[i].vLifeTime.y = 0.f;
            }
        }
    }

    m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBufferParticleInstance::SpreadParticles(_float fTimeDelta)
{
    if (false == IsActivate())
        return;

    D3D11_MAPPED_SUBRESOURCE		SubResource{};

    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    VTX_INSTANCE* pVertices = static_cast<VTX_INSTANCE*>(SubResource.pData);

    for (size_t i = 0; i < m_iNumInstances; i++)
    {
        _vector		vDir = XMLoadFloat4(&pVertices[i].vTranslation) - XMVectorSetW(XMLoadFloat3(&m_vPivot), 1.f);

        XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);

        pVertices[i].vLifeTime.y += fTimeDelta;

        if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
        {
            if (true == m_bIsLoopParticle)
            {
                pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
                pVertices[i].vLifeTime.y = 0.f;
            }
            else
            {
                pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
                if (AlmostEqual(pVertices[i].vLifeTime.y, m_fMaxLifeTime, 0.01f))
                    DeActivate();
            }
        }
    }

    m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBufferParticleInstance::ReverseSpreadParticles(_float fTimeDelta)
{
    if (false == IsActivate())
        return;

    D3D11_MAPPED_SUBRESOURCE		SubResource{};

    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    VTX_INSTANCE* pVertices = static_cast<VTX_INSTANCE*>(SubResource.pData);

    for (size_t i = 0; i < m_iNumInstances; i++)
    {
        _vector		vDir = XMLoadFloat4(&pVertices[i].vTranslation) - XMVectorSetW(XMLoadFloat3(&m_vPivot), 1.f);

        vDir = -vDir;

        XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);

        pVertices[i].vLifeTime.y += fTimeDelta;

        if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
        {
            if (true == m_bIsLoopParticle)
            {
                pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
                pVertices[i].vLifeTime.y = 0.f;
            }
            else
            {
                pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
                if (AlmostEqual(pVertices[i].vLifeTime.y, m_fMaxLifeTime, 0.01f))
                    DeActivate();
            }
        }
    }

    m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBufferParticleInstance::ResetParticles()
{
    if (m_bIsLoopParticle)
    {
        if (m_bActivate)
            m_bActivate = false;
        return;
    }

    D3D11_MAPPED_SUBRESOURCE		SubResource{};

    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    VTX_INSTANCE* pVertices = static_cast<VTX_INSTANCE*>(SubResource.pData);

    for (size_t i = 0; i < m_iNumInstances; i++)
    {
        pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
        pVertices[i].vLifeTime.y = 0.f;
    }

    m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBufferParticleInstance::Free()
{
    __super::Free();

    // 원본일 때만 삭제한다.
    if (false == m_bIsCloned)
    {
        SafeDeleteArray(m_pInstanceVertices);
        SafeDeleteArray(m_pSpeeds);
    }

    SafeRelease(m_pVBInstance);
}
