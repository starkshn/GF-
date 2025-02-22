#include "QuadTree.h"
#include "GameInstance.h"

CQuadTree::CQuadTree()
	:
	m_pGameInst(CGameInstance::GetInstance())
{
	SafeAddRef(m_pGameInst);
}

HRESULT CQuadTree::Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	m_iCorners[COR_LT] = iLT;
	m_iCorners[COR_RT] = iRT;
	m_iCorners[COR_RB] = iRB;
	m_iCorners[COR_LB] = iLB;

	// 가장 마지막으로 옴
	if (1 == m_iCorners[COR_RT] - m_iCorners[COR_LT])
		return S_OK;

	// 중심은 (LT + RB) / 2
	m_iCenter = (m_iCorners[COR_LT] + m_iCorners[COR_RB]) >> 1;

	_uint iLC, iTC, iRC, iBC; // LeftCenter, TopCenter, RightCenter, BottomCenter
	iLC = (m_iCorners[COR_LT] + m_iCorners[COR_LB]) >> 1;
	iTC = (m_iCorners[COR_LT] + m_iCorners[COR_RT]) >> 1;
	iRC = (m_iCorners[COR_RT] + m_iCorners[COR_RB]) >> 1;
	iBC = (m_iCorners[COR_LB] + m_iCorners[COR_RB]) >> 1;

	m_pChildren[COR_LT] = CQuadTree::Create(m_iCorners[COR_LT], iTC, m_iCenter, iLC);
	m_pChildren[COR_RT] = CQuadTree::Create(iTC, m_iCorners[COR_RT], iRC, m_iCenter);
	m_pChildren[COR_RB] = CQuadTree::Create(m_iCenter, iRC, m_iCorners[COR_RB], iBC);
	m_pChildren[COR_LB] = CQuadTree::Create(iLC, m_iCenter, iBC, m_iCorners[COR_LB]);

	return S_OK;
}

void CQuadTree::MakeNeighbors()
{
}

void CQuadTree::Culling(const _float3* pVertexPositions, _uint* pIndices, _uint* pNumIndices, _fmatrix WorldMatrix)
{
	if (nullptr == m_pChildren[COR_LT])
	{
		_uint		iIndices[] = {
			m_iCorners[COR_LT],
			m_iCorners[COR_RT],
			m_iCorners[COR_RB],
			m_iCorners[COR_LB],
		};

		_bool		isIn[4] = {
			m_pGameInst->IsInFrustumLocalSpace(XMLoadFloat3(&pVertexPositions[iIndices[0]])),
			m_pGameInst->IsInFrustumLocalSpace(XMLoadFloat3(&pVertexPositions[iIndices[1]])),
			m_pGameInst->IsInFrustumLocalSpace(XMLoadFloat3(&pVertexPositions[iIndices[2]])),
			m_pGameInst->IsInFrustumLocalSpace(XMLoadFloat3(&pVertexPositions[iIndices[3]]))
		};

		if (true == isIn[0] &&
			true == isIn[1] &&
			true == isIn[2])
		{
			pIndices[(*pNumIndices)++] = iIndices[0];
			pIndices[(*pNumIndices)++] = iIndices[1];
			pIndices[(*pNumIndices)++] = iIndices[2];
		}

		if (true == isIn[0] &&
			true == isIn[2] &&
			true == isIn[3])
		{
			pIndices[(*pNumIndices)++] = iIndices[0];
			pIndices[(*pNumIndices)++] = iIndices[2];
			pIndices[(*pNumIndices)++] = iIndices[3];
		}


		return;
	}


	_float		fRange = XMVectorGetX(XMVector3Length(XMLoadFloat3(&pVertexPositions[m_iCorners[COR_LT]]) - XMLoadFloat3(&pVertexPositions[m_iCenter])));

	if (true == m_pGameInst->IsInFrustumLocalSpace(XMLoadFloat3(&pVertexPositions[m_iCenter]), fRange))
	{
		for (size_t i = 0; i < COR_END; i++)
		{
			if (nullptr != m_pChildren[i])
				m_pChildren[i]->Culling(pVertexPositions, pIndices, pNumIndices, WorldMatrix);
		}
	}
}

_bool CQuadTree::IsDraw(const _float3* pVertexPos, _fmatrix WorldMatrixInv)
{
	return false;
}

CQuadTree* CQuadTree::Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	CQuadTree* pInstance = new CQuadTree();

	if (FAILED(pInstance->Initialize(iLT, iRT, iRB, iLB)))
	{
		MSG_BOX("Failed to Created : CQuadTree");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CQuadTree::Free()
{
	__super::Free();

	for (auto& pChild : m_pChildren)
		SafeRelease(pChild);

	SafeRelease(m_pGameInst);
}
