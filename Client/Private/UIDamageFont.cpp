#include "pch.h"
#include "UIDamageFont.h"
#include "GameInstance.h"
#include "VIBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"

CUIDamageFont::CUIDamageFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CUIElement(pDevice, pContext)
{
}

CUIDamageFont::CUIDamageFont(const CUIDamageFont& Prototype)
    :
    CUIElement(Prototype)
{
}

HRESULT CUIDamageFont::InitializePrototype()
{
    return S_OK;
}

HRESULT CUIDamageFont::Initialize(void* pArg)
{
    if (!pArg)
        return E_FAIL;

    UI_DAMAGE_FONT_DESC* pDesc = static_cast<UI_DAMAGE_FONT_DESC*>(pArg);
    m_eUIDamageColor = pDesc->eUIDamageFontColor;
    m_fStartScale = pDesc->fStartScale;
    m_fEndScale = pDesc->fEndSacle;
	m_fScalingSP = pDesc->fScalingSP;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents(nullptr)))
        return E_FAIL;

    return S_OK;
}

void CUIDamageFont::PriorityUpdate(_float fTimeDelta)
{

}

void CUIDamageFont::Update(_float fTimeDelta)
{
	if (m_bRenderAvtive)
	{
		m_fAcc += fTimeDelta * m_fScalingSP;

		if (m_fStartScale - m_fAcc < m_fEndScale)
		{
			m_fAcc = m_fEndScale;
			m_bRenderAvtive = false;
			m_pTransformCom->Scaling(m_fStartScale - m_fAcc, m_fStartScale - m_fAcc, 1.f);
		}
		else
		{
			m_pTransformCom->Scaling(m_fStartScale - m_fAcc, m_fStartScale - m_fAcc, 1.f);
		}
	}
	else
	{
		m_fAcc = 0.f;
		m_pTransformCom->Scaling(m_fStartScale, m_fStartScale, m_fStartScale);
	}
}

void CUIDamageFont::LateUpdate(_float fTimeDelta)
{
	if (m_bRenderAvtive)
		m_pGameInst->AddRenderObject(CRenderer::RG_UI, this);
}

HRESULT CUIDamageFont::Render()
{
	if (FAILED(BindShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(9);
	m_pVIBufferCom->BindBufferDesc();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUIDamageFont::BindShaderResources()
{
	if (FAILED(m_pTransformCom->BindShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureComs[GetDamageFontNum()]->BindShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	_int iColor = GetDamageFontColor();
	if (FAILED(m_pShaderCom->BindRawValue("g_iDamageFontColor", &iColor, sizeof _int)))
		return E_FAIL;

	_float fRatio = (m_fAcc + m_fEndScale) / m_fStartScale;
	// cout << fRatio << endl;
	if (FAILED(m_pShaderCom->BindRawValue("g_CurAlphaTime", &fRatio, sizeof _float)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIDamageFont::ReadyComponents(void* pArg)
{
	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex_UI"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	// Tex
	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Font_0"), TEXT("Com_Texture_0"), reinterpret_cast<CComponent**>(&m_pTextureComs[0]))))
		return E_FAIL;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Font_1"), TEXT("Com_Texture_1"), reinterpret_cast<CComponent**>(&m_pTextureComs[1]))))
		return E_FAIL;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Font_2"), TEXT("Com_Texture_2"), reinterpret_cast<CComponent**>(&m_pTextureComs[2]))))
		return E_FAIL;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Font_3"), TEXT("Com_Texture_3"), reinterpret_cast<CComponent**>(&m_pTextureComs[3]))))
		return E_FAIL;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Font_4"), TEXT("Com_Texture_4"), reinterpret_cast<CComponent**>(&m_pTextureComs[4]))))
		return E_FAIL;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Font_5"), TEXT("Com_Texture_5"), reinterpret_cast<CComponent**>(&m_pTextureComs[5]))))
		return E_FAIL;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Font_6"), TEXT("Com_Texture_6"), reinterpret_cast<CComponent**>(&m_pTextureComs[6]))))
		return E_FAIL;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Font_7"), TEXT("Com_Texture_7"), reinterpret_cast<CComponent**>(&m_pTextureComs[7]))))
		return E_FAIL;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Font_8"), TEXT("Com_Texture_8"), reinterpret_cast<CComponent**>(&m_pTextureComs[8]))))
		return E_FAIL;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Font_9"), TEXT("Com_Texture_9"), reinterpret_cast<CComponent**>(&m_pTextureComs[9]))))
		return E_FAIL;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Font_10"), TEXT("Com_Texture_10"), reinterpret_cast<CComponent**>(&m_pTextureComs[10]))))
		return E_FAIL;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Font_11"), TEXT("Com_Texture_11"), reinterpret_cast<CComponent**>(&m_pTextureComs[11]))))
		return E_FAIL;

	return S_OK;
}

CUIDamageFont* CUIDamageFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIDamageFont* pInst = new CUIDamageFont(pDevice, pContext);

	if (FAILED(pInst->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CUIDamageFont");
		SafeRelease(pInst);
	}

	return pInst;
}

CUIDamageFont* CUIDamageFont::Clone(void* pArg)
{
	CUIDamageFont* pInst = new CUIDamageFont(*this);

	if (FAILED(pInst->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIDamageFont");
		SafeRelease(pInst);
	}

	return pInst;
}

void CUIDamageFont::Free()
{
	__super::Free();

	SafeRelease(m_pShaderCom);
	SafeRelease(m_pVIBufferCom);
	SafeRelease(m_pTextureCom);

	for (_int i = 0; i < 12; ++i)
		SafeRelease(m_pTextureComs[i]);
}
