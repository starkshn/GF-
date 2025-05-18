#include "pch.h"
#include "UITexPerspective.h"
#include "GameInstance.h"
#include "VIBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"
#include "Level_GamePlay.h"

CUITexPerspective::CUITexPerspective(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CUIElement(pDevice, pContext)
{
}

CUITexPerspective::CUITexPerspective(const CUITexPerspective& Prototype)
	:
	CUIElement(Prototype)
{
}

HRESULT CUITexPerspective::InitializePrototype()
{
	return S_OK;
}

HRESULT CUITexPerspective::Initialize(void* pArg)
{
	if (!pArg)
		return E_FAIL;

	UI_TEX_P_DESC* pDesc = static_cast<UI_TEX_P_DESC*>(pArg);
	if (pDesc)
	{
		m_eUITexPType = pDesc->eUITexPType;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(ReadyComponents(nullptr)))
		return E_FAIL;
	
	return S_OK;
}

void CUITexPerspective::PriorityUpdate(_float fTimeDelta)
{
}

void CUITexPerspective::Update(_float fTimeDelta)
{
	if (m_eShowType != UI_SHOW_TYPE::UI_PERSPECTIVE)
		return;

	if (IsActiveRender())
	{
		if (GetTargetObj())
		{
			auto pCurLevel = m_pGameInst->GetCurrentLevel();
			if (!pCurLevel)
				return;

			if (static_cast<CLevel_GamePlay*>(pCurLevel)->IsGoToBossLevel())
			{
				DeActiveRender();
				return;
			}

			if (!GetTargetObj()->IsObjInCam(LEVEL_GLOBAL))
				return;

			CTransform* pCom = static_cast<CTransform*>(GetTargetObj()->GetComponent(TEXT("Com_Transform")));
			if (pCom)
			{
				_vector vWorldPos = pCom->GetTransformState(CTransform::STATE_POS);

				// Cal World -> View -> World
				_float4 vWorldPosTmp = { };
				XMStoreFloat4(&vWorldPosTmp, CalScreenPosFromWorldPos(vWorldPos));
				_vector vNewWorldPos = CalWorldPosFromScreenPos(vWorldPosTmp.x, vWorldPosTmp.y);
				m_pTransformCom->SetTransformState(CTransform::STATE_POS, vNewWorldPos);
				m_pTransformCom->Scaling(m_fSSizeX, m_fSSizeY, 1.f);
			}
		}
	}
}

void CUITexPerspective::LateUpdate(_float fTimeDelta)
{
	if (IsActiveRender())
		m_pGameInst->AddRenderObject(CRenderer::RG_UI, this);
}

HRESULT CUITexPerspective::Render()
{
	if (FAILED(BindShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->BindBufferDesc();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUITexPerspective::BindShaderResources()
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

HRESULT CUITexPerspective::ReadyComponents(void* pArg)
{
	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex_UI"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	switch (m_eUITexPType)
	{
	case Client::CUITexPerspective::UI_TEX_P_DOOR:
	{
		if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_UI_Tex_Door"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
		break;
	case Client::CUITexPerspective::UI_TEX_P_END:
		break;
	default:
		break;
	}

	return S_OK;
}

CUITexPerspective* CUITexPerspective::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUITexPerspective* pInst = new CUITexPerspective(pDevice, pContext);

	if (FAILED(pInst->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CUITexPerspective");
		SafeRelease(pInst);
	}

	return pInst;
}

CUITexPerspective* CUITexPerspective::Clone(void* pArg)
{
	CUITexPerspective* pInst = new CUITexPerspective(*this);

	if (FAILED(pInst->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUITexPerspective");
		SafeRelease(pInst);
	}

	return pInst;
}

void CUITexPerspective::Free()
{
	__super::Free();

	SafeRelease(m_pShaderCom);
	SafeRelease(m_pVIBufferCom);
	SafeRelease(m_pTextureCom);
}
