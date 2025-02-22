#include "Mesh.h"
#include "Collider.h"
#include "Engine_Defines.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CVIBuffer{ pDevice, pContext }
{

}

HRESULT CMesh::InitializePrototype(CModel::TYPE eModelType, CModel* pModel, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	m_iMaterialIndex = pAIMesh->mMaterialIndex;

	// 메쉬의 이름 복사 (나중에 중요하기 쓰인다)
	strcpy_s(m_szName, pAIMesh->mName.data);

	// 정점 버퍼 정보 셋팅
	m_iNumVertexBuffers = 1;					// 정점 버퍼 개수 선언 및 정의
	m_iNumVertices = pAIMesh->mNumVertices;		// 정점 개수 복사

	// 인덱스 버퍼 정보 셋팅
	m_iIndexStride = sizeof(_uint);				// 정점이 몇개 일지 모르니.
	m_iNumIndices = pAIMesh->mNumFaces * 3;		// 면당 3개의 인덱스
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	HRESULT hr = CModel::TYPE_NONANIM == eModelType ? ReadyVertexBufferForNonAnim(pAIMesh, PreTransformMatrix) : ReadyVertexBufferForAnim(pAIMesh, pModel);
	if (FAILED(hr))
		return E_FAIL;

#pragma endregion


#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 정점버퍼를 몇 바이트 할당할까요? */
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
	{
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[0];
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[1];
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[2];
	}

	ZeroMemory(&m_SubResourceDesc, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceDesc.pSysMem = pIndices;

	if (FAILED(__super::CreateBuffer(&m_pIB)))
		return E_FAIL;

	SafeDeleteArray(pIndices);

#pragma endregion

	return S_OK;
}

// NONANIM의 경우
HRESULT CMesh::InitializePrototype(const char* pMeshName, _int iMeshMaterialIndex, const vector<AI_NONANIM_VTX>& vecVtxInfos, const vector<AI_NONANIM_INDEX>& vecFacesInfo)
{
	
	strcpy_s(m_szName, pMeshName);										// 1. 메쉬 이름 복사.
	m_iMaterialIndex = iMeshMaterialIndex;
	

	// 정점 버퍼 정보 셋팅
	m_iNumVertexBuffers = 1;											// 정점 버퍼 개수 선언 및 정의
	m_iNumVertices = static_cast<_uint>(vecVtxInfos.size());			// 정점 개수 복사

	// 인덱스 버퍼 정보 셋팅
	m_iIndexStride = sizeof(_uint);										// 정점이 몇개 일지 모르니.
	m_iNumIndices = static_cast<_uint>(vecFacesInfo.size() * 3);		// 면당 3개의 인덱스
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	

#pragma region VERTEX_BUFFER

	m_iVertexStride = sizeof(VTX_MESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 정점버퍼를 몇 바이트 할당할까요? */
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTX_MESH* pVertices = new VTX_MESH[m_iNumVertices];
	m_pVertices = new VTX_MESH[m_iNumVertices];

	ZeroMemory(pVertices, sizeof(VTX_MESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		XMStoreFloat3(&pVertices[i].vPosition, XMLoadFloat3(&vecVtxInfos[i].vPosition));
		XMStoreFloat3(&pVertices[i].vNormal, XMLoadFloat3(&vecVtxInfos[i].vNormal));
		XMStoreFloat2(&pVertices[i].vTexcoord, XMLoadFloat2(&vecVtxInfos[i].vTexcoord));
		XMStoreFloat3(&pVertices[i].vTangent, XMLoadFloat3(&vecVtxInfos[i].vTangent));


		XMStoreFloat3(&m_pVertices[i].vPosition, XMLoadFloat3(&vecVtxInfos[i].vPosition));
		XMStoreFloat3(&m_pVertices[i].vNormal, XMLoadFloat3(&vecVtxInfos[i].vNormal));
		XMStoreFloat2(&m_pVertices[i].vTexcoord, XMLoadFloat2(&vecVtxInfos[i].vTexcoord));
		XMStoreFloat3(&m_pVertices[i].vTangent, XMLoadFloat3(&vecVtxInfos[i].vTangent));
	}

	ZeroMemory(&m_SubResourceDesc, sizeof m_SubResourceDesc);
	m_SubResourceDesc.pSysMem = pVertices;

	if (FAILED(__super::CreateBuffer(&m_pVB)))
		return E_FAIL;

	SafeDeleteArray(pVertices);

#pragma endregion
	

#pragma region INDEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 정점버퍼를 몇 바이트 할당할까요? */
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	_uint* pIndices = new _uint[m_iNumIndices];
	m_pIndices = new _uint[m_iNumIndices];

	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	for (size_t i = 0; i < vecFacesInfo.size(); i++)
	{
		pIndices[iNumIndices++] = vecFacesInfo[i].iIndex0;
		m_pIndices[iNumIndices] = vecFacesInfo[i].iIndex0;


		pIndices[iNumIndices++] = vecFacesInfo[i].iIndex1;
		m_pIndices[iNumIndices] = vecFacesInfo[i].iIndex1;


		pIndices[iNumIndices++] = vecFacesInfo[i].iIndex2;
		m_pIndices[iNumIndices] = vecFacesInfo[i].iIndex2;
	}

	ZeroMemory(&m_SubResourceDesc, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceDesc.pSysMem = pIndices;

	if (FAILED(__super::CreateBuffer(&m_pIB)))
		return E_FAIL;

	SafeDeleteArray(pIndices);

#pragma endregion


	return S_OK;
}

// Anim인경우 초가화 함수
HRESULT CMesh::InitializePrototype(_fmatrix PreTransformMatrix, AI_ANIM_MESH_INFO AnimMeshInfo, vector<VTX_ANIMMESH>& vecVtxAnimMesh, vector<AI_NONANIM_INDEX>& vecAnimMeshIndex, vector<AI_BONEINDEX_OFFSETMATRX>& vecBoneIndexOffsetMatrix)
{
	m_iNumBones = AnimMeshInfo.iNumBones;
	m_iMaterialIndex = AnimMeshInfo.iAnimMeshMaterialIndex;

	strcpy_s(m_szName, AnimMeshInfo.szAnimMeshName);								// 1. 메쉬 이름 복사.

	// 정점 버퍼 정보 셋팅
	m_iNumVertexBuffers = 1;														// 정점 버퍼 개수 선언 및 정의
	m_iNumVertices = static_cast<_uint>(AnimMeshInfo.iAnimMeshVertices);			// 정점 개수 복사

	// 인덱스 버퍼 정보 셋팅
	m_iIndexStride = sizeof(_uint);													// 정점이 몇개 일지 모르니.
	m_iNumIndices = static_cast<_uint>(AnimMeshInfo.iAnimMeshNumFaces * 3);			// 면당 3개의 인덱스
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


#pragma region VERTEX_BUFFER

	m_iVertexStride = sizeof(VTX_ANIMMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 정점버퍼를 몇 바이트 할당할까요? */
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTX_ANIMMESH* pVertices = new VTX_ANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTX_ANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		pVertices[i].vPosition = vecVtxAnimMesh[i].vPosition;
		pVertices[i].vNormal = vecVtxAnimMesh[i].vNormal;
		pVertices[i].vTexcoord = vecVtxAnimMesh[i].vTexcoord;
		pVertices[i].vTangent = vecVtxAnimMesh[i].vTangent;
		pVertices[i].vBlendIndices = vecVtxAnimMesh[i].vBlendIndices;
		pVertices[i].vBlendWeights = vecVtxAnimMesh[i].vBlendWeights;
	}

	ZeroMemory(&m_SubResourceDesc, sizeof m_SubResourceDesc);
	m_SubResourceDesc.pSysMem = pVertices;

	if (FAILED(__super::CreateBuffer(&m_pVB)))
		return E_FAIL;

	SafeDeleteArray(pVertices);

#pragma endregion



#pragma region INDEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 정점버퍼를 몇 바이트 할당할까요? */
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);
	
	_uint		iNumIndices = { 0 };

	for (size_t i = 0; i < vecAnimMeshIndex.size(); i++)
	{
		pIndices[iNumIndices++] = vecAnimMeshIndex[i].iIndex0;
		pIndices[iNumIndices++] = vecAnimMeshIndex[i].iIndex1;
		pIndices[iNumIndices++] = vecAnimMeshIndex[i].iIndex2;
	}

	ZeroMemory(&m_SubResourceDesc, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceDesc.pSysMem = pIndices;

	if (FAILED(__super::CreateBuffer(&m_pIB)))
		return E_FAIL;

	SafeDeleteArray(pIndices);

#pragma endregion
	

#pragma region BONEINDEX_OFFSETMATRIX

	for (_int i = 0; i < vecBoneIndexOffsetMatrix.size(); ++i)
	{
		m_BoneIndices.push_back(vecBoneIndexOffsetMatrix[i].iBoneIndex);
		m_BoneOffsetMatrices.push_back(vecBoneIndexOffsetMatrix[i].OffsetMatrix);
	}

#pragma endregion


	return S_OK;
}



HRESULT CMesh::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CMesh::BindBoneMatrices(CShader* pShader, const _char* pConstantName, const vector<CBone*>& Bones)
{
	ZeroMemory(m_BoneMatrices, sizeof(_float4x4) * 512);

	_uint	iNumBones = 0;

	// 현재 m_BoneIndices의 iBoneIndex에 해당하는 이 번호를 i라하자.
	// m_BoneOffsetMatrices 메쉬마다 각 다른 뼈의 정보를 써야할 때 사용한다.
	// 모델이 다른데 같은 애니매이션을 적용하는 경우
	for (auto& iBoneIndex : m_BoneIndices)
	{
		XMStoreFloat4x4(&m_BoneMatrices[iNumBones], XMLoadFloat4x4(&m_BoneOffsetMatrices[iNumBones]) * Bones[iBoneIndex]->GetCombinedTransformationMatrix());
		++iNumBones;
	}
	
	return pShader->BindMatrices(pConstantName, m_BoneMatrices, 512);
}

_vector CMesh::FindHitPos(_fvector vOriginPos, _fvector vRayDir, _float fDist, _matrix WorldMatrix, _matrix PreTransformMatrix)
{
	_float3 vHitPos = { 0.f, 0.f, 0.f };

	_matrix Ret = XMMatrixMultiply(WorldMatrix, PreTransformMatrix);

	for (_uint i = 0; i < m_iNumIndices; i += 3)
	{
		_vector v0 = XMVector3TransformCoord(XMLoadFloat3(&m_pVertices[m_pIndices[i]].vPosition), Ret);
		_vector v1 = XMVector3TransformCoord(XMLoadFloat3(&m_pVertices[m_pIndices[i + 1]].vPosition), Ret);
		_vector v2 = XMVector3TransformCoord(XMLoadFloat3(&m_pVertices[m_pIndices[i + 2]].vPosition), Ret);

		_float distance;
		_float closestDistance = FLT_MAX;

		if (DirectX::TriangleTests::Intersects(vOriginPos, vRayDir, v0, v1, v2, distance))
		{
			if (distance < closestDistance)
			{
				closestDistance = distance;
				_vector hitPos = vOriginPos + distance * vRayDir;
				XMStoreFloat3(&vHitPos, hitPos);
			}
		}
	}


	return XMLoadFloat3(&vHitPos);
}


HRESULT CMesh::ReadyVertexBufferForNonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	m_iVertexStride = sizeof(VTX_MESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 정점버퍼를 몇 바이트 할당할까요? */
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTX_MESH* pVertices = new VTX_MESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTX_MESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		// aiMesh의 Vertixes의 타입과 pVertices의 타입이 다르기 때문에 그냥 memcpy 해버린다.
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix));

		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));

		// 241014 기준으로 아직 배우지 않았지만 vTangent도 방향 벡터이기 때문에 곱해준다.
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vTangent, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), PreTransformMatrix));
	}

	ZeroMemory(&m_SubResourceDesc, sizeof m_SubResourceDesc);
	m_SubResourceDesc.pSysMem = pVertices;

	if (FAILED(__super::CreateBuffer(&m_pVB)))
		return E_FAIL;

	SafeDeleteArray(pVertices);

	return S_OK;
}

