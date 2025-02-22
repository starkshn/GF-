#include "Tool_Camera.h"
#include "Tool_Defines.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Transform.h"

CTool_Camera::CTool_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CGameObject(pDevice, pContext)
{
}

CTool_Camera::CTool_Camera(const CTool_Camera& Prototype)
	:
	CGameObject(Prototype)
{
}

HRESULT CTool_Camera::InitializePrototype()
{
	return S_OK;
}

HRESULT CTool_Camera::Initialize(void* pArg)
{
	if (__super::Initialize(pArg))
		return E_FAIL;

	if (Ready_Components())
		return E_FAIL;

	m_pTransformCom->SetSpeed(20.f);
	m_pTransformCom->SetRotationSpeed(3.f);

	return S_OK;
}

void CTool_Camera::PriorityUpdate(_float fTimeDelta)
{

}

void CTool_Camera::Update(_float fTimeDelta)
{
	if (GetKeyState('W') & 0x8000)
	{
		m_pTransformCom->GoFoward(fTimeDelta);
	}

	if (GetKeyState('S') & 0x8000)
	{
		m_pTransformCom->GoBackward(fTimeDelta);
	}

	if (GetKeyState('A') & 0x8000)
	{
		m_pTransformCom->GoLeft(fTimeDelta);
	}

	if (GetKeyState('D') & 0x8000)
	{
		m_pTransformCom->GoRight(fTimeDelta);
	}

	if (GetKeyState('E') & 0x8000)
	{
		m_pTransformCom->GoUp(fTimeDelta);
	}

	if (GetKeyState('Q') & 0x8000)
	{
		m_pTransformCom->GoDown(fTimeDelta);
	}

	if (GetKeyState(VK_LEFT) & 0x8000)
	{
		_vector vUP = m_pTransformCom->GetTransformState(CTransform::STATE_UP);
		m_pTransformCom->Turn(_fvector(vUP), -fTimeDelta);
	}

	if (GetKeyState(VK_RIGHT) & 0x8000)
	{
		_vector vUP = m_pTransformCom->GetTransformState(CTransform::STATE_UP);
		m_pTransformCom->Turn(_fvector(vUP), fTimeDelta);
	}

	if (GetKeyState(VK_UP) & 0x8000)
	{
		_vector vRight = m_pTransformCom->GetTransformState(CTransform::STATE_RIGHT);
		m_pTransformCom->Turn(_fvector(vRight), -fTimeDelta);
	}

	if (GetKeyState(VK_DOWN) & 0x8000)
	{
		_vector vRight = m_pTransformCom->GetTransformState(CTransform::STATE_RIGHT);
		m_pTransformCom->Turn(_fvector(vRight), fTimeDelta);
	}

	_float4x4 matWorldView = m_pTransformCom->GetWorldMatrixToolCam();
	_matrix matWorld;
	matWorld = XMLoadFloat4x4(&matWorldView);

	_matrix matInv = XMMatrixInverse(nullptr, matWorld);
	XMStoreFloat4x4(&m_ViewMatrix, matInv);

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixPerspectiveFovLH(90.f, static_cast<_float>(g_iWinSizeX / g_iWinSizeY), 1.f, 1000.f));
}

void CTool_Camera::LateUpdate(_float fTimeDelta)
{
	m_pGameInst->AddRenderObject(CRenderer::RG_PRIORITY, this);
}

HRESULT CTool_Camera::Render()
{
	if (Bind_ShaderResources())
		return E_FAIL;
	
	return S_OK;
}

HRESULT CTool_Camera::Ready_Components()
{
	if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxNorTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxNorTex_TempWater"), TEXT("Com_Shader_TempWater"), reinterpret_cast<CComponent**>(&m_pShaderTempWaterCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTool_Camera::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderTempWaterCom->BindMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderTempWaterCom->BindMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	return S_OK;
}

CTool_Camera* CTool_Camera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTool_Camera* pInst = new CTool_Camera(pDevice, pContext);

	if (FAILED(pInst->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CTool_Camera");
		Safe_Release(pInst);
	}

	return pInst;
}

CGameObject* CTool_Camera::Clone(void* pArg)
{
	CTool_Camera* pInst = new CTool_Camera(*this);

	if (FAILED(pInst->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTool_Camera");
		Safe_Release(pInst);
	}

	return pInst;
}

void CTool_Camera::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
}
