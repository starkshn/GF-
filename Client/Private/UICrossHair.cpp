#include "pch.h"
#include "UICrossHair.h"
#include "GameInstance.h"
#include "VIBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"

CUICrossHair::CUICrossHair(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CUIElement(pDevice, pContext)
{
}

CUICrossHair::CUICrossHair(const CUICrossHair& Prototype)
    :
    CUIElement(Prototype)
{
}

HRESULT CUICrossHair::InitializePrototype()
{
    return S_OK;
}

HRESULT CUICrossHair::Initialize(void* pArg)
{
    if (!pArg)
        return E_FAIL;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents(nullptr)))
        return E_FAIL;

    return S_OK;
}

void CUICrossHair::PriorityUpdate(_float fTimeDelta)
{
}

void CUICrossHair::Update(_float fTimeDelta)
{
    static _float fAcc = 0.f;
    static _float fAccTime = 0.f;
    static _bool bOnce = true;
    const static _float fInitScale = 50.f;

    if (m_bStart)
    {
        fAccTime += fTimeDelta;

        if (bOnce)
        {
            fAcc += fTimeDelta * 250.f;

            if (fAccTime > 0.1f)
            {
                bOnce = false;
                fAccTime = 0.f;
            }
        }
        else
        {
            fAcc -= fTimeDelta * 250.f;
            if (fAccTime > 0.1f)
            {
                bOnce = true;
                fAcc = 0.f;
                fAccTime = 0.f;
                m_bStart = false;
                m_pTransformCom->Scaling(fInitScale, fInitScale, fInitScale);
            }
        }

        m_pTransformCom->Scaling(fInitScale + fAcc, fInitScale + fAcc, fInitScale + fAcc);
        cout << fInitScale + fAcc << endl;
    }
}

void CUICrossHair::LateUpdate(_float fTimeDelta)
{
    if (!IsActiveRender())
        return;

    m_pGameInst->AddRenderObject(CRenderer::RG_UI, this);
}

HRESULT CUICrossHair::Render()
{
    if (FAILED(BindShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(0);
    m_pVIBufferCom->BindBufferDesc();
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CUICrossHair::BindShaderResources()
{
    if (FAILED(m_pTransformCom->BindShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->BindShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUICrossHair::ReadyComponents(void* pArg)
{
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex_UI"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_CrossHair"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

CUICrossHair* CUICrossHair::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUICrossHair* pInst = new CUICrossHair(pDevice, pContext);

    if (FAILED(pInst->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CUICrossHair");
        SafeRelease(pInst);
    }

    return pInst;
}

CUICrossHair* CUICrossHair::Clone(void* pArg)
{
    CUICrossHair* pInst = new CUICrossHair(*this);

    if (FAILED(pInst->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CUICrossHair");
        SafeRelease(pInst);
    }

    return pInst;
}

void CUICrossHair::Free()
{
    __super::Free();

    SafeRelease(m_pShaderCom);
    SafeRelease(m_pVIBufferCom);
    SafeRelease(m_pTextureCom);
}
