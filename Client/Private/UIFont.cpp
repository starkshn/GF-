#include "pch.h"
#include "UIFont.h"
#include "GameInstance.h"
#include "VIBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"

CUIFont::CUIFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CUIElement(pDevice, pContext)
{
}

CUIFont::CUIFont(const CUIFont& Prototype)
	:
	CUIElement(Prototype)
{
}

HRESULT CUIFont::InitializePrototype()
{
	return S_OK;
}

HRESULT CUIFont::Initialize(void* pArg)
{
	if (pArg)
	{
		UI_FONT_DESC* pDesc = static_cast<UI_FONT_DESC*>(pArg);
		m_eFontColor = pDesc->eFontColor;

		if (pDesc->fFontScalingSP != 0.f)
			m_fFontScalingSP = pDesc->fFontScalingSP;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(ReadyComponents(nullptr)))
		return E_FAIL;


	m_fMaxSizeX = m_fSSizeX + 50.f;
	m_fMaxSizeY = m_fSSizeY + 50.f;

	m_fMinSizeX = m_fSSizeX;
	m_fMinSizeY = m_fSSizeY;

	return S_OK;
}

void CUIFont::PriorityUpdate(_float fTimeDelta)
{
}

void CUIFont::Update(_float fTimeDelta)
{
	if (m_bStartIn)
		IncreaseScaling(fTimeDelta);
	else if (m_bStartDe)
		DecreaseScaling(fTimeDelta);
}

void CUIFont::LateUpdate(_float fTimeDelta)
{
	if (m_bRenderAvtive)
		m_pGameInst->AddRenderObject(CRenderer::RG_UI, this);
}

HRESULT CUIFont::Render()
{
	if (FAILED(BindShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->BindBufferDesc();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUIFont::BindShaderResources()
{
	if (FAILED(m_pTransformCom->BindShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	_uint iIdx = GetNum();
	if (FAILED(m_pTextureComs[iIdx]->BindShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	_int iColor = _int(m_eFontColor);
	if (FAILED(m_pShaderCom->BindRawValue("g_iFontColorType", &iColor, sizeof _int)))
		return E_FAIL;

	switch (m_eFontColor)
	{
	case Client::CUIFont::FONT_DEFAULT:
		break;
	case Client::CUIFont::FONT_BLUE:
		if (FAILED(m_pTextureCom->BindShaderResource(m_pShaderCom, "g_TextureFontColor", 0)))
			return E_FAIL;
		break;
	case Client::CUIFont::FONT_GREEN:
		if (FAILED(m_pTextureCom->BindShaderResource(m_pShaderCom, "g_TextureFontColor", 0)))
			return E_FAIL;
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CUIFont::ReadyComponents(void* pArg)
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


	switch (m_eFontColor)
	{
	case Client::CUIFont::FONT_DEFAULT:

		break;
	case Client::CUIFont::FONT_BLUE:
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Font_Blue"), TEXT("Com_Texture_12"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
		break;
	case Client::CUIFont::FONT_GREEN:
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Font_Green"), TEXT("Com_Texture_13"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
		break;
	default:
		break;
	}

	return S_OK;
}

void CUIFont::StartIncreaseScaling()
{
	if (!m_bStartIn)
	{
		m_fAccSizeX = m_fMinSizeX;
		m_fAccSizeY = m_fMinSizeY;

		m_bStartIn = true;
		m_bStartDe = false;
	}
}

void CUIFont::StartDecreaseScaling()
{
	if (!m_bStartDe)
	{
		m_bStartIn = false;
		m_bStartDe = true;
	}
}

void CUIFont::StopScaling()
{

	m_bStartIn = false;
	m_bStartDe = false;

	m_pTransformCom->Scaling(m_fMinSizeX, m_fMinSizeY, 1.f);
}

void CUIFont::IncreaseScaling(_float fTimeDelta)
{
	m_fAccSizeX += m_fFontScalingSP * fTimeDelta;
	m_fAccSizeX = min(m_fMaxSizeX, m_fAccSizeX);

	m_fAccSizeY += m_fFontScalingSP * fTimeDelta;
	m_fAccSizeY = min(m_fMaxSizeY, m_fAccSizeY);

	m_pTransformCom->Scaling(m_fAccSizeX, m_fAccSizeY, 1.f);
}

void CUIFont::DecreaseScaling(_float fTimeDelta)
{
	m_fAccSizeX = max(m_fMinSizeX, m_fAccSizeX -= m_fFontScalingSP * fTimeDelta);
	m_fAccSizeY = max(m_fMinSizeY, m_fAccSizeY -= m_fFontScalingSP * fTimeDelta);
	m_pTransformCom->Scaling(m_fAccSizeX, m_fAccSizeY, 1.f);
}

CUIFont* CUIFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIFont* pInst = new CUIFont(pDevice, pContext);

	if (FAILED(pInst->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CUIFont");
		SafeRelease(pInst);
	}

	return pInst;
}

CUIFont* CUIFont::Clone(void* pArg)
{
	CUIFont* pInst = new CUIFont(*this);

	if (FAILED(pInst->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIFont");
		SafeRelease(pInst);
	}

	return pInst;
}

void CUIFont::Free()
{
	__super::Free();

	SafeRelease(m_pShaderCom);
	SafeRelease(m_pVIBufferCom);
	SafeRelease(m_pTextureCom);

	for (_int i = 0; i < 12; ++i)
		SafeRelease(m_pTextureComs[i]);
}
