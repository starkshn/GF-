#include "Texture.h"
#include "Shader.h"

CTexture::CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CComponent(pDevice, pContext)
{
}

CTexture::CTexture(const CTexture& Prototype)
    :
    CComponent(Prototype),
    m_iNumSRVs(Prototype.m_iNumSRVs),
    m_SRVs{ Prototype.m_SRVs }
{
    for (auto& pSRV : m_SRVs)
        SafeAddRef(pSRV);
}

HRESULT CTexture::InitializePrototype(const _tchar* pTextureFilePath, _uint iNumTexture)
{
    m_iNumSRVs = iNumTexture;

    _tchar			szTextureFilePath[MAX_PATH] = TEXT("");

    for (size_t i = 0; i < m_iNumSRVs; i++)
    {
        // https://www.notion.so/wsprintf-240930-1114bccaf9eb803a9782f255852318db?pvs=4
        // 형식화된 문자열을 생성하는 함수
        wsprintf(szTextureFilePath, pTextureFilePath, i);

        /* 드라이브경로, 디렉토리경로, 파일네임, 파일확장자. */
        // 드라이브 경로, 길이, dir 경로, 길이, 파일 이름, 길이, 파일 확장자, 길이
        _tchar		szEXT[MAX_PATH] = TEXT("");

        /* D:\정의훈\147\3d\Framework\Client\Bin\Resources\Textures\Default.jpg */
        _wsplitpath_s(szTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);

        HRESULT		hr = {};

        ID3D11ShaderResourceView* pSRV = { nullptr };

        if (false == lstrcmpW(szEXT, TEXT(".dds")))
        {
            hr = DirectX::CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);
        }
        else if (false == lstrcmpW(szEXT, TEXT(".tga")))
        {

        }
        else
        {
            hr = DirectX::CreateWICTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);
        }


        if (FAILED(hr))
            return E_FAIL;

        m_SRVs.push_back(pSRV);
    }


    return S_OK;
}

HRESULT CTexture::Initialize(void* pArg)
{
    return S_OK;
}


HRESULT CTexture::BindShaderResource(CShader* pShader, const _char* pConstantName, _uint iSRVIdx)
{
    if (iSRVIdx >= m_iNumSRVs) return E_FAIL;
    
    return pShader->BindSRV(pConstantName, m_SRVs[iSRVIdx]);
}

HRESULT CTexture::BindShaderResources(CShader* pShader, const _char* pConstantName)
{
    // m_SRVs는 벡터니까 가장 앞주소를 front로, 벡터의 사이즈를 넘긴다.(241025, Terrain Mask)
    return pShader->BindSRVs(pConstantName, &m_SRVs.front(), static_cast<_uint>(m_SRVs.size()));
}

CTexture* CTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures)
{
    CTexture* pInstance = new CTexture(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype(pTextureFilePath, iNumTextures)))
    {
        MSG_BOX("Failed to Created : CTexture");
        SafeRelease(pInstance);
    }
    return pInstance;
}

CComponent* CTexture::Clone(void* pArg)
{
    CTexture* pInstance = new CTexture(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CTexture");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CTexture::Free()
{
    __super::Free();

    for (auto& pSRV : m_SRVs)
        SafeRelease(pSRV);

    m_SRVs.clear();
}
