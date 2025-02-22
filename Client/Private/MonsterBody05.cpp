#include "pch.h"
#include "MonsterBody05.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster05.h"


CMonsterBody05::CMonsterBody05(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CMonsterBodyBase(pDevice, pContext)
{
}

CMonsterBody05::CMonsterBody05(const CMonsterBody05& Prototype)
	:
	CMonsterBodyBase(Prototype)
{
}

HRESULT CMonsterBody05::InitializePrototype()
{
	return S_OK;
}

HRESULT CMonsterBody05::Initialize(void* pArg)
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

	iIndex = GetAnimationIndexByAnimationName("DEACTIVE");
	m_pModelComs[0]->SetupAnimation(iIndex, CAnimation::LOOP);
	m_pModelComs[0]->SetCurrentAnimationIndex(iIndex);

	m_iPrevTickState = CMonsterBase::MONSTER_STATE::MS_DEACTIVE;
	
	return S_OK;
}

void CMonsterBody05::PriorityUpdate(_float fTimeDelta)
{

}

void CMonsterBody05::Update(_float fTimeDelta)
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
	else
	{
		m_bIsAnimationPlaying = m_pModelComs[GetCurrentModelIndex()]->PlayAnimation(fTimeDelta);
		XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->GetWorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));

		if (CMonsterBase::MONSTER_STATE::MS_ACTIVE & *m_pState)
			FindAlertPoint(CMonsterBase::MONSTER_STATE::MS_ACTIVE, *m_pState, "ACTIVE", "ACTIVE");
		else if (CMonsterBase::MONSTER_STATE::MS_ATTACK & *m_pState)
		{
			FindAlertPoint(CMonsterBase::MONSTER_STATE::MS_ATTACK, *m_pState, "ATTACK01", "ATTACK01");
			FindAlertPoint(CMonsterBase::MONSTER_STATE::MS_ATTACK, *m_pState, "ATTACK01", "ATTACK01START");
			FindAlertPoint(CMonsterBase::MONSTER_STATE::MS_ATTACK, *m_pState, "ATTACK02", "ATTACK02START");


		}

		if (false == m_bIsAnimationPlaying)
			AnimationEnd();
	}
}

void CMonsterBody05::LateUpdate(_float fTimeDelta)
{
	m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CMonsterBody05::Render()
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

void CMonsterBody05::InitializeAnimNameAndIndex()
{
	m_pModelComs[0]->SetAnimationName(0, "IDLE");
	m_pModelComs[0]->SetAnimationName(2, "CHASE");
	m_pModelComs[0]->SetAnimationName(4, "MOVEFRONT");
	m_pModelComs[0]->SetAnimationName(5, "MOVERIGHT");
	m_pModelComs[0]->SetAnimationName(6, "MOVEFBACK");
	m_pModelComs[0]->SetAnimationName(7, "MOVELEFT");

	m_pModelComs[0]->SetAnimationName(12, "ACTIVE");
	CAnimation* pActiveAnim = m_pModelComs[0]->GetAnimation(12);
	pActiveAnim->InsertAlertPoint("ACTIVE", 0.5f);

	m_pModelComs[0]->SetAnimationName(8, "DEACTIVE");

	m_pModelComs[0]->SetAnimationName(13, "JUMPSTART");
	m_pModelComs[0]->SetAnimationName(14, "JUMPEND");

	m_pModelComs[0]->SetAnimationName(15, "GETHIT");

	m_pModelComs[0]->SetAnimationName(16, "ATTACK01");	// 원거리 공격 (뭐 날리는 공격)
	CAnimation* pAttack01Anim = m_pModelComs[0]->GetAnimation(16);
	pAttack01Anim->InsertAlertPoint("ATTACK01", 0.5f);
	pAttack01Anim->InsertAlertPoint("ATTACK01START", 0.25f);

	m_pModelComs[0]->SetAnimationName(17, "ATTACK02");	// 칼로 내려찍기
	{
		CAnimation* pAnim = m_pModelComs[0]->GetAnimation(17);
		pAnim->InsertAlertPoint("ATTACK02START", 0.25);
	}


	m_pModelComs[0]->SetAnimationName(18, "ATTACK03");	// 방패로 밀치기

	m_ModelsAnimationNames[0].insert({ "IDLE", 0 });
	m_ModelsAnimationNames[0].insert({ "CHASE", 2 });
	m_ModelsAnimationNames[0].insert({ "MOVEFRONT", 4 });
	m_ModelsAnimationNames[0].insert({ "MOVERIGHT", 5 });
	m_ModelsAnimationNames[0].insert({ "MOVEFBACK", 6 });
	m_ModelsAnimationNames[0].insert({ "MOVELEFT", 7 });

	m_ModelsAnimationNames[0].insert({ "ACTIVE", 12 });
	m_ModelsAnimationNames[0].insert({ "DEACTIVE", 8 });

	m_ModelsAnimationNames[0].insert({ "JUMPSTART", 13 });
	m_ModelsAnimationNames[0].insert({ "JUMPEND", 14 });

	m_ModelsAnimationNames[0].insert({ "ATTACK01", 16 });
	m_ModelsAnimationNames[0].insert({ "ATTACK02", 17 });
	m_ModelsAnimationNames[0].insert({ "ATTACK03", 18 });
}

void CMonsterBody05::AnimationEnd(_uint iCurrentModelIndex)
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
		}
	}

	if (m_bIsInterpolationing)
		m_bIsInterpolationing = false;

	ResetAlertCnt(iCurrentEndAnimIndex);
}

