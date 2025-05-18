#include "Converter.h"

IMPLEMENT_SINGLETON(CConverter)

CConverter::CConverter()
{

}

void CConverter::ExportAssetFile(const _char* pModelFilePath, const _char* pModelExportPath, CModel::TYPE eModelType, _fmatrix PreTransformMatrix)
{
	_uint   iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };

	if (eModelType != CModel::TYPE_ANIM)
		iFlag |= aiProcess_PreTransformVertices;

	// 읽는다.
	m_pAIScene = m_pImporter.ReadFile(pModelFilePath, iFlag);

    if (0 == m_pAIScene)
        return;

    ofstream OutFile(pModelExportPath, ios::out | ios_base::binary);
    if (!OutFile.is_open())
        return;
    
    // 뼈 부터 정보를 써준다.
    if (eModelType == CModel::TYPE_ANIM)
    {
        // 1. 뼈 정보 쓰기
        {
            aiNode* pAINode = m_pAIScene->mRootNode; // 루트 노드 가져온다. (뼈의 시작점)

            _int    iRootIndex = -1; // 루트 시작 인덱스
            ReadBones(pAINode, iRootIndex, PreTransformMatrix); // 루트 부터 재귀적으로 뼈들의 정보를 읽어서 써준다.

            // 1-1. 전체 뼈 개수 쓰기
            _int iLenBone = static_cast<_int>(m_BoneInfos.size());
            OutFile.write((_char*)&iLenBone, sizeof(_int));

            // 1-2. 뼈 개수 만큼 뼈 정보 쓰기
            for (_int i = 0; i < iLenBone; ++i)
                OutFile.write((_char*)&m_BoneInfos[i], sizeof(AI_BONE_INFO));

            m_bTrans = false;
        }

        // 2. 애니매이션 정보 쓰기
        {
            // 2-1. 애니매이션 전체 개수 쓰기
            _int iNumAnimations = static_cast<_int>(m_pAIScene->mNumAnimations);
            OutFile.write((_char*)&iNumAnimations, sizeof(_int));

            for (_int i = 0; i < iNumAnimations; ++i)
            {
                aiAnimation* pAIAnimation = m_pAIScene->mAnimations[i];
                if (nullptr == pAIAnimation) 
                    continue;

                // 2-2. i번째 애니매이션 정보 쓰기(이름, 이름길이, duration, tickpersecond, channel개수)
                AI_ANIMATION_INFO vAnimInfo;
                strcpy_s(vAnimInfo.szAnimName, pAIAnimation->mName.data);               
                vAnimInfo.iLenAnimName = static_cast<_int>(strlen(vAnimInfo.szAnimName) + 1);        
                vAnimInfo.fDuration = static_cast<_float>(pAIAnimation->mDuration);                  
                vAnimInfo.fTickPerSecond = static_cast<_float>(pAIAnimation->mTicksPerSecond);       
                vAnimInfo.iNumChannels = static_cast<_int>(pAIAnimation->mNumChannels);
                OutFile.write((_char*)&vAnimInfo, sizeof(AI_ANIMATION_INFO));                       
                
                for (_int iChannelIndex = 0; iChannelIndex < vAnimInfo.iNumChannels; ++iChannelIndex)
                {
                    aiNodeAnim* pAINodeAnim = pAIAnimation->mChannels[iChannelIndex];
                    AI_CHANNEL_INFO vChInfo;

                    // 2-3. i번째 애니매이션의 iChannelIndex번째 채널의 정보 쓰기(이름, 이름길이, 실제 뼈 인덱스, keyframe)
                    strcpy_s(vChInfo.szChannelName, pAINodeAnim->mNodeName.data);
                    vChInfo.iLenChannelName = static_cast<_int>(strlen(vChInfo.szChannelName) + 1);
                    vChInfo.iBoneIndex = Get_BoneIndex(vChInfo.szChannelName);
                    vChInfo.iNumKeyFrame = max(pAINodeAnim->mNumScalingKeys, pAINodeAnim->mNumRotationKeys);
                    vChInfo.iNumKeyFrame = max(vChInfo.iNumKeyFrame, pAINodeAnim->mNumPositionKeys);
                    OutFile.write((_char*)&vChInfo, sizeof(AI_CHANNEL_INFO));
                    
                    _float3		vScale = {}, vPosition = {}; _float4	vRotation = {};

                    // i번째 애니매이션의 iChannelIndex번째 채널의 최대 키프레임 수만큼 루프를 돌며 KEYFRAME을 생성한다.
                    for (_uint iKeyFrameIndex = 0; iKeyFrameIndex < vChInfo.iNumKeyFrame; ++iKeyFrameIndex)
                    {
                        KEYFRAME KeyFrame = {};

                        if (iKeyFrameIndex < pAIAnimation->mChannels[iChannelIndex]->mNumScalingKeys)
                        {
                            memcpy(&vScale, &pAIAnimation->mChannels[iChannelIndex]->mScalingKeys[iKeyFrameIndex].mValue, sizeof(_float3));
                            KeyFrame.vKeyFramePosition = static_cast<_float>(pAIAnimation->mChannels[iChannelIndex]->mScalingKeys[iKeyFrameIndex].mTime);
                        }

                        if (iKeyFrameIndex < pAIAnimation->mChannels[iChannelIndex]->mNumRotationKeys)
                        {
                            vRotation.x = pAIAnimation->mChannels[iChannelIndex]->mRotationKeys[iKeyFrameIndex].mValue.x;   
                            vRotation.y = pAIAnimation->mChannels[iChannelIndex]->mRotationKeys[iKeyFrameIndex].mValue.y;   
                            vRotation.z = pAIAnimation->mChannels[iChannelIndex]->mRotationKeys[iKeyFrameIndex].mValue.z;   
                            vRotation.w = pAIAnimation->mChannels[iChannelIndex]->mRotationKeys[iKeyFrameIndex].mValue.w;   
                            KeyFrame.vKeyFramePosition = static_cast<_float>(pAIAnimation->mChannels[iChannelIndex]->mRotationKeys[iKeyFrameIndex].mTime);
                        }

                        if (iKeyFrameIndex < pAIAnimation->mChannels[iChannelIndex]->mNumPositionKeys)
                        {
                            memcpy(&vPosition, &pAIAnimation->mChannels[iChannelIndex]->mPositionKeys[iKeyFrameIndex].mValue, sizeof(_float3));
                            KeyFrame.vKeyFramePosition = static_cast<_float>(pAIAnimation->mChannels[iChannelIndex]->mPositionKeys[iKeyFrameIndex].mTime);
                        }

                        KeyFrame.vScale = vScale;
                        KeyFrame.vRotation = vRotation;
                        KeyFrame.vPosition = vPosition;

                        // 2-4. i번째 애니매이션의 iChannelIndex번째 채널의 iKeyFrameIndex번째 키프레임 정보 쓰기
                        OutFile.write((_char*)&KeyFrame, sizeof(KEYFRAME));
                    }
                }
            }
        }

        // 3-1. 메쉬의 전채 개수 쓰기
        _int iNumMeshes = static_cast<_int>(m_pAIScene->mNumMeshes);
        OutFile.write((_char*)(&iNumMeshes), sizeof(_int));

        vector<AI_BONEINDEX_OFFSETMATRX> BoneIndicesOffsetMatrcies;

        for (_int iMeshIndex = 0; iMeshIndex < iNumMeshes; ++iMeshIndex)
        {
            const aiMesh* pAIMesh = m_pAIScene->mMeshes[iMeshIndex];

            AI_ANIM_MESH_INFO AnimMeshnfo = {};
            strcpy_s(AnimMeshnfo.szAnimMeshName, pAIMesh->mName.data);
            AnimMeshnfo.iLenAnimMeshName = static_cast<_int>(strlen(AnimMeshnfo.szAnimMeshName) + 1);
            AnimMeshnfo.iAnimMeshMaterialIndex = static_cast<_int>(m_pAIScene->mMeshes[iMeshIndex]->mMaterialIndex);
            AnimMeshnfo.iAnimMeshVertices = static_cast<_int>(pAIMesh->mNumVertices);
            AnimMeshnfo.iAnimMeshNumFaces = static_cast<_int>(pAIMesh->mNumFaces);
            AnimMeshnfo.iNumBones = static_cast<_int>(pAIMesh->mNumBones);

            VTX_ANIMMESH* pVertices = new VTX_ANIMMESH[AnimMeshnfo.iAnimMeshVertices];
            ZeroMemory(pVertices, sizeof(VTX_ANIMMESH) * AnimMeshnfo.iAnimMeshVertices);

            BoneIndicesOffsetMatrcies.clear();

            for (_int k = 0; k < AnimMeshnfo.iNumBones; ++k)
            {
                aiBone* pAIBone = pAIMesh->mBones[k];
                _float4x4	OffsetMatrix = {};
                memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
                XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));
                BoneIndicesOffsetMatrcies.push_back({ Get_BoneIndex(pAIBone->mName.data), OffsetMatrix });      // 전치 해서 저장

                for (_uint l = 0; l < pAIBone->mNumWeights; ++l)
                {
                    if (0 == pVertices[pAIBone->mWeights[l].mVertexId].vBlendWeights.x)
                    {
                        pVertices[pAIBone->mWeights[l].mVertexId].vBlendIndices.x = k;
                        pVertices[pAIBone->mWeights[l].mVertexId].vBlendWeights.x = pAIBone->mWeights[l].mWeight;
                    }
                    else if (0 == pVertices[pAIBone->mWeights[l].mVertexId].vBlendWeights.y)
                    {
                        pVertices[pAIBone->mWeights[l].mVertexId].vBlendIndices.y = k;
                        pVertices[pAIBone->mWeights[l].mVertexId].vBlendWeights.y = pAIBone->mWeights[l].mWeight;
                    }
                    else if (0 == pVertices[pAIBone->mWeights[l].mVertexId].vBlendWeights.z)
                    {
                        pVertices[pAIBone->mWeights[l].mVertexId].vBlendIndices.z = k;
                        pVertices[pAIBone->mWeights[l].mVertexId].vBlendWeights.z = pAIBone->mWeights[l].mWeight;
                    }
                    else if (0 == pVertices[pAIBone->mWeights[l].mVertexId].vBlendWeights.w)
                    {
                        pVertices[pAIBone->mWeights[l].mVertexId].vBlendIndices.w = k;
                        pVertices[pAIBone->mWeights[l].mVertexId].vBlendWeights.w = pAIBone->mWeights[l].mWeight;
                    }
                }
            }

            if (0 == pAIMesh->mNumBones)
            {
                AnimMeshnfo.iNumBones = 1;
                _float4x4 mat = {};
                XMStoreFloat4x4(&mat, XMMatrixIdentity());
                BoneIndicesOffsetMatrcies.push_back(AI_BONEINDEX_OFFSETMATRX{ Get_BoneIndex(AnimMeshnfo.szAnimMeshName), mat });
            }

            // 3-2. 메쉬 정보 쓰기(이름, 이름 길이, 메쉬 머터리얼 인덱스, 정점 개수, Face개수, 뼈 개수)
            OutFile.write((_char*)&AnimMeshnfo, sizeof(AI_ANIM_MESH_INFO));


            // i번째 매쉬의 버텍스 개수 만큼 루프를 돈다.
            for (_int j = 0; j < AnimMeshnfo.iAnimMeshVertices; ++j)
            {
                // 3-3. 메쉬의 정점 정보를 쓴다.
                VTX_ANIMMESH VtxAnimMesh;
                memcpy(&VtxAnimMesh.vPosition, &pAIMesh->mVertices[j], sizeof(_float3));
                memcpy(&VtxAnimMesh.vNormal, &pAIMesh->mNormals[j], sizeof(_float3));
                memcpy(&VtxAnimMesh.vTexcoord, &pAIMesh->mTextureCoords[0][j], sizeof(_float2));
                memcpy(&VtxAnimMesh.vTangent, &pAIMesh->mTangents[j], sizeof(_float3));


                if (0 == pAIMesh->mNumBones)
                {
                    VtxAnimMesh.vBlendIndices = XMUINT4{0, 0, 0, 0};
                    VtxAnimMesh.vBlendWeights = _float4{0.f, 0.f, 0.f, 0.f};
                }
                else
                {
                    VtxAnimMesh.vBlendIndices = pVertices[j].vBlendIndices;
                    VtxAnimMesh.vBlendWeights = pVertices[j].vBlendWeights;
                }

                OutFile.write((_char*)&VtxAnimMesh, sizeof(VTX_ANIMMESH));
            }

            SafeDeleteArray(pVertices);

            for (_uint j = 0; j < pAIMesh->mNumFaces; ++j)
            {
                AI_NONANIM_INDEX vInfo;
                vInfo.iIndex0 = pAIMesh->mFaces[j].mIndices[0];
                vInfo.iIndex1 = pAIMesh->mFaces[j].mIndices[1];
                vInfo.iIndex2 = pAIMesh->mFaces[j].mIndices[2];
                OutFile.write((_char*)&vInfo, sizeof(AI_NONANIM_INDEX));
            }

            // 3-3. 메쉬의 뼈인덱스, OffsetMatrix 정보 쓴다.
            for (_int k = 0; k < AnimMeshnfo.iNumBones; ++k)
                OutFile.write((_char*)&BoneIndicesOffsetMatrcies[k], sizeof(AI_BONEINDEX_OFFSETMATRX));
        }

        // 2. Material 정보 쓰기
        {
            _int iNumMaterials = m_pAIScene->mNumMaterials;
            OutFile.write((_char*)&iNumMaterials, sizeof(_int));                      // 9. material 개수를 쓴다.

            for (_int i = 0; i < iNumMaterials; ++i)
            {
                aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];

                for (_int j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
                {
                    _int   iNumTextures = static_cast<_int>(pAIMaterial->GetTextureCount(aiTextureType(j)));
                    OutFile.write((_char*)&iNumTextures, sizeof(_int));               // 10. 각 텍스쳐에 해당하는 material개수를 쓴다.

                    // 0이 아닌경우 텍스쳐가 존재한다.
                    if (iNumTextures != 0)
                    {
                        for (_int k = 0; k < iNumTextures; ++k)
                        {
                            aiString    strTexturePath;

                            // aiTextureType(j) 타입에 해당하는 k 번째 텍스쳐를 가져오고, 이 경로를 strTexturePath에 담는다.
                            if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), k, &strTexturePath)))
                                break;

                            _char       szDrive[MAX_PATH] = "";         // 드라이브 경로
                            _char       szDirectory[MAX_PATH] = "";     // 디렉토리
                            _char       szFileName[MAX_PATH] = "";      // file 이름
                            _char       szExt[MAX_PATH] = "";           // 확장자
                            _char       szFullPath[MAX_PATH] = "";      // 전체 경로

                            _splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);
                            _splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

                            strcpy_s(szFullPath, szDrive);
                            strcat_s(szFullPath, szDirectory);
                            strcat_s(szFullPath, szFileName);
                            strcat_s(szFullPath, szExt);

                            _tchar		szFinalFullPath[MAX_PATH] = TEXT("");

                            MultiByteToWideChar(CP_ACP, 0, szFullPath, (_int)strlen(szFullPath), szFinalFullPath, MAX_PATH);

                            _int iLen = _int(strlen(szFullPath) + 1); // 널문자 까지

                            _int iPathLen = (MultiByteToWideChar(CP_ACP, 0, szFullPath, (_int)(strlen(szFullPath)), szFinalFullPath, MAX_PATH) + 1) * sizeof(_tchar);//널문자까지

                            OutFile.write((_char*)&iLen, sizeof(_int));                          // 11. 경로의 길이를 쓴다.
                            OutFile.write((_char*)szFullPath, sizeof(_char) * iLen);                  // 12. 경로 쓴다.
                        }
                    }
                }
            }
        }
    }
    else
    {
        _int iNumMeshes = m_pAIScene->mNumMeshes;
        OutFile.write((char*)(&iNumMeshes), sizeof(_int));                         // 1. 메쉬의 개수 쓰기

        // 1. 메쉬 정보 쓰기
        {
            for (_int i = 0; i < iNumMeshes; ++i)
            {
                _char			szMeshName[MAX_PATH] = "";
                const aiMesh* pAIMesh = m_pAIScene->mMeshes[i];
                strcpy_s(szMeshName, pAIMesh->mName.data);
                _int LenMeshName = static_cast<_int>(strlen(szMeshName) + 1);        // 널문자 포함

                _int iMeshMaterialIndex = m_pAIScene->mMeshes[i]->mMaterialIndex;

                OutFile.write((char*)&LenMeshName, sizeof(_int));                       // 2. 메쉬의 이름의 길이 쓰기
                OutFile.write((char*)szMeshName, sizeof(char) * LenMeshName);           // 3. 메쉬의 이름 쓰기

                OutFile.write((char*)&iMeshMaterialIndex, sizeof(_int));                // 4. 메쉬의 material 인덱스 쓰기

                OutFile.write((char*)&pAIMesh->mNumVertices, sizeof(_int));             // 5. 메쉬의 정점 개수 쓰기
                OutFile.write((char*)&pAIMesh->mNumFaces, sizeof(_int));                // 6. 메쉬의 면의 개수 쓰기(나중에읽을떄*3)

                for (_int j = 0; j < (_int)pAIMesh->mNumVertices; ++j)
                {
                    AI_NONANIM_VTX vInfo;
                    memcpy(&vInfo.vPosition, &pAIMesh->mVertices[j], sizeof(_float3));
                    memcpy(&vInfo.vNormal, &pAIMesh->mNormals[j], sizeof(_float3));
                    memcpy(&vInfo.vTexcoord, &pAIMesh->mTextureCoords[0][j], sizeof(_float2));
                    memcpy(&vInfo.vTangent, &pAIMesh->mTangents[j], sizeof(_float3));

                    XMStoreFloat3(&vInfo.vPosition, XMVector3TransformCoord(XMLoadFloat3(&vInfo.vPosition), PreTransformMatrix));
                    XMStoreFloat3(&vInfo.vNormal, XMVector3TransformNormal(XMLoadFloat3(&vInfo.vNormal), PreTransformMatrix));
                    XMStoreFloat3(&vInfo.vTangent, XMVector3TransformNormal(XMLoadFloat3(&vInfo.vTangent), PreTransformMatrix));


                    OutFile.write((char*)&vInfo, sizeof(AI_NONANIM_VTX));               // 7. 메쉬의 정점 정보를 쓴다.

                }

                for (_int j = 0; j < (_int)pAIMesh->mNumFaces; ++j)
                {
                    AI_NONANIM_INDEX vInfo;
                    vInfo.iIndex0 = pAIMesh->mFaces[j].mIndices[0];
                    vInfo.iIndex1 = pAIMesh->mFaces[j].mIndices[1];
                    vInfo.iIndex2 = pAIMesh->mFaces[j].mIndices[2];
                    OutFile.write((char*)&vInfo, sizeof(AI_NONANIM_INDEX));           // 8. 메쉬의 인덱스 정보를 쓴다.
                }
            }
        }


        // 2. Material 정보 쓰기
        {
            _int iNumMaterials = m_pAIScene->mNumMaterials;
            OutFile.write((char*)&iNumMaterials, sizeof(_int));                      // 9. material 개수를 쓴다.

            for (_int i = 0; i < iNumMaterials; ++i)
            {
                aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];

                for (_int j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
                {
                    _int   iNumTextures = pAIMaterial->GetTextureCount(aiTextureType(j));
                    OutFile.write((char*)&iNumTextures, sizeof(_int));               // 10. 각 텍스쳐에 해당하는 material개수를 쓴다.

                    // 0이 아닌경우 텍스쳐가 존재한다.
                    if (iNumTextures != 0)
                    {
                        for (_int k = 0; k < iNumTextures; ++k)
                        {
                            aiString    strTexturePath;

                            // aiTextureType(j) 타입에 해당하는 k 번째 텍스쳐를 가져오고, 이 경로를 strTexturePath에 담는다.
                            if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), k, &strTexturePath)))
                                break;

                            _char       szDrive[MAX_PATH] = "";         // 드라이브 경로
                            _char       szDirectory[MAX_PATH] = "";     // 디렉토리
                            _char       szFileName[MAX_PATH] = "";      // file 이름
                            _char       szExt[MAX_PATH] = "";           // 확장자
                            _char       szFullPath[MAX_PATH] = "";      // 전체 경로

                            _splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);
                            _splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

                            strcpy_s(szFullPath, szDrive);
                            strcat_s(szFullPath, szDirectory);
                            strcat_s(szFullPath, szFileName);
                            strcat_s(szFullPath, szExt);

                            _tchar		szFinalFullPath[MAX_PATH] = TEXT("");

                            MultiByteToWideChar(CP_ACP, 0, szFullPath, (_int)strlen(szFullPath), szFinalFullPath, MAX_PATH);

                            _int iLen = _int(strlen(szFullPath) + 1); // 널문자 까지

                            _int iPathLen = (MultiByteToWideChar(CP_ACP, 0, szFullPath, (_int)(strlen(szFullPath)), szFinalFullPath, MAX_PATH) + 1) * sizeof(wchar_t);//널문자까지

                            OutFile.write((char*)&iLen, sizeof(_int));                          // 11. 경로의 길이를 쓴다.
                            OutFile.write((char*)szFullPath, sizeof(char) * iLen);                  // 12. 경로 쓴다.
                        }
                    }
                }
            }
        }
    }
   
    OutFile.close();
}

