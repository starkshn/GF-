#include "pch.h"
#include "UITex.h"
#include "GameInstance.h"
#include "VIBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"

CUITex::CUITex(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CUIElement(pDevice, pContext)
{

}

CUITex::CUITex(const CUITex& Prototype)
	:
	CUIElement(Prototype)
{

}

HRESULT CUITex::InitializePrototype()
{
	return S_OK;
}

HRESULT CUITex::Initialize(void* pArg)
{
	if (!pArg)
		return E_FAIL;

	UI_TEX_DESC* pDesc = static_cast<UI_TEX_DESC*>(pArg);
	m_eUITexType = pDesc->eUITexType;

	if (pDesc->eUITexEffectType == UI_TEX_EFFECT_ALPHA)
		m_eUITexEffectType = UI_TEX_EFFECT_ALPHA;

	if (pDesc->iAnimCol != 0 && pDesc->iAnimRow != 0)
	{
		m_bUseAnimUI = pDesc->bUseAnim;
		m_iAnimRow = pDesc->iAnimRow;
		m_iAnimCol = pDesc->iAnimCol;
		m_fAnimMaxTime = pDesc->fAnimMaxTime;
	}

	m_pTextureGunsCom.resize(4);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(ReadyComponents(nullptr)))
		return E_FAIL;

	return S_OK;
}

void CUITex::PriorityUpdate(_float fTimeDelta)
{

}

void CUITex::Update(_float fTimeDelta)
{
	if (m_eShowType == UI_SHOW_TYPE::UI_PERSPECTIVE)
	{
		if (auto pOwner = GetPerspectiveUIOwner())
		{
			_float2 vOwnerSP = pOwner->GetPerspectiveScreenPos();
			_vector vWorldPos = CalWorldPosFromOwnerScreenPos(vOwnerSP);
			m_pTransformCom->SetTransformState(CTransform::STATE_POS, vWorldPos);
			m_pTransformCom->Scaling(GetSizeX(), GetSizeY(), 0.f);
		}
	}

	if (m_bStartAlphaEffect)
	{
		m_fAccAlphaTime += fTimeDelta;
		if (m_fAccAlphaTime > m_fMaxAlphaTime)
		{
			m_fAccAlphaTime = 0.f;
			m_bStartAlphaEffect = false;
			DeActiveRender();
		}
	}

	if (m_bUseAnimUI)
	{
		m_fAccAnimTime += fTimeDelta;
		if (m_fAccAnimTime > m_fAnimMaxTime)
		{
			m_fAccAnimTime = 0.f;
		}
	}

	if (m_bStartScaling)
	{
		// °Å = ¼Ó * ½Ã
		m_fScalingSP = m_fScalingMaxSizeX / 1.5f;
		static _float fAcc = 0.f;
		fAcc += fTimeDelta * m_fScalingSP;
		if (fAcc > m_fScalingMaxSizeX)
		{
			fAcc = m_fScalingMaxSizeX + 1.f;

			if (m_bHold)
			{
				static _float fHoldAcc = 0.f;
				fHoldAcc += fTimeDelta;

				if (fHoldAcc > m_fHoldTime)
				{
					fAcc = 0.f;
					fHoldAcc = 0.f;
					m_bStartScaling = false;
					m_pTransformCom->Scaling(fAcc, fAcc, 1.f);
				}
			}
			else
			{
				fAcc = 0.f;
				m_bStartScaling = false;
				m_pTransformCom->Scaling(fAcc, fAcc, 1.f);
			}
		}
		else
		{
			m_pTransformCom->Scaling(fAcc, fAcc, 1.f);
			_vector vRight = m_pTransformCom->GetTransformState(CTransform::STATE_RIGHT);
			_vector vUP = m_pTransformCom->GetTransformState(CTransform::STATE_UP);
		}
	}

}

