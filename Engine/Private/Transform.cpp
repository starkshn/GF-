#include "Transform.h"
#include "Shader.h"
#include "Navigation.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CComponent(pDevice, pContext)
{
}

CTransform::CTransform(const CTransform& Prototype)
	:
	CComponent(Prototype)
{
}

// 스케일을 계산하는 함수이다.
// XMVector3Length호출시 전달 받은 XMVector를 통해서 길이를 구한다. 만약 길이(크기)가 2인경우 반환되는 벡터(XMVECTOR)가 모두 2, 2, 2, 2로 채워져 있기 때문에(무슨 의도를 가지고 있지만 불편하다) 이중 특정 원소 하나를 가져 온다.
_float3 CTransform::ComputeScaled()
{
	return _float3(XMVectorGetX(XMVector3Length(GetTransformState(STATE_RIGHT))), XMVectorGetX(XMVector3Length(GetTransformState(STATE_UP))), XMVectorGetX(XMVector3Length(GetTransformState(STATE_LOOK))));
}

// 원형 초기화 함수
HRESULT CTransform::InitializePrototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

// 복사본 초기화 함수
HRESULT CTransform::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(pArg);
	
	m_fSPPerSec = pDesc->fSpeedPerSec;
	m_fRotSPPerSec = pDesc->fRotSpeedPerSec;

	return S_OK;
}

HRESULT CTransform::BindShaderResource(CShader* pShader, const _char* pConstantName)
{
	return pShader->BindMatrix(pConstantName, &m_WorldMatrix);
}

// _11, _22, _33이 물체의 크기를 나타내지 않는다. (회전이 없다면 나타내는 것이 맞지만 게임에서는 회전이 무조건 들어가기 때문에 배제한다)
// 해당함수는 오브젝트의 크키를 조절하는 함수이다. 이를 위해서 right, up, look을 정규화 시킨다음에 fX, fY, fZ만큼 곱해서 크기를 늘려준다.
void CTransform::Scaling(_float fX, _float fY, _float fZ)
{
	SetTransformState(STATE_RIGHT, XMVector3Normalize(GetTransformState(STATE_RIGHT)) * fX);
	SetTransformState(STATE_UP, XMVector3Normalize(GetTransformState(STATE_UP)) * fY);
	SetTransformState(STATE_LOOK, XMVector3Normalize(GetTransformState(STATE_LOOK)) * fZ);		
}

// 3D오브젝트가 앞으로 나아가는 코드
void CTransform::GoFoward(_float fTimeDelta, CNavigation* pNav)
{
	_vector		vPos = GetTransformState(STATE_POS);			// 현재 월드 상의 위치
	_vector		vLook = GetTransformState(STATE_LOOK);			// 플레이어 Look Vector
	vLook = XMVector3Normalize(vLook);							

	// NavMesh가 없는 경우
	if (nullptr == pNav)
	{
		vPos += vLook * m_fSPPerSec * fTimeDelta;
		SetTransformState(STATE_POS, vPos);
	}
	// 네브 메쉬가 있는경우
	// - 이웃이 있는지 없는지 분기해주고, 이웃이 없다면 슬라이딩 벡터 방향으로 이동을 해준다.
	else
		FindPositionOnNavigation(vLook, pNav, fTimeDelta);
}

void CTransform::GoBackward(_float fTimeDelta, CNavigation* pNav)
{
	_vector vPos = GetTransformState(STATE_POS);
	_vector vLook = GetTransformState(STATE_LOOK);
	vLook = XMVector3Normalize(vLook);

	if (nullptr == pNav)
	{
		vPos -= vLook * m_fSPPerSec * fTimeDelta;
		SetTransformState(STATE_POS, vPos);
	}
	else
		FindPositionOnNavigation(-vLook, pNav, fTimeDelta);
}

void CTransform::GoLeft(_float fTimeDelta, CNavigation* pNav)
{
	_vector vPos = GetTransformState(STATE_POS);
	_vector vRight = GetTransformState(STATE_RIGHT);
	vRight = XMVector3Normalize(vRight);

	if (nullptr == pNav)
	{
		vPos -= vRight * m_fSPPerSec * fTimeDelta;
		SetTransformState(STATE_POS, vPos);
	}
	else
		FindPositionOnNavigation(-vRight, pNav, fTimeDelta);
}

