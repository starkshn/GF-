#include "Model.h"
#include "Mesh.h"
#include "Shader.h"
#include "Bone.h"
#include <filesystem>
#include "Channel.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CComponent{ pDevice, pContext }
{
}

CModel::CModel(const CModel& Prototype)
    :
    CComponent(Prototype),
    m_eModelType(Prototype.m_eModelType),
    m_pAIScene(Prototype.m_pAIScene),
    m_iNumMeshes(Prototype.m_iNumMeshes),
    m_Meshes(Prototype.m_Meshes),
    m_iNumMaterials(Prototype.m_iNumMaterials),
    m_Materials(Prototype.m_Materials),
    m_PreTransformMatrix(Prototype.m_PreTransformMatrix),
    // m_Bones(Prototype.m_Bones),  얕은 복사의 문제 때문에 깊은 복사를 진행해주기 위해서 주석처리
    m_iNumAnimations(Prototype.m_iNumAnimations)
    // m_Animations(Prototype.m_Animations) 얕은 복사의 문제 때문에 깊은 복사를 진행해주기 위해서 주석처리
{
    for (auto& Textures : m_Materials)
        for (auto& SRVs : Textures)
            for (auto& pSRV : SRVs)
                SafeAddRef(pSRV);

    // 원형 객체로 부터의 복사 생성자를 호출하여 깊은 복사를 진행해준다.
    // - CBone의 경우 포인터 멤버 변수를 들고있지 않으므로 Default 복사 생성자를 호출하여 멤버 변수들을 얕은 복사 해준다.
    for (auto& pPrototypeBone : Prototype.m_Bones)
        m_Bones.push_back(pPrototypeBone->Clone());

    for (auto& pPrototypeBone : Prototype.m_TempOriginBones)
        m_TempOriginBones.push_back(pPrototypeBone->Clone());

    for (auto& pMesh : m_Meshes)
        SafeAddRef(pMesh);

    // 원형 객체로 부터의 복사 생성자를 호출하여 깊은 복사를 진행해준다.
    // - CAnimation의 경우 모델마다 다른 CAnimation 객체 자체를 가지게 하기 위해서 깊은 복사를 해준다.
    // - CAnimation의 멤버 변수들은 다 얕은 복사를 진행해준다. (CChannel 포함 -> Safe_AddRef)
    for (auto& pAnimation : Prototype.m_Animations)
        m_Animations.push_back(pAnimation->Clone());

    m_strModelName = Prototype.m_strModelName;
}


_uint CModel::GetBoneIndex(const _char* pBoneName) const
{
    _uint iBoneIndex = 0;

    auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
    {
        if (false == strcmp(pBone->GetName(), pBoneName))
            return true;

        ++iBoneIndex;

        return false;
    });


    if (iter == m_Bones.end())
    {
        MSG_BOX("없어");
    }

    return iBoneIndex;
}

const _float4x4* CModel::GetBoneCombinedMatrix(const _char* pTargetBoneName) const
{
    auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
        {
            if (false == strcmp(pBone->GetName(), pTargetBoneName))
                return true;

            return false;
        }
    );

    if (iter == m_Bones.end())
        return nullptr;
    
    return (*iter)->GetCombinedTransformMatrixFloat4x4();
}

CBone* CModel::GetBone(const _char* pTargetBoneName)
{
    auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
        {
            if (false == strcmp(pBone->GetName(), pTargetBoneName))
                return true;

            return false;
        }
    );

    if (iter == m_Bones.end())
        return nullptr;

    return *iter;
}

void CModel::SetTransformationMatriciesFirstTime(_uint iTargetAnimationIndex, _float fTimeDelta)
{
    if (iTargetAnimationIndex >= m_Animations.size())
        return;

    m_Animations[iTargetAnimationIndex]->UpdateTransformationMatrices(m_Bones, false, fTimeDelta);
}

void CModel::SetAnimationName(_uint iAnimIndex, string strName)
{
    if (iAnimIndex >= m_Animations.size())
    {
        MSG_BOX("Anim Index가 범위를 벗어남");
        return;
    }

    m_Animations[iAnimIndex]->SetAnimName(strName);
}

void CModel::SetCurrentAnimationIndex(_int iAnimIndex)
{
    if (iAnimIndex >= m_iNumAnimations || m_Animations[iAnimIndex]->IsUseless())
        return;

    m_iCurrentAnimIndex = iAnimIndex;
}

void CModel::SetInterpolationAnimationIndex(_int iFrom, _int iTo)
{
    // ResetCurrentAnimation();
    m_iInterpolateFromIndex = iFrom;
    m_iInterpolateToIndex = iTo;
    m_bInterpolateAnimation = true;
}

CAnimation* CModel::GetAnimation(_int iAnimIndex)
{
    if (iAnimIndex >= m_Animations.size())
        return nullptr;

    return m_Animations[iAnimIndex];
}

_int CModel::GetNextValidAnimationIndex()
{
    _int iNextValidAnimIndex = m_iCurrentAnimIndex + 1;
    if (iNextValidAnimIndex >= m_Animations.size()) return m_iCurrentAnimIndex;

    while (iNextValidAnimIndex < m_Animations.size())
    {
        if (!m_Animations[iNextValidAnimIndex]->IsUseless()) 
            return iNextValidAnimIndex;
        else ++iNextValidAnimIndex;
    }

    return m_iCurrentAnimIndex;
}

