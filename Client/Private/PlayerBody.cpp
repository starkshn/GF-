#include "pch.h"
#include "PlayerBody.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"


CPlayerBody::CPlayerBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CStateObjectBody(pDevice, pContext)
{
}

CPlayerBody::CPlayerBody(const CPlayerBody& Prototype)
    :
    CStateObjectBody(Prototype)
{
}

HRESULT CPlayerBody::InitializePrototype()
{
    return S_OK;
}

HRESULT CPlayerBody::Initialize(void* pArg)
{
    if (nullptr != pArg)
    {
        STATE_OBJECT_BODY_DESC* pDesc = static_cast<STATE_OBJECT_BODY_DESC*>(pArg);
        m_pPlayer = dynamic_cast<CPlayer*>(pDesc->pOwner);
        if (nullptr == m_pPlayer)
            return E_FAIL;
    }

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    if (FAILED(LoadPlayerAnimations()))
        return E_FAIL;

    if (FAILED(SetupPlayerModelFirstAnimationsState()))
        return E_FAIL;

    // 초기 TransformationMatrix설정
    if (FAILED(SetupPlayerModelFristTransformationMatrices()))
        return E_FAIL;

    // 임시 나중에 RIGHT_1201로
    SetCurrentModelIndex((_uint)PLAYER_MODEL_INDEX::RIGHT_1201);

    return S_OK;
}

void CPlayerBody::PriorityUpdate(_float fTimeDelta)
{

}

void CPlayerBody::Update(_float fTimeDelta)
{
    if (CanChangeAnimation())
    {
        ChangeAnimationByAnimIndex(GetAnimationIndexByState(m_iPrevTickState, GetCurrentModelIndex()), GetAnimationIndexByState(*m_pState, GetCurrentModelIndex()), GetCurrentModelIndex());
        m_iPrevTickState = *m_pState;
    }

    m_bIsAnimationPlaying = m_pModelComs[GetCurrentModelIndex()]->PlayAnimation(fTimeDelta, true);

    // 애니매이션이 끝난 경우
    if (false == m_bIsAnimationPlaying)
    {
        AnimationEnd(GetCurrentModelIndex());
        m_bIsAnimationPlaying = true;

        if (m_bIsInterpolationing)
        {
            m_iInterpolationIndex = -1;
            m_bIsInterpolationing = false;
        }
    }


    if (CPlayer::DRAW & *m_pState)
        m_vTempAccTime += fTimeDelta;

    if (CPlayer::DRAW & *m_pState)
        FindAlertPoint(CPlayer::DRAW, *m_pState, "DRAW", "DRAW");

    // 현재 CTestPlayerBody의 월드 행렬은 나의 월드 변환행렬(다른 말로 로컬 변환행렬) 곱하기 부모의 월드 변환행렬이 된다.
    XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->GetWorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));


    CheckAlertPointSoundName();
}

void CPlayerBody::LateUpdate(_float fTimeDelta)
{
    m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);
    // m_pGameInst->AddRenderObject(CRenderer::RG_SHADOW, this);
}

