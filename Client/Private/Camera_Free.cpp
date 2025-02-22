#include "pch.h"
#include "Camera_Free.h"
#include "GameInstance.h"
#include "Player.h"

CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CCamera(pDevice, pContext)
{
}

CCamera_Free::CCamera_Free(const CCamera_Free& Prototype)
	:
	CCamera(Prototype)
{
}

HRESULT CCamera_Free::InitializePrototype()
{
	return S_OK;
}

HRESULT CCamera_Free::Initialize(void* pArg)
{
	CAMERA_FREE_DESC* pDesc = static_cast<CAMERA_FREE_DESC*>(pArg);

	m_fMouseSensor = pDesc->fMouseSensor;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, m_iLevelIndex, TEXT("A")));
	m_pPlayerTransform = static_cast<CTransform*>(m_pPlayer->GetComponent(TEXT("Com_Transform")));

	SafeAddRef(m_pPlayer);
	SafeAddRef(m_pPlayerTransform);

	return S_OK;
}

void CCamera_Free::PriorityUpdate(_float fTimeDelta)
{
	switch (m_eEffectType)
	{
	case Client::CCamera_Free::EFFECT_DEFAULT:
	{
		if (m_bAttachToPlayer)
			UpdateImGameModeCamera(fTimeDelta);
		else
			UpdateDebugModeCamera(fTimeDelta);
	}
		break;
	case Client::CCamera_Free::EFFECT_CINEMATIC:
		UpdateCinematic(fTimeDelta);
		break;
	case Client::CCamera_Free::EFFECT_SHAKE:
		UpdateShake(fTimeDelta);
		break;
	case Client::CCamera_Free::EFFECT_END:
		break;
	default:
		break;
	}

}

void CCamera_Free::Update(_float fTimeDelta)
{
	
}

void CCamera_Free::LateUpdate(_float fTimeDelta)
{

}

HRESULT CCamera_Free::Render()
{
	return S_OK;
}

void CCamera_Free::UpdateImGameModeCamera(_float fTimeDelta)
{
	m_pTransformCom->SetTransformState(CTransform::STATE_RIGHT, m_pPlayerTransform->GetTransformState(CTransform::STATE::STATE_RIGHT));
	m_pTransformCom->SetTransformState(CTransform::STATE_UP, m_pPlayerTransform->GetTransformState(CTransform::STATE::STATE_UP));
	m_pTransformCom->SetTransformState(CTransform::STATE_LOOK, m_pPlayerTransform->GetTransformState(CTransform::STATE::STATE_LOOK));
	m_pTransformCom->SetTransformState(CTransform::STATE_POS, m_pPlayerTransform->GetTransformState(CTransform::STATE::STATE_POS));
	__super::Compute_PipeLine_Matrices();
}

void CCamera_Free::UpdateDebugModeCamera(_float fTimeDelta)
{
	if (m_pGameInst->KeyDown(KEY::TAB))
	{
		m_bBlockCamMove == false ? m_bBlockCamMove = true : m_bBlockCamMove = false;
	}

	if (m_pGameInst->KeyPressing(KEY::W))
	{
		if (m_bBlockCamMove) return;

		m_pTransformCom->GoFoward(fTimeDelta);
	}

	if (m_pGameInst->KeyPressing(KEY::S))
	{
		if (m_bBlockCamMove) return;

		m_pTransformCom->GoBackward(fTimeDelta);
	}

	if (m_pGameInst->KeyPressing(KEY::A))
	{
		if (m_bBlockCamMove) return;

		m_pTransformCom->GoLeft(fTimeDelta);
	}

	if (m_pGameInst->KeyPressing(KEY::D))
	{
		if (m_bBlockCamMove) return;

		m_pTransformCom->GoRight(fTimeDelta);
	}

	if (m_pGameInst->KeyPressing(KEY::E))
	{
		if (m_bBlockCamMove) return;

		m_pTransformCom->GoUp(fTimeDelta);
	}

	if (m_pGameInst->KeyPressing(KEY::Q))
	{
		if (m_bBlockCamMove) return;

		m_pTransformCom->GoDown(fTimeDelta);
	}

	_long MouseMove = {};

	if (MouseMove = m_pGameInst->GetMouseMove(MOUSE_MOVE::X))
	{
		if (m_bBlockCamMove) return;

		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * fTimeDelta * m_fMouseSensor);
	}

	if (MouseMove = m_pGameInst->GetMouseMove(MOUSE_MOVE::Y))
	{
		if (m_bBlockCamMove) return;

		m_pTransformCom->Turn(m_pTransformCom->GetTransformState(CTransform::STATE_RIGHT), MouseMove * fTimeDelta * m_fMouseSensor);
	}

	__super::Compute_PipeLine_Matrices();
}

