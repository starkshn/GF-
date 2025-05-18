#include "pch.h"
#include "BossBody.h"
#include "GameInstance.h"
#include "Model.h"
#include "Boss.h"
#include "Texture.h"
#include "UITex.h"
#include "GameInstance.h"

CBossBody::CBossBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CMonsterBodyBase(pDevice, pContext)
{
}

CBossBody::CBossBody(const CBossBody& Prototype)
	:
	CMonsterBodyBase(Prototype)
{
}

HRESULT CBossBody::InitializePrototype()
{
	return S_OK;
}

HRESULT CBossBody::Initialize(void* pArg)
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

	m_iPrevTickState = CMonsterBase::MONSTER_STATE::MS_DEACTIVE;

	// 
	// m_pGameInst->PlayEffect(TEXT("BossCutSceneJump.mp3"), 1.f);

	
	return S_OK;
}

void CBossBody::PriorityUpdate(_float fTimeDelta)
{
	
}

void CBossBody::Update(_float fTimeDelta)
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


	if (CMonsterBase::MONSTER_STATE::MS_ATTACK & *m_pState)
	{
		if (0 == m_pOwnerMonster->GetAttackAnimIndex())
		{
			FindAlertPoint(CMonsterBase::MONSTER_STATE::MS_ATTACK, *m_pState, "ATTACK01", "ATTACK01");
			FindAlertPoint(CMonsterBase::MONSTER_STATE::MS_ATTACK, *m_pState, "ATTACK01", "SMASH");
		}
		else if (1 == m_pOwnerMonster->GetAttackAnimIndex())
			FindAlertPoint(CMonsterBase::MONSTER_STATE::MS_ATTACK, *m_pState, "ATTACK02", "ATTACK02");
		else if (2 == m_pOwnerMonster->GetAttackAnimIndex())
			FindAlertPoint(CMonsterBase::MONSTER_STATE::MS_ATTACK, *m_pState, "ATTACK03", "ATTACK03");
		else if (3 == m_pOwnerMonster->GetAttackAnimIndex())
			FindAlertPoint(CMonsterBase::MONSTER_STATE::MS_ATTACK, *m_pState, "ATTACK04", "ATTACK04");
	}
	else if (CMonsterBase::MONSTER_STATE::MS_ACTIVE & *m_pState)
	{
		FindAlertPoint(CMonsterBase::MONSTER_STATE::MS_ACTIVE, *m_pState, "ACTIVE", "ACTIVE");
	}


	if (CMonsterBase::MONSTER_STATE::MS_ACTIVE & *m_pState)
	{
		FindAlertPointSounds(CMonsterBase::MONSTER_STATE::MS_ACTIVE, *m_pState, "ACTIVE", "ACTIVE_JUMP");

		FindAlertPointSounds(CMonsterBase::MONSTER_STATE::MS_ACTIVE, *m_pState, "ACTIVE", "ACTIVE_WEAPON1");
		FindAlertPointSounds(CMonsterBase::MONSTER_STATE::MS_ACTIVE, *m_pState, "ACTIVE", "ACTIVE_WEAPON2");
		FindAlertPointSounds(CMonsterBase::MONSTER_STATE::MS_ACTIVE, *m_pState, "ACTIVE", "ACTIVE_WEAPON3");
		// FindAlertPoint(CMonsterBase::MONSTER_STATE::MS_ACTIVE, *m_pState, "ACTIVE", "ACTIVE");
	}

	if (false == m_bIsAnimationPlaying)
		AnimationEnd();

	static _bool bStart = false;

	if (m_pOwnerMonster->IsDead() && !bStart)
	{
		m_fDeadAccTime += fTimeDelta;

		if (m_fDeadAccTime >= m_fDeadMaxTime)
		{
			m_fDeadAccTime = 0.f;
			StartNoise();
			bStart = true;
		}

		cout << "노이즈 시작 까지 남은 시간 비율 : " << m_fDeadAccTime / m_fDeadMaxTime << endl;
	}

	if (m_bNoiseStart)
	{
		m_fNoiseAccTime += fTimeDelta * 1.f;
		if (m_fNoiseAccTime >= m_fNoiseMaxTime)
		{
			m_fNoiseAccTime = 0.f;
			EndNoise();
		}

		cout << "노이즈 시간 비율 : " << m_fNoiseAccTime / m_fNoiseMaxTime << endl;

		static _bool bOnce = false;
		if (!bOnce)
		{
			bOnce = 1;
			m_pGameInst->PlayEffect(TEXT("BossDeadFinal.mp3"), 1.f);
		}
	}

	CheckAlertPointSoundName();
}

