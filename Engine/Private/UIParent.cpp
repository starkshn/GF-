#include "UIParent.h"
#include "GameInstance.h"

CUIParent::CUIParent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CUIObject(pDevice, pContext)
{
}

CUIParent::CUIParent(const CUIParent& Prototype)
	:
	CUIObject(Prototype)
{

}

HRESULT CUIParent::InitializePrototype()
{
	return S_OK;
}

HRESULT CUIParent::Initialize(void* pArg)
{
	if (!pArg) return E_FAIL;
	
	UI_DEFAULT_INFO* pDesc = static_cast<UI_DEFAULT_INFO*>(pArg);
	m_eUIType = pDesc->eUIType;
	m_eShowType = pDesc->eShowType;
	SetDepth(pDesc->iDepth);

	m_fSX = pDesc->fSX;
	m_fSY = pDesc->fSY;
	m_fSSizeX = pDesc->fSSizeX;
	m_fSSizeY = pDesc->fSSizeY;

	if (pDesc->pUIOwner)
		m_pUIOwner = pDesc->pUIOwner;

	if (m_eShowType == UI_SHOW_TYPE::UI_PERSPECTIVE)
		if (pDesc->pPerspectiveUIOwner)
			m_pPerpectiveUIOwner = pDesc->pPerspectiveUIOwner;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (FAILED(Init(m_eShowType, m_eUIType, *pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIParent::Init(UI_SHOW_TYPE eShowType, UI_TYPE eType, UI_DEFAULT_INFO Desc)
{
	if (UI_SHOW_TYPE::UI_END == eShowType)
		return E_FAIL;

	switch (eShowType)
	{
	case Engine::UI_SHOW_TYPE::UI_ORTHO:
		if (FAILED(InitializeOrthoMatrix(Desc)))
			return E_FAIL;
		
		break;
	case Engine::UI_SHOW_TYPE::UI_PERSPECTIVE:
		if (FAILED(InitializePerpectiveMatrix(Desc)))
			return E_FAIL;
		
		break;
	case Engine::UI_SHOW_TYPE::UI_END:
		break;
	default:
		break;
	}
	return S_OK;
}

// 직교 투영
HRESULT CUIParent::InitializeOrthoMatrix(UI_DEFAULT_INFO Desc)
{
	_uint iNumViewPorts = 1;
	D3D11_VIEWPORT ViewportDesc = {};
	m_pContext->RSGetViewports(&iNumViewPorts, &ViewportDesc);

	// 직교 투영을 위한 뷰 행렬을 만들어준다.
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	// 스크린 상의 사이즈 설정
	m_fSSizeX = Desc.fSSizeX;
	m_fSSizeY = Desc.fSSizeY;

	// 스크린 상의 위치(중점) 설정
	m_fSX = Desc.fSX;
	m_fSY = Desc.fSY;

	if (!m_pTransformCom)
		return E_FAIL;

	m_pTransformCom->Scaling(m_fSSizeX, m_fSSizeY, 1.f);
	m_pTransformCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(m_fSX - ViewportDesc.Width * 0.5f, -m_fSY + ViewportDesc.Height * 0.5f, 0.f, 1.f));

	return S_OK;
}

// 원근 
HRESULT CUIParent::InitializePerpectiveMatrix(UI_DEFAULT_INFO Desc)
{
	_uint iNumViewPorts = 1;
	D3D11_VIEWPORT ViewportDesc = {};
	m_pContext->RSGetViewports(&iNumViewPorts, &ViewportDesc);

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	return S_OK;
}

void CUIParent::UpdatePerspectiveMatrix()
{
	if (m_eShowType != UI_SHOW_TYPE::UI_PERSPECTIVE)
		return;

	if (!m_pUIOwner)
	{
		MSG_BOX("UI Owner is nullptr");
		return;
	}
	
	CTransform* pCom = static_cast<CTransform*>(m_pUIOwner->GetComponent(TEXT("Com_Transform")));
	if (!pCom)
		return;

	_vector vUP = XMVector3Normalize(pCom->GetTransformState(CTransform::STATE_UP));
	

	_uint iNumViewPorts = 1;
	D3D11_VIEWPORT ViewportDesc = {};
	m_pContext->RSGetViewports(&iNumViewPorts, &ViewportDesc);

	// Owner의 World * View * Proj : 월드 * 뷰 * 투영을 직접 계산하여 NDC를 얻어 스크린 좌표를 계산한다.
	_vector vOwnerWorldPos = pCom->GetTransformState(CTransform::STATE_POS) + vUP * 2.f;
	_matrix View = m_pGameInst->GetTransformMatrix(CPipeLine::D3DTS_VIEW);
	_matrix Proj = m_pGameInst->GetTransformMatrix(CPipeLine::D3DTS_PROJ);
	
	_vector vViewPos = XMVector4Transform(vOwnerWorldPos, View);
	_vector vClipSpace = XMVector4Transform(vViewPos, Proj);

	// 뷰 스페이스 상의 Z값
	_float fW = XMVectorGetW(vClipSpace);
	
	// Z나누기 수행
	vClipSpace = XMVectorSetX(vClipSpace, XMVectorGetX(vClipSpace) / fW);
	vClipSpace = XMVectorSetY(vClipSpace, XMVectorGetY(vClipSpace) / fW);

	_vector vNDC = vClipSpace;

	_vector vScreenPos = { (XMVectorGetX(vNDC) + 1.f) * ViewportDesc.Width * 0.5f, -(XMVectorGetY(vNDC) - 1.f) * ViewportDesc.Height * 0.5f };

	m_vPerpectiveScreenPos.x = XMVectorGetX(vScreenPos);
	m_vPerpectiveScreenPos.y = XMVectorGetY(vScreenPos);
	
	_vector vNewWorldPos = {XMVectorGetX(vScreenPos) - ViewportDesc.Width * 0.5f, -XMVectorGetY(vScreenPos) + ViewportDesc.Height * 0.5f, 0.0f, 1.f};

	m_pTransformCom->SetTransformState(CTransform::STATE_POS, vNewWorldPos);
	m_pTransformCom->Scaling(GetSizeX(), GetSizeY(), 1.f);
}

_vector CUIParent::CalWorldPosFromOwnerScreenPos(_float2 vOwnerUIScreenPos)
{
	_uint iNumViewPorts = 1;
	D3D11_VIEWPORT ViewportDesc = {};
	m_pContext->RSGetViewports(&iNumViewPorts, &ViewportDesc);
	vOwnerUIScreenPos.x += GetSX();
	vOwnerUIScreenPos.y += GetSY();
	return _vector{ vOwnerUIScreenPos.x - ViewportDesc.Width * 0.5f, -vOwnerUIScreenPos.y + ViewportDesc.Height * 0.5f, 0.f, 1.f};
}

_bool CUIParent::GetPerpectiveUIOwnerIsInFrustum()
{
	if (m_eShowType == UI_SHOW_TYPE::UI_PERSPECTIVE)
		if (m_pUIOwner)
			return m_pUIOwner->IsInFrustum();
}

_bool CUIParent::CheckPerspectiveCondition()
{
	if (auto pOwner = GetPerspectiveUIOwner())
	{
		if (pOwner->GetPerpectiveUIOwnerIsInFrustum())
		{
			return true;
		}
	}
	return false;
}

_bool CUIParent::IsInCamera(_uint iTargetLevel)
{
	_uint iLevel = m_pGameInst->GetCurrentLevelIndex();
	auto pPlayer = m_pGameInst->GetGameObjctInLayerBack(iTargetLevel, iLevel, TEXT("A"));
	if (!pPlayer) 
		return false;

	CTransform* pCom = static_cast<CTransform*>(pPlayer->GetComponent(TEXT("Com_Transform")));
	if (!pCom) 
		return false;

	_vector vPlayerPos = pCom->GetTransformState(CTransform::STATE_POS);
	_vector vPlayerLook = XMVector3Normalize(pCom->GetTransformState(CTransform::STATE_LOOK));
	
	_vector vUIWorldPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);

	_vector vMontoPlayerDir = -XMVector3Normalize(vPlayerPos - vUIWorldPos);
	_float fDot = XMVectorGetX(XMVector3Dot(vPlayerLook, vMontoPlayerDir));

	if (fDot >= 0.f) 
		return true;
	else 
		return false;
}

_vector CUIParent::CalWorldPosFromScreenPos(_float fSX, _float fSY)
{
	auto ViewDesc = m_pGameInst->GetViewPortDesc();
	return _vector{ fSX - ViewDesc.Width * 0.5f, -fSY + ViewDesc.Height * 0.5f, 0.f, 1.f };
}

_vector CUIParent::CalScreenPosFromWorldPos(_fvector vWorldPos)
{	
	_uint iNumViewPorts = 1;
	D3D11_VIEWPORT ViewportDesc = {};
	m_pContext->RSGetViewports(&iNumViewPorts, &ViewportDesc);

	// Owner의 World * View * Proj : 월드 * 뷰 * 투영을 직접 계산하여 NDC를 얻어 스크린 좌표를 계산한다.
	_matrix View = m_pGameInst->GetTransformMatrix(CPipeLine::D3DTS_VIEW);
	_matrix Proj = m_pGameInst->GetTransformMatrix(CPipeLine::D3DTS_PROJ);

	_vector vViewPos = XMVector4Transform(vWorldPos, View);
	_vector vClipSpace = XMVector4Transform(vViewPos, Proj);

	// 뷰 스페이스 상의 Z값
	_float fW = XMVectorGetW(vClipSpace);

	// Z나누기 수행
	vClipSpace = XMVectorSetX(vClipSpace, XMVectorGetX(vClipSpace) / fW);
	vClipSpace = XMVectorSetY(vClipSpace, XMVectorGetY(vClipSpace) / fW);

	_vector vNDC = vClipSpace;

	return _vector{ (XMVectorGetX(vNDC) + 1.f) * ViewportDesc.Width * 0.5f, -(XMVectorGetY(vNDC) - 1.f) * ViewportDesc.Height * 0.5f };
}

void CUIParent::Free()
{
	__super::Free();
}
