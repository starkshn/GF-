#include "pch.h"
#include "UIHPBar.h"
#include "GameInstance.h"
#include "VIBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"
#include "GlobalFuncs.h"
#include "MonsterBase.h"

CUIHPBar::CUIHPBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CUIElement(pDevice, pContext)
{

}

CUIHPBar::CUIHPBar(const CUIHPBar& Prototype)
	:
	CUIElement(Prototype)
{

}

HRESULT CUIHPBar::InitializePrototype()
{
	return S_OK;
}

HRESULT CUIHPBar::Initialize(void* pArg)
{
	if (pArg)
	{
		UI_HP_BAR_DESC* pDesc = static_cast<UI_HP_BAR_DESC*>(pArg);
		m_eHPType = pDesc->eHPType;
		m_eHPBarColor = pDesc->eHPBarColor;
		m_fMaxHPBarVal = pDesc->fMaxHPBarVal;

		m_fCurHP = m_fMaxHPBarVal;
		m_fPrevHP = m_fMaxHPBarVal;
		m_fHPVal = m_fMaxHPBarVal;
	}
	else
		return E_FAIL;

	if (m_fMaxHPBarVal == -1)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(ReadyComponents(nullptr)))
		return E_FAIL;

	return S_OK;
}

void CUIHPBar::PriorityUpdate(_float fTimeDelta)
{
}

void CUIHPBar::Update(_float fTimeDelta)
{
	if (m_bUpdateHP)
	{
		m_fCurUpdateRatio -= (fTimeDelta * 30.f);
		if (m_fCurUpdateRatio <= m_fCurHP)
		{
			m_fCurUpdateRatio = m_fCurHP;
			m_bUpdateHP = false;
		}

		if (m_bOnce)
		{
			m_fExpansionTime += fTimeDelta;
			if (m_fExpansionTime > 0.1f)
			{
				m_bOnce = 0;
			}
		}
		else
		{
			m_fExpansionTime -= fTimeDelta;
			if (m_fExpansionTime < 0.f)
			{
				m_bOnce = 1;
				m_fExpansionTime = 0.f;
			}
		}

	}


	if (m_eShowType == UI_SHOW_TYPE::UI_PERSPECTIVE)
	{
		if (auto pOwner = GetPerspectiveUIOwner())
		{
			_float2 vOwnerSP = pOwner->GetPerspectiveScreenPos();
			_vector vWorldPos = CalWorldPosFromOwnerScreenPos(vOwnerSP);
			m_pTransformCom->SetTransformState(CTransform::STATE_POS, vWorldPos);
			m_pTransformCom->Scaling(GetSizeX(), GetSizeY(), 0.f);

			if (m_pVertical)
			{
				// pUIVertical->ActiveRender();
				auto pCom = static_cast<CTransform*>(m_pVertical->GetComponent(TEXT("Com_Transform")));
				
				_float fSizeX = GetSizeX() / 2;
				vWorldPos.m128_f32[0] -= fSizeX;
				
				_int iMax = static_cast<CMonsterBase*>(pOwner->GetUIOwner())->GetMaxHP();
				_int iCurHP = static_cast<CMonsterBase*>(pOwner->GetUIOwner())->GetCurHP();
				_int iHMax = iMax / 2;
				_int iHHMax = iHMax / 2;

				if (iCurHP > iHMax)
				{
					iCurHP -= iHMax;
					vWorldPos.m128_f32[1] += 18;
				}
				
				_float fRatio = (_float)iCurHP / (_float)iHMax;
				vWorldPos.m128_f32[0] += (GetSizeX() * fRatio);

				pCom->SetTransformState(CTransform::STATE_POS, vWorldPos);
				pCom->Scaling(10.f, 50.f, 5.f);


				if (!m_pVertical->IsActiveRender())
					m_pVertical = nullptr;

				// pCom->GetTransformState(CTransform::STATE_POS);
			}
		}
	}
}

void CUIHPBar::LateUpdate(_float fTimeDelta)
{
	if (m_eShowType == UI_SHOW_TYPE::UI_PERSPECTIVE)
	{
		if (!CanRender())
			return;
	}

	if (IsActiveRender())
	{
		m_pGameInst->AddRenderObject(CRenderer::RG_UI, this);

	}

	if (m_bHPOnce == 0)
	{
		m_fHPAcc += fTimeDelta;
		if (m_fHPAcc > 0.3f)
		{
			m_fHPAcc = 0.f;
			m_bHPOnce = 1;
		}
	}
}

