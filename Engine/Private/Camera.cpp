#include "Camera.h"
#include "GameInstance.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: 
	CGameObject(pDevice, pContext)
{
}

CCamera::CCamera(const CCamera& Prototype)
	:
	CGameObject(Prototype)
{
}

HRESULT CCamera::InitializePrototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(pArg);

	// 뷰 스페이스 변환 행렬에 대한 Transform 동기화 작업 진행
	if (nullptr != m_pTransformCom)
	{
		// Set_State는 분명 XMVECTOR(_vector)를 받아주고 있다.
		// 근데 여기서 w 성분이 0인채로 넘기면 안된다. -> 위치이기 때문에 W 성분을 1로 채우고 넘기기 위하여 XMVectorSetW사용.
		m_pTransformCom->SetTransformState(CTransform::STATE_POS, XMVectorSetW(XMLoadFloat3(&pDesc->vEye), 1.f));
		
		// 위치(vEye)를 먼저 잡아주고 LookAt을 호출해야한다(회전)
		m_pTransformCom->LookAt(XMVectorSetW(XMLoadFloat3(&pDesc->vAt), 1.f));
	}

	m_fFovy = pDesc->fFovY;
	m_fAspect = pDesc->fAspectRatio;
	m_fNear = pDesc->fNear;
	m_fFar = pDesc->fFar;

	return S_OK;
}

void CCamera::PriorityUpdate(_float fTimeDelta)
{
}

void CCamera::Update(_float fTimeDelta)
{
}

void CCamera::LateUpdate(_float fTimeDelta)
{
}

HRESULT CCamera::Render()
{
	return S_OK;
}

void CCamera::Compute_PipeLine_Matrices()
{
	// 뷰 스페이스 변환 행렬을 넘겨준다.
	// 현재 카메라의 월드 변환 행렬의 '역행렬'이 뷰스페이스 변환 행렬이다.
	m_pGameInst->SetTransformMatrix(CPipeLine::DTS::D3DTS_VIEW, m_pTransformCom->GetWorldMatrixInv());
	m_pGameInst->SetTransformMatrix(CPipeLine::DTS::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));
}

void CCamera::Free()
{
	__super::Free();
}
