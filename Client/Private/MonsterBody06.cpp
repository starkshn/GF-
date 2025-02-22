#include "pch.h"
#include "MonsterBody06.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster02.h"
#include "Animation.h"

CMonsterBody06::CMonsterBody06(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CMonsterBodyBase(pDevice, pContext)
{
}

CMonsterBody06::CMonsterBody06(const CMonsterBody06& Prototype)
    :
    CMonsterBodyBase(Prototype)
{
}

HRESULT CMonsterBody06::InitializePrototype()
{
    return S_OK;
}

HRESULT CMonsterBody06::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(ReadyComponents()))
		return E_FAIL;

	InitializeAnimNameAndIndex();

	m_pModelComs[0]->SetTransformationMatriciesFirstTime(GetAnimationIndexByAnimationName("IDLE"));

	_int iIndex = GetAnimationIndexByAnimationName("CHASE");
	m_pModelComs[0]->SetupAnimation(iIndex, CAnimation::LOOP);
	
	iIndex = GetAnimationIndexByAnimationName("IDLE");
	m_pModelComs[0]->SetupAnimation(iIndex, CAnimation::LOOP);	

	iIndex = GetAnimationIndexByAnimationName("ACTIVE");
	m_pModelComs[0]->SetupAnimation(iIndex, CAnimation::ONCE);

	iIndex = GetAnimationIndexByAnimationName("DEACTIVE");
	m_pModelComs[0]->SetupAnimation(iIndex, CAnimation::LOOP);
	m_pModelComs[0]->SetCurrentAnimationIndex(iIndex);

	m_iPrevTickState = CMonsterBase::MONSTER_STATE::MS_DEACTIVE;
	// m_iPrevPlayAnimIndex = 2;

	return S_OK;
}

void CMonsterBody06::PriorityUpdate(_float fTimeDelta)
{
}

void CMonsterBody06::Update(_float fTimeDelta)
{
	// 0.3초마다 갱신한다.
	if (m_pOwnerMonster->GetUpdateState())
	{
		// CMonster01의 이전 틱 상태와 현재 상태가 다르다면 상태가 변경된 것이라 간주한다.
		if (m_iPrevTickState != *m_pState)
		{
			if (CMonsterBase::MONSTER_STATE::MS_DEACTIVE & m_iPrevTickState)
			{
				m_iPrevPlayAnimIndex = GetAnimationIndexByState(*m_pState);
				ChangeAnimationByAnimIndex(m_iPrevPlayAnimIndex, GetAnimationIndexByState(CMonsterBase::MONSTER_STATE::MS_ACTIVE));
				m_iPrevTickState = *m_pState;
			}
			else
			{
				ChangeAnimation(*m_pState);
				m_iPrevPlayAnimIndex = GetAnimationIndexByState(*m_pState);
				m_iPrevTickState = *m_pState;
			}
		}
		// 상태는 같은데 애니매이션 인덱스가 변경된 경우가 있는지 살펴본다.
		// 같은 상태에서 애니매이션이 변경되어야 한다. ATTACK01 <-> ATTACK02
		else
		{
			if (CMonsterBase::MONSTER_STATE::MS_ATTACK & *m_pState)
			{
				// 상태는 같은데 공격 애니매이션 인덱스가 다른 경우
				_int iCurrentAttackIndex = GetAnimationIndexByAnimationName(GetAnimationNameByState(*m_pState));
				if (m_iPrevPlayAnimIndex != iCurrentAttackIndex)
				{
					m_iPrevPlayAnimIndex = iCurrentAttackIndex;
					ChangeAnimation(m_iPrevPlayAnimIndex, iCurrentAttackIndex, 0);
				}
			}
		}
	}

	
	m_bIsAnimationPlaying = m_pModelComs[GetCurrentModelIndex()]->PlayAnimation(fTimeDelta);


	// 현재 공격 상태인 경우 애니매이션 인덱스를 가져와서 ATTACK02와 일치하는지 비교하고 일치한다면 AlertPoint의 Name을 가져온다.
	if (CMonsterBase::MONSTER_STATE::MS_ATTACK & *m_pState)
	{
		FindAlertPoint(CMonsterBase::MONSTER_STATE::MS_ATTACK, *m_pState, "ATTACK02", "RUSH");
		FindAlertPoint(CMonsterBase::MONSTER_STATE::MS_ATTACK, *m_pState, "ATTACK01", "ATTACK01START");
	}
	else if (CMonsterBase::MONSTER_STATE::MS_ROLL_LEFT & *m_pState)
		FindAlertPoint(CMonsterBase::MONSTER_STATE::MS_ROLL_LEFT, *m_pState, "ROLLLEFT", "ROLLLEFT");
	else if (CMonsterBase::MONSTER_STATE::MS_ROLL_RIGHT & *m_pState)
		FindAlertPoint(CMonsterBase::MONSTER_STATE::MS_ROLL_RIGHT, *m_pState, "ROLLRIGHT", "ROLLRIGHT");
	else if (CMonsterBase::MONSTER_STATE::MS_ACTIVE & *m_pState)
		FindAlertPoint(CMonsterBase::MONSTER_STATE::MS_ACTIVE, *m_pState, "ACTIVE", "ACTIVE");

	if (false == m_bIsAnimationPlaying)
		AnimationEnd();

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->GetWorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));

	CheckAlertPointSoundName();
}

