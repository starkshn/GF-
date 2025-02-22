#include "pch.h"
#include "Arrow.h"

CArrow::CArrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CProjectile(pDevice, pContext)
{
}

CArrow::CArrow(const CArrow& Prototype)
	:
	CProjectile(Prototype)
{
}

HRESULT CArrow::InitializePrototype()
{
	return S_OK;
}

HRESULT CArrow::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CArrow::PriorityUpdate(_float fTimeDelta)
{
}

void CArrow::Update(_float fTimeDelta)
{
}

void CArrow::LateUpdate(_float fTimeDelta)
{
}

HRESULT CArrow::Render()
{
	return S_OK;
}

HRESULT CArrow::ReadyComponents()
{
	return S_OK;
}

HRESULT CArrow::BindShaderResources()
{
	return S_OK;
}

CArrow* CArrow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CArrow* pInstance = new CArrow(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CArrow");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject* CArrow::Clone(void* pArg)
{
	CArrow* pInstance = new CArrow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CArrow");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CArrow::Free()
{
	__super::Free();
}