string CMonsterBody05::GetAnimationNameByState(_uint iState)
{
	if (CMonsterBase::MONSTER_STATE::MS_IDLE & iState)
		return "IDLE";
	else if (CMonsterBase::MONSTER_STATE::MS_CHASE & iState)
		return "CHASE";
	else if (CMonsterBase::MONSTER_STATE::MS_ATTACK & iState)
	{
		_int iCurrentAttackAnimIndex = m_pOwnerMonster->GetAttackAnimIndex();
		if (CMonster05::ATTACK_SHIELD == iCurrentAttackAnimIndex)
			return "ATTACK03";
		else if (CMonster05::ATTACK_SWORD_DOWN == iCurrentAttackAnimIndex)
			return "ATTACK02";
		else if (CMonster05::ATTACK_SWORD_SWING == iCurrentAttackAnimIndex)
			return "ATTACK01";
	}
	else if (CMonsterBase::MONSTER_STATE::MS_DEACTIVE & iState)
		return "SPAWN";
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
	else if (CMonsterBase::MONSTER_STATE::MS_MOVE_FRONT & iState)
		return "MOVEFRONT";
	else if (CMonsterBase::MONSTER_STATE::MS_MOVE_LEFT & iState)
		return "MOVELEFT";
	else if (CMonsterBase::MONSTER_STATE::MS_MOVE_RIGHT & iState)
		return "MOVERIGHT";
	else if (CMonsterBase::MONSTER_STATE::MS_MOVE_BACK & iState)
		return "MOVEBACK";

	return "NONE";
}

void CMonsterBody05::CheckAlertPointSoundName()
{
	string strCurName = m_pModelComs[GetCurrentModelIndex()]->GetAlertPointSoundName();

	if (m_strPrevAlertName == strCurName)
		return;


	_int iCurAttackAnimIndex = GetAnimationIndexByState(*m_pState);
	string str = GetAnimationNameByAnimationIndex(iCurAttackAnimIndex, GetCurrentModelIndex());

	// Active
	{

	}

	// 몬스터 스폰 공격
	{
		if (str == "ATTACK02")
		{
			if (strCurName == "RUSH_START")
				m_pGameInst->PlayEffect(TEXT("Monster02RushStart.mp3"), 1.f);

			if (strCurName == "RUSH_ATTACK")
				m_pGameInst->PlayEffect(TEXT("Monster02RushAttack.mp3"), 1.f);
		}
	}

	{
		if (str == "ATTACK01")
		{
			if (strCurName == "ATTACK01")
				m_pGameInst->PlayEffect(TEXT("MonsterSphere.mp3"), 1.f);
		}
	}

	{
		if (str == "ROLLRIGHT")
		{
			if (strCurName == "ROLL_RIGHT")
				m_pGameInst->PlayEffect(TEXT("MonterRoll2.mp3"), 1.f);
		}
	}


	{
		if (str == "ROLLLEFT")
		{
			if (strCurName == "ROLLRIGHT")
				m_pGameInst->PlayEffect(TEXT("MonterRoll2.mp3"), 1.f);
		}
	}


	m_strPrevAlertName = strCurName;
}

HRESULT CMonsterBody05::ReadyComponents()
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
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Model_Monster05"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelComs[0]))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonsterBody05::BindShaderResources()
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

CMonsterBody05* CMonsterBody05::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonsterBody05* pInstance = new CMonsterBody05(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CMonsterBody05");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject* CMonsterBody05::Clone(void* pArg)
{
	CMonsterBody05* pInstance = new CMonsterBody05(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonsterBody05");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CMonsterBody05::Free()
{
	__super::Free();

	SafeRelease(m_pShaderCom);

	for (auto& pModel : m_pModelComs)
		if (nullptr != pModel)
			SafeRelease(pModel);

	m_pModelComs.clear();
}