void CConverter::ExportAssetBonesToXML(const _char* pModelFilePath, const _char* pModelExportPath, CModel::TYPE eModelType)
{
    _uint   iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };

    if (eModelType != CModel::TYPE_ANIM)
        iFlag |= aiProcess_PreTransformVertices;

    // 읽는다.
    m_pAIScene = m_pImporter.ReadFile(pModelFilePath, iFlag);

    if (eModelType == CModel::TYPE_ANIM)
    {
        m_pDocBone = new tinyxml2::XMLDocument;

        tinyxml2::XMLDeclaration* decl = m_pDocBone->NewDeclaration();
        m_pDocBone->LinkEndChild(decl);

        m_pBoneRoot = m_pDocBone->NewElement("Bones");
        m_pDocBone->LinkEndChild(m_pBoneRoot);

        // 1. 뼈 정보 쓰기
        {
            aiNode* pAINode = m_pAIScene->mRootNode;
            ReadBones(m_pBoneRoot, pAINode, -1);
        }

        m_pDocBone->SaveFile(pModelExportPath);

    }
}

void CConverter::ExportAnimToXML(const _char* pModelFilePath, const _char* pModelExportPath)
{
    _uint   iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };
    m_pAIScene = m_pImporter.ReadFile(pModelFilePath, iFlag);

    aiNode* pAINode = m_pAIScene->mRootNode;                                        // 루트 노드 가져온다. (뼈의 시작점)
    _int    iRootIndex = -1;
    _fmatrix PreTransformMatrix = XMMatrixIdentity();
    ReadBones(pAINode, iRootIndex, PreTransformMatrix);

    m_pDocAnim = new tinyxml2::XMLDocument;
    tinyxml2::XMLDeclaration* decl = m_pDocAnim->NewDeclaration();
    m_pDocAnim->LinkEndChild(decl);
    
    m_pRootAnim = m_pDocAnim->NewElement("Animations");
    m_pDocAnim->LinkEndChild(m_pRootAnim);

    tinyxml2::XMLElement* pNode = m_pDocAnim->NewElement("Animation");
    m_pRootAnim->LinkEndChild(pNode);

    for (_int i = 0; i < static_cast<_int>(m_pAIScene->mNumAnimations); ++i)
    {
        tinyxml2::XMLElement* pAnimationElement = nullptr;
        pAnimationElement = m_pDocAnim->NewElement("AnimationInfo");

        pAnimationElement->SetAttribute("Name", m_pAIScene->mAnimations[i]->mName.C_Str());
        pAnimationElement->SetAttribute("TickPerSecond", m_pAIScene->mAnimations[i]->mTicksPerSecond);
        pAnimationElement->SetAttribute("NumChannels", m_pAIScene->mAnimations[i]->mNumChannels);
        pNode->LinkEndChild(pAnimationElement);

        tinyxml2::XMLElement* pChannelRootElement = nullptr;
        pChannelRootElement = m_pDocAnim->NewElement("ChannelsInfo");
        pNode->LinkEndChild(pChannelRootElement);


        for (_int j = 0; j < static_cast<_int>(m_pAIScene->mAnimations[i]->mNumChannels); ++j)
        {
            tinyxml2::XMLElement* pChannelElement = nullptr;
            pChannelElement = m_pDocAnim->NewElement("Channel");

            
            pChannelElement->SetAttribute("ChannelName", m_pAIScene->mAnimations[i]->mChannels[j]->mNodeName.C_Str());
            pChannelElement->SetAttribute("BoneIndex", Get_BoneIndex(m_pAIScene->mAnimations[i]->mChannels[j]->mNodeName.C_Str()));
           

            _int iNumKeyFrames = 0;
            iNumKeyFrames = max(m_pAIScene->mAnimations[i]->mChannels[j]->mNumScalingKeys, m_pAIScene->mAnimations[i]->mChannels[j]->mNumRotationKeys);
            iNumKeyFrames = max(iNumKeyFrames, static_cast<_int>(m_pAIScene->mAnimations[i]->mChannels[j]->mNumPositionKeys));

            pChannelElement->SetAttribute("KeyFrameCount", iNumKeyFrames);
            pChannelRootElement->LinkEndChild(pChannelElement);



             tinyxml2::XMLElement* pKeyFrameRootElement = nullptr;
             pKeyFrameRootElement = m_pDocAnim->NewElement("KeyFramesInfo");
             pChannelElement->LinkEndChild(pKeyFrameRootElement);

        }
    }

    
    m_pDocAnim->SaveFile(pModelExportPath);

}



