#pragma once
#include "Component.h"

// Transform의 경우 객체 생성시 강제로 Transform Com를 가지고 있게 할 것이다.
// 
// 기능
// - 객체의 월드 상태(m_WorldMatrix)를 표현한다
// - 객체의 월드 상태 변환의 기능을 수행한다. ex) 앞으로 간다, 회전한다, 바라본다.
// - 월드행렬을 정점에게 적용할 수 있도록 shader로 월드행렬을 던지는 기능을 수행한다.


BEGIN(Engine)

class CShader;
class CNavigation;

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE {STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POS, STATE_END, };

	typedef struct TransformDesc
	{
		_float	fSpeedPerSec{};
		_float	fRotSpeedPerSec{};

	} TRANSFORM_DESC;

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& Prototype);
	virtual ~CTransform() override = default;

public:
	void					SetTransformState(STATE eState, _fvector vState)
	{ 
		XMStoreFloat4((_float4*)&m_WorldMatrix.m[eState], vState); 
	}

	// XMLoadFloat4x4 반환 타입에 __vectorcall이 붙어 있는데 해당 함수 호출 규칙은 가능한 경우 함수에 대한 인수를 레지스터에 전달 하도록 지정하는 호출 규약이다.
	// https://learn.microsoft.com/ko-kr/cpp/cpp/vectorcall?view=msvc-170
	_vector					GetTransformState(STATE eState) { return XMLoadFloat4x4(&m_WorldMatrix).r[eState]; }

	_matrix					GetWorldMatrix() { return XMLoadFloat4x4(&m_WorldMatrix); }
	const _float4x4*		GetWorldMatrixPtr() { return &m_WorldMatrix; }
	_matrix					GetWorldMatrixInv() { return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)); }

	
	_float3					ComputeScaled();

public:
	virtual HRESULT			InitializePrototype();
	virtual HRESULT			Initialize(void* pArg);

public:
	HRESULT					BindShaderResource(CShader* pShader, const _char* pConstantName);

public:
	void					Scaling(_float fX, _float fY, _float fZ);
	void					SetSpeed(_float fSP) { m_fSPPerSec = fSP; }
	void					SetRotationSpeed(_float fSP) { m_fRotSPPerSec = fSP; }

public:
	void					GoFoward(_float fTimeDelta, CNavigation* pNav = nullptr);
	void					GoBackward(_float fTimeDelta, CNavigation* pNav = nullptr);
	void					GoLeft(_float fTimeDelta, CNavigation* pNav = nullptr);
	void					GoRight(_float fTimeDelta, CNavigation* pNav = nullptr);

	void					GoRightFoward(_float fTimeDelta, CNavigation* pNav = nullptr);
	void					GoLeftFoward(_float fTimeDelta, CNavigation* pNav = nullptr);
	void					GoRightBack(_float fTimeDelta, CNavigation* pNav = nullptr);
	void					GoLeftBack(_float fTimeDelta, CNavigation* pNav = nullptr);



	void					FindPositionOnNavigation(_fvector vDir, CNavigation* pNav, _float fTimeDelta);

	void					LookAt(_fvector vAt); // vAt을 바라보는 함수
	void					Turn(_fvector vAxis, _float fTimeDelta); // 기존 회전을 기준으로 추가로 정해진 속도로 회전한다.
	void					Rotation(_fvector vAxis, _float fRadian); // 항등 상태를 기준으로 지정한 각도로 회전한다.


	// For Tool_Camera
	void					GoUp(_float fTimeDelta);
	void					GoDown(_float fTimeDelta);
	_float4x4				GetWorldMatrixToolCam() { return m_WorldMatrix; }

public:
	_float					GetSpeed() { return m_fSPPerSec; }


private:
	// row major 방식 openGL은 col major이다.
	_float4x4				m_WorldMatrix;			// _float4x4 typedef 걸려 있긴 한데 해당 자료형은 행렬 곱셈등을 지원 X
	_float					m_fSPPerSec = 0.f;		// 초당 속도
	_float					m_fRotSPPerSec = 0.f;	// 초당 회전 속도

public:
	static CTransform*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
	
};

END