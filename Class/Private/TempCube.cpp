#include "TempCube.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Level_Tool.h"
#include "ColBoundingBox.h"



CTempCube::CTempCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CGameObject(pDevice, pContext)
{
}

CTempCube::CTempCube(const CTempCube& Prototype)
    :
    CGameObject(Prototype)
{
}

HRESULT CTempCube::InitializePrototype()
{
    return S_OK;
}

HRESULT CTempCube::Initialize(void* pArg)
{
    if (__super::Initialize(pArg))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->SetTransformState(CTransform::STATE_POS, _vector{ 5.f, 0.f, 0.f, 1.f });

    return S_OK;
}

void CTempCube::PriorityUpdate(_float fTimeDelta)
{
    
}

void CTempCube::Update(_float fTimeDelta)
{
    m_pBoundingBox->Update_Bouding_Box(m_pTransformCom);
}

void CTempCube::LateUpdate(_float fTimeDelta)
{
    m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CTempCube::Render()
{
    _float4x4 viewMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_VIEW);
    _float4x4 projMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_PROJ);

    if (FAILED(m_pBoundingBox->Render_Bounding_Box(m_pTransformCom, XMLoadFloat4x4(&viewMatrix), XMLoadFloat4x4(&projMatrix))))
        return E_FAIL;

    return S_OK;
}

HRESULT CTempCube::Ready_Components()
{
    // Com_Texture
    {
        if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Logo"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
    }

    // Com_BoudingBox
    {
        if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_BoundingBox"), TEXT("Com_BoundingBox"), reinterpret_cast<CComponent**>(&m_pBoundingBox))))
            return E_FAIL;
    }


    return S_OK;
}

CTempCube* CTempCube::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTempCube* pInst = new CTempCube(pDevice, pContext);

    if (FAILED(pInst->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CTempCube");
        SafeRelease(pInst);
    }

    return pInst;
}

CGameObject* CTempCube::Clone(void* pArg)
{
    CTempCube* pInst = new CTempCube(*this);

    if (FAILED(pInst->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CTempCube");
        SafeRelease(pInst);
    }

    return pInst;
}

void CTempCube::Free()
{
    __super::Free();

    SafeRelease(m_pTextureCom);
    SafeRelease(m_pBoundingBox);
}
