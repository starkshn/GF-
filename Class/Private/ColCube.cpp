
#include "ColCube.h"
#include "GameInstance.h"
#include "ColBoundingBox.h"
#include "Transform.h"
#include "Level_Tool.h"

CColCube::CColCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CGameObject(pDevice, pContext)
{
}

CColCube::CColCube(const CColCube& Prototype)
	:
	CGameObject(Prototype)
{
}

HRESULT CColCube::InitializePrototype()
{
	return S_OK;
}

HRESULT CColCube::Initialize(void* pArg)
{
	if (__super::Initialize(pArg))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;

}

void CColCube::PriorityUpdate(_float fTimeDelta)
{
}

void CColCube::Update(_float fTimeDelta)
{
	m_pBoundingBox->Update_Bouding_Box(m_pTransformCom);
}

void CColCube::LateUpdate(_float fTimeDelta)
{
	m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CColCube::Render()
{
	_float4x4 viewMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_VIEW);
	_float4x4 projMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_PROJ);

	if (FAILED(m_pBoundingBox->Render_Bounding_Box(m_pTransformCom, XMLoadFloat4x4(&viewMatrix), XMLoadFloat4x4(&projMatrix))))
		return E_FAIL;

	return S_OK;
}

HRESULT CColCube::Ready_Components()
{
	// Com_BoudingBox
	{
		if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_BoundingBox"), TEXT("Com_BoundingBox"), reinterpret_cast<CComponent**>(&m_pBoundingBox))))
			return E_FAIL;
	}

	return S_OK;
}



CColCube* CColCube::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CColCube* pInst = new CColCube(pDevice, pContext);

	if (FAILED(pInst->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CColCube");
		SafeRelease(pInst);
	}

	return pInst;
}

CGameObject* CColCube::Clone(void* pArg)
{
	CColCube* pInst = new CColCube(*this);

	if (FAILED(pInst->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CColCube");
		SafeRelease(pInst);
	}

	return pInst;
}

void CColCube::Free()
{
	__super::Free();

	SafeRelease(m_pBoundingBox);
}
