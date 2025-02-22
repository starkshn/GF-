#include "Tool_Terrain.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Transform.h"
#include <DirectXCollision.h>
#include <DirectXMath.h>
#include <iostream>
using namespace DirectX::TriangleTests;

CTool_Terrain::CTool_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CGameObject{ pDevice, pContext }
{

}

CTool_Terrain::CTool_Terrain(const CTool_Terrain& Prototype)
    :
    CGameObject(Prototype)
{

}

HRESULT CTool_Terrain::InitializePrototype()
{
    return S_OK;
}

HRESULT CTool_Terrain::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(nullptr)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    return S_OK;
}

void CTool_Terrain::PriorityUpdate(_float fTimeDelta)
{

}

void CTool_Terrain::Update(_float fTimeDelta)
{
    _long MouseMove = {};

    if (MouseMove = m_pGameInst->Get_DIMouseState(DIM_LB))
    {
        // TODO
        _ulong MSX = m_pGameInst->Get_DIMouseMove(DIMS_X);
        _ulong MSY = m_pGameInst->Get_DIMouseMove(DIMS_Y);


        // 마우스 스크린 좌표
        POINT ptMousePos;
        GetCursorPos(&ptMousePos);
        ScreenToClient(g_hWnd, &ptMousePos);

        _float4 vPickPos;
        _float fDis = 0;
        Picking(ptMousePos.x, ptMousePos.y, vPickPos, fDis);

        cout << ptMousePos.x << ' ' << ptMousePos.y << endl;
    }
    
}

void CTool_Terrain::LateUpdate(_float fTimeDelta)
{
    m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CTool_Terrain::Render()
{
    if (FAILED(BindShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_Buffer_Desc();

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CTool_Terrain::ReadyComponents()
{
    /* Com_Shader */
    if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxNorTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Terrain"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* Com_Texture */
    if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Terrain"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CTool_Terrain::BindShaderResources()
{
    if (FAILED(m_pTransformCom->BindShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    _float4x4 viewMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_VIEW);
    _float4x4 projMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_PROJ);

    if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &viewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &projMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->BindShaderResource(m_pShaderCom, "g_Texture", 0))) 
        return E_FAIL;

    return S_OK;
}

_bool CTool_Terrain::Picking(_int iScreenX, _int iScreenY, _float4& vPickPos, _float& fDistance)
{
    // 뷰, 투영(원근) 행렬을 얻어온다.
    _matrix     viewMatrix = m_pGameInst->GetTransformMatrix(CPipeLine::DTS::D3DTS_VIEW);
    _matrix     projMatrix = m_pGameInst->GetTransformMatrix(CPipeLine::DTS::D3DTS_PROJ);

    // 마우스 클릭 위치를 NDC로 변환
    _float      mouseNDCX = (_float(iScreenX) / _float(g_iWinSizeX)) * 2.f - 1.f; // -1.0 ~ 1.0
    _float      mouseNDCY = 1.f - (_float(iScreenY) / _float(g_iWinSizeY)) * 2.f; // -1.0 ~ 1.0

    // 뷰, 투영 역행렬 계산
    _matrix     invV = XMMatrixInverse(nullptr, viewMatrix);
    _matrix     invP = XMMatrixInverse(nullptr, projMatrix);

    // _matrix     invVP = XMMatrixInverse(nullptr, XMMatrixMultiply(viewMatrix, projMatrix));
    _matrix     invVP = invP * invV;

    // NDC를 월드 공간으로 변환
    // NDC상에서 가장 가까운 곳과 가장 먼 곳의 좌표를 잡아준다.
    _vector     vNearPoint = XMVectorSet(mouseNDCX, mouseNDCY, 0.f, 1.f);
    _vector     vFarPoint = XMVectorSet(mouseNDCX, mouseNDCY, 1.f, 1.f);
    
    // 이 두 점을 빼면 Ray가 나온다.
    _vector     vWorldNear = XMVector3TransformCoord(vNearPoint, invVP);
    _vector     vWorldFar = XMVector3TransformCoord(vFarPoint, invVP);

    // 뷰 스페이스 변환 행렬의 역행렬의 3행 성분이 카메라 월드 위치이다.
    _matrix     vInvView = XMMatrixInverse(nullptr, viewMatrix);
    _vector     vRayOrigin = vInvView.r[3];
    XMVectorSetW(vRayOrigin, 1.f);

    _float4 vPos;
    XMStoreFloat4(&vPos, vRayOrigin);

    // Ray의 방향을 구해준다.
    _vector     vRayDir = XMVector3Normalize(XMVectorSubtract(vWorldFar, vWorldNear));

    _float4     vRayTemp;
    XMStoreFloat4(&vRayTemp, vRayDir);
    
    _uint       iVertexCntZ = m_pVIBufferCom->Get_VerticesZ();
    _uint       iVertexCntX = m_pVIBufferCom->Get_VerticesX();

    // 결과
    _bool       bRet = false;

    const vector<VTXNORTEX>& vertices = m_pVIBufferCom->Get_Vertices_Vector();

    _float      fDist = 0.f;

    for (_uint z = 0; z < iVertexCntZ - 1; ++z)
    {
        for (_uint x = 0; x < iVertexCntX - 1; ++x)
        {
            _uint idx = z * iVertexCntX + x;
            _uint indices[] = { idx + iVertexCntX , idx + iVertexCntX + 1, idx + 1, idx };
            
            _vector p0 = XMVectorSet(vertices[indices[0]].vPosition.x, vertices[indices[0]].vPosition.y, vertices[indices[0]].vPosition.z, 1.f);
            _vector p1 = XMVectorSet(vertices[indices[1]].vPosition.x, vertices[indices[1]].vPosition.y, vertices[indices[1]].vPosition.z, 1.f);
            _vector p2 = XMVectorSet(vertices[indices[2]].vPosition.x, vertices[indices[2]].vPosition.y, vertices[indices[2]].vPosition.z, 1.f);
            _vector p3 = XMVectorSet(vertices[indices[3]].vPosition.x, vertices[indices[3]].vPosition.y, vertices[indices[3]].vPosition.z, 1.f);

            // inline bool XM_CALLCONV Intersects(FXMVECTOR Origin, FXMVECTOR Direction, FXMVECTOR V0, GXMVECTOR V1, HXMVECTOR V2, float& Dist)
            bRet = Intersects(vRayOrigin, vRayDir, p0, p1, p2, fDist);

            if (bRet)
            {

                _vector vColPos = vRayOrigin + vRayDir * fDist;


                int a = 10;

            }


            bRet = Intersects(vRayOrigin, vRayDir, p0, p2, p3, fDist);

            if (bRet)
            {

                _vector vColPos = vRayOrigin + vRayDir * fDist;

                int a = 10;

            }
        }
    }

    return bRet;
}

CTool_Terrain* CTool_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTool_Terrain* pInstance = new CTool_Terrain(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CTool_Terrain");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTool_Terrain::Clone(void* pArg)
{
    CTool_Terrain* pInstance = new CTool_Terrain(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CTool_Terrain");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTool_Terrain::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
