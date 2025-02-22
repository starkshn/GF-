#pragma once

#include "Renderer.h"
#include "PipeLine.h"
#include "Prototype_Manager.h"
#include "Input_Device.h"
#include "Height.h"
#include "Shadow.h"

#include "UIObject.h"
#include "UIParent.h"
#include "UIPanel.h"
#include "UIElement.h"

/* 1. 클라이언트 개발자가 사용할 만한 엔진에서 제공해주는 기능들을 보여준다 .*/
/* 2. 엔진의 기능을 원활하게 사용할 수 있도록 엔진을 초기화한다.Initialize_Engine */
/* 3. 사용 종료시에 엔진 기능 구현을 위해서 할당했던 모든 데이터를 삭제한다. Release_Engine*/
/* 4. 엔진에서 갱신이 필요한 모든 객체를 한곳에 모아서 갱신해 준다. */

BEGIN(Engine)

// forward declarations...
class CLevel;
class CGraphic_Device;
class CTimer_Manager;
class CLevel_Manager;
class CPrototype_Manager;
class CObject_Manager;
class CBase;
class CLightManager;
class CLayer;
class CFontManager;
class CRenderTargetManager;
class CPicking;
class CFrustum;
class CUIManager;
class CSoundManager;

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)

private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public: /* For.GameInstance */
	HRESULT						InitializeEngine(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
	void						UpdateEngine(_float fTimeDelta);
	HRESULT						RenderBegin(const _float4& vClearColor = _float4(0.f, 0.f, 1.f, 1.f));
	HRESULT						Draw();
	HRESULT						Render_End();
	void						Clear(const _int iLevelID);

public:
	_float						ComputeRandomNormal();
	_float						ComputeRandom(_float fMin, _float fMax);

	// For Time MGR
public:
	_float						GetTimeDelta(const _wstring& strTimerTag);
	HRESULT						AddTimer(const _wstring& strTimerTag);
	void						UpdateTimeDelta(const _wstring& strTimerTag);


	// For Renderer
public:
	HRESULT						AddRenderObject(CRenderer::RENDER_GROUP eRenderGroup, class CGameObject* pRenderObject);


	// For Level MGR
public:
	HRESULT						OpenLevel(_int iLevelIndex, CLevel* pNewLevel);
	CLevel*						GetCurrentLevel();
	_uint						GetCurrentLevelIndex();

	// For Proto MGR
public:
	HRESULT						AddPrototype(_uint iLevelIdx, const _wstring& strProtoTypeTag, CBase* pProtoType);
	CBase*						ClonePrototype(Engine::PROTOTYPE eType, _uint iLevelIdx, const _wstring& strProtoTypeTag, void* pArg);


	// For Obj MGR
public:
	HRESULT						AddGameObjectToLayer(_uint iProtoTypeLevelIdx, const _wstring& strProtoTypeTag, _uint iLevelIdx, const _wstring& strLayerTag, void* pArg = nullptr);
	CLayer*						FindLayer(_uint iLevelIdx, const _wstring& strLayerTag);
	CGameObject*				GetPickedGameObject(_uint iLevelID, _fvector vRayOrigin, _fvector vRayDir, _float& fDist, _float4& vPickingPos);
	_bool						RemoveObject(_uint iLevelID, CGameObject* pRemoveObject);
	CComponent*					GetComponent(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iObjectIndex = 0);
	CComponent*					GetComponent(_uint iLevelIndex, const _wstring& strLayerTag, _uint iParObjetIndex, const _wstring& strPartComponentTag, _uint iObjectIndex = 0);

	void						ActiveMonsters(_uint iLevelIndex, const _wstring& strLayerTag, const _uint iTriggerID);

	// 단순 충돌만 확인하는 함수
	_bool						CheckIntersectInLayer(_uint iLevelIndex, const _wstring& strLayerTag, const CCollider* pSrc);

	// 충돌 정보도 함께 받는 함수
	_bool						CheckIntersectInLayer(_uint iLevelIndex, const _wstring& strLayerTag, const CCollider* pSrc, HIT_RESULT& HitResult);

	_bool						CheckIntersectInLayerOnlyProjectile(_uint iLevelIndex, const _wstring& strLayerTag, CCollider* pProjectileCol, HIT_RESULT& HitResult);

	// For Tool Project
public:
	// 현재 레이어에 등록된 오브젝트중 가장 뒤에 녀석 빼옴.
	CGameObject*				GetGameObjctInLayer(_uint iProtoTypeLevelIdx, _uint iLevelIdx, const _wstring& strLayerTag, const string& strObjectTag);
	CGameObject*				GetGameObjctInLayerBack(_uint iProtoTypeLevelIdx, _uint iLevelIdx, const _wstring& strLayerTag);

	// For PipeLine
public:
	void						SetTransformMatrix(CPipeLine::DTS eState, _fmatrix TransformMatrix);
	_matrix						GetTransformMatrix(CPipeLine::DTS eState);
	_float4x4					GetTransformMatrixFloat4x4(CPipeLine::DTS eState);
	_float4x4					GetTransformMatrixFloat4x4Inverse(CPipeLine::DTS eState);
	const _float4*				GetCameraPosition() const;
	const _float4*				GetCameraState(CAMERA_STATE eState);
	_matrix						GetTransformMatrixInverse(CPipeLine::DTS eState);

	// For Light Manager
public:
	HRESULT						AddLight(const LIGHT_DESC& LightDesc);
	const LIGHT_DESC*			GetLightDesc(_uint iIdx) const;
	HRESULT						RenderLights(CShader* pShader, CVIBufferRect* pVIBuffer);
	HRESULT						ActivePointLight(const string& strLightName);
	HRESULT						DeActivePointLight(const string& strLightName);
	HRESULT						UpdatePointLightPos(const string& strLightName, _fvector vWorldPos);

	// For RenderTargetManager
public:
	HRESULT						AddRenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT						AddMRT(const _wstring& MRTTaag, const _wstring& strTargetTag);
	HRESULT						BeginMRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr, _bool bIsClear = true);
	HRESULT						EndMRT();
	HRESULT						BindRTShaderResource(CShader* pShader, const _char* pConstantName, const _wstring& strRenderTargetTag);
	HRESULT						CopyRTResource(const _wstring& strTargetTag, ID3D11Texture2D* pDest);

