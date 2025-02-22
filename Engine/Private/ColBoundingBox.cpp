#include "ColBoundingBox.h"
#include "Shader.h"
#include "VIBuffer_ColCube.h"
#include "Transform.h"


CColBoundingBox::CColBoundingBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CComponent(pDevice, pContext)
{
    // 기본 래스터라이저 상태 설정
    D3D11_RASTERIZER_DESC rasterDesc;
    ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
    rasterDesc.FillMode = D3D11_FILL_SOLID; // 기본 상태는 솔리드
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.FrontCounterClockwise = FALSE;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.SlopeScaledDepthBias = 0.0f;
    rasterDesc.MultisampleEnable = FALSE;
    rasterDesc.AntialiasedLineEnable = FALSE;

    m_pDevice->CreateRasterizerState(&rasterDesc, &m_pDefaultRasterizerState);
}

CColBoundingBox::CColBoundingBox(const CColBoundingBox& Prototype)
    :
    CComponent(Prototype),
    m_pShaderCom(Prototype.m_pShaderCom),
    m_pVIBufferCom(Prototype.m_pVIBufferCom),
    m_pDefaultRasterizerState(Prototype.m_pDefaultRasterizerState)

{
    SafeAddRef(m_pShaderCom);
    SafeAddRef(m_pVIBufferCom);
    SafeAddRef(m_pDefaultRasterizerState);
}

HRESULT CColBoundingBox::InitializePrototype(CBase* pShader, CBase* pVIBuffer)
{
    m_pShaderCom = dynamic_cast<CShader*>(pShader);
    m_pVIBufferCom = dynamic_cast<CVIBuffer_ColCube*>(pVIBuffer);

    if (nullptr == m_pShaderCom || nullptr == m_pVIBufferCom)
        return E_FAIL;

    return S_OK;
}

HRESULT CColBoundingBox::Initialize(void* pArg)
{
    return S_OK;
}

void CColBoundingBox::Update_Bouding_Box(CTransform* pTransform)
{
    _float3 vScale = pTransform->ComputeScaled();
    _vector vPos = pTransform->GetTransformState(CTransform::STATE_POS);
    _float4 vPosTemp;
    XMStoreFloat4(&vPosTemp, vPos);
    m_BoundingBox.Center = { vPosTemp.x, vPosTemp.y, vPosTemp.z };
    m_BoundingBox.Extents = { 0.5f * vScale.x, 0.5f * vScale.y, 0.5f * vScale.z };
}

HRESULT CColBoundingBox::Render_Bounding_Box(CTransform* pTransform, _matrix viewMat, _matrix projMat)
{
    if (FAILED(pTransform->BindShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    _float4x4 viewMatrix;
    _float4x4 projMatrix;

    XMStoreFloat4x4(&viewMatrix, viewMat);
    XMStoreFloat4x4(&projMatrix, projMat);

    if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &viewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &projMatrix)))
        return E_FAIL;

    _float3 vR = _float3(1.f, 0.f, 0.f);
    _float3 vG = _float3(0.f, 1.f, 0.f);

    if (m_bCurPicked == true)
        m_pShaderCom->BindRawValue("g_Color", &vR, sizeof(_float3));
    else
        m_pShaderCom->BindRawValue("g_Color", &vG, sizeof(_float3));

    // 0 default, 1 wireframe
    m_pShaderCom->Begin(0);

    m_pVIBufferCom->BindBufferDesc();

    m_pVIBufferCom->Render();

    m_pContext->RSSetState(m_pDefaultRasterizerState);

    return S_OK;
}

_bool CColBoundingBox::Check_Picking(_fvector vRayOrigin, _fvector vRayDir, _float& fDist, _float4& vPickingPos)
{
    if (!m_bUsePicking) return false;
    if (m_BoundingBox.Intersects(vRayOrigin, vRayDir, fDist))
    {
        _vector vPickedPos = vRayOrigin + vRayDir * fDist;
        XMStoreFloat4(&vPickingPos, vPickedPos);

        m_bCurPicked = true;

        return true;
    }
    return false;
}

CColBoundingBox* CColBoundingBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBase* pShader, CBase* pVIBuffer)
{
    CColBoundingBox* pInst = new CColBoundingBox(pDevice, pContext);

    if (FAILED(pInst->InitializePrototype(pShader, pVIBuffer)))
    {
        MSG_BOX("Failed to Created : CColBoudingBox");
        SafeRelease(pInst);
    }

    return pInst;
}

CComponent* CColBoundingBox::Clone(void* pArg)
{
    CColBoundingBox* pInst = new CColBoundingBox(*this);

    if (FAILED(pInst->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CColBoudingBox");
        SafeRelease(pInst);
    }

    return pInst;
}

void CColBoundingBox::Free()
{
    __super::Free();

    SafeRelease(m_pShaderCom);
    SafeRelease(m_pVIBufferCom);
    SafeRelease(m_pDefaultRasterizerState);
}
