#include "pch.h"
#include "DebugCamera.h"
#include "GameInstance.h"
#include "Player.h"

CDebugCamera::CDebugCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CCamera(pDevice, pContext)
{
}

CDebugCamera::CDebugCamera(const CDebugCamera& Prototype)
	:
	CCamera(Prototype)
{
}

HRESULT CDebugCamera::InitializePrototype()
{
	return S_OK;
}

HRESULT CDebugCamera::Initialize(void* pArg)
{
	CAMERA_FREE_DESC* pDesc = static_cast<CAMERA_FREE_DESC*>(pArg);
	m_fMouseSensor = pDesc->fMouseSensor;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CDebugCamera::PriorityUpdate(_float fTimeDelta)
{
	UpdateDebugModeCamera(fTimeDelta);
}

void CDebugCamera::Update(_float fTimeDelta)
{

}

void CDebugCamera::LateUpdate(_float fTimeDelta)
{

}

HRESULT CDebugCamera::Render()
{
	return S_OK;
}

void CDebugCamera::UpdateImGameModeCamera(_float fTimeDelta)
{
	//m_pTransformCom->SetTransformState(CTransform::STATE_RIGHT, m_pPlayerTransform->GetTransformState(CTransform::STATE::STATE_RIGHT));
	//m_pTransformCom->SetTransformState(CTransform::STATE_UP, m_pPlayerTransform->GetTransformState(CTransform::STATE::STATE_UP));
	//m_pTransformCom->SetTransformState(CTransform::STATE_LOOK, m_pPlayerTransform->GetTransformState(CTransform::STATE::STATE_LOOK));
	//m_pTransformCom->SetTransformState(CTransform::STATE_POS, m_pPlayerTransform->GetTransformState(CTransform::STATE::STATE_POS));
	__super::Compute_PipeLine_Matrices();
}

void CDebugCamera::UpdateDebugModeCamera(_float fTimeDelta)
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

CDebugCamera* CDebugCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDebugCamera* pInstance = new CDebugCamera(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CDebugCamera");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject* CDebugCamera::Clone(void* pArg)
{
	CDebugCamera* pInstance = new CDebugCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDebugCamera");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CDebugCamera::Free()
{
	__super::Free();
}
