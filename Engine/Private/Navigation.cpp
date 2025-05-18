#include "Navigation.h"
#include "Cell.h"
#include "GameInstance.h"
#include "Shader.h"
#include "GlobalFuncs.h"

_float4x4 CNavigation::m_WorldMatrix = {};

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : 
    CComponent(pDevice, pContext)
{

}

CNavigation::CNavigation(const CNavigation& Prototype)
    :
    CComponent(Prototype),
    m_Cells(Prototype.m_Cells)
#ifdef _DEBUG
    ,
    m_pShader(Prototype.m_pShader)
#endif
{

#ifdef _DEBUG
    SafeAddRef(m_pShader);
#endif

    for (auto& pCell : m_Cells)
        SafeAddRef(pCell);
}

HRESULT CNavigation::InitializePrototype(const _tchar* pNavigationDataFile)
{
    _ulong dwByte = {};
    HANDLE hFile = CreateFile(pNavigationDataFile, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (0 == hFile)
        return E_FAIL;

    while (true)
    {
        _vector     vPointsTemp[3] = {};
        _float3     vPoints[3] = {};


        ReadFile(hFile, &vPointsTemp, sizeof(_vector) * 3, &dwByte, nullptr);
        
        XMStoreFloat3(&vPoints[0], vPointsTemp[0]);
        XMStoreFloat3(&vPoints[1], vPointsTemp[1]);
        XMStoreFloat3(&vPoints[2], vPointsTemp[2]);

        if (0 == dwByte)
            break;

        CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, static_cast<_int>(m_Cells.size()));

        if (nullptr == pCell)
            return E_FAIL;

        m_Cells.push_back(pCell);
    }

    CloseHandle(hFile);

    if (FAILED(SetUpNeighbors()))
        return E_FAIL;


#ifdef _DEBUG

    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../../EngineSDK/hlsl/ShaderCell.hlsl"), VTX_POS::Elements, VTX_POS::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;

#endif

    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

    return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return S_OK;

    NAVIGATION_DESC* pDesc = static_cast<NAVIGATION_DESC*>(pArg);
    m_iCurrentCellIndex = pDesc->iStartIndex;

    if (-1 == m_iCurrentCellIndex)
    {
        _vector vWorldPos = XMLoadFloat3(&pDesc->vWorldPos);
        _vector vCheckLocalPos = XMVector3TransformCoord(vWorldPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));

        m_iCurrentCellIndex = FindCellIndex(vCheckLocalPos);
        if (-1 == m_iCurrentCellIndex)
            return E_FAIL;
    }


    return S_OK;
}

void CNavigation::Update(_fmatrix WorldMatrix)
{
    XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
}

_bool CNavigation::IsInCell(_fvector vWorldPos, _fvector vDstworldDir, _float3& vSlidingDir)
{
    // 모든 셀들을 월드로 보내는 작업보다 특정 월드 좌표를 지형의 로컬로 보내서 셀안인지 아닌지를 판단하는게 연산이 더 적기 때문에 매개변수 vWorldPos를 로컬로 변환 시켜 계산해준다.
    // - 현재 m_WorldMatrix는 항등 행렬이다.
    _vector vCheckLocalPos = XMVector3TransformCoord(vWorldPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));

    // vLocalDstDir는 현재 플레이어의 이동방향 벡터이다.
    _vector vLocalDstDir = XMVector3TransformNormal(vDstworldDir, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));
    vLocalDstDir = XMVector3Normalize(vLocalDstDir);

    _int iNeighborCellIndex = -1;
    _float3 vCellNormal;

    // 셀안에 있다면 true를 그냥 바로 반환한다. 
    if (m_Cells[m_iCurrentCellIndex]->IsInCell(vCheckLocalPos, iNeighborCellIndex, vCellNormal))
        return true;
    // 안에 있던 셀에서 밖으로 나간 경우 처리 & 예외 처리를 진행해준다.
    else
    {
        _vector vCurrentCellNormal = XMLoadFloat3(&vCellNormal);
        vCurrentCellNormal = XMVector3Normalize(vCurrentCellNormal);

        _float  fRad = (XMVectorGetX(XMVector3Dot(vLocalDstDir, vCurrentCellNormal)));      // Look벡터와 삼각형 라인의 법선 벡터와 내적한 결과값
        _vector vCellNormalVector = vCurrentCellNormal * fRad;                              // vLookDir에 수선의 발을 내린 벡터

        // '플레이어 이동방향 벡터' - '삼각형 한 면의 수직인 정사영된 법선벡터' = '슬라이딩 벡터'가 나온다.
        _vector vSlide = XMVector3Normalize(vLocalDstDir - vCellNormalVector);              // 슬라이딩 벡터

        // 로컬 상의 슬라이딩 벡터를 월드로 변경한다.
        XMVector3TransformNormal(vSlide, XMLoadFloat4x4(&m_WorldMatrix));
        vSlide = XMVector3Normalize(vSlide);
        XMStoreFloat3(&vSlidingDir, vSlide);

        // vWorldPos에 이웃이 없다면 갈 수 없으므로 false를 반환한다.
        // - 이미 위에서 IsInCell를 호출하면서 iNeighborCellIndex는 -1이나 현재 셀의 이웃한 셀의 인덱스가 채워져 있을 것이라 예상이 가능하다.
        if (-1 == iNeighborCellIndex)
        {
            return false;
        }
        // 만약 나간 방향에 이웃이 있는 경우
        else
        {
            // 아래처럼 처리를 해버리게 되면 현재 틱에 정확한 이웃을 찾을 수 없다(다음 틱에 찾을 순 있긴 하다)
            // 하지만 셀이 촘촘하고 플레이어의 이동속도가 큰 경우 다음틱에 셀을 도 빠져나갔다면 이웃한 셀을 영구적으로 찾을 수 없는 문제가 발생한다.
            while (true)
            {
                if (-1 == iNeighborCellIndex)
                    return false;

                if (m_Cells[iNeighborCellIndex]->IsInCell(vCheckLocalPos, iNeighborCellIndex, vCellNormal))
                {
                    m_iCurrentCellIndex = iNeighborCellIndex;
                    return true;
                }
            }
            return true;
        }
    }

    return false;
}

