#include "pch.h"
#include "StateObjectBody.h"
#include "GameInstance.h"
#include "Model.h"
#include "StateObjectContainer.h"

CStateObjectBody::CStateObjectBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CPartObject(pDevice, pContext)
{
}

CStateObjectBody::CStateObjectBody(const CStateObjectBody& Prototype)
    :
    CPartObject(Prototype)
{
}

HRESULT CStateObjectBody::InitializePrototype()
{
    return S_OK;
}

HRESULT CStateObjectBody::Initialize(void* pArg)
{
    STATE_OBJECT_BODY_DESC* pDesc = static_cast<STATE_OBJECT_BODY_DESC*>(pArg);

    if (nullptr != pDesc)
    {
        m_pState = pDesc->pState;
        m_pPrevState = pDesc->pPrevState;
        m_iNumModels = pDesc->iNumModels;
        m_pOwner = pDesc->pOwner;
        m_iNumModels = pDesc->iNumModels;

        SafeAddRef(m_pOwner);
    }

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    return S_OK;
}

void CStateObjectBody::PriorityUpdate(_float fTimeDelta)
{

}

void CStateObjectBody::Update(_float fTimeDelta)
{

}

void CStateObjectBody::LateUpdate(_float fTimeDelta)
{

}

HRESULT CStateObjectBody::Render()
{
    return S_OK;
}

CModel* CStateObjectBody::GetModelByName(string strName)
{
    auto iter = find_if(m_pModelComs.begin(), m_pModelComs.end(), [&strName](CModel* pModel) 
        {
            if (false == strcmp(pModel->GetModelName().c_str(), strName.c_str()))
                return true;
            return false;
        }
    );

    if (iter != m_pModelComs.end())
        return *iter;
    return nullptr;
}

_int CStateObjectBody::GetAnimationIndexByState(_uint iState, _uint iModelIndex)
{
    if (iModelIndex >= m_iNumModels)
        return -1;
    
    string strCurrentAnimationName = GetAnimationNameByState(iState);
    if (false == strcmp(strCurrentAnimationName.c_str(), "NONE"))
    {
        MSG_BOX("GetModelAnimationIndexByState Error, 잘못된 iState 값");
        return -1;
    }
    else 
        return GetAnimationIndexByAnimationName(strCurrentAnimationName, iModelIndex);
}

string CStateObjectBody::GetAnimationNameByAnimationName(string strAnimKeyName, _uint iModelIndex)
{
    auto iter = m_ModelsAnimationNames[iModelIndex].find(strAnimKeyName);
    if (iter != m_ModelsAnimationNames[iModelIndex].end())
        return (*iter).first;
    return "";
}

_int CStateObjectBody::GetAnimationIndexByAnimationName(string strAnimName, _uint iModelIndex)
{
    if (iModelIndex >= m_iNumModels)
        return -1;
        
    auto iter = m_ModelsAnimationNames[iModelIndex].find(strAnimName);
    if (iter == m_ModelsAnimationNames[iModelIndex].end())
        return -1;
    return (*iter).second;
}

string CStateObjectBody::GetAnimationNameByState(_uint iState)
{
    if (CStateObjectContainer::STATE_IDLE & iState) 
        return "IDLE";
    else if (CStateObjectContainer::STATE_WALK & iState) 
        return "WALK";
    else if (CStateObjectContainer::STATE_JUMP & iState)
        return "JUMP";
    else if (CStateObjectContainer::STATE_FIRE & iState)
        return "FIRE";
    else if (CStateObjectContainer::STATE_RELOAD & iState)
        return "RELOAD";
    else if (CStateObjectContainer::STATE_PUT & iState)
        return "PUT";
    else if (CStateObjectContainer::STATE_DRAW & iState)
        return "DRAW";
    

    return "NONE";
}

string CStateObjectBody::GetAnimationNameByAnimationIndex(_int iAnimationIndex, _uint iModelIndex)
{
    if (iModelIndex >= m_iNumModels)
        return "NONE";

    auto iter = find_if(m_ModelsAnimationNames[iModelIndex].begin(), m_ModelsAnimationNames[iModelIndex].end(), [&](const pair<string, _int>& pair) 
        {
            if (pair.second == iAnimationIndex)
                return true;
            return false;
        });

    if (iter != m_ModelsAnimationNames[iModelIndex].end())
        return iter->first;
    return "NONE";
}


HRESULT CStateObjectBody::ReadyComponents()
{
    return S_OK;
}

HRESULT CStateObjectBody::BindShaderResources()
{
    return S_OK;
}

void CStateObjectBody::Free()
{
    __super::Free();

    if (nullptr != m_pOwner)
        SafeRelease(m_pOwner);
}