HRESULT CMesh::ReadyVertexBufferForAnim(const aiMesh* pAIMesh, CModel* pModel)
{
	m_iVertexStride = sizeof(VTX_ANIMMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 정점버퍼를 몇 바이트 할당할까요? */
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTX_ANIMMESH* pVertices = new VTX_ANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTX_ANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	// vBlendIndices, vBlendWeights의 경우 밖에서 구해주어야한다.
	
	m_iNumBones = pAIMesh->mNumBones; // 이 메시에게 영향을 주는 뼈의 개수

	// aiBone : 이 메시에 영향을 주는 뼈가 어떤 정점에게 얼마나 영향을 주는지에 대한 정보를 가지는지를 가진다.
	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		// 현재 해당 메쉬의(pAIMesh의)mBones의 i번째 aiBone을 가져온다.
		aiBone* pAIBone = pAIMesh->mBones[i];

		// 각 메쉬의 애니매이션을 표현하기 위한 뼈들의 상태를 저장 == OffsetMatrix
		// 또는 다른 모델에서 같은 애니매이션을 사용하기 위해 쓴다.
		_float4x4	OffsetMatrix = {};
		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));		// col-major로 들어가 있기때문에 전치해서 다시 저장해준다.
		m_BoneOffsetMatrices.push_back(OffsetMatrix);

		// 현재 pAIBone(pAIMesh의 i번째 aiBone)의 이름에 해당하는 인덱스를 찾아 m_BoneIndices에 추가해준다.
		// m_BoneIndices는 모델의 실제 본의 인덱스를 가진다.
		m_BoneIndices.push_back(pModel->GetBoneIndex(pAIBone->mName.data));

		// pAIBone->mNumWeights : 이 뼈는 몇개의 정점에게 영향을 주는가?
		// 즉, 영향을 주는 정점 수 만큼 순회한다.
		for (_uint j = 0; j < pAIBone->mNumWeights; ++j)
		{
			// pAIBone->mNumWeights[j] : pAIBone이 영향을 주는 j번째 정점의 정보
			// pAIBone->mNumWeights[j].mVertexId : pAIBone이 영향을 주는 j번째 정점의 인덱스
			// vBlendWeights의 x y z w로 현재 정점의 BlendWeights를 확인하고 채워준다.

			// 현재 pVertices의 pAIBone->mWeights[j].mVertexId번째 정점의 본 인덱스는(vBlendIndices.x는) i다.
			// 현재 pVertixes의 pAIBone->mWeights->mVertexId번째 정점의 가중치(vBlendWeights.x)는 pAIBone->mWeights의 j번째 Weight의 mWeight이다.
			if (0 == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.x = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x = pAIBone->mWeights[j].mWeight;
			}
			else if (0 == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.y = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y = pAIBone->mWeights[j].mWeight;
			}
			else if (0 == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.z = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z = pAIBone->mWeights[j].mWeight;
			}
			else if (0 == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.w = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w = pAIBone->mWeights[j].mWeight;
			}
		}
	}


	// 특정 메쉬는 뼈가 없을 수도 있다.
	// Fiona의 경우 sword라는 메쉬는 뼈가 없어서 원점에 배치되어서 offsetMatrix가 영행렬이 되어 보이지 않는다.
	// 이 경우를 방지?하고 원래 붙어야할 뼈에 붙게 하기 위해서 이름으로 실제 붙을 뼈를 찾고 붙여준다.
	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		m_BoneIndices.push_back(pModel->GetBoneIndex(m_szName));

		_float4x4		OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

		m_BoneOffsetMatrices.push_back(OffsetMatrix);
	}

	ZeroMemory(&m_SubResourceDesc, sizeof m_SubResourceDesc);
	m_SubResourceDesc.pSysMem = pVertices;

	if (FAILED(__super::CreateBuffer(&m_pVB)))
		return E_FAIL;

	SafeDeleteArray(pVertices);

	return S_OK;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, CModel* pModel, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype(eModelType, pModel, pAIMesh, PreTransformMatrix)))
	{
		MSG_BOX("Failed to Created : CMesh");
		SafeRelease(pInstance);
	}
	return pInstance;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const char* pMeshName, _int iMeshMaterialIndex, const vector<AI_NONANIM_VTX>& vecVtxInfos, const vector<AI_NONANIM_INDEX>& vecIndixesInfo)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype(pMeshName, iMeshMaterialIndex, vecVtxInfos, vecIndixesInfo)))
	{
		MSG_BOX("Failed to Created : CMesh");
		SafeRelease(pInstance);
	}
	return pInstance;
}


CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _fmatrix PreTransformMatrix, AI_ANIM_MESH_INFO AnimMeshInfo, vector<VTX_ANIMMESH>& vecVtxAnimMesh, vector<AI_NONANIM_INDEX>& vecAnimMeshIndex, vector<AI_BONEINDEX_OFFSETMATRX>& vecBoneIndexOffsetMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype(PreTransformMatrix, AnimMeshInfo, vecVtxAnimMesh, vecAnimMeshIndex, vecBoneIndexOffsetMatrix)))
	{
		MSG_BOX("Failed to Created : CMesh");
		SafeRelease(pInstance);
	}
	return pInstance;
}

CComponent* CMesh::Clone(void* pArg)
{
	return nullptr;
}

void CMesh::Free()
{
    __super::Free();

	if (nullptr != m_pVertices)
		delete[] m_pVertices;

	if (nullptr != m_pIndices)
		delete[] m_pIndices;
}