_bool CNavigation::IsInCell(_fvector vWorldPos)
{
    _vector vCheckLocalPos = XMVector3TransformCoord(vWorldPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));
    _int iNeighborCellIndex = -1;

    if (m_Cells[m_iCurrentCellIndex]->IsInCell(vCheckLocalPos, iNeighborCellIndex))
        return true;
    else
    {
        // 나간 방향에 이웃이 없다면 갈 수 없으므로 false를 반환한다.
        // - 이미 위에서 IsInCell를 호출하면서 iNeighborCellIndex는 -1이나 현재 셀의 이웃한 셀의 인덱스가 채워져 있을 것이라 예상이 가능하다.
        if (-1 == iNeighborCellIndex)
        {
            return false;
        }
        // 만약 나간 방향에 이웃이 있는 경우
        else
        {
            while (true)
            {
                if (-1 == iNeighborCellIndex)
                    return false;

                if (m_Cells[iNeighborCellIndex]->IsInCell(vCheckLocalPos, iNeighborCellIndex))
                {
                    m_iCurrentCellIndex = iNeighborCellIndex;
                    return true;
                }
            }
            return true;
        }
    }

    // return false;
}

_bool CNavigation::IsInOtherCell(_fvector vWorldPos, _fvector vWorldSlidingDir, _float3& vResultDir)
{
    _float3 vCheckLocalPos;
    XMStoreFloat3(&vCheckLocalPos, XMVector3TransformCoord(vWorldPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix))));
    _int iNeighborCellIndex = -1;
    vector<CCell*> ClosetPointCells;
    _int iCurCellIndex = GetCurrentCellIndex();

    for (auto& pCell : m_Cells)
    {
        if (pCell->GetCellIndex() == iCurCellIndex)
            continue;

        _float3 vPointA = pCell->GetPoint(CCell::POINT_A);
        _float3 vPointB = pCell->GetPoint(CCell::POINT_B);
        _float3 vPointC = pCell->GetPoint(CCell::POINT_C);

        if (AlmostEqual(vCheckLocalPos.x, vPointA.x, 0.1f) && AlmostEqual(vCheckLocalPos.y, vPointA.y, 0.1f), AlmostEqual(vCheckLocalPos.z, vPointA.z, 0.1f))
            ClosetPointCells.push_back(pCell);
        else if (AlmostEqual(vCheckLocalPos.x, vPointB.x, 0.1f) && AlmostEqual(vCheckLocalPos.y, vPointB.y, 0.1f), AlmostEqual(vCheckLocalPos.z, vPointB.z, 0.1f))
            ClosetPointCells.push_back(pCell);
        else if (AlmostEqual(vCheckLocalPos.x, vPointC.x, 0.1f) && AlmostEqual(vCheckLocalPos.y, vPointC.y, 0.1f), AlmostEqual(vCheckLocalPos.z, vPointC.z, 0.1f))
            ClosetPointCells.push_back(pCell);
    }

    vector<pair<_float3, _int>> FarPoints;
    _vector vSrc = XMVector3TransformCoord(vWorldPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));

    for (auto& pSelecetedCell : ClosetPointCells)
    {
        _float  fMaxDis = -1.f;

        _float3 vPointA = pSelecetedCell->GetPoint(CCell::POINT_A);
        _float3 vPointB = pSelecetedCell->GetPoint(CCell::POINT_B);
        _float3 vPointC = pSelecetedCell->GetPoint(CCell::POINT_C);

        _vector vpointA = XMLoadFloat3(&vPointA);
        _vector vpointB = XMLoadFloat3(&vPointB);
        _vector vpointC = XMLoadFloat3(&vPointC);

        _float faDist = fabs(XMVectorGetX(XMVector3Length(vpointA - vSrc)));
        _float fbDist = fabs(XMVectorGetX(XMVector3Length(vpointB - vSrc)));
        _float fcDist = fabs(XMVectorGetX(XMVector3Length(vpointC - vSrc)));
        
        if (faDist > fbDist && faDist > fcDist)
        {
            FarPoints.push_back({ vPointA, pSelecetedCell->GetCellIndex()});
        }
        else if (fbDist > faDist && fbDist > fcDist)
        {
            FarPoints.push_back({ vPointB, pSelecetedCell->GetCellIndex() });
        }
        else if (fcDist > faDist && fcDist > fbDist)
        {
            FarPoints.push_back({ vPointC , pSelecetedCell->GetCellIndex() });
        }
    }
    
    _vector vLocalSlidingDir = XMVector3TransformCoord(vWorldSlidingDir, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));
    vLocalSlidingDir = XMVector3Normalize(vLocalSlidingDir);
    _float fMinDot = 1.f;
    _vector vRetDir = {0.f, 0.f, 0.f};
    _int iRetIndex = 0;

    for (auto vFarPos : FarPoints)
    {
        vFarPos.first.y = 0.f;
        vSrc.m128_f32[1] = 0.f;

        _vector vFar = XMLoadFloat3(&vFarPos.first);
        _vector vDst = XMVector3Normalize(vFar - vSrc);
        
        _float fDot = fabs(XMVectorGetX(XMVector3Dot(vLocalSlidingDir, vDst)));

        if (fMinDot > fDot)
        {
            fMinDot = fDot;
            vRetDir = vDst;
            iRetIndex = vFarPos.second;
        }
    }
    
    if (XMVector3Equal(vRetDir, _vector{ 0.f, 0.f, 0.f}))
        return false;

    XMStoreFloat3(&vResultDir, vRetDir);
    m_iCurrentCellIndex = iRetIndex;

    return true;
}

