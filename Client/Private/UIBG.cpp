#include "pch.h"
#include "UIBG.h"
#include "GameInstance.h"
#include "VIBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"
#include "Player.h"

CUIBG::CUIBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CUIPanel(pDevice, pContext)
{
}

CUIBG::CUIBG(const CUIBG& Prototype)
	:
	CUIPanel(Prototype)
{
}

HRESULT CUIBG::InitializePrototype()
{
	return S_OK;
}

HRESULT CUIBG::Initialize(void* pArg)
{
	if (!pArg)
		return E_FAIL;

	UI_BG_DESC* pDesc = static_cast<UI_BG_DESC*>(pArg);
	m_eBGType = pDesc->eBGType;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(ReadyComponents(nullptr)))
		return E_FAIL;

	return S_OK;
}

void CUIBG::PriorityUpdate(_float fTimeDelta)
{

}

void CUIBG::Update(_float fTimeDelta)
{
	if (m_eShowType == UI_SHOW_TYPE::UI_PERSPECTIVE)
	{
		UpdatePerspectiveMatrix();


		_vector OwnerPos = static_cast<CTransform*>(m_pUIOwner->GetComponent(TEXT("Com_Transform")))->GetTransformState(CTransform::STATE_POS);

		_int iLevel = m_pGameInst->GetCurrentLevelIndex();
		auto pObj = m_pGameInst->GetGameObjctInLayerBack(LEVEL_GAMEPLAY, iLevel, TEXT("A"));
		CPlayer* pPlayer = static_cast<CPlayer*>(pObj);
		CTransform* pCom = static_cast<CTransform*>(pPlayer->GetComponent(TEXT("Com_Transform")));
		_vector vPlayerPos = pCom->GetTransformState(CTransform::STATE_POS);
		_vector vPlayerLook = XMVector3Normalize(pCom->GetTransformState(CTransform::STATE_LOOK));
		// vPlayerLook = XMVectorSetY(vPlayerLook, 0.f);


		_vector vMonsterToPlayerDir = -XMVector3Normalize(vPlayerPos - OwnerPos);
		// vMontoPlayerDir.m128_f32[1] = 0.f;

		_float fDot = XMVectorGetX(XMVector3Dot(vPlayerLook, vMonsterToPlayerDir));

		if (fDot >= 0.f)
		{
			m_bDotRender = true;

			auto ChildContainer = GetChildUIContainer();
			for (auto iter = ChildContainer.begin(); iter != ChildContainer.end(); ++iter)
			{
				iter->second->SetDoRender();
			}
		}
		else
		{
			m_bDotRender = false;

			auto ChildContainer = GetChildUIContainer();
			for (auto iter = ChildContainer.begin(); iter != ChildContainer.end(); ++iter)
			{
				iter->second->SetDoNotRender();
			}
		}
	}
}

void CUIBG::LateUpdate(_float fTimeDelta)
{
	if (m_eShowType == UI_SHOW_TYPE::UI_PERSPECTIVE)
	{
		if (m_bDotRender)
			m_pGameInst->AddRenderObject(CRenderer::RG_UI, this);
	}
	else
	{
		if (IsActiveRender())
			m_pGameInst->AddRenderObject(CRenderer::RG_UI, this);
	}
}

HRESULT CUIBG::Render()
{
	if (FAILED(BindShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);
	m_pVIBufferCom->BindBufferDesc();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUIBG::BindShaderResources()
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

HRESULT CUIBG::ReadyComponents(void* pArg)
{
	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex_UI"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	switch (m_eBGType)
	{
	case Client::CUIBG::PLAYER_GUN_INFO:
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_BG_PlayerGunInfo"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
		break;
	case Client::CUIBG::PLAYER_SKILL:
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_BG_PlayerSkill"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
		break;
	case Client::CUIBG::PLAYER_SKILL_MID:
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_BG_PlayerSkillMid"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;
	case Client::CUIBG::MOSTER_HP:
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_BG_MonsterHP"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
		break;
	case CUIBG::MINI_MAP:
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_BG_MiniMap"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;
	case CUIBG::BOSS_HP:
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_BG_BossHP"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;
	case Client::CUIBG::BG_END:
		break;
	default:
		break;
	}


	return S_OK;
}

CUIBG* CUIBG::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIBG* pInst = new CUIBG(pDevice, pContext);

	if (FAILED(pInst->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CUIBG");
		SafeRelease(pInst);
	}

	return pInst;
}

CUIBG* CUIBG::Clone(void* pArg)
{
	CUIBG* pInst = new CUIBG(*this);

	if (FAILED(pInst->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIBG");
		SafeRelease(pInst);
	}

	return pInst;
}

void CUIBG::Free()
{
	__super::Free();

	SafeRelease(m_pShaderCom);
	SafeRelease(m_pVIBufferCom);
	SafeRelease(m_pTextureCom);
}
