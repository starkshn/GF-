#include "pch.h"
#include "MonsterBody01.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster01.h"


CMonsterBody01::CMonsterBody01(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CMonsterBodyBase(pDevice, pContext)
{
}

CMonsterBody01::CMonsterBody01(const CMonsterBody01& Prototype)
	:
	CMonsterBodyBase(Prototype)
{
}

HRESULT CMonsterBody01::InitializePrototype()
{
	return S_OK;
}

HRESULT CMonsterBody01::Initialize(void* pArg)
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

void CMonsterBody01::PriorityUpdate(_float fTimeDelta)
{


	// static _float fAcc = 0.f;
	// fAcc += fTimeDelta;
	// m_pTransformCom->Scaling(1.f + fAcc, 1.f + fAcc, 1.f + fAcc);
}

void CMonsterBody01::Update(_float fTimeDelta)
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

		if (CMonsterBase::MONSTER_STATE::MS_ROLL_LEFT & *m_pState)
		{
			FindAlertPoint(CMonsterBase::MONSTER_STATE::MS_ROLL_LEFT, *m_pState, "ROLLLEFT", "ROLLLEFT_START");
			FindAlertPoint(CMonsterBase::MONSTER_STATE::MS_ROLL_LEFT, *m_pState, "ROLLLEFT", "ROLLLEFT");

		}

		if (CMonsterBase::MONSTER_STATE::MS_ROLL_RIGHT & *m_pState)
		{
			FindAlertPoint(CMonsterBase::MONSTER_STATE::MS_ROLL_RIGHT, *m_pState, "ROLLRIGHT", "ROLLRIGHT_START");
			FindAlertPoint(CMonsterBase::MONSTER_STATE::MS_ROLL_RIGHT, *m_pState, "ROLLRIGHT", "ROLLRIGHT");
		}

		if (CMonsterBase::MONSTER_STATE::MS_ACTIVE & *m_pState)
			FindAlertPoint(CMonsterBase::MONSTER_STATE::MS_ACTIVE, *m_pState, "ACTIVE", "ACTIVE");

		if (CMonsterBase::MONSTER_STATE::MS_ATTACK & *m_pState)
		{
			FindAlertPoint(CMonsterBase::MONSTER_STATE::MS_ATTACK, *m_pState, "ATTACK01", "Attack01Start");
			FindAlertPoint(CMonsterBase::MONSTER_STATE::MS_ATTACK, *m_pState, "ATTACK02", "Attack02Start");
		}

		if (false == m_bIsAnimationPlaying)
			AnimationEnd();
	}

	CheckAlertPointSoundName();
}

void CMonsterBody01::LateUpdate(_float fTimeDelta)
{
	m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);
	m_pGameInst->AddRenderObject(CRenderer::RG_SHADOW, this);

}