_vector CNavigation::SetUpHeightOnNavigation(_fvector vPosition)
{
    _vector vCheckLocalPos = XMVector3TransformCoord(vPosition, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));
    vCheckLocalPos = XMVectorSetY(vPosition, ComputeCellHeight(m_iCurrentCellIndex, vCheckLocalPos));
    return XMVector3TransformCoord(vCheckLocalPos, XMLoadFloat4x4(&m_WorldMatrix));
}

_int CNavigation::FindCellIndex(_fvector vPos)
{
    _vector vCheckLocalPos = XMVector3TransformCoord(vPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));
    _int iCurCellIndex = -1;

    for (auto& pCell : m_Cells)
    {
        m_iCurrentCellIndex = pCell->IsInCell(vCheckLocalPos);
        if (-1 == m_iCurrentCellIndex)
            continue;
        
        break;
    }

    return m_iCurrentCellIndex;
}

HRESULT CNavigation::SetUpNeighbors()
{
    // 이중 루프를 돌면서 이웃이 되는 셀을 찾아준다.
    // 로딩간에 사용할 것이라 부담이 되는 작업은 아니다.
    for (auto& pSrc : m_Cells)
    {
        for (auto& pDst : m_Cells)
        {
            // 자기 자신과 일치한다면 건너뛴다.
            if (pSrc == pDst)
                continue;

            if (pDst->ComparePoints(pSrc->GetPoint(CCell::POINT_A), pSrc->GetPoint(CCell::POINT_B)))
                pSrc->SetNeighbor(CCell::LINE_AB, pDst);
            else if (pDst->ComparePoints(pSrc->GetPoint(CCell::POINT_B), pSrc->GetPoint(CCell::POINT_C)))
                pSrc->SetNeighbor(CCell::LINE_BC, pDst);
            else if (pDst->ComparePoints(pSrc->GetPoint(CCell::POINT_C), pSrc->GetPoint(CCell::POINT_A)))
                pSrc->SetNeighbor(CCell::LINE_CA, pDst);
        }
    }

    return S_OK;
}

_float CNavigation::ComputeCellHeight(_int iCellIndex, _fvector vPosition)
{
    if (-1 == iCellIndex)
        return 0.f;
    
    return m_Cells[iCellIndex]->ComputeCellHeight(vPosition);
}


#ifdef _DEBUG

HRESULT CNavigation::Render()
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

    for (auto& pCell : m_Cells)
        if (nullptr != pCell)
            pCell->Render();

    return S_OK;
}

#endif


CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFile)
{
    CNavigation* pInstance = new CNavigation(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype(pNavigationDataFile)))
    {
        MSG_BOX("Failed to Created : CNavigation");
        SafeRelease(pInstance);
    }
    return pInstance;
}

CComponent* CNavigation::Clone(void* pArg)
{
    CNavigation* pInstance = new CNavigation(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CNavigation");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CNavigation::Free()
{
    __super::Free();

#ifdef _DEBUG

    SafeRelease(m_pShader);

#endif

    for (auto& pCell : m_Cells)
        SafeRelease(pCell);

    m_Cells.clear();
}