void CBossBody::LateUpdate(_float fTimeDelta)
{
	//if (m_bRender)
	//{
	//	_vector vWorldPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);
	//	if (m_pGameInst->IsInFrustumWorldSpace(vWorldPos))
	//	{
	//		m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);
	//	}
	//}
	if (m_bRender)
		m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CBossBody::Render()
{
	if (FAILED(BindShaderResources()))
		return E_FAIL;

	// 현재 사용하고 있는 모델의 메쉬 개수를 가져온다.
	_uint   iNumMeshes = m_pModelComs[GetCurrentModelIndex()]->GetNumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (m_bNoiseStart)
		{
			if (FAILED(m_pModelComs[GetCurrentModelIndex()]->BindMaterial(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			if (FAILED(m_pModelComs[GetCurrentModelIndex()]->BindBoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
				return E_FAIL;

			m_pShaderCom->Begin(3);

			m_pModelComs[GetCurrentModelIndex()]->Render(i);
		}
		else
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

void CBossBody::InitializeAnimNameAndIndex()
{
	m_pModelComs[0]->SetAnimationName(0, "IDLE");

	m_pModelComs[0]->SetAnimationName(1, "WALKFRONT");
	m_pModelComs[0]->SetAnimationName(2, "WALKRIGHT");
	m_pModelComs[0]->SetAnimationName(3, "WALKBACK");
	m_pModelComs[0]->SetAnimationName(4, "WALKLEFT");
	m_pModelComs[0]->SetAnimationName(5, "CHASE");

	// 등장 애니매이션
	m_pModelComs[0]->SetAnimationName(6, "ACTIVE");
	{
		CAnimation* pActiveAnim = m_pModelComs[0]->GetAnimation(6);

		pActiveAnim->InsertAlertPointSound("ACTIVE_JUMP", 1.0f);
		pActiveAnim->InsertAlertPointSound("ACTIVE_WEAPON1", 2.5f);
		pActiveAnim->InsertAlertPointSound("ACTIVE_WEAPON2", 2.6f);
		pActiveAnim->InsertAlertPointSound("ACTIVE_WEAPON3", 2.8f);
		pActiveAnim->InsertAlertPointSound("ACTIVE_ROAR", 4.5f);

		pActiveAnim->InsertAlertPoint("ACTIVE", 6.f);
	}

	m_pModelComs[0]->SetAnimationName(7, "DEAD");
	{
		CAnimation* pAnimDead = m_pModelComs[0]->GetAnimation(7);
		pAnimDead->InsertAlertPointSound("DEAD_START", 0.2f);
		pAnimDead->InsertAlertPointSound("DEAD_END", 3.8f);
		// pAnimDead->InsertAlertPointSound("DEAD_FINAL", 9.f);

	}

	m_pModelComs[0]->SetAnimationName(8, "JUMPSTART");
	m_pModelComs[0]->SetAnimationName(9, "JUMPEND");
	m_pModelComs[0]->SetAnimationName(10, "GETHITRIGHT");
	m_pModelComs[0]->SetAnimationName(11, "GETHITLEFT");
	// m_pModelComs[0]->SetAnimationName(12, "CHARGEHP");			// <- HP 채우는 스킬
	m_pModelComs[0]->SetAnimationName(12, "ATTACK04");				// <- HP 채우는 스킬
	CAnimation* pAttack04 = m_pModelComs[0]->GetAnimation(12);
	pAttack04->InsertAlertPoint("ATTACK04", 1.25f);

	{
		pAttack04->InsertAlertPointSound("SPAWN_STATR", 0.5f);
		pAttack04->InsertAlertPointSound("SPAWN_END",  1.5f);
	}



	m_pModelComs[0]->SetAnimationName(13, "ATTACK01");			//	즉사 판정, 점프 양손 내려찍기 공격, 범위 공격
	CAnimation* pAttack01 = m_pModelComs[0]->GetAnimation(13);
	pAttack01->InsertAlertPoint("ATTACK01", 0.7f);
	pAttack01->InsertAlertPoint("SMASH", 1.8f);
	{
		pAttack01->InsertAlertPointSound("ATTACK01_START_ROAR", 0.3f);
		pAttack01->InsertAlertPointSound("ATTACK01_SMASH", 1.3f);


		pAttack01->InsertAlertPointSound("ATTACK01_SMASH_END", 1.5f);

	}


	m_pModelComs[0]->SetAnimationName(14, "ATTACK02");			//	돌덩이 던지기 공격, 원기옥 모으듯이 공격함, 원거리 공격
	CAnimation* pAttack02 = m_pModelComs[0]->GetAnimation(14);
	pAttack02->InsertAlertPoint("ATTACK02", 1.7f);
	{
		pAttack02->InsertAlertPointSound("BOSS_ROCK_S_ROAR", 0.2f);
		pAttack02->InsertAlertPointSound("BOSS_ROCK_GATHER", 0.8f);
		pAttack02->InsertAlertPointSound("BOSS_ROCK_E_ROAR", 1.5f);
	}

	m_pModelComs[0]->SetAnimationName(15, "ATTACK03");			//	한손으로 바닥 내려찍기 공격, 근거리
	CAnimation* pAttack03 = m_pModelComs[0]->GetAnimation(15);
	pAttack03->InsertAlertPoint("ATTACK03", 1.f);
	{
		pAttack03->InsertAlertPointSound("ATTACK03_ROAR", 0.2f);
		pAttack03->InsertAlertPointSound("ATTACK03_SMASH", 0.8f);
	}


	//m_pModelComs[0]->SetAnimationName(16, "ATTACK04READY");		//	원거리 공격 준비자세, 미사일 발사, 원거리 공격
	//CAnimation* pAttack04Ready = m_pModelComs[0]->GetAnimation(16);
	//pAttack04Ready->InsertAlertPoint("ATTACK04READY", 1.f);


	//m_pModelComs[0]->SetAnimationName(17, "ATTACK04FIRE");		//	원거리 공격 준비자세, 미사일 발사, 원거리 공격
	//CAnimation* pAttack04Fire = m_pModelComs[0]->GetAnimation(17);
	//pAttack04Fire->InsertAlertPoint("ATTACK04FIRE", 0.5f);


	//m_pModelComs[0]->SetAnimationName(18, "ATTACK04END");		//	원거리 공격 End
	//CAnimation* pAttack04End = m_pModelComs[0]->GetAnimation(18);
	//pAttack04End->InsertAlertPoint("ATTACK04END", 1.f);

	
	m_ModelsAnimationNames[0].insert({ "IDLE", 0 });
	m_ModelsAnimationNames[0].insert({ "WALKFRONT", 1 });
	m_ModelsAnimationNames[0].insert({ "WALKRIGHT", 2 });
	m_ModelsAnimationNames[0].insert({ "WALKBACK", 3 });
	m_ModelsAnimationNames[0].insert({ "WALKLEFT", 4 });
	m_ModelsAnimationNames[0].insert({ "CHASE", 5 });
	m_ModelsAnimationNames[0].insert({ "ACTIVE", 6 });
	m_ModelsAnimationNames[0].insert({ "DEAD", 7 });
	m_ModelsAnimationNames[0].insert({ "JUMPSTART", 8 });
	m_ModelsAnimationNames[0].insert({ "JUMPEND", 9 });
	m_ModelsAnimationNames[0].insert({ "GETHITRIGHT", 10 });
	m_ModelsAnimationNames[0].insert({ "GETHITLEFT", 11 });
	// m_ModelsAnimationNames[0].insert({ "CHARGEHP", 12 });
	m_ModelsAnimationNames[0].insert({ "ATTACK04", 12 });
	m_ModelsAnimationNames[0].insert({ "ATTACK01", 13 });
	m_ModelsAnimationNames[0].insert({ "ATTACK02", 14 });
	m_ModelsAnimationNames[0].insert({ "ATTACK03", 15 });
	//m_ModelsAnimationNames[0].insert({ "ATTACK04READY", 16 });
	//m_ModelsAnimationNames[0].insert({ "ATTACK04FIRE", 17 });
	//m_ModelsAnimationNames[0].insert({ "ATTACK04END", 18 });


	m_AttackAlertChecks.insert({ "ACTIVE", false });
	m_AttackAlertChecks.insert({ "DEAD", false });

	m_AttackAlertChecks.insert({ "ATTACK01", false });
	m_AttackAlertChecks.insert({ "SMASH", false });
	m_AttackAlertChecks.insert({ "ATTACK02", false });
	m_AttackAlertChecks.insert({ "ATTACK03", false });
	m_AttackAlertChecks.insert({ "ATTACK04", false });
}

void CBossBody::AnimationEnd(_uint iCurrentModelIndex)
{
	_int iCurrentEndAnimIndex = GetAnimationIndexByState(*m_pState);

	if (false == m_bIsInterpolationing)
	{
		if (CMonsterBase::MONSTER_STATE::MS_ATTACK & *m_pState)
		{
			string strName = GetAnimationNameByAnimationIndex(iCurrentEndAnimIndex);
			if (strName == "ATTACK01")
			{
				ResetAttackAlert("ATTACK01");
				ResetAttackAlert("SMASH");
			}
			else
				ResetAttackAlert(strName);

			m_pModelComs[0]->ResetCurrentAnimation();
			m_pOwnerMonster->SubState(CMonsterBase::MONSTER_STATE::MS_ATTACK);
			m_pOwnerMonster->AddState(CMonsterBase::MONSTER_STATE::MS_IDLE);
			m_iPrevPlayAnimIndex = GetAnimationIndexByState(CMonsterBase::MONSTER_STATE::MS_IDLE);
		}
	}

	if (m_bIsInterpolationing)
	{
		// m_pModelComs[0]->ResetCurrentAnimation(); // 이거 주석해제 하면 모든 애니매이션이 두번 재생된다.
		m_bIsInterpolationing = false;
	}
}

string CBossBody::GetAnimationNameByState(_uint iState)
{
	if (CMonsterBase::MONSTER_STATE::MS_IDLE & iState)
		return "IDLE";
	else if (CMonsterBase::MONSTER_STATE::MS_CHASE & iState)
		return "CHASE";
	else if (CMonsterBase::MONSTER_STATE::MS_ATTACK & iState)
	{
		_int iCurrentAttackAnimIndex = m_pOwnerMonster->GetAttackAnimIndex();
		if (CBoss::ATTACK_INDEX::ATTACK01 == iCurrentAttackAnimIndex)
			return "ATTACK01";
		else if (CBoss::ATTACK_INDEX::ATTACK02 == iCurrentAttackAnimIndex)
			return "ATTACK02";
		else if (CBoss::ATTACK_INDEX::ATTACK03 == iCurrentAttackAnimIndex)
			return "ATTACK03";
		else if (CBoss::ATTACK_INDEX::ATTACK04 == iCurrentAttackAnimIndex)
			return "ATTACK04";
	}
	else if (CMonsterBase::MONSTER_STATE::MS_DEACTIVE & iState)
		return "SPAWN";
	else if (CMonsterBase::MONSTER_STATE::MS_GETHIT & iState)
		return "GETHIT";
	else if (CMonsterBase::MONSTER_STATE::MS_ACTIVE & iState)
		return "ACTIVE";
	else if (CMonsterBase::MONSTER_STATE::MS_DEACTIVE & iState)
		return "DEACTIVE";
	else if (CMonsterBase::MONSTER_STATE::MS_BOSS_DEAD & iState)
		return "DEAD";

	return "NONE";
}

_bool CBossBody::IsUsedAttackAlert(string strAlertName)
{
	_bool bRet = false;
	auto it = find_if(m_AttackAlertChecks.begin(), m_AttackAlertChecks.end(), [&](auto& pair)
		{
			if (strAlertName == pair.first) // std::string의 == 연산자 사용
			{
				if (pair.second)
					bRet = true;
				return true; // 조건이 충족되면 true를 반환해 반복을 중단
			}
			return false;
		});

	if (it == m_AttackAlertChecks.end())
		return true;
	return bRet;
}

void CBossBody::ResetAttackAlert(string strAlertName)
{
	auto it = find_if(m_AttackAlertChecks.begin(), m_AttackAlertChecks.end(), [&strAlertName](auto& pair)
		{
			if (strAlertName == pair.first) // std::string의 == 연산자 사용
			{
				pair.second = false;
				return true; // 조건이 충족되면 true를 반환해 반복을 중단
			}
			return false;
		});
}

void CBossBody::UseAttackAlert(string strAlertName)
{
	auto it = find_if(m_AttackAlertChecks.begin(), m_AttackAlertChecks.end(), [&strAlertName](auto& pair)
		{
			if (strAlertName == pair.first) // std::string의 == 연산자 사용
			{
				pair.second = true;
				return true; // 조건이 충족되면 true를 반환해 반복을 중단
			}
			return false;
		});
}

void CBossBody::FindAlertPoint(_uint iMonsterState, _uint iState, string strCompAnimName, string AlertPointName)
{
	if (iMonsterState & iState)
	{
		_int iCurAttackAnimIndex = GetAnimationIndexByState(iState);
		if (false == strcmp(strCompAnimName.c_str(), GetAnimationNameByAnimationIndex(iCurAttackAnimIndex).c_str()))
		{
			string strCurrentAlertName = m_pModelComs[GetCurrentModelIndex()]->GetAnimation(iCurAttackAnimIndex)->GetAlertPointName();
			if (false == strcmp(AlertPointName.c_str(), strCurrentAlertName.c_str()) && false == IsUsedAttackAlert(AlertPointName))
			{
				m_pOwnerMonster->SetAlertPointName(AlertPointName);
				UseAttackAlert(AlertPointName);
			}
		}
	}
}

void CBossBody::FindAlertPointSounds(_uint iMonsterState, _uint iState, string strCompAnimName, string AlertPointName)
{
	if (iMonsterState & iState)
	{
		_int iCurAttackAnimIndex = GetAnimationIndexByState(iState);
		if (false == strcmp(strCompAnimName.c_str(), GetAnimationNameByAnimationIndex(iCurAttackAnimIndex).c_str()))
		{
			string strCurrentAlertName = m_pModelComs[GetCurrentModelIndex()]->GetAnimation(iCurAttackAnimIndex)->GetAlertPointName();
			if (false == strcmp(AlertPointName.c_str(), strCurrentAlertName.c_str()))
			{
				// m_pOwnerMonster->SetAlertPointName(AlertPointName);
				// UseAttackAlert(AlertPointName);

				// Active
				{
					/*if (AlertPointName == "ACTIVE_JUMP")
					{
						m_pGameInst->PlayEffect(TEXT("BossCutSceneJump.mp3"), 1.f);
					}

					if (AlertPointName == "ACTIVE_WEAPON1" || AlertPointName == "ACTIVE_WEAPON2" || AlertPointName == "ACTIVE_WEAPON3")
					{
						m_pGameInst->PlayEffect(TEXT("BossCutSceneWeapon.mp3"), 1.f);
					}

					if (AlertPointName == "ACTIVE_ROAR")
					{
						m_pGameInst->PlayEffect(TEXT("BossCutSceneRoar.mp3"), 1.f);
					}*/
				}
				

				// ACTIVE_ROAR
			}
		}
	}
}

void CBossBody::CheckAlertPointSoundName()
{
	string strCurName = m_pModelComs[GetCurrentModelIndex()]->GetAlertPointSoundName();

	if (m_strPrevAlertName == strCurName)
		return;


	_int iCurAttackAnimIndex = GetAnimationIndexByState(*m_pState);
	string str = GetAnimationNameByAnimationIndex(iCurAttackAnimIndex, GetCurrentModelIndex());

	// Active
	{

		if (str == "ACTIVE")
		{
			if (strCurName == "ACTIVE_JUMP")
				m_pGameInst->PlayEffect(TEXT("BossCutSceneJump.mp3"), 1.f);

			if (strCurName == "ACTIVE_WEAPON1")
				m_pGameInst->PlayEffect(TEXT("BossCutSceneWeapon.mp3"), 1.f);

			if (strCurName == "ACTIVE_WEAPON2")
				m_pGameInst->PlayEffect(TEXT("BossCutSceneWeapon.mp3"), 1.f);

			if (strCurName == "ACTIVE_WEAPON3")
				m_pGameInst->PlayEffect(TEXT("BossCutSceneWeapon.mp3"), 1.f);

			if (strCurName == "ACTIVE_ROAR")
				m_pGameInst->PlayEffect(TEXT("BossCutSceneRoar.mp3"), 1.f);
		}
	}
	
	// 몬스터 스폰 공격
	{
		if (str == "ATTACK04")
		{
			if (strCurName == "SPAWN_STATR")
				m_pGameInst->PlayEffect(TEXT("BossMonsterSpawnStart.mp3"), 1.f);

			if (strCurName == "SPAWN_END")
				m_pGameInst->PlayEffect(TEXT("BossMonsterSpawnEnd.mp3"), 1.f);

		}

		// return;
	}

	// 돌덩이 던지는 공격
	{
		
		if (str == "ATTACK02")
		{
			if (strCurName == "BOSS_ROCK_S_ROAR")
				m_pGameInst->PlayEffect(TEXT("BossRockStartRoar.mp3"), 1.f);

			if (strCurName == "BOSS_ROCK_GATHER")
				m_pGameInst->PlayEffect(TEXT("BossRockGather.mp3"), 1.f);

			if (strCurName == "BOSS_ROCK_E_ROAR")
				m_pGameInst->PlayEffect(TEXT("BossRockEndRoar.mp3"), 1.f);
		}
	}


	// ATTACK01
	// 날아서 양손 찍기
	{
		if (str == "ATTACK01")
		{
			if (strCurName == "ATTACK01_START_ROAR")
				m_pGameInst->PlayEffect(TEXT("BossRockStartRoar.mp3"), 1.f);

			if (strCurName == "ATTACK01_SMASH")
				m_pGameInst->PlayEffect(TEXT("BossAttack01Smash.mp3"), 1.f);

			if (strCurName == "ATTACK01_SMASH_END")
				m_pGameInst->PlayEffect(TEXT("BossTwoHand.mp3"), 1.f);
		}
		// 
	}

	// ATTACK03
	// 한손
	{
		if (str == "ATTACK03")
		{
			if (strCurName == "ATTACK03_ROAR")
				m_pGameInst->PlayEffect(TEXT("BossOneHandRoar.mp3"), 1.f);

			if (strCurName == "ATTACK03_SMASH")
				m_pGameInst->PlayEffect(TEXT("BossOneHandSmash.mp3"), 1.f);
		}
	}
	
	// DEAD
	// 한손
	{
		if (str == "DEAD")
		{
			if (strCurName == "DEAD_START")
			{
				m_pGameInst->PlayEffect(TEXT("BossDead.mp3"), 1.f);
				m_pGameInst->StopBGM();
			}

			if (strCurName == "DEAD_END")
				m_pGameInst->PlayEffect(TEXT("BossDeadEnd.mp3"), 1.f);


		}
	}

	m_strPrevAlertName = strCurName;

}

void CBossBody::EndNoise()
{
	m_bNoiseStart = false;

	CUITex* pUI = static_cast<CUITex*>(m_pGameInst->GetEleUI("UITexVictory"));
	if (pUI)
	{
		if (pUI->GetStartScaling())
			return;

		CUITex::UI_TEX_SCALE_DESC Desc;
		Desc.fScalingMaxSizeX = 600.f;
		Desc.fScalingMaxSizeY = 400.f;
		Desc.fScalingMinSizeX = 0.f;
		Desc.fScalingMinSizeY = 0.f;

		Desc.bHold = true;
		Desc.fHoldTime = 2.f;

		pUI->SetScalingDesc(Desc);
		pUI->StartScaling();
	}

	m_bRender = false;
	
}

HRESULT CBossBody::ReadyComponents()
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
		if (FAILED(AddComponent(LEVEL_BOSS, TEXT("Prototype_Component_Model_Boss"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelComs[0]))))
			return E_FAIL;
	}

	// Dead Noise Tex
	{
		/* Com_Texture */
		if (FAILED(AddComponent(LEVEL_BOSS, TEXT("Prototype_Component_Texture_BossDeadNoise"), TEXT("Com_Texture_Noise"), reinterpret_cast<CComponent**>(&m_pTextureNoiseCom))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBossBody::BindShaderResources()
{
	if (FAILED(m_pShaderCom->BindMatrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	_float4x4 viewMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_VIEW);
	_float4x4 projMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_PROJ);

	if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &viewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &projMatrix)))
		return E_FAIL;

	if (m_bNoiseStart)
	{
		_bool bStartNoise = m_bNoiseStart;
		if (FAILED(m_pShaderCom->BindRawValue("g_NoiseStart", &bStartNoise, sizeof _bool)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->BindRawValue("g_NoiseMaxTime", &m_fNoiseMaxTime, sizeof _float)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->BindRawValue("g_NoiseCurTime", &m_fNoiseAccTime, sizeof _float)))
			return E_FAIL;

		if (FAILED(m_pTextureNoiseCom->BindShaderResource(m_pShaderCom, "g_NoiseTexture")))
			return E_FAIL;
	}

	return S_OK;
}

CBossBody* CBossBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBossBody* pInstance = new CBossBody(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CBossBody");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject* CBossBody::Clone(void* pArg)
{
	CBossBody* pInstance = new CBossBody(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBossBody");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CBossBody::Free()
{
	__super::Free();

	SafeRelease(m_pShaderCom);

	for (auto& pModel : m_pModelComs)
		if (nullptr != pModel)
			SafeRelease(pModel);

	m_pModelComs.clear();
}