HRESULT CUIHPBar::Render()
{
	if (FAILED(BindShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(4);
	m_pVIBufferCom->BindBufferDesc();
	m_pVIBufferCom->Render();

	return S_OK;
}

void CUIHPBar::SetHPVal(_int iVal)
{
	m_fPrevHP = m_fCurHP;				// 현재값을 이전에 저장
	m_fCurUpdateRatio = m_fPrevHP;		// m_iPrevHP -> m_iHPVal까지 줄어들 예정
	m_fHPVal = iVal;					// 현재 값을 저장
	m_fCurHP = m_fHPVal;
	m_bUpdateHP = true;


	if (m_bHPOnce)
	{
		m_bHPOnce = 0;
		m_pVertical = static_cast<CUIElement*>(m_pGameInst->GetVerticalUI());
		if (m_pVertical)
		{
			m_pVertical->ActiveRender();
		}
	}



	//if (m_bHPOnce2)
	//{
	//	m_bHPOnce2 = 0;
	//	return;
	//}


	//if (m_bHPOnce2)
	//{
	//	m_bHPOnce2 = 0;

	//	
	//}
}

void CUIHPBar::SetHPValPlayer(_int iVal)
{
	m_fPrevHP = m_fCurHP;				// 현재값을 이전에 저장
	m_fCurUpdateRatio = m_fPrevHP;		// m_iPrevHP -> m_iHPVal까지 줄어들 예정
	m_fHPVal = iVal;					// 현재 값을 저장
	m_fCurHP = m_fHPVal;
	m_bUpdateHP = true;


}

HRESULT CUIHPBar::BindShaderResources()
{
	if (FAILED(m_pTransformCom->BindShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->BindShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pHPBarColor->BindShaderResource(m_pShaderCom, "g_TextureHPBarColor", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->BindRawValue("g_fHPMax", &m_fMaxHPBarVal, sizeof _float)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->BindRawValue("g_fPrevHP", &m_fPrevHP, sizeof _float)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->BindRawValue("g_fCurUpdateHP", &m_fCurUpdateRatio, sizeof _float)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->BindRawValue("g_fCurHP", &m_fCurHP, sizeof _float)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->BindRawValue("g_fExpansionTime", &m_fExpansionTime, sizeof _float)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIHPBar::ReadyComponents(void* pArg)
{
	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex_UI"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	switch (m_eHPType)
	{
	case Client::CUIHPBar::HP_PLAYER:
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_PlayerHPBarBG"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
		break;
	case Client::CUIHPBar::HP_MONSTER:
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_MonsterHPBG"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
		
		break;
	case Client::CUIHPBar::HP_TPYE_END:
		break;
	default:
		break;
	}

	switch (m_eHPBarColor)
	{
	case Client::CUIHPBar::HP_BLUE:
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_HP_Blue"), TEXT("Com_Texture_Blue"), reinterpret_cast<CComponent**>(&m_pHPBarColor))))
			return E_FAIL;
		break;
	case Client::CUIHPBar::HP_GREEN:
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_HP_Green"), TEXT("Com_Texture_Green"), reinterpret_cast<CComponent**>(&m_pHPBarColor))))
			return E_FAIL;
		break;
	case Client::CUIHPBar::HP_RED:
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_HP_Red"), TEXT("Com_Texture_Red"), reinterpret_cast<CComponent**>(&m_pHPBarColor))))
			return E_FAIL;
		break;
	case Client::CUIHPBar::HP_COLOR_END:
		break;
	default:
		break;
	}

	return S_OK;
}

CUIHPBar* CUIHPBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIHPBar* pInst = new CUIHPBar(pDevice, pContext);

	if (FAILED(pInst->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CUIHPBar");
		SafeRelease(pInst);
	}

	return pInst;
}

CUIHPBar* CUIHPBar::Clone(void* pArg)
{
	CUIHPBar* pInst = new CUIHPBar(*this);

	if (FAILED(pInst->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIHPBar");
		SafeRelease(pInst);
	}

	return pInst;
}

void CUIHPBar::Free()
{
	__super::Free();

	SafeRelease(m_pShaderCom);
	SafeRelease(m_pVIBufferCom);
	SafeRelease(m_pTextureCom);
	SafeRelease(m_pHPBarColor);
	
}