_int CModel::GetPrevValidAnimationIndex()
{
    _int iNextValidAnimIndex = m_iCurrentAnimIndex - 1;
    if (iNextValidAnimIndex < 0) return m_iCurrentAnimIndex;

    while (iNextValidAnimIndex >= 0)
    {
        if (!m_Animations[iNextValidAnimIndex]->IsUseless()) 
            return iNextValidAnimIndex;
        else --iNextValidAnimIndex;
    }

    return m_iCurrentAnimIndex;
}

void CModel::ReleaseLockAnim()
{
    m_bLockAnimation = false;

    for (_int i = 0; i < m_Bones.size(); ++i)
        m_Bones[i]->SetTransformationMatrix(XMLoadFloat4x4(m_TempOriginBones[i]->GetTransformationMatrix()));
}

_vector CModel::FindHitPos(_fvector vOriginPos, _fvector vRayDir, _float fDist, _matrix WorldMatrix)
{
    for (_int i = 0; i < m_Meshes.size(); ++i)
    {
        _vector vHitPos = m_Meshes[i]->FindHitPos(vOriginPos, vRayDir, fDist, WorldMatrix, XMLoadFloat4x4(&m_PreTransformMatrix));

        if (XMVector3Equal(_vector{ 0.f, 0.f, 0.f }, vHitPos))
            continue;
        else
            return vHitPos;
    }
}

void CModel::SetInterpolationWithSameAnim()
{
    InterpolationwithSameAnim = true;
}