void CConverter::ReadBones(const aiNode* pAINode, _int iParentIndex, _fmatrix PreTransformMatrix)
{
    AI_BONE_INFO vInfo = {};
    strcpy_s(vInfo.szBoneName, pAINode->mName.data);
    vInfo.iLenBoneName = static_cast<_int>(strlen(vInfo.szBoneName) + 1);
    vInfo.iParentIndex = iParentIndex;
    vInfo.iNumChild = pAINode->mNumChildren;
    memcpy(&vInfo.TransformMatrix, &pAINode->mTransformation, sizeof(_float4x4));
    XMStoreFloat4x4(&vInfo.TransformMatrix, XMMatrixTranspose(XMLoadFloat4x4(&vInfo.TransformMatrix)));
    XMStoreFloat4x4(&vInfo.CombinedMatrix, XMMatrixIdentity());

    _float4 Trans; 
    memcpy(&Trans, (_float4*)&vInfo.TransformMatrix._41, sizeof(_float4));
    
    if (m_bTrans == false && (Trans.x != 0.f || Trans.y != 0.f || Trans.z != 0.f))
    {
        _float4 t = _float4{ 0.01f, 0.01f, 0.01f, 1.f };
        // m_bTrans = true;
        memcpy((_float4*)&vInfo.TransformMatrix._41, &t, sizeof(_float4));
    }

    m_BoneInfos.push_back(vInfo);


    array<_char, MAX_PATH> pathArr;
    memcpy(pathArr.data(), vInfo.szBoneName, sizeof(char) * vInfo.iLenBoneName);
    m_Bones.push_back(pathArr);


    _int iNextParentIndex = static_cast<_int>(m_Bones.size() - 1);
    for (_int i = 0; i < vInfo.iNumChild; ++i)
        ReadBones(pAINode->mChildren[i], iNextParentIndex, PreTransformMatrix);
}

