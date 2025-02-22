#pragma once
#include "Base.h"

// assimp에서는 뼈의 정보를 세가지 타입으로 제공한다.
// aiNode, aiBone, aiAnimLoad

BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone(ID3D11Device* pDevivce, ID3D11DeviceContext* pContext);
	CBone();
	virtual ~CBone() override = default;

public:
	HRESULT				Initialize(const aiNode* pAINode, _int iParentBoneIndex);
	HRESULT				Initialize(const _char* szBoneName, _int iParentIndex, _fmatrix TransformMatrix, _fmatrix CombinedMatrix);

	// 본들의 경우 가장 최 상단의 부모에 PreTransformMatrix를 곱해주면 자식들은 이에 영향을 받아 양의 Z축으로 회전해 있을 것이다.
	void				UpdateCombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix);

public:
	const _char*		GetName() const { return m_szBoneName; }
	const _int			GetParentIndex() { return m_iParentBoneIndex; }
	_matrix				GetCombinedTransformationMatrix() const { return XMLoadFloat4x4(&m_CombineTransfomationMatrix); }
	const _float4x4*	GetCombinedTransformMatrixFloat4x4() const { return &m_CombineTransfomationMatrix; }
	void				SetTransformationMatrix(_fmatrix TransformationMatrix) { XMStoreFloat4x4(&m_TransformaionMatrix, TransformationMatrix); }
	void				SetTransformMatrixRightVector(_fvector vRight);

public:
	_float4x4*			GetTransformationMatrix() { return &m_TransformaionMatrix; }

private:
	_char				m_szBoneName[MAX_PATH] = {};			// CBone의 이름(CMesh에서 찾는 기준이 된다)
	_float4x4			m_TransformaionMatrix = {};				// 부모를 기준으로 한 나의 로컬 변환 행렬이다. (== 로컬 변환 행렬)
	_float4x4			m_CombineTransfomationMatrix = {};		// 나의 '로컬 변환 행렬' * '부모의 로컬 변환 행렬'
	_int				m_iParentBoneIndex = -1;				// 부모의 인덱스 번호

public:
	static CBone*		Create(ID3D11Device* pDevivce, ID3D11DeviceContext* pContext, const aiNode* pAINode, _int iParentBoneIndex);
	static CBone*		Create(const _char* szBoneName, _int iParentIndex, _fmatrix TransformMatrix, _fmatrix CombinedMatrix);
	virtual CBone*		Clone();
	virtual void		Free() override;
};

END