HRESULT CModel::InitializePrototype(TYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix, const vector<_int>& UselessAnimIndex, _bool bIsPlayer, _bool bReadFromFile, _bool bReverseLoad, string strName)
{
    m_eModelType = eModelType;
    m_strModelName = strName;

    // 파일로 부터 읽는 경우
    if (bReadFromFile)
    {
        XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

        if (FAILED(LoadDatFile(pModelFilePath, eModelType, UselessAnimIndex, PreTransformMatrix, bIsPlayer, bReverseLoad)))
            return E_FAIL;

        return S_OK;
    }
    // 직접 생성하는 경우
    else
    {
        // 241014
        // aiProcess_GlobalScale 이라는 플래그를 넣어주어서 큰 모델의 경우 스케일을 줄일 수 있지만 애초에 작은 모델들은 거의 보이지 않을 정도로 작아진다.
        // 따라서 PreTransformMatrix라는 멤버 변수들을 두어서 CMesh를 생성해 낼때 인자로 전달뒤 각각의 CMesh들이 정점, 법선, 탄젠트 벡터에 곱해 로컬의 위치를 양의 Z축을 바라도록 수정해준다.
        _uint   iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };

        if (eModelType == TYPE_NONANIM)
            iFlag |= aiProcess_PreTransformVertices;

        XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

        // 읽는다.
        m_pAIScene = m_pImporter.ReadFile(pModelFilePath, iFlag);

        if (0 == m_pAIScene)
            return E_FAIL;

        if (FAILED(ReadyBones(m_pAIScene->mRootNode, -1)))
            return E_FAIL;

        if (FAILED(ReadyMeshes()))
            return E_FAIL;

        if (FAILED(ReadyAnimations()))
            return E_FAIL;

        if (FAILED(ReadyMaterials(pModelFilePath, bIsPlayer)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
    if (iMeshIndex >= m_iNumMeshes)
        return E_FAIL;

    m_Meshes[iMeshIndex]->BindBufferDesc();
    m_Meshes[iMeshIndex]->Render();

    return S_OK;
}

_bool CModel::PlayAnimation(_float fTimeDelta, _bool bIsPlayer)
{
    if (InterpolationwithSameAnim)
    {
        CAnimation* pTargetAnim = m_Animations[m_iCurrentAnimIndex];
        _bool bRet = m_Animations[m_iCurrentAnimIndex]->UpdateTransformationMatricesInterpolateWithSameAnim(m_Bones, pTargetAnim, fTimeDelta, bIsPlayer);

        if (!bRet)
        {
            InterpolationwithSameAnim = true;
        }

        return bRet;
    }
    else if (m_bInterpolateAnimation)
    {
        m_bIsInterpolationing = InterpolateAnimation(fTimeDelta, bIsPlayer);

        // 보간이 끝났다면
        if (false == m_bIsInterpolationing)
        {
            if (false == bIsPlayer)
                ResetCurrentAnimation();

            // 보간이 끝났다면
            m_iCurrentAnimIndex = m_iInterpolateToIndex;
            m_bInterpolateAnimation = false;
            m_iInterpolateFromIndex = -1;
            m_iInterpolateToIndex = -1;
            m_bIsInterpolationing = false;
        }
        else
        {
            for (auto& pBone : m_Bones)
                pBone->UpdateCombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
        }
    }
    else
    {
        // 표현하고자 하는 애니메이션에 따른, 뼈의 상태(TransformationMatrix)를 갱신해준다.
        if (!m_bLockAnimation)
            m_bIsPlayingAnim = m_Animations[m_iCurrentAnimIndex]->UpdateTransformationMatrices(m_Bones, m_isLoop, fTimeDelta);

        // 우리는 Ready_Bones에서 재귀적 호출로 mRootNode부터 모든 뼈들을 계층 구조로 만들어 주었다.
        // 이제 표현하고자 하는 애니매이션에 따라, 뼈의 상태(TransformationMatrix)를 갱신해 나가준다.
        // 뼈의 바뀐 TransformationMatrix를 활용하여 렌더링 하기 위한 최종 뼈의 CombinedTransformMatrix를 만들어 내준다.
        for (auto& pBone : m_Bones)
            pBone->UpdateCombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
    }

    return m_bIsPlayingAnim;
}

HRESULT CModel::LoadAnimFile(LOADANIMTYPE eType, string strModelNameFrom)
{
    string strFilePath = "../Bin/Resources/Models/GF_Player/";
    string strModelName = m_strModelName;
    string strAnimFileName;

    // TODO
    switch (eType)
    {
    case Engine::LOADANIM_IDLE:
        strAnimFileName = "IDLE";
        break;
    case Engine::LOADANIM_WALK:
        strAnimFileName = "WALK";
        break;
    case Engine::LOADANIM_PUT:
        strAnimFileName = "PUT";
        break;
    case Engine::LOADANIM_DRAW:
        strAnimFileName = "DRAW";
        break;
    case Engine::LOADANIM_JUMP:
        strAnimFileName = "JUMP";
        break;
    case Engine::LOADANIM_HOME:
        strAnimFileName = "HOME";
    case Engine::LOADANIM_END:
        break;
    default:
        break;
    }

    string retFilePath = strFilePath + strModelName + "/" + strAnimFileName + ".anim";

    filesystem::path path = filesystem::absolute(retFilePath);

    if (!std::filesystem::exists(path))
    {
        std::cerr << "Error: File does not exist at given path." << std::endl;
        return E_FAIL;
    }
    
    ifstream InFile(path.c_str(), ios::in | ios_base::binary);
    if (!InFile.is_open())
        return E_FAIL;

    AI_ANIMATION_INFO AnimInfo = {};
    InFile.read((_char*)&AnimInfo, sizeof(AI_ANIMATION_INFO));

    CAnimation* pAnimation = CAnimation::Create(AnimInfo);
    m_Animations.push_back(pAnimation);
    m_iNumAnimations++;

    vector<CChannel*>& Channels = pAnimation->GetChannelVector();
    vector<KEYFRAME>* pCurKeyframes = nullptr;

    for (_int i = 0; i < AnimInfo.iNumChannels; ++i)
    {
        AI_CHANNEL_INFO CHInfo = {};
        InFile.read((_char*)&CHInfo, sizeof(AI_CHANNEL_INFO));
        CChannel* pCH = CChannel::Create(CHInfo);
        Channels.push_back(pCH);

        vector<KEYFRAME>& KeyFrames = pCH->GetKeyFrameVector();
        pCurKeyframes = &KeyFrames;
        for (_uint j = 0; j < CHInfo.iNumKeyFrame; ++j)
        {
            KEYFRAME Keyframe = {};
            InFile.read((_char*)&Keyframe, sizeof(KEYFRAME));
            KeyFrames.push_back(Keyframe);
        }
    }

    return S_OK;
}

_bool CModel::InterpolateAnimation(_float fTimeDelta, _bool bIsPlayer)
{
    CAnimation* pTargetAnim = m_Animations[m_iInterpolateToIndex];
    return m_Animations[m_iCurrentAnimIndex]->UpdateTransformationMatricesInterpolate(m_Bones, pTargetAnim, fTimeDelta, bIsPlayer);
}

HRESULT CModel::BindBoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
    return m_Meshes[iMeshIndex]->BindBoneMatrices(pShader, pConstantName, m_Bones);
}

HRESULT CModel::BindMaterial(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eType, _uint iTextureIndex)
{
    if (m_Materials[m_Meshes[iMeshIndex]->GetMaterialIndex()][eType].empty())
        return S_OK;

    // SRV를 바인딩한다. 어떤 ConstantName인지, m_Material의 [m_Meshes[iMeshIndex]가 쓰고있는 material 번호] : 텍스쳐를 뜻함 A라 하자.
    // - A텍스쳐들 중에 eType에 해당하는 텍스쳐들 중에 iTextureIndex번째 텍스쳐를 바인딩한다.
    return pShader->BindSRV(pConstantName, m_Materials[m_Meshes[iMeshIndex]->GetMaterialIndex()][_uint(eType)][iTextureIndex]);
}


HRESULT CModel::ReadyMeshes()
{
    m_iNumMeshes = m_pAIScene->mNumMeshes;

    for (_uint i = 0; i < m_iNumMeshes; ++i)
    {
        // 장치들, aiMesh*를 넘겨준다. 
        // 하나의 모델은 여러개의 메쉬를 가질 수 있기 때문이다.
        // (m_PreTransformMatrix도 같이 전달해준다. -> 양의 Z축을 바라 보는게 로컬이 되도록 241014)
        CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, this, m_pAIScene->mMeshes[i], XMLoadFloat4x4(&m_PreTransformMatrix));
        if (nullptr == pMesh)
            return E_FAIL;

        m_Meshes.push_back(pMesh);
    }

    return S_OK;
}

