#include "Tool_Main_Camera.h"
#include "GameInstance.h"
#include "Input_Device.h"

CTool_Main_Camera::CTool_Main_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CCamera(pDevice, pContext)
{
}

CTool_Main_Camera::CTool_Main_Camera(const CTool_Main_Camera& Prototype)
	:
	CCamera(Prototype)
{
}

HRESULT CTool_Main_Camera::InitializePrototype()
{
	return S_OK;
}

HRESULT CTool_Main_Camera::Initialize(void* pArg)
{
	TOOL_MAIN_CAM_DESC* pDesc = static_cast<TOOL_MAIN_CAM_DESC*>(pArg);

	m_fMouseSensor = pDesc->fMouseSensor;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CTool_Main_Camera::PriorityUpdate(_float fTimeDelta)
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

	// 파이프 라인에 뷰, 투영 행렬 계산하도록 넘긴다.
	__super::Compute_PipeLine_Matrices();
}

void CTool_Main_Camera::Update(_float fTimeDelta)
{
}

void CTool_Main_Camera::LateUpdate(_float fTimeDelta)
{
}

HRESULT CTool_Main_Camera::Render()
{
	return S_OK;
}

CTool_Main_Camera* CTool_Main_Camera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTool_Main_Camera* pInstance = new CTool_Main_Camera(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CTool_Main_Camera");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject* CTool_Main_Camera::Clone(void* pArg)
{
	CTool_Main_Camera* pInstance = new CTool_Main_Camera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTool_Main_Camera");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CTool_Main_Camera::Free()
{
	__super::Free();
}