void CTransform::GoRight(_float fTimeDelta, CNavigation* pNav)
{
	_vector vPos = GetTransformState(STATE_POS);
	_vector vRight = GetTransformState(STATE_RIGHT);
	vRight = XMVector3Normalize(vRight);

	if (nullptr == pNav)
	{
		vPos += vRight * m_fSPPerSec * fTimeDelta;
		SetTransformState(STATE_POS, vPos);
	}
	else
		FindPositionOnNavigation(vRight, pNav, fTimeDelta);
}

void CTransform::GoRightFoward(_float fTimeDelta, CNavigation* pNav)
{
	_vector vPos = GetTransformState(STATE_POS);		
	_vector vRight = GetTransformState(STATE_RIGHT);	
	_vector vLook = GetTransformState(STATE_LOOK);		
	_vector vDir = XMVector3Normalize(vRight + vLook);	

	if (nullptr == pNav)
	{
		vPos += vDir * m_fSPPerSec * fTimeDelta;
		SetTransformState(STATE_POS, vPos);
	}
	else
		FindPositionOnNavigation(vDir, pNav, fTimeDelta);
}

void CTransform::GoLeftFoward(_float fTimeDelta, CNavigation* pNav)
{
	_vector vPos = GetTransformState(STATE_POS);
	_vector vLeft = -GetTransformState(STATE_RIGHT);
	_vector vLook = GetTransformState(STATE_LOOK);
	_vector vDir = XMVector3Normalize(vLeft + vLook);

	if (nullptr == pNav)
	{
		vPos += vDir * m_fSPPerSec * fTimeDelta;
		SetTransformState(STATE_POS, vPos);
	}
	else
		FindPositionOnNavigation(vDir, pNav, fTimeDelta);
}

void CTransform::GoRightBack(_float fTimeDelta, CNavigation* pNav)
{
	_vector vPos = GetTransformState(STATE_POS);
	_vector vRight = GetTransformState(STATE_RIGHT);
	_vector vBack = -GetTransformState(STATE_LOOK);
	_vector vDir = XMVector3Normalize(vRight + vBack);

	if (nullptr == pNav)
	{
		vPos += vDir * m_fSPPerSec * fTimeDelta;
		SetTransformState(STATE_POS, vPos);
	}
	else
		FindPositionOnNavigation(vDir, pNav, fTimeDelta);
}

void CTransform::GoLeftBack(_float fTimeDelta, CNavigation* pNav)
{
	_vector vPos = GetTransformState(STATE_POS);
	_vector vRight = -GetTransformState(STATE_RIGHT);
	_vector vBack = -GetTransformState(STATE_LOOK);
	_vector vDir = XMVector3Normalize(vRight + vBack);

	if (nullptr == pNav)
	{
		vPos += vDir * m_fSPPerSec * fTimeDelta;
		SetTransformState(STATE_POS, vPos);
	}
	else
		FindPositionOnNavigation(vDir, pNav, fTimeDelta);
}