HRESULT CModel::LoadDatFile(const _char* pModelFilePath, TYPE eModelType, const vector<_int>& UselessIndex, _fmatrix PreTransformMatrix, _bool bIsPlayer, _bool bReverseLoad)
{
    ifstream InFile(pModelFilePath, ios::in | ios_base::binary);

    if (!InFile.is_open())
        return E_FAIL;


    m_eModelType = eModelType;

    if (eModelType == TYPE::TYPE_ANIM)
    {
        // 1. 뼈 전체 개수 읽기
        {
            _int iLenBone = 0;
            InFile.read((_char*)&iLenBone, sizeof(_int));

            // 뼈 전체 개수만큼 뼈를 생성하기
            for (_int i = 0; i < iLenBone; ++i)
            {
                // 1-1. 뼈 정보 읽기
                AI_BONE_INFO    vInfo;
                InFile.read((_char*)&vInfo, sizeof(AI_BONE_INFO));

                CBone* pBone = CBone::Create(vInfo.szBoneName, vInfo.iParentIndex, XMLoadFloat4x4(&vInfo.TransformMatrix), XMLoadFloat4x4(&vInfo.CombinedMatrix));

                if (nullptr == pBone)
                    return E_FAIL;

                // iLenBone개 만큼 m_Bones에 추가해준다.
                m_Bones.push_back(pBone);
            }




            for (auto& Bone : m_Bones)
                m_TempOriginBones.push_back(Bone);
        }

        // 2. 애니매이션 읽기
        {
            // 2-1. 애니매이션 개수 읽기
            InFile.read((_char*)&m_iNumAnimations, sizeof(_int));

            for (_int i = 0; i < m_iNumAnimations; ++i)
            {
                // 2-2. i번 애니매이션 정보 읽기
                AI_ANIMATION_INFO vAnimInfo;
                InFile.read((_char*)&vAnimInfo, sizeof(AI_ANIMATION_INFO));
                
                CAnimation* pAnimation = CAnimation::Create(vAnimInfo);
                m_Animations.push_back(pAnimation);

                vector<CChannel*>& Channels = pAnimation->GetChannelVector();
                for (_int j = 0; j < vAnimInfo.iNumChannels; ++j)
                {
                    // 2-3. i번째 애니매이션의 j번째 채널 정보 읽기
                    AI_CHANNEL_INFO ChannelInfo;
                    InFile.read((_char*)&ChannelInfo, sizeof(AI_CHANNEL_INFO));

                    CChannel* pCH = CChannel::Create(ChannelInfo);
                    Channels.push_back(pCH);

                    vector<KEYFRAME>& KeyFrames = pCH->GetKeyFrameVector();
                    for (_int k = 0; k < (_int)ChannelInfo.iNumKeyFrame; ++k)
                    {
                        // 2-4. i번째 애니매이션의 j번째 채널의 k개의 키프레임 정보를 읽는다.
                        KEYFRAME KeyFrame = {};
                        InFile.read((_char*)&KeyFrame, sizeof(KEYFRAME));
                        KeyFrames.push_back(KeyFrame);
                    }
                }
            }

            // 사용 못하는 애니매이션 처리, -1이라면 그냥 넘어가고 -1이 아니라 다른 인덱스가 있다면 인덱스에 해당하는 애니매이션을 사용하지 않겠다고 설정한다.
            for (_int i = 0; i < UselessIndex.size(); ++i)
            {
                if (-1 == UselessIndex[i]) break;
                m_Animations[UselessIndex[i]]->SetUselessAnim();
            }
        }
        

        // 3. 메쉬 정보 읽기.
        {
            // 3-1. 전체 메쉬 개쉬 읽기
            InFile.read((_char*)&m_iNumMeshes, sizeof(_int));

            for (_int i = 0; i < (_int)m_iNumMeshes; ++i)
            {
                // 3-2. 메쉬 정보 읽기.
                AI_ANIM_MESH_INFO AnimMeshnfo = {};
                InFile.read((_char*)&AnimMeshnfo, sizeof(AI_ANIM_MESH_INFO));

                vector<VTX_ANIMMESH> vecVtxAnimMesh;
                for (_int j = 0; j < AnimMeshnfo.iAnimMeshVertices; ++j)
                {
                    VTX_ANIMMESH VtxAnimMesh;
                    InFile.read((_char*)&VtxAnimMesh, sizeof(VTX_ANIMMESH));
                    vecVtxAnimMesh.push_back(VtxAnimMesh);
                }

                vector<AI_NONANIM_INDEX> vecMeshIndexInfo;
                for (_int j = 0; j < AnimMeshnfo.iAnimMeshNumFaces; ++j)
                {
                    AI_NONANIM_INDEX VtxAnimIndices;
                    InFile.read((_char*)&VtxAnimIndices, sizeof(AI_NONANIM_INDEX));
                    vecMeshIndexInfo.push_back(VtxAnimIndices);
                }

                vector<AI_BONEINDEX_OFFSETMATRX> vecMeshBoneIndexOffsetMatrix;
                for (_int j = 0; j < AnimMeshnfo.iNumBones; ++j)
                {
                    AI_BONEINDEX_OFFSETMATRX MeshBoneIndexOffsetMatrix;
                    InFile.read((_char*)&MeshBoneIndexOffsetMatrix, sizeof(AI_BONEINDEX_OFFSETMATRX));
                    vecMeshBoneIndexOffsetMatrix.push_back(MeshBoneIndexOffsetMatrix);
                }

                CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, PreTransformMatrix, AnimMeshnfo, vecVtxAnimMesh, vecMeshIndexInfo, vecMeshBoneIndexOffsetMatrix);

                m_Meshes.push_back(pMesh);
            }
        }


    }
    // NONANIM인 경우
    else
    {
        _int iNumMeshes = {};
        InFile.read((char*)&iNumMeshes, sizeof(_int));                              // 1. 메쉬의 개수를 읽는다.
        m_iNumMeshes = iNumMeshes;

        for (_int i = 0; i < iNumMeshes; ++i)
        {
            _int iLen = 0;
            InFile.read((char*)&iLen, sizeof(_int));                                // 2. 메쉬 이름의 길이를 읽는다.

            _char szMeshName[MAX_PATH];
            InFile.read((char*)szMeshName, sizeof(char) * iLen);                    // 3. 메쉬 이름의 길이만큼 경로를 읽는다.

            _int iMeshMaterialIndex = 0;
            InFile.read((char*)&iMeshMaterialIndex, sizeof(_int));                  // 4. 메쉬의 material 인덱스를 읽는다.

            _int iNumVertices = 0;
            InFile.read((char*)&iNumVertices, sizeof(_int));                        // 6. 메쉬의 정점 개수를 읽는다.

            _int iNumFaces = 0;
            InFile.read((char*)&iNumFaces, sizeof(_int));                           // 6. 메쉬의 면의 개수를 읽는다.

            vector<AI_NONANIM_VTX> vecVtxNonAnimInfos;
            for (_int j = 0; j < iNumVertices; ++j)
            {
                AI_NONANIM_VTX vInfo;
                InFile.read((char*)&vInfo, sizeof(AI_NONANIM_VTX));                 // 7. 메쉬의 각 정점 정보를 읽는다.
                vecVtxNonAnimInfos.push_back(vInfo);                                // 7-1. 메쉬의 정점 정보 벡터에 추가.
            }

            vector<AI_NONANIM_INDEX> vecFaceIndicesNoneAnim;
            for (_int j = 0; j < iNumFaces; ++j)
            {
                AI_NONANIM_INDEX vInfo;
                InFile.read((char*)(&vInfo), sizeof(AI_NONANIM_INDEX));        // 8. 메쉬의 인덱스 정보를 쓴다.
                vecFaceIndicesNoneAnim.push_back(vInfo);                       // 8-1. 메쉬의 인덱스 벡터에 추가
            }

            CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, szMeshName, iMeshMaterialIndex, vecVtxNonAnimInfos, vecFaceIndicesNoneAnim);
            if (nullptr == pMesh)
                return E_FAIL;

            m_Meshes.push_back(pMesh);
            SafeAddRef(pMesh);
        }
    }


    // 9. Material 개수를 읽는다.
    _int iNumMaterials;
    InFile.read((char*)&iNumMaterials, sizeof(_int));
    m_Materials.resize(iNumMaterials);

    // 플레이어인 경우 Material인덱스를 강제로 변경한다.
    if (bIsPlayer)
    {
        for (auto& pMesh : m_Meshes)
        {
            // 팔 메쉬인 경우 해당 메쉬의 MaterialIndex를 0으로 변경한다.
            if (false == strcmp(pMesh->GetMeshName(), "hero_fpp_101_A_L") || false == strcmp(pMesh->GetMeshName(), "hero_fpp_101_A_R"))
                pMesh->SetMaterialIndex(0);
            // 팔 메쉬가 아닌경우 해당 메쉬의 MaterialIndex를 1로 변경한다.(총의 경우가 이 경우이다)
            else
                pMesh->SetMaterialIndex(1);
        }
    }
    
    // 플레이인 경우 예외적으로 읽어준다.
    if (bIsPlayer)
    {
        if (bReverseLoad)
        {
            m_Materials[0].resize(AI_TEXTURE_TYPE_MAX);

            _char		szDrive[MAX_PATH] = "";
            _char		szDirectory[MAX_PATH] = "";
            _char		szFileName[MAX_PATH] = "hero_fpp_101_A_c";
            _char		szExt[MAX_PATH] = ".png";

            _char		szFullPath[MAX_PATH] = "";
            _splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

            strcpy_s(szFullPath, szDrive);
            strcat_s(szFullPath, szDirectory);
            strcat_s(szFullPath, szFileName);
            strcat_s(szFullPath, szExt);

            //DRIVE경로+ 디렉토리 경로 + 텍스처 이름 + 텍스처 확장자 (텍스처도 메시랑 같이 있어야됨)
            _tchar		szPerfectPath[MAX_PATH] = TEXT("");
            MultiByteToWideChar(CP_ACP, 0, szFullPath, _int(strlen(szFullPath)), szPerfectPath, MAX_PATH);
            ID3D11ShaderResourceView* pSRV = { nullptr };

            if (false == strcmp(szExt, ".dds"))
            {
                if (SUCCEEDED(CreateDDSTextureFromFile(m_pDevice, szPerfectPath, nullptr, &pSRV)))
                    m_Materials[0][1].push_back(pSRV);
            }
            else if (false == strcmp(szExt, ".tga"))
                int a = 10;
            else
            {
                if (SUCCEEDED(CreateWICTextureFromFile(m_pDevice, szPerfectPath, nullptr, &pSRV)))
                    m_Materials[0][1].push_back(pSRV);
            }
            
            for (_int i = 1; i < iNumMaterials; ++i)
            {
                m_Materials[i].resize(AI_TEXTURE_TYPE_MAX);

                for (_int j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
                {
                    _int iNumTextures = 0;
                    InFile.read((char*)&iNumTextures, sizeof(_int));                   // 10. Material의 텍스쳐 개수를 읽는다.

                    if (0 == iNumTextures)
                    {
                        continue;
                    }
                    else
                    {
                        for (_int k = 0; k < iNumTextures; ++k)
                        {
                            _int iTexturePathLen = 0;
                            InFile.read((char*)&iTexturePathLen, sizeof(_int));                        // 11. 경로 길이를 읽는다.

                            _char szTexturePath[MAX_PATH];
                            InFile.read((char*)szTexturePath, sizeof(char) * iTexturePathLen);         // 12. 경로 읽는다.


                            std::filesystem::path pathObj(pModelFilePath);
                            std::string ext = pathObj.extension().string();
                            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

                            _tchar		szFinalFullPath[MAX_PATH] = TEXT("");
                            MultiByteToWideChar(CP_ACP, 0, szTexturePath, iTexturePathLen, szFinalFullPath, MAX_PATH);


                            ID3D11ShaderResourceView* pSRV = nullptr;

                            if (false == strcmp(ext.c_str(), ".dds"))
                            {
                                if (SUCCEEDED(CreateDDSTextureFromFile(m_pDevice, szFinalFullPath, nullptr, &pSRV)))
                                    m_Materials[i][j].push_back(pSRV);
                            }
                            else if (false == strcmp(ext.c_str(), ".tag"))
                            {
                                _int a = 10;
                            }
                            else
                            {
                                if (SUCCEEDED(CreateWICTextureFromFile(m_pDevice, szFinalFullPath, nullptr, &pSRV)))
                                    m_Materials[i][j].push_back(pSRV);
                            }
                        }
                    }
                }
            }
        }
        else
        {
            m_Materials[0].resize(AI_TEXTURE_TYPE_MAX);

            _char		szDrive[MAX_PATH] = "";
            _char		szDirectory[MAX_PATH] = "";
            _char		szFileName[MAX_PATH] = "hero_fpp_101_A_c";
            _char		szExt[MAX_PATH] = ".png";

            _char		szFullPath[MAX_PATH] = "";
            _splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

            strcpy_s(szFullPath, szDrive);
            strcat_s(szFullPath, szDirectory);
            strcat_s(szFullPath, szFileName);
            strcat_s(szFullPath, szExt);

            //DRIVE경로+ 디렉토리 경로 + 텍스처 이름 + 텍스처 확장자 (텍스처도 메시랑 같이 있어야됨)
            _tchar		szPerfectPath[MAX_PATH] = TEXT("");
            MultiByteToWideChar(CP_ACP, 0, szFullPath, _int(strlen(szFullPath)), szPerfectPath, MAX_PATH);
            ID3D11ShaderResourceView* pSRV = { nullptr };
            if (false == strcmp(szExt, ".dds"))
            {
                if (SUCCEEDED(CreateDDSTextureFromFile(m_pDevice, szPerfectPath, nullptr, &pSRV)))
                    m_Materials[0][1].push_back(pSRV);
            }
            else if (false == strcmp(szExt, ".tga"))
                int a = 10;
            else
            {
                if (SUCCEEDED(CreateWICTextureFromFile(m_pDevice, szPerfectPath, nullptr, &pSRV)))
                    m_Materials[0][1].push_back(pSRV);
            }

            for (_int i = 0; i < iNumMaterials; ++i)
            {
                m_Materials[i].resize(AI_TEXTURE_TYPE_MAX);

                for (_int j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
                {
                    _int iNumTextures = 0;
                    InFile.read((char*)&iNumTextures, sizeof(_int));                   // 10. Material의 텍스쳐 개수를 읽는다.

                    if (0 == iNumTextures) continue;

                    for (_int k = 0; k < iNumTextures; ++k)
                    {
                        _int iTexturePathLen = 0;
                        InFile.read((char*)&iTexturePathLen, sizeof(_int));                        // 11. 경로 길이를 읽는다.

                        _char szTexturePath[MAX_PATH];
                        InFile.read((char*)szTexturePath, sizeof(char) * iTexturePathLen);         // 12. 경로 읽는다.


                        std::filesystem::path pathObj(pModelFilePath);
                        std::string ext = pathObj.extension().string();
                        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

                        _tchar		szFinalFullPath[MAX_PATH] = TEXT("");
                        MultiByteToWideChar(CP_ACP, 0, szTexturePath, iTexturePathLen, szFinalFullPath, MAX_PATH);


                        ID3D11ShaderResourceView* pSRV = nullptr;

                        if (false == strcmp(ext.c_str(), ".dds"))
                        {
                            if (SUCCEEDED(CreateDDSTextureFromFile(m_pDevice, szFinalFullPath, nullptr, &pSRV)))
                                m_Materials[i][j].push_back(pSRV);
                        }
                        else if (false == strcmp(ext.c_str(), ".tag"))
                        {
                            _int a = 10;
                        }
                        else
                        {
                            if (SUCCEEDED(CreateWICTextureFromFile(m_pDevice, szFinalFullPath, nullptr, &pSRV)))
                                m_Materials[i][j].push_back(pSRV);
                        }
                    }
                }
            }
        }       
    }
    else
    {
        for (_int i = 0; i < iNumMaterials; ++i)
        {
            m_Materials[i].resize(AI_TEXTURE_TYPE_MAX);

            for (_int j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
            {
                _int iNumTextures = 0;
                InFile.read((char*)&iNumTextures, sizeof(_int));                   // 10. Material의 텍스쳐 개수를 읽는다.

                if (0 == iNumTextures) continue;

                for (_int k = 0; k < iNumTextures; ++k)
                {
                    _int iTexturePathLen = 0;
                    InFile.read((char*)&iTexturePathLen, sizeof(_int));                        // 11. 경로 길이를 읽는다.

                    _char szTexturePath[MAX_PATH];
                    InFile.read((char*)szTexturePath, sizeof(char) * iTexturePathLen);         // 12. 경로 읽는다.


                    std::filesystem::path pathObj(pModelFilePath);
                    std::string ext = pathObj.extension().string();
                    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

                    _tchar		szFinalFullPath[MAX_PATH] = TEXT("");
                    MultiByteToWideChar(CP_ACP, 0, szTexturePath, iTexturePathLen, szFinalFullPath, MAX_PATH);


                    ID3D11ShaderResourceView* pSRV = nullptr;

                    if (false == strcmp(ext.c_str(), ".dds"))
                    {
                        if (SUCCEEDED(CreateDDSTextureFromFile(m_pDevice, szFinalFullPath, nullptr, &pSRV)))
                            m_Materials[i][j].push_back(pSRV);
                    }
                    else if (false == strcmp(ext.c_str(), ".tag"))
                    {
                        _int a = 10;
                    }
                    else
                    {
                        if (SUCCEEDED(CreateWICTextureFromFile(m_pDevice, szFinalFullPath, nullptr, &pSRV)))
                            m_Materials[i][j].push_back(pSRV);
                    }
                }
            }
        }
    }
    

    InFile.close();

    return S_OK;
}


// vector<vector<vector<ID3D11ShaderResourceView*>>>	m_Materials;
// - Material이라는 벡터가 Texture들을 들고 있다 0 ~ 18 번째 종류까지
// - 이 Texture(정확히는 Diffuse Map(Texture))들을 그리기 위한(쉐이딩 하기 위한) ShaderSRV를 Texture 벡터가 가지고 있는 형태이다.
// - (가독성이 많이 떨어지니 따로 클래스 분기 처리하는게 좋다.)
HRESULT CModel::ReadyMaterials(const _char* pModelFilePath, _bool bIsPlayer)
{
    m_iNumMaterials = m_pAIScene->mNumMaterials;

    m_Materials.resize(m_iNumMaterials);

    // 플레이인 경우 예외
    if (bIsPlayer)
    {
        m_Materials[0].resize(AI_TEXTURE_TYPE_MAX);

        _char		szDrive[MAX_PATH] = "";
        _char		szDirectory[MAX_PATH] = "";
        _char		szFileName[MAX_PATH] = "hero_fpp_101_A_c";
        _char		szExt[MAX_PATH] = ".png";

        _char		szFullPath[MAX_PATH] = "";
        _splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

        strcpy_s(szFullPath, szDrive);
        strcat_s(szFullPath, szDirectory);
        strcat_s(szFullPath, szFileName);
        strcat_s(szFullPath, szExt);

        //DRIVE경로+ 디렉토리 경로 + 텍스처 이름 + 텍스처 확장자 (텍스처도 메시랑 같이 있어야됨)
        _tchar		szPerfectPath[MAX_PATH] = TEXT("");
        MultiByteToWideChar(CP_ACP, 0, szFullPath, _int(strlen(szFullPath)), szPerfectPath, MAX_PATH);
        ID3D11ShaderResourceView* pSRV = { nullptr };
        if (false == strcmp(szExt, ".dds"))
        {
            if (SUCCEEDED(CreateDDSTextureFromFile(m_pDevice, szPerfectPath, nullptr, &pSRV)))
                m_Materials[0][1].push_back(pSRV);
        }
        else if (false == strcmp(szExt, ".tga"))
            int a = 10;
        else
        {
            if (SUCCEEDED(CreateWICTextureFromFile(m_pDevice, szPerfectPath, nullptr, &pSRV)))
                m_Materials[0][1].push_back(pSRV);
        }
    }
   

    for (_uint i = 0; i < m_iNumMaterials; ++i)
    {
        // AI_TEXTURE_TYPE_MAX 거의 모든 종류의 텍스쳐를 지원하는데 가장 마지막 인덱스를 뜻하는 메크로 이다 18이다.
        // - 0 : Dummy, 1 : Diffuse, 2: 어쩌구... 18 END
        m_Materials[i].resize(AI_TEXTURE_TYPE_MAX);

        // 로드한 m_pAIScene으로 부터 Material을 가져오는데 몇번째 Material을 가져올 것인지.
        // - Material은 여러개 이다. 만약 4개라 가정하면 0번째 Material 부터 가져온다는 뜻이다.
        aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];

        // 0 ~ 18번째 텍스쳐 종류를 순회한다.
        // - (0은 더미라 보통 1부터 시작하기도 한다.)
        for (_uint j = 1; j < AI_TEXTURE_TYPE_MAX; ++j)
        {
            // GetTextureCount : 어떤 종류의 텍스쳐의 개수를 가져 올 것이냐?
            // - 현재 Diffuse Material에 해당하는 텍스쳐 개수를 가져오고 싶다.
            // - (웬만하면 1개인데 지인짜 가끔 2개 이상씩 나오는 경우가 있다.)
            _uint   iNumTextures = pAIMaterial->GetTextureCount(aiTextureType(j));

           

            // j가 1이라 가정하면 '현재 Diffuse 텍스쳐에 해당하는 개수 만큼 가져온다' 는 뜻이 된다.
            for (_uint k = 0; k < iNumTextures; ++k)
            {
                aiString    strTexturePath;

                // aiTextureType(j) 타입에 해당하는 k 번째 텍스쳐를 가져오고, 이 경로를 strTexturePath에 담는다.
                if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), k, &strTexturePath)))
                    break;

                _char       szDrive[MAX_PATH] = "";         // 드라이브 경로
                _char       szDirectory[MAX_PATH] = "";     // 디렉토리
                _char       szFileName[MAX_PATH] = "";      // file 이름
                _char       szExt[MAX_PATH] = "";             // 확장자

                _char       szFullPath[MAX_PATH] = "";      // 전체 경로


                _splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);
                _splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

                strcpy_s(szFullPath, szDrive);
                strcat_s(szFullPath, szDirectory);
                strcat_s(szFullPath, szFileName);
                strcat_s(szFullPath, szExt);

                _tchar		szFinalFullPath[MAX_PATH] = TEXT("");

                MultiByteToWideChar(CP_ACP, 0, szFullPath, _uint(strlen(szFullPath)), szFinalFullPath, MAX_PATH);

                ID3D11ShaderResourceView* pSRV = nullptr;

                if (false == strcmp(szExt, ".dds"))
                {
                    if (SUCCEEDED(CreateDDSTextureFromFile(m_pDevice, szFinalFullPath, nullptr, &pSRV)))
                        m_Materials[i][j].push_back(pSRV);
                }
                else if (false == strcmp(szExt, ".tag"))
                {
                    _int a = 10;
                }
                else
                {
                    if (SUCCEEDED(CreateWICTextureFromFile(m_pDevice, szFinalFullPath, nullptr, &pSRV)))
                        m_Materials[i][j].push_back(pSRV);
                }
            }
        }
    }

     return S_OK;
}

