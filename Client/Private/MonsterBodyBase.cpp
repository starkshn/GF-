#include "pch.h"
#include "MonsterBodyBase.h"
#include "MonsterBase.h"


CMonsterBodyBase::CMonsterBodyBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CStateObjectBody(pDevice, pContext)
{

}

CMonsterBodyBase::CMonsterBodyBase(const CMonsterBodyBase& Prototype)
    :
    CStateObjectBody(Prototype)
{

}

HRESULT CMonsterBodyBase::InitializePrototype()
{
    return S_OK;
}
HRESULT CMonsterBodyBase::Initialize(void* pArg)
{
    if (nullptr != pArg)
    {
        STATE_OBJECT_BODY_DESC*  pDesc = static_cast<STATE_OBJECT_BODY_DESC*>(pArg);
        m_pOwnerMonster = static_cast<CMonsterBase*>(pDesc->pOwner);
        if (nullptr == m_pOwnerMonster)
            return E_FAIL;
    }

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CMonsterBodyBase::PriorityUpdate(_float fTimeDelta)
{

}

void CMonsterBodyBase::Update(_float fTimeDelta)
{

}

void CMonsterBodyBase::LateUpdate(_float fTimeDelta)
{

}

HRESULT CMonsterBodyBase::Render()
{
    return S_OK;
}

void CMonsterBodyBase::SetPlayAnimation(_uint iState)
{
    _int iCurAnimIndex = 0;

    if (*m_pState & CMonsterBase::MONSTER_STATE::MS_IDLE)
    {
        iCurAnimIndex = GetAnimationIndexByAnimationName("IDLE", GetCurrentModelIndex());
        if (-1 != iCurAnimIndex)
            m_pModelComs[GetCurrentModelIndex()]->SetupAnimation(iCurAnimIndex, CAnimation::LOOP);
        else
            MSG_BOX("SetPlayAnimaion Index Error!");
    }
    else if (*m_pState & CMonsterBase::MONSTER_STATE::MS_CHASE)
    {
        iCurAnimIndex = GetAnimationIndexByAnimationName("CHASE", GetCurrentModelIndex());
        if (-1 != iCurAnimIndex)
            m_pModelComs[GetCurrentModelIndex()]->SetupAnimation(iCurAnimIndex, CAnimation::LOOP);
        else
            MSG_BOX("SetPlayAnimaion Index Error!");
    }
    else if (*m_pState & CMonsterBase::MONSTER_STATE::MS_DEACTIVE)
    {
        iCurAnimIndex = GetAnimationIndexByAnimationName("SPAWN", GetCurrentModelIndex());
        if (-1 != iCurAnimIndex)
            m_pModelComs[GetCurrentModelIndex()]->SetupAnimation(iCurAnimIndex, CAnimation::ONCE);
        else
            MSG_BOX("SetPlayAnimaion Index Error!");
    }
    else if (*m_pState & CMonsterBase::MONSTER_STATE::MS_ATTACK)
    {
        // TODO 
        // 특정 조건에 따라서 ATTACK01, ATTACK02중 선택해야한다.


        iCurAnimIndex = GetAnimationIndexByAnimationName("ATTACK01", GetCurrentModelIndex());
        if (-1 != iCurAnimIndex)
            m_pModelComs[GetCurrentModelIndex()]->SetupAnimation(iCurAnimIndex, CAnimation::ONCE);
        else
            MSG_BOX("SetPlayAnimaion Index Error!");
    }
    else if (*m_pState & CMonsterBase::MONSTER_STATE::MS_GETHIT)
    {
        iCurAnimIndex = GetAnimationIndexByAnimationName("GETHIT", GetCurrentModelIndex());
        if (-1 != iCurAnimIndex)
            m_pModelComs[GetCurrentModelIndex()]->SetupAnimation(iCurAnimIndex, CAnimation::ONCE);
        else
            MSG_BOX("SetPlayAnimaion Index Error!");
    }
    else if (*m_pState & CMonsterBase::MONSTER_STATE::MS_ROLL_RIGHT)
    {
        // TODO
        // 특정 조건에 따라서 ROLLRIGHT, ROOLLEFT중 선택해야한다.

        iCurAnimIndex = GetAnimationIndexByAnimationName("ROLLRIGHT", GetCurrentModelIndex());
        if (-1 != iCurAnimIndex)
            m_pModelComs[GetCurrentModelIndex()]->SetupAnimation(iCurAnimIndex, CAnimation::ONCE);
        else
            MSG_BOX("SetPlayAnimaion Index Error!");
    }
    else if (*m_pState & CMonsterBase::MONSTER_STATE::MS_ROLL_LEFT)
    {
        // TODO
        // 특정 조건에 따라서 ROLLRIGHT, ROOLLEFT중 선택해야한다.

        iCurAnimIndex = GetAnimationIndexByAnimationName("ROLLLEFT", GetCurrentModelIndex());
        if (-1 != iCurAnimIndex)
            m_pModelComs[GetCurrentModelIndex()]->SetupAnimation(iCurAnimIndex, CAnimation::ONCE);
        else
            MSG_BOX("SetPlayAnimaion Index Error!");
    }
}

void CMonsterBodyBase::ChangeAnimation(_uint iState, _uint iCurrentModelIndex)
{
    // 현재 애니매이션 재생중이라면
    if (m_pModelComs[iCurrentModelIndex]->IsAnimationPlaying())
        ChangeAnimationWhenPlaying(iState, iCurrentModelIndex);
}

void CMonsterBodyBase::ChangeAnimation(_uint iFrom, _uint iTo, _uint iCurrentModelIndex)
{
    m_pModelComs[iCurrentModelIndex]->SetInterpolationAnimationIndex(iFrom, iTo);
    m_bIsInterpolationing = true;
}

void CMonsterBodyBase::ChangeAnimationByAnimIndex(_uint iFrom, _uint iTo, _uint iCurrentModelIndex)
{
    m_pModelComs[iCurrentModelIndex]->SetInterpolationAnimationIndex(iFrom, iTo);
    m_bIsInterpolationing = true;
}


void CMonsterBodyBase::ChangeAnimationWhenEnd(_uint iCurrentModelIndex)
{
    // TODO
}

void CMonsterBodyBase::ChangeAnimationWhenPlaying(_uint iState, _uint iCurrentModelIndex)
{
    ChangeAnimationInterpolation();
}

void CMonsterBodyBase::ChangeAnimationImmediately(_uint iState, _uint iCurrentModelIndex)
{
    // TODO
}

void CMonsterBodyBase::ChangeAnimationInterpolation(_uint iCurrentModelIndex)
{
    _int iFrom = GetAnimationIndexByState(m_iPrevTickState, iCurrentModelIndex);
    _int iTo = GetAnimationIndexByState(*m_pState, iCurrentModelIndex);
    m_pModelComs[iCurrentModelIndex]->SetInterpolationAnimationIndex(iFrom, iTo);
    m_bIsInterpolationing = true;
}

string CMonsterBodyBase::GetAnimationNameByState(_uint iState)
{
    if (CMonsterBase::MONSTER_STATE::MS_IDLE & iState)
        return "IDLE";
    else if (CMonsterBase::MONSTER_STATE::MS_CHASE & iState)
        return "CHASE";
    else if (CMonsterBase::MONSTER_STATE::MS_ATTACK & iState)
        return "ATTACK01";
    else if (CMonsterBase::MONSTER_STATE::MS_DEACTIVE & iState)
        return "SPAWN";
    else if (CMonsterBase::MONSTER_STATE::MS_GETHIT & iState)
        return "GETHIT";
    else if (CMonsterBase::MONSTER_STATE::MS_ROLL_LEFT & iState)
        return "ROLLLEFT";
    else if (CMonsterBase::MONSTER_STATE::MS_ROLL_RIGHT & iState)
        return "ROLLRIGHT";
    else if (CMonsterBase::MONSTER_STATE::MS_DEACTIVE & iState)
        return "DEACTIVE";
    else if (CMonsterBase::MONSTER_STATE::MS_ACTIVE & iState)
        return "ACTIVE";

    return "NONE";
}

void CMonsterBodyBase::FindAlertPoint(_uint iMonsterState, _uint iState, string strCompAnimName, string AlertPointName)
{
    if (iMonsterState & iState)
    {
        _int iCurAttackAnimIndex = GetAnimationIndexByState(iState);
        if (false == strcmp(strCompAnimName.c_str(), GetAnimationNameByAnimationIndex(iCurAttackAnimIndex).c_str()))
        {
            string strCurrentAlertName = m_pModelComs[GetCurrentModelIndex()]->GetAnimation(iCurAttackAnimIndex)->GetAlertPointName();
            if (false == strcmp(AlertPointName.c_str(), strCurrentAlertName.c_str()))
            {
                if (0 == m_iAlertCnt[iCurAttackAnimIndex])
                {
                    m_iAlertCnt[iCurAttackAnimIndex]++;
                    m_pOwnerMonster->SetAlertPointName(AlertPointName);
                    cout << "Monster Alert : " << " " << AlertPointName << endl;
                }
            }
        }
    }
}


HRESULT CMonsterBodyBase::ReadyComponents()
{
    return S_OK;
}

HRESULT CMonsterBodyBase::BindShaderResources()
{
    return S_OK;
}

void CMonsterBodyBase::Free()
{
    __super::Free();
}

