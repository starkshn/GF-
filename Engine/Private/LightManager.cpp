#include "LightManager.h"
#include "Light.h"

CLightManager::CLightManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    m_pDevice{ pDevice },
    m_pContext{ pContext }
{
    SafeAddRef(m_pDevice);
    SafeAddRef(m_pContext);

}

const LIGHT_DESC* CLightManager::GetLightDesc(_uint iIdx) const
{
    auto iter = m_Lights.begin();

    for (_uint i = 0; i < iIdx; ++i) ++iter;

    return (*iter)->GetLightDesc();
}

HRESULT CLightManager::Initialize()
{
    return S_OK;
}

HRESULT CLightManager::AddLight(const LIGHT_DESC& LightDesc)
{
    CLight* pLight = CLight::Create(m_pDevice, m_pContext, LightDesc);

    if (nullptr == pLight)
        return E_FAIL;

    m_Lights.push_back(pLight);

    return S_OK;
}

HRESULT CLightManager::Render(CShader* pShader, CVIBufferRect* pVIBuffer)
{
    for (auto& pLight : m_Lights)
    {
        if (pLight->IsActive())
            pLight->Render(pShader, pVIBuffer);
    }
    
    return S_OK;
}

_bool CLightManager::IsActiveLight(const string& strLightName)
{
    CLight* pLight = FindLight(strLightName);
    if (nullptr == pLight)
        return false;

    return pLight->IsActive();
}

void CLightManager::ActiveLight(const string& strLightName)
{
    CLight* pLight = FindLight(strLightName);
    if (nullptr == pLight)
        return;

    if (pLight->IsActive())
        return;

    pLight->ActiveLight();
}

void CLightManager::DeActiveLight(const string& strLightName)
{
    CLight* pLight = FindLight(strLightName);
    if (nullptr == pLight)
        return;

    if (false == pLight->IsActive())
        return;

    pLight->DeActiveLight();
}

void CLightManager::UpdateLightPos(const string& strLightName, _fvector vWorldPos)
{
    CLight* pLight = FindLight(strLightName);
    if (nullptr == pLight)
        return;

    if (false == pLight->IsActive())
        return;

    if (LIGHT_DESC::TYPE_POINT == pLight->GetLightDesc()->TYPE_POINT)
        pLight->SetPointLightPos(vWorldPos);
}

void CLightManager::UpdatePointLightRange(const string& strLightName, _float fRange)
{
    CLight* pLight = FindLight(strLightName);
    if (nullptr == pLight)
        return;

    if (false == pLight->IsActive())
        return;

    if (LIGHT_DESC::TYPE_POINT == pLight->GetLightDesc()->TYPE_POINT)
        pLight->SetPointLightRange(fRange);
}

CLight* CLightManager::FindLight(const string& strLightName)
{
    auto iter = find_if(m_Lights.begin(), m_Lights.end(), [&](CLight* pLight)->_bool
        {
            if (false == strcmp(strLightName.c_str(), pLight->GetLightDesc()->strLightName.c_str()))
                return true;
            return false;
        }
    );
    
    if (iter == m_Lights.end())
        return nullptr;
    return *iter;
}

void CLightManager::Clear(_uint iLevelID)
{
    for (auto& pLight : m_Lights)
        SafeRelease(pLight);

    m_Lights.clear();
}

CLightManager* CLightManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLightManager* pInstance = new CLightManager(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLightManager");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CLightManager::Free()
{
    __super::Free();

    for (auto& pLight : m_Lights)
        SafeRelease(pLight);

    m_Lights.clear();

    SafeRelease(m_pDevice);
    SafeRelease(m_pContext);
}