HRESULT CModel::ReadyBones(const aiNode* pAINode, _int iParentBoneIndex)
{
    CBone* pBone = CBone::Create(m_pDevice, m_pContext, pAINode, iParentBoneIndex);

    if (nullptr == pBone)
        return E_FAIL;

    // pBone을 추가한다.
    // 현재 CModel의 m_Bones에 모든 자식 본들을 다 추가해준다.
    m_Bones.push_back(pBone);

    iParentBoneIndex = static_cast<_int>(m_Bones.size() - 1);

    for (_int i = 0; i < _int(pAINode->mNumChildren); ++i)
    {
        ReadyBones(pAINode->mChildren[i], iParentBoneIndex);
    }

    return S_OK;
}


HRESULT CModel::ReadyAnimations()
{
    m_iNumAnimations = m_pAIScene->mNumAnimations;

    for (_int i = 0; i < m_iNumAnimations; ++i)
    {
        CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], this);
        if (nullptr == pAnimation)
            return E_FAIL;

        m_Animations.push_back(pAnimation);
    }

    return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix, const vector<_int>& UselessAnimIndex, _bool bIsPlayer, _bool bReadFromFile, _bool bReverseLoad, string strName)
{
    CModel* pInstance = new CModel(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype(eModelType, pModelFilePath, PreTransformMatrix, UselessAnimIndex, bIsPlayer, bReadFromFile, bReverseLoad, strName)))
    {
        MSG_BOX("Failed to Created : CModel");
        SafeRelease(pInstance);
    }
    return pInstance;
}


CComponent* CModel::Clone(void* pArg)
{
    CModel* pInstance = new CModel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CModel");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CModel::Free()
{
    __super::Free();

    for (auto& Textures : m_Materials)
    {
        for (auto& SRVs : Textures)
        {
            for (auto& pSRV : SRVs)
                SafeRelease(pSRV);

            SRVs.clear();
        }

        Textures.clear();
    }
    m_Materials.clear();


    for (auto& pMesh : m_Meshes)
        SafeRelease(pMesh);
    m_Meshes.clear();


    for (auto& pAnimation : m_Animations)
        SafeRelease(pAnimation);
    m_Animations.clear();


    for (auto& pBone : m_Bones)
        SafeRelease(pBone);
    m_Bones.clear();

    if (false == m_TempOriginBones.empty())
    {
        for (auto& pBone : m_TempOriginBones)
            SafeRelease(pBone);
    }

    m_pImporter.FreeScene();
}