void CUITex::LateUpdate(_float fTimeDelta)
{
	if (!IsActiveRender())
		return;

	if (m_eShowType == UI_SHOW_TYPE::UI_PERSPECTIVE)
	{
		if (!CanRender())
			return;
	}

	if (m_bRenderAvtive && !m_bUseAnimUI)
		m_pGameInst->AddRenderObject(CRenderer::RG_UI, this);
	else if (m_bUseAnimUI && m_bStartAnim)
		m_pGameInst->AddRenderObject(CRenderer::RG_UI, this);
}

HRESULT CUITex::Render()
{
	if (FAILED(BindShaderResources()))
		return E_FAIL;

	if (m_bUseAnimUI && m_bStartAnim)
	{
		m_pShaderCom->Begin(8);

		m_pVIBufferCom->BindBufferDesc();
		m_pVIBufferCom->Render();
		
		return S_OK;
	}

	if (!m_bUseTexCoordFlex)
	{
		if (m_eUITexEffectType == UI_TEX_EFFECT_DEFAULT)
			m_pShaderCom->Begin(5);
		else if (m_eUITexEffectType == UI_TEX_EFFECT_ALPHA)
			m_pShaderCom->Begin(7);
	}
	else
	{
		m_pShaderCom->Begin(6);
	}

	m_pVIBufferCom->BindBufferDesc();
	m_pVIBufferCom->Render();

	return S_OK;
}

void CUITex::SetScalingDesc(UI_TEX_SCALE_DESC Desc)
{
	m_bUseScaling = true;
	m_fScalingMaxSizeX = Desc.fScalingMaxSizeX;
	m_fScalingMinSizeY = Desc.fScalingMinSizeY;

	m_fScalingMinSizeX = Desc.fScalingMinSizeX;
	m_fScalingMinSizeY = Desc.fScalingMinSizeY;

	m_bHold = Desc.bHold;
	m_fHoldTime = Desc.fHoldTime;
}

void CUITex::StartScaling()
{
	m_bStartScaling = true;
	ActiveRender();
	if (!m_bStartScaling)
		m_pTransformCom->Scaling(m_fScalingMinSizeX, m_fScalingMinSizeY, 0.f);
}