// NavMesh상에서 이동이 가능한지 아닌지 여부를 판별하고, 이동가능하다면 vDir방향으로 그대로 이동시키고
// 이동이 불가능하다면 이웃한 Cell(삼각형)을 찾고 Cell이 없다면 삼각형 한 면에 평행한 방향 벡터(슬라이딩 벡터)로 이동시킨다.
void CTransform::FindPositionOnNavigation(_fvector vDir, CNavigation* pNav, _float fTimeDelta)
{
	_vector		vPos = GetTransformState(STATE_POS);
	_vector		vCurDstDir = XMVector3Normalize(vDir);

	// 먼저 이동시키지 않고 vNextTickPos 라는 것을 통해 다음 틱 이동 위치를 미리 계산하여 이 값으로 Cell을 찾는다.
	_vector		vNextTickPos = vPos + vCurDstDir * m_fSPPerSec * fTimeDelta;
	_float3		vSlidingDir = { 0.f, 0.f, 0.f };

	// vNextTickPos 가 여진히 Cell안에 있는 경우 그대로 이동 시켜준다.
	if (pNav->IsInCell(vNextTickPos, vCurDstDir, vSlidingDir))
		SetTransformState(STATE_POS, vNextTickPos);
	// vNextTickPos 가 Cell 밖에 있는 경우 & vNextTickPos의 위치에 Cell이 없는 경우
	else
	{
		// 슬라이딩 벡터 방향으로 갈 다음 틱 위치를 계산한다.
		vNextTickPos = vPos + XMVector3Normalize(XMLoadFloat3(&vSlidingDir)) * m_fSPPerSec * fTimeDelta;

		// 슬라이딩 벡터 방향으로 이동했는데 셀안에 있는 경우
		if (pNav->IsInCell(vNextTickPos))
			SetTransformState(STATE_POS, vNextTickPos);
		// 슬라이딩 벡터 방향으로 이동했는데 셀안에 없는 경우
		else
		{
			_float3 vResultDir = { 0.f, 0.f, 0.f };
			if (pNav->IsInOtherCell(vPos, XMLoadFloat3(&vSlidingDir), vResultDir))
			{
				vNextTickPos = vPos + XMVector3Normalize(XMLoadFloat3(&vResultDir)) * m_fSPPerSec * fTimeDelta;

				if (pNav->IsInCell(vNextTickPos))
					SetTransformState(STATE_POS, vNextTickPos);
			}
		}
	}
}

// 뷰 스페이스 행렬을 직접 만들 때와 똑같다.
// https://cjbworld.tistory.com/71
void CTransform::LookAt(_fvector vAt)
{
	_float3	vScale = ComputeScaled();

	_vector vPos = GetTransformState(STATE_POS);
	_vector vLook = vAt - vPos;
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector vUp = XMVector3Cross(vLook, vRight);

	SetTransformState(STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	SetTransformState(STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	SetTransformState(STATE_LOOK, XMVector3Normalize(vLook)* vScale.z);
}

// 특정 축으로 ftimeDelta만큼 계속 도는 함수이다.
void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_vector vRight = GetTransformState(STATE_RIGHT);
	_vector vUp = GetTransformState(STATE_UP);
	_vector vLook = GetTransformState(STATE_LOOK);

	_matrix RotMatrix = XMMatrixRotationAxis(vAxis, m_fRotSPPerSec * fTimeDelta);

	SetTransformState(STATE_RIGHT, XMVector3TransformNormal(vRight, RotMatrix));
	SetTransformState(STATE_UP, XMVector3TransformNormal(vUp, RotMatrix));
	SetTransformState(STATE_LOOK, XMVector3TransformNormal(vLook, RotMatrix));
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_float3 vScale = ComputeScaled();
	
	_vector	vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vector	vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vector	vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	_matrix RotMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	SetTransformState(STATE_RIGHT, XMVector3TransformNormal(vRight, RotMatrix));
	SetTransformState(STATE_UP, XMVector3TransformNormal(vUp, RotMatrix));
	SetTransformState(STATE_LOOK, XMVector3TransformNormal(vLook, RotMatrix));

}

void CTransform::GoUp(_float fTimeDelta)
{
	_vector vPos = GetTransformState(STATE_POS);
	_vector vUp = GetTransformState(STATE_UP);

	vPos += XMVector3Normalize(vUp) * m_fSPPerSec * fTimeDelta;

	SetTransformState(STATE_POS, vPos);
}

void CTransform::GoDown(_float fTimeDelta)
{
	_vector vPos = GetTransformState(STATE_POS);
	_vector vUp = GetTransformState(STATE_UP);

	vPos -= XMVector3Normalize(vUp) * m_fSPPerSec * fTimeDelta;

	SetTransformState(STATE_POS, vPos);
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTransform* pInst = new CTransform(pDevice, pContext);

	if (FAILED(pInst->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CTransform");
		SafeRelease(pInst);
	}

	return pInst;
}

CComponent* CTransform::Clone(void* pArg)
{
	CTransform* pInst = new CTransform(*this);

	if (FAILED(pInst->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTransform");
		SafeRelease(pInst);
	}

	return pInst;
}

void CTransform::Free()
{
	__super::Free();
}