#ifdef _DEBUG
	HRESULT						ReadyRTVDebug(const _wstring& strTargetTag, _float fScreenX, _float fScreenY, _float fSizeX, _float fSizeY);
	HRESULT						RenderRTVDebug(const _wstring& strMRTTag, CShader* pShader, CVIBufferRect* pVIBuffer);
#endif

	void						AdjustBlur();


public:
	HRESULT						SpawnObjectAtPickingPos(_uint iProtoTypeLevelIdx, const _wstring& strProtoTypeTag, _uint iLevelIdx, const _wstring& strLayerTag, _float3 vSpawnPos, string strName, void* pArg = nullptr);
	HRESULT						SpawnObject(_uint iProtoTypeLevelIdx, const _wstring& strProtoTypeTag, _uint iLevelIdx, const _wstring& strLayerTag, void* pArg = nullptr);

	// Key Manager
private:
	const KEY_STATE				GetKeyState(KEY eKey) { return m_pInputDevice->GetKeyState(eKey); }
	const KEY_STATE				GetMouseState(MOUSE_KEY eKey) { return m_pInputDevice->GetMouseKeyState(eKey); }

public:
	_bool						KeyCheck(KEY eKey, KEY_STATE eKeyState) { return GetKeyState(eKey) == eKeyState; }
	_bool						KeyPressing(KEY eKey) { return KeyCheck(eKey, KEY_STATE::PRESSING); }
	_bool						KeyDown(KEY eKey) { return KeyCheck(eKey, KEY_STATE::DOWN); }
	_bool						KeyUp(KEY eKey) { return KeyCheck(eKey, KEY_STATE::UP); }


	_bool						MouseCheck(MOUSE_KEY eKey, KEY_STATE eKeyState) { return GetMouseState(eKey) == eKeyState; }
	_bool						MousePressing(MOUSE_KEY eKey) { return MouseCheck(eKey, KEY_STATE::PRESSING); }
	_bool						MouseDown(MOUSE_KEY eKey) {return MouseCheck(eKey, KEY_STATE::DOWN); }
	_bool						MouseUp(MOUSE_KEY eKey) { return MouseCheck(eKey, KEY_STATE::UP); }

	_long						GetMouseMove(MOUSE_MOVE eMouseMove) { return m_pInputDevice->Get_DIMouseMove(eMouseMove); }