HRESULT CUITex::BindShaderResources()
{
	if (FAILED(m_pTransformCom->BindShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (UI_TEX_TYPE::PLAYER_GUN_TEX == m_eUITexType)
	{
		if (FAILED(m_pTextureGunsCom[m_iGunTexIdx]->BindShaderResource(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTextureCom->BindShaderResource(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;
	}

	if (m_bUseTexCoordFlex)
	{
		if (FAILED(m_pShaderCom->BindRawValue("g_bUseTexCoordFlex", &m_bUseTexCoordFlex, sizeof _bool)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->BindRawValue("g_fTexCoordX", &m_fTexCoordX, sizeof _float)))
			return E_FAIL;
	}

	if (m_bStartAlphaEffect && m_eUITexEffectType == UI_TEX_EFFECT_ALPHA)
	{
		if (FAILED(m_pShaderCom->BindRawValue("g_MaxAlphaTime", &m_fMaxAlphaTime, sizeof _float)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->BindRawValue("g_CurAlphaTime", &m_fAccAlphaTime, sizeof _float)))
			return E_FAIL;
	}

	if (m_bUseAnimUI && m_bStartAnim)
	{
		if (FAILED(m_pShaderCom->BindRawValue("g_iAnimRow", &m_iAnimRow, sizeof _int)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->BindRawValue("g_iAnimCol", &m_iAnimCol, sizeof _int)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->BindRawValue("g_fMaxAnimTime", &m_fAnimMaxTime, sizeof _float)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->BindRawValue("g_fCurAnimtime", &m_fAccAnimTime, sizeof _float)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CUITex::ReadyComponents(void* pArg)
{
	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex_UI"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;


	switch (m_eUITexType)
	{
	case Client::CUITex::PLAYER_GUN_KEY_NUMBER_1:
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_KeyNumber1"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
		break;
	case Client::CUITex::PLAYER_GUN_KEY_NUMBER_2:
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_KeyNumber2"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
		break;
	case Client::CUITex::PLAYER_GUN_KEY_NUMBER_3:
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_KeyNumber3"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
		break;
	case Client::CUITex::PLAYER_GUN_TEX:
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_1202"), TEXT("Com_Texture_1202"), reinterpret_cast<CComponent**>(&m_pTextureGunsCom[0]))))
			return E_FAIL;

		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_1004"), TEXT("Com_Texture_1004"), reinterpret_cast<CComponent**>(&m_pTextureGunsCom[1]))))
			return E_FAIL;

		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_1506"), TEXT("Com_Texture_1506"), reinterpret_cast<CComponent**>(&m_pTextureGunsCom[2]))))
			return E_FAIL;

		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_1508"), TEXT("Com_Texture_1508"), reinterpret_cast<CComponent**>(&m_pTextureGunsCom[3]))))
			return E_FAIL;
	}
		break;
	case Client::CUITex::PLAYER_GUN_BULLET:
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_1506"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;
	case Client::CUITex::PLAYER_SKILL_DASH_BG:
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_PlayerSkillDashBG"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
		break;
	case Client::CUITex::PLAYER_SKILL_DASH:
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_PlayerSkillDash"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
		break;
	case Client::CUITex::TEX_KEY_SHIFT:
	{

	}
		break;
	case Client::CUITex::TEX_KEY_E:
	{

	}
		break;
	case Client::CUITex::MONSTER_HP_BG:
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_MonsterHPBG"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
		break;
	case CUITex::MINI_MAP_PLAYER_POS:
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_MiniMapPlayerPos"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;
	case CUITex::MINI_MAP_MONSTER_POS:
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_MiniMapMonsterPos"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;
	case CUITex::MINI_MAP_BOSS:
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_MiniMapBossPos"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;
	case CUITex::MINI_DOOR:
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_Door"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;
	
	case Client::CUITex::BOSS_ICON:
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_BossIcon"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
		break;
	case Client::CUITex::BOSS_HP_BG:
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_BossHPBG"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;
	case Client::CUITex::PLAYER_SKILL_DASH_EFFECT:
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_PlayerDashEffect"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;
	case Client::CUITex::PLAYER_DAMAGED_EFFECT:
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_PlayerDamageEffect"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;
	case Client::CUITex::PLAYER_SKILL_MID:
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_PlayerSkillMid"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;
	case Client::CUITex::PLAYER_SKILL_MID_ANIM:
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_PlayerSkillMidAnim"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;

	case Client::CUITex::PLAYER_KEY_SHIFT:
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_PlayerKeyShift"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;

	case Client::CUITex::PLAYER_KEY_E:
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_PlayerKeyE"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;

	case Client::CUITex::PLAYER_KEY_Q:
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_PlayerKeyQ"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;

	case Client::CUITex::PLAYER_KEY_F:
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_PlayerKeyF"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;
	case Client::CUITex::VICTORY:
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_Victory"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;
	case Client::CUITex::TEX_END:
		break;

	default:
		break;
	}

	return S_OK;
}

CUITex* CUITex::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUITex* pInst = new CUITex(pDevice, pContext);

	if (FAILED(pInst->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CUITex");
		SafeRelease(pInst);
	}

	return pInst;
}

CUITex* CUITex::Clone(void* pArg)
{
	CUITex* pInst = new CUITex(*this);

	if (FAILED(pInst->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUITex");
		SafeRelease(pInst);
	}

	return pInst;
}

void CUITex::Free()
{
	__super::Free();

	SafeRelease(m_pShaderCom);
	SafeRelease(m_pVIBufferCom);
	SafeRelease(m_pTextureCom);

	for_each(m_pTextureGunsCom.begin(), m_pTextureGunsCom.end(), [](CTexture* pTex) {SafeRelease(pTex); });
	m_pTextureGunsCom.clear();
}
