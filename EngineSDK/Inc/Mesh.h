#pragma once
#include "VIBuffer.h"
#include "Model.h"
#include "Bone.h"
#include "Shader.h"

BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMesh() = default;

public:
	virtual HRESULT			InitializePrototype(CModel::TYPE eModelType, CModel* pModel, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);

	// NONANIM 초기화
	virtual HRESULT			InitializePrototype(const char* pMeshName, _int iMeshMaterialIndex, const vector<AI_NONANIM_VTX>& vecVtxInfos, const vector<AI_NONANIM_INDEX>& vecIndixesInfo);

	// ANIM 초기화
	virtual HRESULT			InitializePrototype(_fmatrix PreTransformMatrix, AI_ANIM_MESH_INFO AnimMeshInfo, vector<VTX_ANIMMESH>& vecVtxAnimMesh, vector<AI_NONANIM_INDEX>& vecAnimMeshIndex, vector<AI_BONEINDEX_OFFSETMATRX>& vecBoneIndexOffsetMatrix);


	virtual HRESULT			Initialize(void* pArg);


public:
	HRESULT					ReadyVertexBufferForNonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT					ReadyVertexBufferForAnim(const aiMesh* pAIMesh, CModel* pModel);

public:
	HRESULT					BindBoneMatrices(CShader* pShader, const _char* pConstantName, const vector<CBone*>& Bones);


public:
	// 내가 사용해야할 material 인덱스 리턴
	_uint					GetMaterialIndex() const { return m_iMaterialIndex; }
	void					SetMaterialIndex(_uint iMaterialIndex) { m_iMaterialIndex = iMaterialIndex ; }

	const _char*			GetMeshName() { return m_szName; }

public:
	_vector					FindHitPos(_fvector vOriginPos, _fvector vRayDir, _float fDist, _matrix WorldMatrix, _matrix PreTransformMatrix);


private:
	_char					m_szName[MAX_PATH] = "";
	_uint					m_iMaterialIndex = {};	// 현재 이 Mesh가 사용해야할 material 인덱스 번호
	_uint					m_iNumBones = {};		// 현재 메쉬의 본 개수
	vector<_uint>			m_BoneIndices;			// '이 모델을 구성하는 전체 뼈의 몇번째 뼈들이 이 메시에 영향을 주는거야?' 라는 정보를 저장

	
	_float4x4				m_BoneMatrices[512];	// Shader 행렬에 던져줄 512개의 상태행렬을 멤버 변수로 담아둔다.
	vector<_float4x4>		m_BoneOffsetMatrices;	// 모델은 다른데 같은 애니매이션을 사용해야하는 경우 보간값을 담고 있는 행렬


private:
	VTX_MESH*				m_pVertices = nullptr;
	_uint*					m_pIndices = nullptr;


public:
	static CMesh*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, CModel* pModel, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);

	// NONANIM Create
	static CMesh*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const char* pMeshName, _int iMeshMaterialIndex, const vector<AI_NONANIM_VTX>& vecVtxInfos, const vector<AI_NONANIM_INDEX>& vecFacesInfo);

	// ANIM 초기화
	static CMesh*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _fmatrix PreTransformMatrix, AI_ANIM_MESH_INFO AnimMeshInfo, vector<VTX_ANIMMESH>& vecVtxAnimMesh, vector<AI_NONANIM_INDEX>& vecAnimMeshIndex, vector<AI_BONEINDEX_OFFSETMATRX>& vecBoneIndexOffsetMatrix);

	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
};

END