public: /* Font_Manager */
	HRESULT						AddFont(const _wstring& strFontTag, const _tchar* pFontFilePath);
	HRESULT						RenderFont(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor, _float fRotation = 0.f, const _float2& vOrigin = _float2(0.f, 0.f));

public:
	// Utils
	_vector						ConvertHitNormalToPivotPos(const _float3& vHitNormal);

public: /* Picking */
	_bool						IsPicked(_Inout_ _float4* pWorldPos);

	// Height
public:
	const _float4x4*			GetHeightTransform(CHeight::D3D_TRANSFORM_STATE eState);
	void						SetUpHeightTransformMatrix(CHeight::D3D_TRANSFORM_STATE eState, _fmatrix TransformMatrix);
	_float						ComputeHeight(_fvector vWorldPos);

	// Shadow
public:
	const _float4x4*			GetShadowTransform(CShadow::D3D_TRANSFORM_STATE eState);
	void						SetUpShadowTransformMatrix(CShadow::D3D_TRANSFORM_STATE eState, _fmatrix TransformMatrix);

#ifdef _DEBUG
public:	
	HRESULT						AddDebugComponent(class CComponent* pDebugCom);

#endif

	// UI
public:
	HRESULT						AddPanelUIToManager(string strName, CUIPanel* pUI);
	HRESULT						AddElementUIToManager(string strName, CUIElement* pUI);

	HRESULT						AddElementToPanel(string strPanelName, string strEleName, _float fSX, _float fSY);

	CUIElement*					GetEleUIFromPanel(string strPanelName, string strEleName);
	CUIElement*					GetEleUI(string strEleName);

	CUIPanel*					GetPanelUI(string strPanel);

	void						SetDamageFont(_int iDamage, _int iColor, _float fSX, _float fSY);


	void						AddPlayerUI(CUIParent* pUI);
	void						RenderOffPlayerUI();
	void						RenderOnPlayerUI();



	HRESULT						AddElementUIToManagerVertical(string strName, CUIElement* pUI);
	CUIElement*					GetVerticalUI();

	// Frustum
public:
	_bool						IsInFrustumWorldSpace(_fvector vWorldPos, _float fRange = 0.f);
	_bool						IsInFrustumLocalSpace(_fvector vLocalPos, _float fRange = 0.f);
	void						TransformFrustumToLocalSpace(_fmatrix WorldMatrix);


public:
	D3D11_VIEWPORT				GetViewPortDesc();


public:/* For SoundManager*/
	void PlayEffect(const TCHAR* pSoundKey, float fVolume);
	void PlayBGM(const TCHAR* pSoundKey, float fVolume);
	void PlayBGM(const TCHAR* pSoundKey, float fVolume, int iUpdate);

	void Resume();
	void Pause();
	void StopBGM();
	void StopAll();

	void SetBGMVolume(float fVolume);
	void SetBGMPitch(float fPitch);
	void UpdateBGM(const _float& fTimeDelta, const _float& fSpeed);
	void SetBGM(int iUpdate);

	void StopSound(_uint eID);
	_uint GetNowEffectID();



	
private:
	CGraphic_Device*			m_pGraphicDevice = { nullptr };
	CInput_Device*				m_pInputDevice = { nullptr };
	CTimer_Manager*				m_pTimerManager = { nullptr };	
	CLevel_Manager*				m_pLevelManager = { nullptr };
	CPrototype_Manager*			m_pPrototypeManager = nullptr;
	CObject_Manager*			m_pObjectManager = nullptr;
	CRenderer*					m_pRenderer = nullptr;
	CPipeLine*					m_pPipeLine = nullptr;
	CLightManager*				m_pLightManager = nullptr;
	CFontManager*				m_pFontManager = nullptr;
	CRenderTargetManager*		m_pRenderTargetManager = nullptr;
	CPicking*					m_pPicking = nullptr;
	CHeight*					m_pHeight = nullptr;
	CShadow*					m_pShadow = nullptr;
	CFrustum*					m_pFrustum = nullptr;
	CUIManager*					m_pUIManager = nullptr;
	CSoundManager*				m_pSoundManager = nullptr;
	
public:
	static void					ReleaseEngine();
	virtual void				Free() override;
};

END