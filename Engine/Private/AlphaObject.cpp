#include "AlphaObject.h"
#include "Transform.h"
#include "GameInstance.h"

CAlphaObject::CAlphaObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CGameObject(pDevice, pContext)
{

}

CAlphaObject::CAlphaObject(const CAlphaObject& Prototype)
    :
    CGameObject(Prototype)
{

}

HRESULT CAlphaObject::InitializePrototype()
{
    return S_OK;
}

HRESULT CAlphaObject::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CAlphaObject::PriorityUpdate(_float fTimeDelta)
{
}

void CAlphaObject::Update(_float fTimeDelta)
{
}

void CAlphaObject::LateUpdate(_float fTimeDelta)
{
}

HRESULT CAlphaObject::Render()
{
    return S_OK;
}

void CAlphaObject::ComputeViewZ()
{
    _vector vPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);

    // 현재 월드 위치에 뷰 행렬을 곱하여 Z성분을 가져온다. 
    // - 뷰 스페이스상에서 카메라 기준 Z값을 담게 된다.
    vPos = XMVector3TransformCoord(vPos, m_pGameInst->GetTransformMatrix(CPipeLine::D3DTS_VIEW));
    m_fViewZ = XMVectorGetZ(vPos);
}

void CAlphaObject::Free()
{
    __super::Free();
}