void CConverter::ReadBones(tinyxml2::XMLElement* pParent, const aiNode* pAINode, _int iParentIndex)
{
    tinyxml2::XMLElement* pBone = m_pDocBone->NewElement("Bone");
    pBone->SetAttribute("Name", pAINode->mName.C_Str());
    pBone->SetAttribute("ParentIndex", iParentIndex);
    pBone->SetAttribute("NumChildren", pAINode->mNumChildren);
    pParent->LinkEndChild(pBone); // 부모 노드에 현재 뼈를 추가

    m_BonesSizeCheck.push_back(0);
    _int iNextParentIndex = static_cast<_int>(m_BonesSizeCheck.size() - 1);

    for (_int i = 0; i < static_cast<_int>(pAINode->mNumChildren); ++i)
        ReadBones(pBone, pAINode->mChildren[i], iNextParentIndex);
}

_int CConverter::Get_BoneIndex(const _char* pBoneName)
{
    _int iBoneIndex = 0;

    auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](const array<_char, MAX_PATH>& arr)->_bool
        {
            if (false == strcmp(arr.data(), pBoneName))
                return true;

            ++iBoneIndex;

            return false;
        });


    if (iter == m_Bones.end())
    {
        MSG_BOX("없어");
        return 99999999;
    }

    return iBoneIndex;
}


void CConverter::Free()
{
	__super::Free();


    delete m_pDocBone;
    delete m_pDocAnim;
}