void CMonsterBody06::LateUpdate(_float fTimeDelta)
{
	m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CMonsterBody06::Render()
{
	if (FAILED(BindShaderResources()))
		return E_FAIL;

	// 현재 사용하고 있는 모델의 메쉬 개수를 가져온다.
	_uint   iNumMeshes = m_pModelComs[GetCurrentModelIndex()]->GetNumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelComs[GetCurrentModelIndex()]->BindMaterial(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelComs[GetCurrentModelIndex()]->BindBoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		m_pShaderCom->Begin(5);
		m_pModelComs[GetCurrentModelIndex()]->Render(i);
	}

	return S_OK;
}

void CMonsterBody06::InitializeAnimNameAndIndex()
{
	m_pModelComs[0]->SetAnimationName(2, "IDLE");
	m_pModelComs[0]->SetAnimationName(5, "CHASE");
	m_pModelComs[0]->SetAnimationName(14, "ATTACK01");	// 찌르기 공격
	{
		CAnimation* pAnim = m_pModelComs[0]->GetAnimation(14);
		pAnim->InsertAlertPoint("ATTACK01START", 0.25f);

		pAnim->InsertAlertPointSound("ATTACK01", 0.25f);
	}

	m_pModelComs[0]->SetAnimationName(15, "ATTACK02");	// 돌진 공격
	CAnimation* pAttack02Anim = m_pModelComs[0]->GetAnimation(15);
	pAttack02Anim->InsertAlertPoint("RUSH", 1.4f);

	m_pModelComs[0]->SetAnimationName(12, "ROLLRIGHT");
	CAnimation* pRollRight = m_pModelComs[0]->GetAnimation(12);
	pRollRight->InsertAlertPoint("ROLLRIGHT", 0.3f);

	m_pModelComs[0]->SetAnimationName(13, "ROLLLEFT");
	CAnimation* pRollLeft = m_pModelComs[0]->GetAnimation(13);
	pRollLeft->InsertAlertPoint("ROLLLEFT", 0.3f);

	m_pModelComs[0]->SetAnimationName(7, "DEACTIVE");
	{
		
	}

	m_pModelComs[0]->SetAnimationName(8, "ACTIVE");
	CAnimation* pActiveAnim = m_pModelComs[0]->GetAnimation(8);
	pActiveAnim->InsertAlertPoint("ACTIVE", 4.5f);

	m_pModelComs[0]->SetAnimationName(4, "GETHIT");

	m_ModelsAnimationNames[0].insert({ "IDLE", 2 });
	m_ModelsAnimationNames[0].insert({ "CHASE", 5 });
	m_ModelsAnimationNames[0].insert({ "ATTACK01", 14 });
	{
		CAnimation* pAnim = m_pModelComs[0]->GetAnimation(14);
		pAnim->InsertAlertPoint("ATTACK01START", 0.25f);

	}


	m_ModelsAnimationNames[0].insert({ "ATTACK02", 15 });
	m_ModelsAnimationNames[0].insert({ "ROLLRIGHT", 12 });
	m_ModelsAnimationNames[0].insert({ "ROLLLEFT", 13 });
	m_ModelsAnimationNames[0].insert({ "GETHIT", 4 });
	m_ModelsAnimationNames[0].insert({ "DEACTIVE", 7 });
	m_ModelsAnimationNames[0].insert({ "ACTIVE", 8 });

}

void CMonsterBody06::AnimationEnd(_uint iCurrentModelIndex)
{
	_int iCurrentEndAnimIndex = GetAnimationIndexByState(*m_pState);

	if (false == m_bIsInterpolationing)
	{
		if (CMonsterBase::MONSTER_STATE::MS_ATTACK & *m_pState)
		{
			m_pModelComs[0]->ResetCurrentAnimation();
			m_pOwnerMonster->SubState(CMonsterBase::MONSTER_STATE::MS_ATTACK);
			m_pOwnerMonster->AddState(CMonsterBase::MONSTER_STATE::MS_IDLE);
			m_iPrevPlayAnimIndex = CMonsterBase::MONSTER_STATE::MS_IDLE;

			// ChangeAnimation(GetModelAnimationIndexByState(CMonsterBase::MONSTER_STATE::MS_ATTACK), GetModelAnimationIndexByState(CMonsterBase::MONSTER_STATE::MS_IDLE), 0);
		}
		else if (CMonsterBase::MONSTER_STATE::MS_ROLL_LEFT & *m_pState || CMonsterBase::MONSTER_STATE::MS_ROLL_RIGHT & *m_pState)
		{
			m_pModelComs[0]->ResetCurrentAnimation();
			m_pOwnerMonster->SubState(*m_pState);
			m_pOwnerMonster->AddState(CMonsterBase::MONSTER_STATE::MS_IDLE);
			m_iPrevPlayAnimIndex = CMonsterBase::MONSTER_STATE::MS_IDLE;
			ChangeAnimation(GetAnimationIndexByState(*m_pState), GetAnimationIndexByState(CMonsterBase::MONSTER_STATE::MS_IDLE), 0);
		}
	}
	
	if (m_bIsInterpolationing)
		m_bIsInterpolationing = false;

	ResetAlertCnt(iCurrentEndAnimIndex);
}

string CMonsterBody06::GetAnimationNameByState(_uint iState)
{
	if (CMonsterBase::MONSTER_STATE::MS_IDLE & iState)
		return "IDLE";
	else if (CMonsterBase::MONSTER_STATE::MS_CHASE & iState)
		return "CHASE";
	else if (CMonsterBase::MONSTER_STATE::MS_ATTACK & iState)
	{
		_int iCurrentAttackAnimIndex = m_pOwnerMonster->GetAttackAnimIndex();
		if (CMonster02::ATTACK_INDEX::ATTACK_DEFAULT == iCurrentAttackAnimIndex)
			return "ATTACK01";
		else if (CMonster02::ATTACK_INDEX::ATTACK_RUSH == iCurrentAttackAnimIndex)
			return "ATTACK02";
	}
	else if (CMonsterBase::MONSTER_STATE::MS_DEACTIVE & iState)
		return "SPAWN";
	else if (CMonsterBase::MONSTER_STATE::MS_GETHIT & iState)
		return "GETHIT";
	else if (CMonsterBase::MONSTER_STATE::MS_ROLL_LEFT & iState)
		return "ROLLLEFT";
	else if (CMonsterBase::MONSTER_STATE::MS_ROLL_RIGHT & iState)
		return "ROLLRIGHT";
	else if (CMonsterBase::MONSTER_STATE::MS_ACTIVE & iState)
		return "ACTIVE";
	else if (CMonsterBase::MONSTER_STATE::MS_DEACTIVE & iState)
		return "DEACTIVE";

	return "NONE";
}

void CMonsterBody06::CheckAlertPointSoundName()
{
	string strCurName = m_pModelComs[GetCurrentModelIndex()]->GetAlertPointSoundName();

	if (m_strPrevAlertName == strCurName)
		return;

	_int iCurAttackAnimIndex = GetAnimationIndexByState(*m_pState);
	string str = GetAnimationNameByAnimationIndex(iCurAttackAnimIndex, GetCurrentModelIndex());

	{
		if (str == "ATTACK01")
		{
			if (strCurName == "ATTACK01")
				m_pGameInst->PlayEffect(TEXT("MonsterSphere.mp3"), 1.f);
		}
	}

	m_strPrevAlertName = strCurName;
}

HRESULT CMonsterBody06::ReadyComponents()
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
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Model_Monster06"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelComs[0]))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonsterBody06::BindShaderResources()
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

CMonsterBody06* CMonsterBody06::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonsterBody06* pInstance = new CMonsterBody06(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CMonsterBody06");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject* CMonsterBody06::Clone(void* pArg)
{
	CMonsterBody06* pInstance = new CMonsterBody06(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonsterBody06");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CMonsterBody06::Free()
{
	__super::Free();

	SafeRelease(m_pShaderCom);

	for (auto& pModel : m_pModelComs)
		if (nullptr != pModel)
			SafeRelease(pModel);

	m_pModelComs.clear();
}
