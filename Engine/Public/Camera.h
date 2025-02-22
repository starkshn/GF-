#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct camera_desc : public CGameObject::GAMEOBJECT_DESC
	{
		_float3		vEye{};
		_float3		vAt{};

		_float		fFovY{ 0.f };
		_float		fAspectRatio{ 0.f };
		_float		fNear{ 0.f };
		_float		fFar{ 0.f };
	} CAMERA_DESC;

protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& Prototype);
	virtual ~CCamera() override = default;

public:
	virtual HRESULT			InitializePrototype();
	virtual HRESULT			Initialize(void* pArg);
	virtual void			PriorityUpdate(_float fTimeDelta);
	virtual void			Update(_float fTimeDelta);
	virtual void			LateUpdate(_float fTimeDelta);
	virtual HRESULT			Render();

protected:
	/* 만들어낸 뷰, 투영 변환 행렬들을 파이프라인 클래스에 저장하겠다. .*/
	/* 뷰, 투영 행렬들을 만들어내기 위한 데이터들을 보관해서 각 행렬들을 만드는 기능을 수행해주자. */
	/* 뷰 -> Transform이 대체하고 있다. */
	/* 투영 -> x */

	_float					m_fFovy = { 0.f };
	_float					m_fAspect = { 0.f };
	_float					m_fNear = { 0.f };
	_float					m_fFar = { 0.f };

protected:
	void					Compute_PipeLine_Matrices();


public:
	virtual CGameObject*	Clone(void* pArg) abstract;
	virtual void			Free() override;
};

END