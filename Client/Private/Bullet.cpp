#include "pch.h"
#include "Bullet.h"

CBullet::CBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CProjectile(pDevice, pContext)
{
}

CBullet::CBullet(const CBullet& Prototype)
	:
	CProjectile(Prototype)
{
}

HRESULT CBullet::InitializePrototype()
{
	return S_OK;
}

HRESULT CBullet::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CBullet::PriorityUpdate(_float fTimeDelta)
{

}

void CBullet::Update(_float fTimeDelta)
{

}

void CBullet::LateUpdate(_float fTimeDelta)
{

}

HRESULT CBullet::Render()
{
	return S_OK;
}

HRESULT CBullet::ReadyComponents()
{
	return S_OK;
}

HRESULT CBullet::BindShaderResources()
{
	return S_OK;
}

CBullet* CBullet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBullet* pInstance = new CBullet(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CBullet");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject* CBullet::Clone(void* pArg)
{
	CBullet* pInstance = new CBullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBullet");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CBullet::Free()
{
	__super::Free();
}
