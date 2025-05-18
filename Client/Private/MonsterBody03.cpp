#include "pch.h"
#include "MonsterBody03.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster01.h"


CMonsterBody03::CMonsterBody03(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CMonsterBodyBase(pDevice, pContext)
{
}

CMonsterBody03::CMonsterBody03(const CMonsterBody03& Prototype)
	:
	CMonsterBodyBase(Prototype)
{
}

HRESULT CMonsterBody03::InitializePrototype()
{
	return S_OK;
}

HRESULT CMonsterBody03::Initialize(void* pArg)
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
	m_pModelComs[0]->SetCurrentAnimationIndex(iIndex);

	iIndex = GetAnimationIndexByAnimationName("ACTIVE");
	m_pModelComs[0]->SetupAnimation(iIndex, CAnimation::ONCE);

	iIndex = GetAnimationIndexByAnimationName("MOVEFRONT");
	m_pModelComs[0]->SetupAnimation(iIndex, CAnimation::LOOP);

	iIndex = GetAnimationIndexByAnimationName("MOVELEFT");
	m_pModelComs[0]->SetupAnimation(iIndex, CAnimation::LOOP);

	iIndex = GetAnimationIndexByAnimationName("MOVERIGHT");
	m_pModelComs[0]->SetupAnimation(iIndex, CAnimation::LOOP);

	iIndex = GetAnimationIndexByAnimationName("MOVEBACK");
	m_pModelComs[0]->SetupAnimation(iIndex, CAnimation::LOOP);

	iIndex = GetAnimationIndexByAnimationName("DEACTIVE");
	m_pModelComs[0]->SetupAnimation(iIndex, CAnimation::LOOP);
	m_pModelComs[0]->SetCurrentAnimationIndex(iIndex);

	m_iPrevTickState = CMonsterBase::MONSTER_STATE::MS_DEACTIVE;
	
	return S_OK;
}

void CMonsterBody03::PriorityUpdate(_float fTimeDelta)
{

}

void CMonsterBody03::Update(_float fTimeDelta)
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
				m_iPrevPlayAnimIndex = GetAnimationIndexByState(*m_pState);
				ChangeAnimation(*m_pState);
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
	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->GetWorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));

	if (CMonsterBase::MONSTER_STATE::MS_ROLL_LEFT & *m_pState)
		FindAlertPoint(CMonsterBase::MONSTER_STATE::MS_ROLL_LEFT, *m_pState, "ROLLLEFT", "ROLLLEFT");
	else if (CMonsterBase::MONSTER_STATE::MS_ROLL_RIGHT & *m_pState)
		FindAlertPoint(CMonsterBase::MONSTER_STATE::MS_ROLL_RIGHT, *m_pState, "ROLLRIGHT", "ROLLRIGHT");
	else if (CMonsterBase::MONSTER_STATE::MS_ACTIVE & *m_pState)
		FindAlertPoint(CMonsterBase::MONSTER_STATE::MS_ACTIVE, *m_pState, "ACTIVE", "ACTIVE");
	else if (CMonsterBase::MONSTER_STATE::MS_ATTACK & *m_pState)
		FindAlertPoint(CMonsterBase::MONSTER_STATE::MS_ATTACK, *m_pState, "ATTACK01", "ATTACK01_START");

	if (false == m_bIsAnimationPlaying)
		AnimationEnd();

	CheckAlertPointSoundName();
}

