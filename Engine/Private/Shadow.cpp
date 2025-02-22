#include "Shadow.h"


CShadow::CShadow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{

	SafeAddRef(m_pDevice);
	SafeAddRef(m_pContext);
}

HRESULT CShadow::Initialize()
{
	return S_OK;
}

void CShadow::Update()
{
}

CShadow* CShadow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShadow* pInstance = new CShadow(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CShadow");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CShadow::Free()
{
	__super::Free();

	SafeRelease(m_pDevice);
	SafeRelease(m_pContext);
}