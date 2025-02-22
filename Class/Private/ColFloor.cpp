#include "ColFloor.h"
#include "GameInstance.h"
#include "ColBoundingBox.h"
#include "Transform.h"

CColFloor::CColFloor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CGameObject(pDevice, pContext)
{

}

CColFloor::CColFloor(const CColFloor& Prototype)
    :
    CGameObject(Prototype)
{

}

HRESULT CColFloor::InitializePrototype()
{
    return S_OK;
}

HRESULT CColFloor::Initialize(void* pArg)
{
    if (__super::Initialize(pArg))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->SetTransformState(CTransform::STATE_POS, _vector{ 0.f, 0.f, 0.f, 1.f });
    m_pTransformCom->Scaling(10.f, 0.5f, 10.f);

    return S_OK;
}

void CColFloor::PriorityUpdate(_float fTimeDelta)
{

}

void CColFloor::Update(_float fTimeDelta)
{
    m_pBoundingBox->Update_Bouding_Box(m_pTransformCom);
}

void CColFloor::LateUpdate(_float fTimeDelta)
{
    m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CColFloor::Render()
{
    _float4x4 viewMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_VIEW);
    _float4x4 projMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_PROJ);

    if (FAILED(m_pBoundingBox->Render_Bounding_Box(m_pTransformCom, XMLoadFloat4x4(&viewMatrix), XMLoadFloat4x4(&projMatrix))))
        return E_FAIL;

    return S_OK;
}

HRESULT CColFloor::Ready_Components()
{
    // Com_BoudingBox
    {
        if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_BoundingBox"), TEXT("Com_BoundingBox"), reinterpret_cast<CComponent**>(&m_pBoundingBox))))
            return E_FAIL;
    }

    return S_OK;
}

CColFloor* CColFloor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CColFloor* pInst = new CColFloor(pDevice, pContext);

    if (FAILED(pInst->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CColFloor");
        SafeRelease(pInst);
    }

    return pInst;
}

CGameObject* CColFloor::Clone(void* pArg)
{
    CColFloor* pInst = new CColFloor(*this);

    if (FAILED(pInst->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CColFloor");
        SafeRelease(pInst);
    }

    return pInst;
}

void CColFloor::Free()
{
    __super::Free();

    SafeRelease(m_pBoundingBox);
}
