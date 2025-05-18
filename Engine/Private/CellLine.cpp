#include "CellLine.h"
#include "GameInstance.h"
#include "Shader.h"
#include "VIBufferLine.h"

_float4x4 CCellLine::m_WorldMatrix = {};

CCellLine::CCellLine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CGameObject(pDevice, pContext)
{
}

CCellLine::CCellLine(const CCellLine& Prototype)
    :
    CGameObject(Prototype),
    m_pShader(Prototype.m_pShader)
{
#ifdef _DEBUG
    SafeAddRef(m_pShader);
    
    m_pVIBuffer = CVIBufferLine::Create(Prototype.m_pDevice, Prototype.m_pContext);

#endif
}

HRESULT CCellLine::InitializePrototype()
{
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../../EngineSDK/hlsl/ShaderLine.hlsl"), VTX_POS::Elements, VTX_POS::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;

    m_pVIBuffer = CVIBufferLine::Create(m_pDevice, m_pContext);

    return S_OK;
}

HRESULT CCellLine::Initialize(void* pArg)
{
    return S_OK;
}

void CCellLine::Update(_fmatrix vWorld)
{
    XMStoreFloat4x4(&m_WorldMatrix, vWorld);
}

void CCellLine::UpdateLine(const _fvector& vStart, const _fvector& vEnd)
{
    m_pVIBuffer->UpdateLineVertices(vStart, vEnd);
}

void CCellLine::SetStartEndLinePos(_fvector vStart, _fvector vEnd)
{
    XMStoreFloat3(&m_vStart, vStart);
    XMStoreFloat3(&m_vEnd, vEnd);
    m_pVIBuffer->SetStartEnd(vStart, vEnd);
}

#ifdef _DEBUG
HRESULT CCellLine::Render()
{
    if (FAILED(m_pShader->BindMatrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;

    _float4x4 viewMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_VIEW);
    _float4x4 projMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_PROJ);

    if (FAILED(m_pShader->BindMatrix("g_ViewMatrix", &viewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShader->BindMatrix("g_ProjMatrix", &projMatrix)))
        return E_FAIL;

    m_pShader->Begin(0);

    m_pVIBuffer->BindBufferDesc();
    m_pVIBuffer->Render();

    return S_OK;
}
#endif

CCellLine* CCellLine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCellLine* pInstance = new CCellLine(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CCellLine");
        SafeRelease(pInstance);
    }
    return pInstance;
}

CGameObject* CCellLine::Clone(void* pArg)
{
    CCellLine* pInstance = new CCellLine(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CCellLine");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CCellLine::Free()
{
    __super::Free();

#ifdef _DEBUG

    SafeRelease(m_pShader);
    // SafeRelease(m_pVIBuffer);

#endif
}
