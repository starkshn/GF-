#include "Bone.h"

CBone::CBone(ID3D11Device* pDevivce, ID3D11DeviceContext* pContext)
{
	SafeAddRef(pDevivce);
	SafeAddRef(pContext);
}

CBone::CBone()
{
	
}

HRESULT CBone::Initialize(const aiNode* pAINode, _int iParentBoneIndex)
{
	// 현재 본의 이름을 저장해둔다.
	strcpy_s(m_szBoneName, pAINode->mName.data);

	// 각 본들의 초기 행렬들을 구성해준다.
	memcpy(&m_TransformaionMatrix, &pAINode->mTransformation, sizeof(_float4x4));

	// Assimp에서 초기 행렬은 col-major롤 들어가있으니 이를 전치시켜서 다시 m_TransformaionMatrix(로컬 변환 행렬)을 구성한다.
	XMStoreFloat4x4(&m_TransformaionMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformaionMatrix)));
	XMStoreFloat4x4(&m_CombineTransfomationMatrix, XMMatrixIdentity());

	m_iParentBoneIndex = iParentBoneIndex;

	return S_OK;
}

// .dat로 부터 데이터 읽는경우 초기화 함수
HRESULT CBone::Initialize(const _char* szBoneName, _int iParentIndex, _fmatrix TransformMatrix, _fmatrix CombinedMatrix)
{
	strcpy_s(m_szBoneName, szBoneName);

	XMStoreFloat4x4(&m_TransformaionMatrix, TransformMatrix);			// 저장된 행렬을 바로 저장
	XMStoreFloat4x4(&m_CombineTransfomationMatrix, CombinedMatrix);		// 아마 항등임
	m_iParentBoneIndex = iParentIndex;
	return S_OK;	
}

void CBone::UpdateCombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix)
{
	if (-1 == m_iParentBoneIndex)
	{
		XMStoreFloat4x4(&m_CombineTransfomationMatrix, XMLoadFloat4x4(&m_TransformaionMatrix) * PreTransformMatrix);
		return;
	}

	XMStoreFloat4x4(&m_CombineTransfomationMatrix, XMLoadFloat4x4(&m_TransformaionMatrix) * XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombineTransfomationMatrix));
}

void CBone::SetTransformMatrixRightVector(_fvector vRight)
{
	_float4 vRightVector;
	XMStoreFloat4(&vRightVector, vRight);
	memcpy(&m_TransformaionMatrix._11, &vRightVector, sizeof(_float4));
}


CBone* CBone::Create(ID3D11Device* pDevivce, ID3D11DeviceContext* pContext, const aiNode* pAINode, _int iParentBoneIndex)
{
	CBone* pInstance = new CBone(pDevivce, pContext);

	if (FAILED(pInstance->Initialize(pAINode, iParentBoneIndex)))
	{
		MSG_BOX("Failed to Created : CBone");
		SafeRelease(pInstance);
	}
	return pInstance;
}


CBone* CBone::Create(const _char* szBoneName, _int iParentIndex, _fmatrix TransformMatrix, _fmatrix CombinedMatrix)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(szBoneName, iParentIndex, TransformMatrix, CombinedMatrix)))
	{
		MSG_BOX("Failed to Created : CBone");
		SafeRelease(pInstance);
	}
	return pInstance;
}


CBone* CBone::Clone()
{
	return new CBone(*this);
}

void CBone::Free()
{
	__super::Free();
}
