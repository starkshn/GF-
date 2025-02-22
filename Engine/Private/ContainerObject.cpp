#include "ContainerObject.h"

CContainerObject::CContainerObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: 
	CGameObject(pDevice, pContext)
{
}

CContainerObject::CContainerObject(const CContainerObject& Prototype)
	:
	CGameObject(Prototype)
{
}

HRESULT CContainerObject::InitializePrototype()
{
	return S_OK;
}

HRESULT CContainerObject::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		CONTAINER_OBJECT_DESC* pDesc = static_cast<CONTAINER_OBJECT_DESC*>(pArg);
		m_PartObjects.resize(pDesc->iNumPartObjects);
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CContainerObject::PriorityUpdate(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
		if (nullptr != pPartObject)
			pPartObject->PriorityUpdate(fTimeDelta);
}

void CContainerObject::Update(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
		if (nullptr != pPartObject)
			pPartObject->Update(fTimeDelta);
}

void CContainerObject::LateUpdate(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
		if (nullptr != pPartObject)
			pPartObject->LateUpdate(fTimeDelta);
}

HRESULT CContainerObject::Render()
{
	return S_OK;
}

CComponent* CContainerObject::GetComponent(const _wstring& strCompTag)
{
	auto iter = m_Components.find(strCompTag);

	if (iter != m_Components.end())
		return iter->second;

	return nullptr;
}

CComponent* CContainerObject::GetComponent(_uint iPartObjectIndex, const _wstring& strPartComponentTag)
{
	if (iPartObjectIndex >= m_PartObjects.size())
		return nullptr;

	return m_PartObjects[iPartObjectIndex]->GetComponent(strPartComponentTag);
}

void CContainerObject::Free()
{
	__super::Free();

	for (auto& pPartObj : m_PartObjects)
		SafeRelease(pPartObj);

	m_PartObjects.clear();
}