void CCamera_Free::UpdateShake(_float fTimeDelta)
{
	static _float fAcc = 0.f;

	fAcc += fTimeDelta;
	if (fAcc > 0.3f)
	{
		fAcc = 0.f;
		m_eEffectType = EFFECT_DEFAULT;
	}
	else
	{
		_vector vPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);
		_vector vLookDir = XMVector3Normalize(m_pTransformCom->GetTransformState(CTransform::STATE_LOOK));
		_vector vLookPos = vPos + vLookDir * 1000.f;

		_float fRandX = m_pGameInst->ComputeRandom(-30.f, 30.f);
		_float fRandY = m_pGameInst->ComputeRandom(-30.f, 30.f);

		_vector vShakePos = vLookPos + _vector{ fRandX, fRandY, 0.f, 0.f };

		// m_pGameInst->ComputeRandom()
		// _vector vLookAtPos{};
		m_pTransformCom->LookAt(vShakePos);
		m_pTransformCom->SetTransformState(CTransform::STATE_POS, m_pPlayerTransform->GetTransformState(CTransform::STATE::STATE_POS));
		__super::Compute_PipeLine_Matrices();
	}


	// __super::Compute_PipeLine_Matrices();
}

void CCamera_Free::UpdateCinematic(_float fTimeDelta)
{
	static _float fMaxTime = 13.f;
	static _float fAccTime = 0.f;
	_vector vLookAtPos = { 0.f, 5.f, 50.f, 1.f };

	m_pGameInst->RenderOffPlayerUI();

	auto pObj = m_pGameInst->GetGameObjctInLayerBack(LEVEL_BOSS, LEVEL_BOSS, TEXT("Layer_Boss"));
	if (pObj)
	{
		CTransform* pTC = static_cast<CTransform*>(pObj->GetComponent(TEXT("Com_Transform")));
		if (pTC)
		{
			_vector vTargetPos = pTC->GetTransformState(CTransform::STATE_POS);

			_vector vLook = -pTC->GetTransformState(CTransform::STATE_RIGHT);
			_matrix RotY = XMMatrixRotationY(XMConvertToRadians(10.f * fAccTime));
			_vector vRotDir = XMVector3Normalize(XMVector3TransformNormal(vLook, RotY));
			_vector vDstPos = vTargetPos += vRotDir * 15.f;
			vDstPos.m128_f32[1] = 2.f;

			m_pTransformCom->SetTransformState(CTransform::STATE_POS, vDstPos);
			m_pTransformCom->LookAt(vLookAtPos);

			__super::Compute_PipeLine_Matrices();
		}
	}


	fAccTime += fTimeDelta * 2.f;
	if (fAccTime >= fMaxTime)
	{
		fAccTime = 0.f;
		m_eEffectType = EFFECT_DEFAULT;
		m_pGameInst->RenderOnPlayerUI();

	}
	else
	{
		// m_pTransformCom->LookAt(vLookAtPos);
		// m_pTransformCom->GetWorldMatrix();
	}

	
}

CCamera_Free* CCamera_Free::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Free* pInstance = new CCamera_Free(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Free");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Free::Clone(void* pArg)
{
	CCamera_Free* pInstance = new CCamera_Free(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Free");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CCamera_Free::Free()
{
	__super::Free();

	SafeRelease(m_pPlayer);
	SafeRelease(m_pPlayerTransform);
}