HRESULT CMonsterBody01::Render()
{
	if (FAILED(BindShaderResources()))
		return E_FAIL;

	if (!m_bOnce)
	{
		_uint   iNumMeshes = m_pModelComs[GetCurrentModelIndex()]->GetNumMeshes();

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			
			if (FAILED(m_pModelComs[GetCurrentModelIndex()]->BindMaterial(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			if (FAILED(m_pModelComs[GetCurrentModelIndex()]->BindBoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
				return E_FAIL;

			m_pShaderCom->Begin(4);
			m_pModelComs[GetCurrentModelIndex()]->Render(i);
		}
	}
	else
	{
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
	}

	return S_OK;
}

HRESULT CMonsterBody01::RenderShadow()
{
	if (FAILED(m_pShaderCom->BindMatrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", m_pGameInst->GetShadowTransform(CShadow::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", m_pGameInst->GetShadowTransform(CShadow::D3DTS_PROJ))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelComs[0]->GetNumMeshes();

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

void CMonsterBody01::InitializeAnimNameAndIndex()
{
	m_pModelComs[0]->SetAnimationName(0, "IDLE");
	m_pModelComs[0]->SetAnimationName(2, "CHASE");

	m_pModelComs[0]->SetAnimationName(13, "ATTACK01");	// 휘두르기
	{
		CAnimation* pAttack01 = m_pModelComs[0]->GetAnimation(13);
		pAttack01->InsertAlertPoint("Attack01Start", 0.25f);

		pAttack01->InsertAlertPointSound("ATTACK01", 0.25f);
	}

	m_pModelComs[0]->SetAnimationName(14, "ATTACK02");	// 찌르기
	{
		CAnimation* pAttack02 = m_pModelComs[0]->GetAnimation(14);
		pAttack02->InsertAlertPoint("Attack02Start", 0.25f);

		pAttack02->InsertAlertPointSound("ATTACK02", 0.25f);
	}

	m_pModelComs[0]->SetAnimationName(11, "ROLLRIGHT");
	CAnimation* pRollRight = m_pModelComs[0]->GetAnimation(11);
	pRollRight->InsertAlertPoint("ROLLRIGHT_START", 0.15f);
	pRollRight->InsertAlertPoint("ROLLRIGHT", 0.3f);
	pRollRight->InsertAlertPointSound("ROLL_RIGHT", 0.25f);

	m_pModelComs[0]->SetAnimationName(12, "ROLLLEFT");
	CAnimation* pRollLeft = m_pModelComs[0]->GetAnimation(12);
	pRollLeft->InsertAlertPoint("ROLLLEFT_START", 0.15f);
	pRollLeft->InsertAlertPoint("ROLLLEFT", 0.3f);
	pRollLeft->InsertAlertPointSound("ROLL_LEFT", 0.25f);

	m_pModelComs[0]->SetAnimationName(5, "ACTIVE");
	{
		CAnimation* pActiveAnim = m_pModelComs[0]->GetAnimation(5);
		pActiveAnim->InsertAlertPoint("ACTIVE", 1.5f);
	}

	m_pModelComs[0]->SetAnimationName(6, "DEACTIVE");
	{

	}

	m_pModelComs[0]->SetAnimationName(10, "GETHIT");
	
	m_ModelsAnimationNames[0].insert({ "IDLE", 0 });
	m_ModelsAnimationNames[0].insert({ "CHASE", 2 });
	m_ModelsAnimationNames[0].insert({ "ATTACK01", 13 });
	m_ModelsAnimationNames[0].insert({ "ATTACK02", 14 });
	m_ModelsAnimationNames[0].insert({ "ROLLRIGHT", 11 });
	m_ModelsAnimationNames[0].insert({ "ROLLLEFT", 12 });
	m_ModelsAnimationNames[0].insert({ "ACTIVE", 5 });
	m_ModelsAnimationNames[0].insert({ "DEACTIVE", 6 });
	m_ModelsAnimationNames[0].insert({ "GETHIT", 10 });
}

void CMonsterBody01::AnimationEnd(_uint iCurrentModelIndex)
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

string CMonsterBody01::GetAnimationNameByState(_uint iState)
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
		else if (CMonster01::ATTACK_INDEX::ATTACK_STAB == iCurrentAttackAnimIndex)
			return "ATTACK02";
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

	return "NONE";
}

void CMonsterBody01::CheckAlertPointSoundName()
{
	string strCurName = m_pModelComs[GetCurrentModelIndex()]->GetAlertPointSoundName();

	if (m_strPrevAlertName == strCurName)
		return;

	_int iCurAttackAnimIndex = GetAnimationIndexByState(*m_pState);
	string str = GetAnimationNameByAnimationIndex(iCurAttackAnimIndex, GetCurrentModelIndex());


	// 몬스터 스폰 공격
	{
		if (str == "ATTACK01")
		{
			if (strCurName == "ATTACK01")
				m_pGameInst->PlayEffect(TEXT("MonsterSwordSwing2.mp3"), 1.f);
		}
	}

	{
		if (str == "ATTACK02")
		{
			if (strCurName == "ATTACK02")
				m_pGameInst->PlayEffect(TEXT("MonsterSphere.mp3"), 1.f);
		}
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

HRESULT CMonsterBody01::ReadyComponents()
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
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Model_Monster01"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelComs[0]))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonsterBody01::BindShaderResources()
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

CMonsterBody01* CMonsterBody01::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonsterBody01* pInstance = new CMonsterBody01(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CMonsterBody01");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject* CMonsterBody01::Clone(void* pArg)
{
	CMonsterBody01* pInstance = new CMonsterBody01(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonsterBody01");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CMonsterBody01::Free()
{
	__super::Free();

	SafeRelease(m_pShaderCom);

	for (auto& pModel : m_pModelComs)
		if (nullptr != pModel)
			SafeRelease(pModel);

	m_pModelComs.clear();
}