HRESULT CPlayerBody::Render()
{
    if (FAILED(BindShaderResources()))
        return E_FAIL;

    // 현재 사용하고 있는 모델의 메쉬 개수를 가져온다.
    _uint   iNumMeshes = m_pModelComs[GetCurrentModelIndex()]->GetNumMeshes();

    _int iMd = GetCurrentModelIndex();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pModelComs[GetCurrentModelIndex()]->BindMaterial(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
            return E_FAIL;

        if (FAILED(m_pModelComs[GetCurrentModelIndex()]->BindBoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

  /*      if (i == 0)
            continue;*/
        m_pShaderCom->Begin(0);
        m_pModelComs[GetCurrentModelIndex()]->Render(i);
    }

    return S_OK;
}

HRESULT CPlayerBody::RenderShadow()
{
    if (FAILED(m_pShaderCom->BindMatrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", m_pGameInst->GetShadowTransform(CShadow::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", m_pGameInst->GetShadowTransform(CShadow::D3DTS_PROJ))))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelComs[GetCurrentModelIndex()]->GetNumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelComs[GetCurrentModelIndex()]->BindMaterial(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
            return E_FAIL;
        if (FAILED(m_pModelComs[GetCurrentModelIndex()]->BindBoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        m_pShaderCom->Begin(2);
        m_pModelComs[GetCurrentModelIndex()]->Render(i);
    }

    return S_OK;
}

HRESULT CPlayerBody::LoadPlayerAnimations()
{
    // 1201_Right, 0 : FIRE, 2 : RELOAD, 4 : IDLE, 5, WALK, 6 : JUMP

    for (_uint i = 0; i < m_iNumModels; ++i)
    {
        if (nullptr != m_pModelComs[i])
        {
            if (PLAYER_MODEL_INDEX::RIGHT_1201 == (PLAYER_MODEL_INDEX)i)
            {
                m_pModelComs[i]->LoadAnimFile(LOADANIMTYPE::LOADANIM_IDLE, "1201_Right");
                m_pModelComs[i]->LoadAnimFile(LOADANIMTYPE::LOADANIM_WALK, "1201_Right");
                m_pModelComs[i]->LoadAnimFile(LOADANIMTYPE::LOADANIM_JUMP, "1201_Right");
                m_pModelComs[i]->LoadAnimFile(LOADANIMTYPE::LOADANIM_PUT, "1201_Right");
                m_pModelComs[i]->LoadAnimFile(LOADANIMTYPE::LOADANIM_DRAW, "1201_Right");
                m_pModelComs[i]->LoadAnimFile(LOADANIMTYPE::LOADANIM_HOME, "1201_Right");

                m_pModelComs[i]->SetAnimationName(0, "FIRE");
                m_pModelComs[i]->GetAnimation(0)->SetInterpolationTime(0.1f);

                m_pModelComs[i]->SetAnimationName(2, "RELOAD");

                m_pModelComs[i]->SetAnimationName(4, "IDLE");
                m_pModelComs[i]->GetAnimation(4)->SetInterpolationTime(0.1f);

                m_pModelComs[i]->SetAnimationName(5, "WALK");
                m_pModelComs[i]->GetAnimation(5)->SetInterpolationTime(0.1f);

                m_pModelComs[i]->SetAnimationName(6, "JUMP");
                m_pModelComs[i]->GetAnimation(6)->SetInterpolationTime(0.1f);


                m_pModelComs[i]->SetAnimationName(7, "PUT");
                m_pModelComs[i]->GetAnimation(7)->SetInterpolationTime(0.1f);
                {
                    CAnimation* pPutAnim = m_pModelComs[0]->GetAnimation(7);
                    pPutAnim->InsertAlertPointSound("PUT", 0.1f);
                }

                m_pModelComs[i]->SetAnimationName(8, "DRAW");
                CAnimation* pDrawAnim = m_pModelComs[0]->GetAnimation(8);
                {
                    pDrawAnim->InsertAlertPoint("DRAW", 1.5f);
                    pDrawAnim->InsertAlertPointSound("DRAW", 0.8f);
                }


                m_pModelComs[i]->GetAnimation(8)->SetInterpolationTime(0.1f);
                m_pModelComs[i]->SetAnimationName(9, "HOME");


                m_ModelsAnimationNames[i].insert({ "IDLE", 4 });
                m_ModelsAnimationNames[i].insert({ "WALK", 5 });
                m_ModelsAnimationNames[i].insert({ "JUMP", 6 });
                m_ModelsAnimationNames[i].insert({ "FIRE", 0 });
                m_ModelsAnimationNames[i].insert({ "RELOAD", 2 });
                {
                    CAnimation* pAnim = m_pModelComs[0]->GetAnimation(2);
                    pAnim->InsertAlertPointSound("RELOAD_START", 0.25f);
                    pAnim->InsertAlertPointSound("RELOAD_END", 0.5f);
                    pAnim->InsertAlertPointSound("RELOAD_FINAL", 1.25f);
                }





                m_ModelsAnimationNames[i].insert({ "PUT", 7 });
                m_ModelsAnimationNames[i].insert({ "DRAW", 8 });
                m_ModelsAnimationNames[i].insert({ "HOME", 9 });
            }
            else if (PLAYER_MODEL_INDEX::RIGHT_1004 == (PLAYER_MODEL_INDEX)i)
            {

                // 1004 불꽃비늘
                // 0 : 못씀, 1 : 장전, 2 : 못씀, 3 : 못씀, 4 : 발사, 5 : IDLE, 6 : WALk, 7 : PUT, 8 : DRAW
                m_pModelComs[i]->LoadAnimFile(LOADANIMTYPE::LOADANIM_IDLE, "1004_Right");
                m_pModelComs[i]->LoadAnimFile(LOADANIMTYPE::LOADANIM_WALK, "1004_Right");
                m_pModelComs[i]->LoadAnimFile(LOADANIMTYPE::LOADANIM_PUT, "1004_Right");
                m_pModelComs[i]->LoadAnimFile(LOADANIMTYPE::LOADANIM_DRAW, "1004_Right");

                m_pModelComs[i]->SetAnimationName(5, "IDLE");
                m_pModelComs[i]->GetAnimation(5)->SetInterpolationTime(0.1f);

                m_pModelComs[i]->SetAnimationName(6, "WALK");
                m_pModelComs[i]->GetAnimation(6)->SetInterpolationTime(0.1f);

                m_pModelComs[i]->SetAnimationName(4, "FIRE");
                m_pModelComs[i]->SetAnimationName(1, "RELOAD");
                {
                    CAnimation* pAnim = m_pModelComs[1]->GetAnimation(1);
                    pAnim->InsertAlertPointSound("RELOAD", 0.5);
                }

                m_pModelComs[i]->SetAnimationName(7, "PUT");
                m_pModelComs[i]->GetAnimation(7)->SetInterpolationTime(0.1f);
                {
                    CAnimation* pPutAnim = m_pModelComs[1]->GetAnimation(7);
                    pPutAnim->InsertAlertPointSound("PUT", 0.1f);
                }

                m_pModelComs[i]->SetAnimationName(8, "DRAW");
                CAnimation* pDrawAnim = m_pModelComs[1]->GetAnimation(8);
                {
                    pDrawAnim->InsertAlertPoint("DRAW", 1.5f);
                    pDrawAnim->InsertAlertPointSound("DRAW", 0.5f);
                }

                m_pModelComs[i]->GetAnimation(8)->SetInterpolationTime(0.1f);

                m_ModelsAnimationNames[i].insert({ "IDLE", 5 });
                m_ModelsAnimationNames[i].insert({ "WALK", 6 });
                m_ModelsAnimationNames[i].insert({ "FIRE", 4 });
                m_ModelsAnimationNames[i].insert({ "RELOAD", 1 });
                m_ModelsAnimationNames[i].insert({ "PUT", 7 });
                m_ModelsAnimationNames[i].insert({ "DRAW", 8 });
            }
            else if (PLAYER_MODEL_INDEX::RIGHT_1503 == (PLAYER_MODEL_INDEX)i)
            {
                // 0 : 못씀, 1 : RELOAD, 2 : X, 3 : FIRE, 4 : X, 5 : X
                m_pModelComs[i]->LoadAnimFile(LOADANIMTYPE::LOADANIM_IDLE, "1503_Right");
                m_pModelComs[i]->LoadAnimFile(LOADANIMTYPE::LOADANIM_WALK, "1503_Right");
                m_pModelComs[i]->LoadAnimFile(LOADANIMTYPE::LOADANIM_PUT, "1503_Right");
                m_pModelComs[i]->LoadAnimFile(LOADANIMTYPE::LOADANIM_DRAW, "1503_Right");

                m_pModelComs[i]->SetAnimationName(6, "IDLE");
                m_pModelComs[i]->SetAnimationName(7, "WALK");
                m_pModelComs[i]->SetAnimationName(3, "FIRE");
                m_pModelComs[i]->SetAnimationName(1, "RELOAD");
                m_pModelComs[i]->SetAnimationName(8, "PUT");
                {
                    CAnimation* pPutAnim = m_pModelComs[2]->GetAnimation(8);
                    pPutAnim->InsertAlertPoint("PUT", 0.15f);
                }


                m_pModelComs[i]->SetAnimationName(9, "DRAW");
                CAnimation* pDrawAnim = m_pModelComs[2]->GetAnimation(9);
                pDrawAnim->InsertAlertPoint("DRAW", 1.5f);
                {
                    pDrawAnim->InsertAlertPointSound("DRAW", 0.5f);
                }

                m_ModelsAnimationNames[i].insert({ "IDLE", 6 });
                m_ModelsAnimationNames[i].insert({ "WALK", 7 });
                m_ModelsAnimationNames[i].insert({ "FIRE", 3 });
                m_ModelsAnimationNames[i].insert({ "RELOAD", 1 });
                {
                    {
                        CAnimation* pAnim = m_pModelComs[2]->GetAnimation(1);
                        pAnim->InsertAlertPointSound("RELOAD", 0.35f);
                    }
                }



                m_ModelsAnimationNames[i].insert({ "PUT", 8 });
                m_ModelsAnimationNames[i].insert({ "DRAW", 9 });
            }
            else if (PLAYER_MODEL_INDEX::RIGHT_1508 == (PLAYER_MODEL_INDEX)i)
            {
                // 0 : FIRE(2발 발싸), 1 : X, 2 : X, 3 : X, 4 : FIRE(X), 5 : FIRE(1발, X), 6 : RELOAD, 7 : X, 8 : X, 9 : X, 10 : X, 11 : X 
                m_pModelComs[i]->LoadAnimFile(LOADANIMTYPE::LOADANIM_IDLE, "1508_Right");
                m_pModelComs[i]->LoadAnimFile(LOADANIMTYPE::LOADANIM_WALK, "1508_Right");
                m_pModelComs[i]->LoadAnimFile(LOADANIMTYPE::LOADANIM_PUT, "1508_Right");
                m_pModelComs[i]->LoadAnimFile(LOADANIMTYPE::LOADANIM_DRAW, "1508_Right");

                m_pModelComs[i]->SetAnimationName(12, "IDLE");
                m_pModelComs[i]->SetAnimationName(13, "WALK");
                m_pModelComs[i]->SetAnimationName(0, "FIRE");
                m_pModelComs[i]->SetAnimationName(6, "RELOAD");
                m_pModelComs[i]->SetAnimationName(14, "PUT");
                {
                    CAnimation* pPutAnim = m_pModelComs[3]->GetAnimation(14);
                    pPutAnim->InsertAlertPointSound("PUT", 0.1f);
                }


                m_pModelComs[i]->SetAnimationName(15, "DRAW");
                CAnimation* pAnim = m_pModelComs[3]->GetAnimation(15);
                pAnim->InsertAlertPoint("DRAW", 1.5f);
                {
                    pAnim->InsertAlertPointSound("DRAW", 0.5f);
                }

                m_ModelsAnimationNames[i].insert({ "IDLE", 12 });
                m_ModelsAnimationNames[i].insert({ "WALK", 13 });
                m_ModelsAnimationNames[i].insert({ "FIRE", 0 });
                m_ModelsAnimationNames[i].insert({ "RELOAD", 6 });
                {
                    CAnimation* pAnim = m_pModelComs[3]->GetAnimation(6);
                    pAnim->InsertAlertPointSound("RELOAD_START", 0.15f);
                    pAnim->InsertAlertPointSound("RELOAD_DO", 0.5f);
                    pAnim->InsertAlertPointSound("RELOAD_END", 1.3f);
                }


                m_ModelsAnimationNames[i].insert({ "PUT", 14 });
                m_ModelsAnimationNames[i].insert({ "DRAW", 15 });
            }
        }
    }

    return S_OK;
}

HRESULT CPlayerBody::SetupPlayerModelFristTransformationMatrices()
{
    for (_uint i = 0; i < m_iNumModels; ++i)
    {
        if (nullptr != m_pModelComs[i])
        {
            // 1201_Right
            if (PLAYER_MODEL_INDEX::RIGHT_1201 == (PLAYER_MODEL_INDEX)i)
                m_pModelComs[(_uint)PLAYER_MODEL_INDEX::RIGHT_1201]->SetTransformationMatriciesFirstTime(GetAnimationIndexByAnimationName("RELOAD", (_uint)PLAYER_MODEL_INDEX::RIGHT_1201), 2.f);
            else if (PLAYER_MODEL_INDEX::RIGHT_1004 == (PLAYER_MODEL_INDEX)i)
                m_pModelComs[(_uint)PLAYER_MODEL_INDEX::RIGHT_1004]->SetTransformationMatriciesFirstTime(GetAnimationIndexByAnimationName("RELOAD", (_uint)PLAYER_MODEL_INDEX::RIGHT_1004), 2.f);
            else if (PLAYER_MODEL_INDEX::RIGHT_1503 == (PLAYER_MODEL_INDEX)i)
                m_pModelComs[(_uint)PLAYER_MODEL_INDEX::RIGHT_1503]->SetTransformationMatriciesFirstTime(GetAnimationIndexByAnimationName("RELOAD", (_uint)PLAYER_MODEL_INDEX::RIGHT_1503), 5.f);
            else if (PLAYER_MODEL_INDEX::RIGHT_1508 == (PLAYER_MODEL_INDEX)i)
                m_pModelComs[(_uint)PLAYER_MODEL_INDEX::RIGHT_1508]->SetTransformationMatriciesFirstTime(GetAnimationIndexByAnimationName("RELOAD", (_uint)PLAYER_MODEL_INDEX::RIGHT_1508), 5.f);
        }
    }

    return S_OK;
}

HRESULT CPlayerBody::SetupPlayerModelFirstAnimationsState()
{
    // 1201_Right
    {
        _int iIndex = GetAnimationIndexByAnimationName("WALK", (_uint)PLAYER_MODEL_INDEX::RIGHT_1201);
        if (-1 != iIndex)
            m_pModelComs[(_uint)PLAYER_MODEL_INDEX::RIGHT_1201]->SetupAnimation(iIndex, CAnimation::LOOP);
        else
            return E_FAIL;

        iIndex = GetAnimationIndexByAnimationName("IDLE", (_uint)PLAYER_MODEL_INDEX::RIGHT_1201);
        if (-1 != iIndex)
            m_pModelComs[(_uint)PLAYER_MODEL_INDEX::RIGHT_1201]->SetupAnimation(iIndex, CAnimation::LOOP);
        else
            return E_FAIL;
    }

    // 1004_Right
    {

        _int iIndex = GetAnimationIndexByAnimationName("WALK", (_uint)PLAYER_MODEL_INDEX::RIGHT_1004);
        if (-1 != iIndex)
            m_pModelComs[(_uint)PLAYER_MODEL_INDEX::RIGHT_1004]->SetupAnimation(iIndex, CAnimation::LOOP);
        else
            return E_FAIL;

        iIndex = GetAnimationIndexByAnimationName("IDLE", (_uint)PLAYER_MODEL_INDEX::RIGHT_1004);
        if (-1 != iIndex)
            m_pModelComs[(_uint)PLAYER_MODEL_INDEX::RIGHT_1004]->SetupAnimation(iIndex, CAnimation::LOOP);
        else
            return E_FAIL;

        CAnimation* pAnim = m_pModelComs[(_uint)PLAYER_MODEL_INDEX::RIGHT_1004]->GetAnimation(GetAnimationIndexByAnimationName("FIRE", (_uint)PLAYER_MODEL_INDEX::RIGHT_1004));
        pAnim->SetFireName("1004");
        pAnim->SetFireInterpolationSP(5.f);
    }

    // 1503_Right
    {

        _int iIndex = GetAnimationIndexByAnimationName("WALK", (_uint)PLAYER_MODEL_INDEX::RIGHT_1503);
        if (-1 != iIndex)
            m_pModelComs[(_uint)PLAYER_MODEL_INDEX::RIGHT_1503]->SetupAnimation(iIndex, CAnimation::LOOP);
        else
            return E_FAIL;

        iIndex = GetAnimationIndexByAnimationName("IDLE", (_uint)PLAYER_MODEL_INDEX::RIGHT_1503);
        if (-1 != iIndex)
            m_pModelComs[(_uint)PLAYER_MODEL_INDEX::RIGHT_1503]->SetupAnimation(iIndex, CAnimation::LOOP);
        else
            return E_FAIL;
    }

    // 1508_Right
    {

        _int iIndex = GetAnimationIndexByAnimationName("WALK", (_uint)PLAYER_MODEL_INDEX::RIGHT_1508);
        if (-1 != iIndex)
            m_pModelComs[(_uint)PLAYER_MODEL_INDEX::RIGHT_1508]->SetupAnimation(iIndex, CAnimation::LOOP);
        else
            return E_FAIL;


        iIndex = GetAnimationIndexByAnimationName("IDLE", (_uint)PLAYER_MODEL_INDEX::RIGHT_1508);
        if (-1 != iIndex)
            m_pModelComs[(_uint)PLAYER_MODEL_INDEX::RIGHT_1508]->SetupAnimation(iIndex, CAnimation::LOOP);
        else
            return E_FAIL;
    }

    return S_OK;
}

void CPlayerBody::SetPlayAnimation(_uint iState)
{
    _int iIndex = -1;
    if (*m_pState & CStateObjectContainer::STATE_IDLE)
    {
        iIndex = GetAnimationIndexByAnimationName("IDLE", GetCurrentModelIndex());
        if (-1 != iIndex)
            m_pModelComs[GetCurrentModelIndex()]->SetupAnimation(iIndex, CAnimation::LOOP);
        else
            MSG_BOX("SetPlayAnimaion Index Error!");
    }
    else if (*m_pState & CStateObjectContainer::STATE_WALK)
    {
        iIndex = GetAnimationIndexByAnimationName("WALK", GetCurrentModelIndex());
        if (-1 != iIndex)
            m_pModelComs[GetCurrentModelIndex()]->SetupAnimation(iIndex, CAnimation::LOOP);
        else
            MSG_BOX("SetPlayAnimaion Index Error!");
    }
    else if (*m_pState & CStateObjectContainer::STATE_JUMP)
    {
        iIndex = GetAnimationIndexByAnimationName("JUMP", GetCurrentModelIndex());
        if (-1 != iIndex)
            m_pModelComs[GetCurrentModelIndex()]->SetupAnimation(iIndex, CAnimation::ONCE);
        else
            MSG_BOX("SetPlayAnimaion Index Error!");
    }
    else if (*m_pState & CStateObjectContainer::STATE_FIRE)
    {
        iIndex = GetAnimationIndexByAnimationName("FIRE", GetCurrentModelIndex());
        if (-1 != iIndex)
            m_pModelComs[GetCurrentModelIndex()]->SetupAnimation(iIndex, CAnimation::ONCE);
        else
            MSG_BOX("SetPlayAnimaion Index Error!");
    }
    else if (*m_pState & CStateObjectContainer::STATE_RELOAD)
    {
        iIndex = GetAnimationIndexByAnimationName("RELOAD", GetCurrentModelIndex());
        if (-1 != iIndex)
            m_pModelComs[GetCurrentModelIndex()]->SetupAnimation(iIndex, CAnimation::ONCE);
        else
            MSG_BOX("SetPlayAnimaion Index Error!");
    }
    else if (*m_pState & CStateObjectContainer::STATE_PUT)
    {
        iIndex = GetAnimationIndexByAnimationName("PUT", GetCurrentModelIndex());
        if (-1 != iIndex)
            m_pModelComs[GetCurrentModelIndex()]->SetupAnimation(iIndex, CAnimation::ONCE);
        else
            MSG_BOX("SetPlayAnimaion Index Error!");
    }
    else if (*m_pState & CStateObjectContainer::STATE_DRAW)
    {
        iIndex = GetAnimationIndexByAnimationName("DRAW", GetCurrentModelIndex());
        if (-1 != iIndex)
            m_pModelComs[GetCurrentModelIndex()]->SetupAnimation(iIndex, CAnimation::ONCE);
        else
            MSG_BOX("SetPlayAnimaion Index Error!");
    }
}

void CPlayerBody::ChangeAnimationByAnimIndex(_uint iFrom, _uint iTo, _uint iModelIndex)
{
    // 내가 직접 만든 애니매이션으로 부터 iTo로 변경해야하는 경우
    if (GetAnimIndex("IDLE", iModelIndex) == iFrom)
    {
        if (GetAnimIndex("WALK", iModelIndex) == iTo)
        {
            m_pModelComs[iModelIndex]->ResetAnimation(iTo);
            ChangeAnimationInterpolationByAnimIndex(iFrom, iTo, iModelIndex);
        }
        else if (GetAnimIndex("JUMP", iModelIndex) == iTo)
            m_pModelComs[iModelIndex]->SetInterpolationAnimationIndex(iFrom, iTo);
        else if (GetAnimIndex("FIRE", iModelIndex) == iTo)
            ChangeAnimationImmediatelyByAnimIndex(iTo, iModelIndex);
        else if (GetAnimIndex("PUT", iModelIndex) == iTo)
        {
            m_pModelComs[iModelIndex]->ResetAnimation(iTo);
            ChangeAnimationImmediatelyByAnimIndex(iTo, iModelIndex);
        }
        else if (GetAnimIndex("RELOAD", iModelIndex) == iTo)
        {
            m_pModelComs[iModelIndex]->ResetAnimation(iTo);
            ChangeAnimationImmediatelyByAnimIndex(iTo, iModelIndex);
        }
    }
    else if (GetAnimIndex("WALK", iModelIndex) == iFrom)
    {
        if (GetAnimIndex("IDLE", iModelIndex) == iTo)
        {
            ChangeAnimationInterpolationByAnimIndex(iFrom, iTo, iModelIndex);
        }
        else if (GetAnimIndex("FIRE", iModelIndex) == iTo)
        {
            ChangeAnimationImmediatelyByAnimIndex(iTo, iModelIndex);
        }
        else if (GetAnimIndex("RELOAD", iModelIndex) == iTo)
        {
            m_pModelComs[iModelIndex]->ResetAnimation(iTo);
            ChangeAnimationImmediatelyByAnimIndex(iTo, iModelIndex);
        }
        else if (GetAnimIndex("PUT", iModelIndex) == iTo)
        {
            m_pModelComs[iModelIndex]->ResetAnimation(iTo);
            ChangeAnimationImmediatelyByAnimIndex(iTo, iModelIndex);
        }
    }
    else if (GetAnimIndex("JUMP", iModelIndex) == iFrom)
    {

    }
    else if (GetAnimIndex("FIRE", iModelIndex) == iFrom)
    {
        if (iFrom == iTo)
        {
            if (1 == iModelIndex)
            {
                // m_pModelComs[iModelIndex]->InterpolationAnimationIndex();
                // m_pModelComs[iModelIndex]->SetInterpolationWithSameAnim();
                m_pModelComs[iModelIndex]->ResetCurrentAnimation();

            }
            else
            {
                m_pModelComs[iModelIndex]->ResetCurrentAnimation();
            }
        }
        else
            ChangeAnimationWhenFireEnd(iFrom, iTo, iModelIndex);
    }
    else if (GetAnimIndex("RELOAD", iModelIndex) == iFrom)
    {
        ChangeAnimationImmediatelyByAnimIndex(iTo, iModelIndex);
    }
    else if (GetAnimIndex("PUT", iModelIndex) == iFrom)
    {
        m_pModelComs[iModelIndex]->ResetAnimation(iFrom);

        SetCurrentModelIndex(m_pPlayer->GetNextDrawModelIndex());

        _uint iCurModelIdx = GetCurrentModelIndex();

        _int iNextAnimIndex = GetAnimationIndexByAnimationName("DRAW", iCurModelIdx);
        ChangeAnimationImmediatelyByAnimIndex(iNextAnimIndex, iCurModelIdx);
    }
    else if (GetAnimIndex("DRAW", iModelIndex) == iFrom)
    {
        m_pModelComs[iModelIndex]->ResetAnimation(iFrom);
        ChangeAnimationWhenDrawEnd(iFrom, iTo, iModelIndex);
    }
}

void CPlayerBody::AnimationEnd(_uint iCurrentModelIndex)
{
    _int iCurrentEndAnimIndex = GetAnimationIndexByState(*m_pState, iCurrentModelIndex);

    if (GetAnimationIndexByAnimationName("IDLE", iCurrentModelIndex) == iCurrentEndAnimIndex)
    {
        if (m_bIsInterpolationing)
        {
            m_pModelComs[iCurrentModelIndex]->ResetAnimationTrackPosition(iCurrentEndAnimIndex);
        }
    }
    else if (GetAnimationIndexByAnimationName("WALK", iCurrentModelIndex) == iCurrentEndAnimIndex)
    {

    }
    else if (GetAnimationIndexByAnimationName("RELOAD", iCurrentModelIndex) == iCurrentEndAnimIndex)
    {
        m_pPlayer->SetCurrentEndAnimationName("RELOAD");
        m_strCurrentEndAnimName = "RELOAD";
    }
    else if (GetAnimationIndexByAnimationName("FIRE", iCurrentModelIndex) == iCurrentEndAnimIndex)
    {
        //// 끝난 애니매이션이 FIRE인데 보간중이 아니였다면 -> FIRE를 그대로 재생해서 끝난 상태이다.
        //if (false == m_bIsInterpolationing)
        //{
        //    // FIRE 애니매이션은 ONCE이기 때문에 여기서 Reset해준다.
        //    m_pModelComs[iCurrentModelIndex]->ResetCurrentAnimation();

        //    // 다음틱에 플레이어가 FIRE애니매이션이 끝났다고 인지할 수 있도록 값을 설정한다. -> 플레이어 UpdateEndAnimation에서 확인후, 상태를 변경한다.
        //    m_pPlayer->SetCurrentEndAnimationName("FIRE");

        //    // 현재 끝난 애니매이션의 이름을 설정한다.
        //    m_strCurrentEndAnimName = "FIRE";
        //}

        m_pModelComs[iCurrentModelIndex]->ResetCurrentAnimation();
        m_pPlayer->SetCurrentEndAnimationName("FIRE");
        m_strCurrentEndAnimName = "FIRE";


        if (1 == GetCurrentModelIndex())
        {
            // m_pModelComs[(_uint)PLAYER_MODEL_INDEX::RIGHT_1004]->SetTransformationMatriciesFirstTime(GetAnimationIndexByAnimationName("FIRE", (_uint)PLAYER_MODEL_INDEX::RIGHT_1004), 10.f);
        }


    }
    else if (GetAnimationIndexByAnimationName("PUT", iCurrentModelIndex) == iCurrentEndAnimIndex)
    {
        m_pModelComs[iCurrentModelIndex]->ResetCurrentAnimation();
        m_pPlayer->SetCurrentEndAnimationName("PUT");
        m_strCurrentEndAnimName = "PUT";
    }
    else if (GetAnimationIndexByAnimationName("DRAW", iCurrentModelIndex) == iCurrentEndAnimIndex)
    {
        m_pModelComs[iCurrentModelIndex]->ResetCurrentAnimation();
        m_pPlayer->SetCurrentEndAnimationName("DRAW");
        m_strCurrentEndAnimName = "DRAW";
    }
}

void CPlayerBody::ChangeAnimationInterpolationByAnimIndex(_uint iFrom, _uint iTo, _uint iCurrentModelIndex)
{
    m_pModelComs[iCurrentModelIndex]->SetInterpolationAnimationIndex(iFrom, iTo);
    m_bIsInterpolationing = true;
}


void CPlayerBody::ChangeAnimationWhenFireEnd(_uint iFrom, _uint iTo, _uint iModelIndex)
{
    if (GetAnimIndex("IDLE", iModelIndex) == iTo)
    {
        ChangeAnimationImmediatelyByAnimIndex(iTo, iModelIndex);
        m_pModelComs[iModelIndex]->ResetInterpolation();
        m_pModelComs[iModelIndex]->ResetAnimation(iTo);

    }
    else if (GetAnimIndex("WALK", iModelIndex) == iTo)
    {
        ChangeAnimationImmediatelyByAnimIndex(iTo, iModelIndex);
        m_pModelComs[iModelIndex]->ResetInterpolation();
        m_pModelComs[iModelIndex]->ResetAnimation(iTo);

    }
}

void CPlayerBody::ChangeAnimationWhenPutEnd(_uint iFrom, _uint iTo, _uint iModelIndex)
{
    // 아래 경우 하나밖에 없다.
    if (GetAnimIndex("DRAW", iModelIndex) == iTo)
    {
        ChangeAnimationImmediatelyByAnimIndex(iTo, iModelIndex);
    }
}

void CPlayerBody::ChangeAnimationWhenDrawEnd(_uint iFrom, _uint iTo, _uint iModelIndex)
{
    if (GetAnimIndex("IDLE", iModelIndex) == iTo)
    {
        ChangeAnimationImmediatelyByAnimIndex(iTo, iModelIndex);
    }
    else if (GetAnimIndex("WALK", iModelIndex) == iTo)
    {
        ChangeAnimationImmediatelyByAnimIndex(iTo, iModelIndex);
    }
}

_uint CPlayerBody::GetAnimIndex(string strAnimName, _uint iModelIndex)
{
    if ((_uint)PLAYER_MODEL_INDEX::RIGHT_1201 == iModelIndex)
    {
        if (false == strcmp("IDLE", strAnimName.c_str()))
            return (_uint)PLAYER_MODEL_ANIM_INDEX_1201::IDLE;
        if (false == strcmp("WALK", strAnimName.c_str()))
            return (_uint)PLAYER_MODEL_ANIM_INDEX_1201::WALK;
        if (false == strcmp("JUMP", strAnimName.c_str()))
            return (_uint)PLAYER_MODEL_ANIM_INDEX_1201::JUMP;
        if (false == strcmp("FIRE", strAnimName.c_str()))
            return (_uint)PLAYER_MODEL_ANIM_INDEX_1201::FIRE;
        if (false == strcmp("RELOAD", strAnimName.c_str()))
            return (_uint)PLAYER_MODEL_ANIM_INDEX_1201::RELOAD;
        if (false == strcmp("PUT", strAnimName.c_str()))
            return (_uint)PLAYER_MODEL_ANIM_INDEX_1201::PUT;
        if (false == strcmp("DRAW", strAnimName.c_str()))
            return (_uint)PLAYER_MODEL_ANIM_INDEX_1201::DRAW;

    }
    else if ((_uint)PLAYER_MODEL_INDEX::RIGHT_1004 == iModelIndex)
    {
        if (false == strcmp("IDLE", strAnimName.c_str()))
            return (_uint)PLAYER_MODEL_ANIM_INDEX_1004::IDLE;
        if (false == strcmp("WALK", strAnimName.c_str()))
            return (_uint)PLAYER_MODEL_ANIM_INDEX_1004::WALK;
        if (false == strcmp("JUMP", strAnimName.c_str()))
            return (_uint)PLAYER_MODEL_ANIM_INDEX_1004::JUMP;
        if (false == strcmp("FIRE", strAnimName.c_str()))
            return (_uint)PLAYER_MODEL_ANIM_INDEX_1004::FIRE;
        if (false == strcmp("RELOAD", strAnimName.c_str()))
            return (_uint)PLAYER_MODEL_ANIM_INDEX_1004::RELOAD;
        if (false == strcmp("PUT", strAnimName.c_str()))
            return (_uint)PLAYER_MODEL_ANIM_INDEX_1004::PUT;
        if (false == strcmp("DRAW", strAnimName.c_str()))
            return (_uint)PLAYER_MODEL_ANIM_INDEX_1004::DRAW;
    }
    else if ((_uint)PLAYER_MODEL_INDEX::RIGHT_1503 == iModelIndex)
    {
        if (false == strcmp("IDLE", strAnimName.c_str()))
            return (_uint)PLAYER_MODEL_ANIM_INDEX_1503::IDLE;
        if (false == strcmp("WALK", strAnimName.c_str()))
            return (_uint)PLAYER_MODEL_ANIM_INDEX_1503::WALK;
        if (false == strcmp("JUMP", strAnimName.c_str()))
            return (_uint)PLAYER_MODEL_ANIM_INDEX_1503::JUMP;
        if (false == strcmp("FIRE", strAnimName.c_str()))
            return (_uint)PLAYER_MODEL_ANIM_INDEX_1503::FIRE;
        if (false == strcmp("RELOAD", strAnimName.c_str()))
            return (_uint)PLAYER_MODEL_ANIM_INDEX_1503::RELOAD;
        if (false == strcmp("PUT", strAnimName.c_str()))
            return (_uint)PLAYER_MODEL_ANIM_INDEX_1503::PUT;
        if (false == strcmp("DRAW", strAnimName.c_str()))
            return (_uint)PLAYER_MODEL_ANIM_INDEX_1503::DRAW;
    }
    else if ((_uint)PLAYER_MODEL_INDEX::RIGHT_1508 == iModelIndex)
    {
        if (false == strcmp("IDLE", strAnimName.c_str()))
            return (_uint)PLAYER_MODEL_ANIM_INDEX_1508::IDLE;
        if (false == strcmp("WALK", strAnimName.c_str()))
            return (_uint)PLAYER_MODEL_ANIM_INDEX_1508::WALK;
        if (false == strcmp("JUMP", strAnimName.c_str()))
            return (_uint)PLAYER_MODEL_ANIM_INDEX_1508::JUMP;
        if (false == strcmp("FIRE", strAnimName.c_str()))
            return (_uint)PLAYER_MODEL_ANIM_INDEX_1508::FIRE;
        if (false == strcmp("RELOAD", strAnimName.c_str()))
            return (_uint)PLAYER_MODEL_ANIM_INDEX_1508::RELOAD;
        if (false == strcmp("PUT", strAnimName.c_str()))
            return (_uint)PLAYER_MODEL_ANIM_INDEX_1508::PUT;
        if (false == strcmp("DRAW", strAnimName.c_str()))
            return (_uint)PLAYER_MODEL_ANIM_INDEX_1508::DRAW;
    }

    MSG_BOX("GetAnimIndex Error!");
    return 999;
}

const _float4x4* CPlayerBody::GetModelSocketMatrix(string strSocketBoneName, _uint iModelIndex)
{
    if (nullptr != m_pModelComs[iModelIndex])
        return m_pModelComs[iModelIndex]->GetBoneCombinedMatrix(strSocketBoneName.c_str());
    return nullptr;
}

_bool CPlayerBody::CanChangeAnimation()
{
    if (m_iPrevTickState == *m_pState)
    {
        if (m_pPlayer->GetResetState() & CPlayer::FIRE)
        {
            m_pPlayer->SetResetState(0);
            return true;
        }

        return false;
    }

    // 이전에 끝난 애니매이션의 이름이 FIRE인경우 변경할 수 있도록 한다.
    if (false == strcmp("FIRE", m_strCurrentEndAnimName.c_str()))
    {
        ResetCurrentEndAnimName();
        return true;
    }
    else if (false == strcmp("PUT", m_strCurrentEndAnimName.c_str()))
    {
        // ResetCurrentEndAnimName();
        return true;
    }
    else if (false == strcmp("DRAW", m_strCurrentEndAnimName.c_str()))
    {
        ResetCurrentEndAnimName();
        return true;
    }
    else if (false == strcmp("RELOAD", m_strCurrentEndAnimName.c_str()))
    {
        ResetCurrentEndAnimName();
        return true;
    }

    _int    iCurModelIdx = (_int)GetCurrentModelIndex();
    _int    iNextAnimIndex = (_int)GetAnimationIndexByState(*m_pState, iCurModelIdx);
    _int    iCurPlayingAnimIndex = m_pModelComs[GetCurrentModelIndex()]->GetCurrentAnimationIndex();
    _bool   bIsPlaying = m_pModelComs[GetCurrentModelIndex()]->IsAnimationPlaying();

    // 변경할 애니매이션이 IDLE인경우
    if (GetAnimIndex("IDLE", iCurModelIdx) == iNextAnimIndex)
    {
        if (GetAnimIndex("WALK", iCurModelIdx) == iCurPlayingAnimIndex)
        {
            m_iInterpolationIndex = iNextAnimIndex;
            m_iCurAnimIndex = iNextAnimIndex;
            return true;
        }
        // FIRE가 재생중인 경우 변경하지 못하도록 한다.
        else if (GetAnimIndex("FIRE", iCurModelIdx) == iCurPlayingAnimIndex)
            return false;
        // PUT이 재생중인 경우 변경하지 못하도록 한다.
        else if (GetAnimIndex("PUT", iCurModelIdx) == iCurPlayingAnimIndex)
            return false;
        // DRAW가 재생중인 경우 변경하지 못하도록 한다.
        else if (GetAnimIndex("DRAW", iCurModelIdx) == iCurPlayingAnimIndex)
            return false;

        m_iInterpolationIndex = -1;
        return false;
    }
    // 변경할 애니매이션이 WALK인경우
    else if (GetAnimIndex("WALK", iCurModelIdx) == iNextAnimIndex)
    {
        if (GetAnimIndex("IDLE", iCurModelIdx) == iCurPlayingAnimIndex)
        {
            m_iInterpolationIndex = iNextAnimIndex;
            m_iCurAnimIndex = iNextAnimIndex;
            return true;
        }
        // FIRE가 재생중인 경우 변경하지 못하도록 한다.
        if (GetAnimIndex("FIRE", iCurModelIdx) == iCurPlayingAnimIndex)
        {
            return false;
        }
        // PUT이 재생중인 경우 변경하지 못하도록 한다.
        else if (GetAnimIndex("PUT", iCurModelIdx) == iCurPlayingAnimIndex)
        {
            return false;
        }
        // DRAW가 재생중인 경우 변경하지 못하도록 한다.
        if (GetAnimIndex("DRAW", iCurModelIdx) == iCurPlayingAnimIndex)
        {
            return false;
        }

        m_iInterpolationIndex = -1;
        return false;
    }
    // 변경할 애니매이션이 FIRE인경우
    else if (GetAnimIndex("FIRE", iCurModelIdx) == iNextAnimIndex)
    {
        // IDLE -> WALK || WALK -> IDLE로 보간중일 때 호출되는 경우
        if (-1 != m_iInterpolationIndex)
        {
            // IDLE이 재생중인 경우 변경할 수 있도록 한다.
            if (GetAnimIndex("IDLE", iCurModelIdx) == iCurPlayingAnimIndex)
            {
                m_iCurAnimIndex = iNextAnimIndex;
                return true;
            }
            // WALK가 재생중인경우 변경할 수 있도록 한다.
            else if (GetAnimIndex("WALK", iCurModelIdx) == iCurPlayingAnimIndex)
            {
                m_iCurAnimIndex = iNextAnimIndex;
                return true;
            }
            m_iInterpolationIndex = -1;
        }
        else
        {
            // IDLE이 재생중인 경우 변경할 수 있도록 한다.
            if (GetAnimIndex("IDLE", iCurModelIdx) == iCurPlayingAnimIndex)
            {
                m_iCurAnimIndex = iNextAnimIndex;
                return true;
            }
            // WALK가 재생중인경우 변경할 수 있도록 한다.
            else if (GetAnimIndex("WALK", iCurModelIdx) == iCurPlayingAnimIndex)
            {
                m_iCurAnimIndex = iNextAnimIndex;
                return true;
            }
            // 현재 재생중인 애니매이션이 DRAW인경우 변경할 수 없도록 한다.
            else if (GetAnimIndex("DRAW", iCurModelIdx) == iCurPlayingAnimIndex)
            {
                return false;
            }
            // 현재 재생중인 애니매이션이 PUT인경우 변경할 수 없도록 한다.
            else if (GetAnimIndex("PUT", iCurModelIdx) == iCurPlayingAnimIndex)
            {
                return false;
            }
            m_iInterpolationIndex = -1;
        }
        
        return false;
    }
    // 변경할 애니매이션이 RELOAD인경우
    else if (GetAnimIndex("RELOAD", iCurModelIdx) == iNextAnimIndex)
    {
        if (GetAnimIndex("RELOAD", iCurModelIdx) == iCurPlayingAnimIndex)
                return false;
        if (GetAnimIndex("FIRE", iCurModelIdx) == iCurPlayingAnimIndex)
            return false;
        else if (GetAnimIndex("DRAW", iCurModelIdx) == iCurPlayingAnimIndex)
            return false;
        else if (GetAnimIndex("PUT", iCurModelIdx) == iCurPlayingAnimIndex)
            return false;

        m_iInterpolationIndex = -1;
        m_iCurAnimIndex = iNextAnimIndex;
        return true;
    }
    else if (GetAnimIndex("PUT", iCurModelIdx) == iNextAnimIndex)
    {
        m_iCurAnimIndex = iNextAnimIndex;
        return true;
    }
    else if (GetAnimIndex("DRAW", iCurModelIdx) == iNextAnimIndex)
    {
        if (false == strcmp("PUT", m_strCurrentEndAnimName.c_str()))
        {
            ResetCurrentEndAnimName();
            return true;
        }
    }
        
    return false;
}

void CPlayerBody::FindAlertPoint(_uint iCurState, _uint iState, string strCompAnimName, string AlertPointName)
{
    if (iCurState & iState)
    {
        _int iCurAttackAnimIndex = GetAnimationIndexByState(iState);
        if (false == strcmp(strCompAnimName.c_str(), GetAnimationNameByAnimationIndex(iCurAttackAnimIndex).c_str()))
        {
            string strCurrentAlertName = m_pModelComs[GetCurrentModelIndex()]->GetAnimation(iCurAttackAnimIndex)->GetAlertPointName();
            if (false == strcmp(AlertPointName.c_str(), strCurrentAlertName.c_str()))
                m_pPlayer->SetCurrentAlertAnimationName(AlertPointName);
        }
    }
}

void CPlayerBody::CheckAlertPointSoundName()
{
    string strCurName = m_pModelComs[GetCurrentModelIndex()]->GetAlertPointSoundName();

    if (m_strPrevAlertName == strCurName)
    {
        _uint iTmp = GetCurrentModelIndex();
        return;
    }

    _int iCurAttackAnimIndex = GetAnimationIndexByState(*m_pState);
    string str = GetAnimationNameByAnimationIndex(iCurAttackAnimIndex, GetCurrentModelIndex());

    _uint iCur = GetCurrentModelIndex();



    if (iCur == 0)
    {
        if (str == "RELOAD")
        {
            if (strCurName == "RELOAD_START")
            {
                m_pGameInst->PlayEffect(TEXT("1205_ReloadStart.ogg"), 1.f);
            }

            else if (strCurName == "RELOAD_END")
            {
                m_pGameInst->PlayEffect(TEXT("1205_ReloadEnd.ogg"), 1.f);
            }

            else if (strCurName == "RELOAD_FINAL")
            {
                m_pGameInst->PlayEffect(TEXT("1205_ReloadFinal.ogg"), 1.f);
            }
        }
        else if (str == "PUT")
        {
            if (strCurName == "PUT")
                m_pGameInst->PlayEffect(TEXT("GunPut.mp3"), 1.f);

        }
        else if (str == "DRAW")
        {
            if (strCurName == "PUT")
                m_pGameInst->PlayEffect(TEXT("GunDraw.mp3"), 1.f);
        }
    }
    else if (iCur == 1)
    {

        if (strCurName == "RELOAD")
        {
            m_pGameInst->PlayEffect(TEXT("1004_Reload.ogg"), 1.f);
        }
        else if (strCurName == "PUT")
        {
            m_pGameInst->PlayEffect(TEXT("GunPut.mp3"), 1.f);

        }
        else if (strCurName == "DRAW")
        {
            m_pGameInst->PlayEffect(TEXT("GunDraw.mp3"), 1.f);

        }
    }
    else if (iCur == 2)
    {
        if (strCurName == "RELOAD")
        {
            m_pGameInst->PlayEffect(TEXT("1503_Reload.ogg"), 1.f);
        }
        else if (strCurName == "PUT")
        {
            m_pGameInst->PlayEffect(TEXT("ArrpwPut.mp3"), 1.f);

        }
        else if (strCurName == "DRAW")
        {
            m_pGameInst->PlayEffect(TEXT("ArrowDraw.mp3"), 1.f);

        }
    }
    else if (iCur == 3)
    {
        if (strCurName == "RELOAD_START")
            m_pGameInst->PlayEffect(TEXT("1508_ReloadStart.ogg"), 1.f);
        else if (strCurName == "RELOAD_DO")
            m_pGameInst->PlayEffect(TEXT("1508_ReloadDo.ogg"), 3.f);
        else if (strCurName == "RELOAD_END")
        {
            // m_pGameInst->PlayEffect(TEXT("1508_ReloadEnd.ogg"), 3.f);
            m_pGameInst->PlayEffect(TEXT("1508_ReloadFinal2.mp3"), 1.f);
        }
        else if (strCurName == "PUT")
        {
            m_pGameInst->PlayEffect(TEXT("ArrpwPut.mp3"), 1.f);

        }
        else if (strCurName == "DRAW")
        {
            m_pGameInst->PlayEffect(TEXT("ArrowDraw.mp3"), 1.f);

        }
    }

    m_strPrevAlertName = strCurName;
}

HRESULT CPlayerBody::ReadyComponents()
{
    m_pModelComs.assign(m_iNumModels, nullptr);
    m_ModelsAnimationNames.resize(m_iNumModels);

    // Com_Shader
    {
        if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
            return E_FAIL;
    }

    // Com_Model
    {
        if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Model_Player_1201"), TEXT("Com_Model_1201"), reinterpret_cast<CComponent**>(&m_pModelComs[(_uint)PLAYER_MODEL_INDEX::RIGHT_1201]))))
            return E_FAIL;

        if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Model_Player_1004"), TEXT("Com_Model_1004"), reinterpret_cast<CComponent**>(&m_pModelComs[(_uint)PLAYER_MODEL_INDEX::RIGHT_1004]))))
            return E_FAIL;

        if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Model_Player_1503"), TEXT("Com_Model_1503"), reinterpret_cast<CComponent**>(&m_pModelComs[(_uint)PLAYER_MODEL_INDEX::RIGHT_1503]))))
            return E_FAIL;

        if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Model_Player_1508"), TEXT("Com_Model_1508"), reinterpret_cast<CComponent**>(&m_pModelComs[(_uint)PLAYER_MODEL_INDEX::RIGHT_1508]))))
            return E_FAIL;
    }
    
    return S_OK;
}

HRESULT CPlayerBody::BindShaderResources()
{
    if (FAILED(m_pShaderCom->BindMatrix("g_WorldMatrix", &m_WorldMatrix))) 
        return E_FAIL;

    _float4x4 viewMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_VIEW);
    _float4x4 projMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_PROJ);

    if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &viewMatrix))) 
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &projMatrix))) 
        return E_FAIL;

    return S_OK;
}

CPlayerBody* CPlayerBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPlayerBody* pInstance = new CPlayerBody(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CPlayerBody");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayerBody::Clone(void* pArg)
{
    CPlayerBody* pInstance = new CPlayerBody(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CPlayerBody");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CPlayerBody::Free()
{
    __super::Free();

    SafeRelease(m_pShaderCom);

    for (auto& pModel : m_pModelComs)
        if (nullptr != pModel)
            SafeRelease(pModel);

    m_pModelComs.clear();
}