void CMonsterBody03::LateUpdate(_float fTimeDelta)
{
	m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CMonsterBody03::Render()
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

void CMonsterBody03::InitializeAnimNameAndIndex()
{
	m_pModelComs[0]->SetAnimationName(3, "IDLE");
	m_pModelComs[0]->SetAnimationName(2, "CHASE");

	m_pModelComs[0]->SetAnimationName(20, "ATTACK01");	// 서서 쏘는 기본 공격
	{
		CAnimation* pAnim = m_pModelComs[0]->GetAnimation(20);
		pAnim->InsertAlertPoint("ATTACK01_START", 0.2f);
		pAnim->InsertAlertPointSound("ATTACK01", 0.2f);
	}
	// m_pModelComs[0]->SetAnimationName(14, "ATTACK02");

	m_pModelComs[0]->SetAnimationName(12, "ROLLRIGHT");
	CAnimation* pRollRight = m_pModelComs[0]->GetAnimation(12);
	pRollRight->InsertAlertPoint("ROLLRIGHT", 0.3f);
	pRollRight->InsertAlertPointSound("ROLL_RIGHT", 0.25f);

	m_pModelComs[0]->SetAnimationName(13, "ROLLLEFT");
	CAnimation* pRollLeft = m_pModelComs[0]->GetAnimation(13);
	pRollLeft->InsertAlertPoint("ROLLLEFT", 0.3f);
	pRollLeft->InsertAlertPointSound("ROLL_LEFT", 0.25f);

	m_pModelComs[0]->SetAnimationName(10, "ACTIVE");
	{
		CAnimation* pActiveAnim = m_pModelComs[0]->GetAnimation(10);
		pActiveAnim->InsertAlertPoint("ACTIVE", 0.5f);
	}

	m_pModelComs[0]->SetAnimationName(18, "DEACTIVE");
	{

	}

	m_pModelComs[0]->SetAnimationName(10, "GETHIT");
	
	m_pModelComs[0]->SetAnimationName(14, "MOVEFRONT");
	m_pModelComs[0]->SetAnimationName(15, "MOVERIGHT");
	m_pModelComs[0]->SetAnimationName(16, "MOVEBACK");
	m_pModelComs[0]->SetAnimationName(17, "MOVELEFT");


	m_ModelsAnimationNames[0].insert({ "IDLE", 3 });
	m_ModelsAnimationNames[0].insert({ "CHASE", 2 });

	m_ModelsAnimationNames[0].insert({ "ATTACK01", 20 });
	{
		CAnimation* pAnim = m_pModelComs[0]->GetAnimation(20);
		pAnim->InsertAlertPoint("ATTACK01START", 0.15f);
	}



	// m_ModelsAnimationNames[0].insert({ "ATTACK02", 14 });
	m_ModelsAnimationNames[0].insert({ "ROLLRIGHT", 12 });
	m_ModelsAnimationNames[0].insert({ "ROLLLEFT", 13 });
	m_ModelsAnimationNames[0].insert({ "ACTIVE", 10 });
	m_ModelsAnimationNames[0].insert({ "DEACTIVE", 18 });
	// m_ModelsAnimationNames[0].insert({ "GETHIT", 6 });

	m_ModelsAnimationNames[0].insert({ "MOVEFRONT", 14 });
	m_ModelsAnimationNames[0].insert({ "MOVERIGHT", 15 });
	m_ModelsAnimationNames[0].insert({ "MOVEBACK", 16 });
	m_ModelsAnimationNames[0].insert({ "MOVELEFT", 17 });

}

void CMonsterBody03::AnimationEnd(_uint iCurrentModelIndex)
{
	_int iCurrentEndAnimIndex = GetAnimationIndexByState(*m_pState);

	if (false == m_bIsInterpolationing)
	{
		if (CMonsterBase::MONSTER_STATE::MS_ATTACK & *m_pState)
		{
			m_pModelComs[0]->ResetCurrentAnimation();
			m_pOwnerMonster->SubState(CMonsterBase::MONSTER_STATE::MS_ATTACK);
			m_pOwnerMonster->AddState(CMonsterBase::MONSTER_STATE::MS_IDLE);
			m_iPrevPlayAnimIndex = GetAnimationIndexByState(CMonsterBase::MONSTER_STATE::MS_IDLE);
			
			// ChangeAnimation(GetModelAnimationIndexByAnimationName(strAttackAnimName), GetModelAnimationIndexByState(CMonsterBase::MONSTER_STATE::MS_IDLE), 0);
		}
		else if (CMonsterBase::MONSTER_STATE::MS_ROLL_LEFT & *m_pState || CMonsterBase::MONSTER_STATE::MS_ROLL_RIGHT & *m_pState)
		{
			m_pModelComs[0]->ResetCurrentAnimation();
			m_pOwnerMonster->SubState(*m_pState);
			m_pOwnerMonster->AddState(CMonsterBase::MONSTER_STATE::MS_IDLE);
			m_iPrevPlayAnimIndex = GetAnimationIndexByState(CMonsterBase::MONSTER_STATE::MS_IDLE);
			SetPlayAnimation(CMonsterBase::MONSTER_STATE::MS_IDLE);

			// ChangeAnimation(GetModelAnimationIndexByState(*m_pState), GetModelAnimationIndexByState(CMonsterBase::MONSTER_STATE::MS_IDLE), 0);
		}
	}

	if (m_bIsInterpolationing)
	{
		// m_pModelComs[0]->ResetCurrentAnimation(); // 이거 주석해제 하면 모든 애니매이션이 두번 재생된다.
		m_bIsInterpolationing = false;
	}

	ResetAlertCnt(iCurrentEndAnimIndex);
}

string CMonsterBody03::GetAnimationNameByState(_uint iState)
{
	if (CMonsterBase::MONSTER_STATE::MS_IDLE & iState)
		return "IDLE";
	else if (CMonsterBase::MONSTER_STATE::MS_CHASE & iState)
		return "CHASE";
	else if (CMonsterBase::MONSTER_STATE::MS_ATTACK & iState)
	{
		_int iCurrentAttackAnimIndex = m_pOwnerMonster->GetAttackAnimIndex();
		if (CMonster01::ATTACK_INDEX::ATTACK_DEFAULT == iCurrentAttackAnimIndex)
			return "ATTACK01";
	}
	else if (CMonsterBase::MONSTER_STATE::MS_GETHIT & iState)
		return "GETHIT";
	else if (CMonsterBase::MONSTER_STATE::MS_ROLL_RIGHT & iState)
		return "ROLLRIGHT";
	else if (CMonsterBase::MONSTER_STATE::MS_ROLL_LEFT & iState)
		return "ROLLLEFT";
	else if (CMonsterBase::MONSTER_STATE::MS_ACTIVE & iState)
		return "ACTIVE";
	else if (CMonsterBase::MONSTER_STATE::MS_DEACTIVE & iState)
		return "DEACTIVE";
	else if (CMonsterBase::MONSTER_STATE::MS_MOVE_FRONT& iState)
		return "MOVEFRONT";
	else if (CMonsterBase::MONSTER_STATE::MS_MOVE_LEFT & iState)
		return "MOVELEFT";
	else if (CMonsterBase::MONSTER_STATE::MS_MOVE_RIGHT & iState)
		return "MOVERIGHT";
	else if (CMonsterBase::MONSTER_STATE::MS_MOVE_BACK & iState)
		return "MOVEBACK";


	return "NONE";
}

void CMonsterBody03::CheckAlertPointSoundName()
{
	string strCurName = m_pModelComs[GetCurrentModelIndex()]->GetAlertPointSoundName();

	if (m_strPrevAlertName == strCurName)
		return;

	_int iCurAttackAnimIndex = GetAnimationIndexByState(*m_pState);
	string str = GetAnimationNameByAnimationIndex(iCurAttackAnimIndex, GetCurrentModelIndex());

	if (str == "ATTACK01")
	{
		if (strCurName == "ATTACK01")
			m_pGameInst->PlayEffect(TEXT("Monster03Attack.mp3"), 1.f);
	}

	if (str == "ROLLRIGHT")
	{
		if (strCurName == "ROLL_RIGHT")
			m_pGameInst->PlayEffect(TEXT("MonsterRoll1.mp3"), 0.7f);
	}

	if (str == "ROLLLEFT")
	{
		if (strCurName == "ROLL_LEFT")
			m_pGameInst->PlayEffect(TEXT("MonsterRoll1.mp3"), 0.7f);
	}


	m_strPrevAlertName = strCurName;

}

HRESULT CMonsterBody03::ReadyComponents()
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
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Model_Monster03"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelComs[0]))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonsterBody03::BindShaderResources()
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

CMonsterBody03* CMonsterBody03::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonsterBody03* pInstance = new CMonsterBody03(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CMonsterBody03");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject* CMonsterBody03::Clone(void* pArg)
{
	CMonsterBody03* pInstance = new CMonsterBody03(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonsterBody03");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CMonsterBody03::Free()
{
	__super::Free();

	SafeRelease(m_pShaderCom);

	for (auto& pModel : m_pModelComs)
		if (nullptr != pModel)
			SafeRelease(pModel);

	m_pModelComs.clear();
}
