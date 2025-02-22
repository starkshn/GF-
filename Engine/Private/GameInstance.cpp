#include "GameInstance.h"

#include "Graphic_Device.h"
#include "Timer_Manager.h"
#include "Level.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Prototype_Manager.h"
#include "Input_Device.h"
#include "LightManager.h"
#include "FontManager.h"
#include "Layer.h"
#include "RenderTargetManager.h"
#include "Picking.h"
#include "Frustum.h"
#include "UIManager.h"
#include "SoundManager.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::InitializeEngine(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	m_pGraphicDevice = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.isWindowed, EngineDesc.iViewportWidth, EngineDesc.iViewportHeight, ppDevice, ppContext);
	if (nullptr == m_pGraphicDevice)
		return E_FAIL;

	m_pInputDevice = CInput_Device::Create(EngineDesc.hInstance, EngineDesc.hWnd);
	if (nullptr == m_pInputDevice)
		return E_FAIL;

	m_pLightManager = CLightManager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pLightManager)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pRenderTargetManager = CRenderTargetManager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderTargetManager)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pPrototypeManager = CPrototype_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pPrototypeManager)
		return E_FAIL;

	m_pObjectManager = CObject_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pObjectManager)
		return E_FAIL;

	m_pTimerManager = CTimer_Manager::Create();
	if (nullptr == m_pTimerManager)
		return E_FAIL;

	m_pLevelManager = CLevel_Manager::Create();
	if (nullptr == m_pLevelManager)
		return E_FAIL;

	m_pFontManager = CFontManager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pFontManager)
		return E_FAIL;

	m_pPicking = CPicking::Create(*ppDevice, *ppContext, EngineDesc.hWnd);
	if (nullptr == m_pPicking)
		return E_FAIL;

	m_pHeight = CHeight::Create(*ppDevice, *ppContext);
	if (nullptr == m_pHeight)
		return E_FAIL;

	m_pShadow = CShadow::Create(*ppDevice, *ppContext);
	if (nullptr == m_pShadow)
		return E_FAIL;

	m_pFrustum = CFrustum::Create();
	if (nullptr == m_pFrustum)
		return E_FAIL;

	m_pUIManager = CUIManager::Create();
	if (nullptr == m_pUIManager)
		return E_FAIL;

	m_pSoundManager = CSoundManager::Create();
	if (!m_pSoundManager)
		return E_FAIL;

	return S_OK;
}

void CGameInstance::UpdateEngine(_float fTimeDelta)
{
	m_pLevelManager->Update(fTimeDelta);

	m_pPicking->Update();

	static _bool bFirst = false;
	if (!bFirst)
	{
		bFirst = true;
		m_pHeight->Update();
	}

	m_pInputDevice->Update_InputDev();


	m_pObjectManager->PriorityUpdate(fTimeDelta);

	// 뷰, 투영이 완성된 시점이다.
	m_pPipeLine->Update();

	m_pFrustum->Update();

	m_pObjectManager->Update(fTimeDelta);

	m_pObjectManager->LateUpdate(fTimeDelta);

	m_pUIManager->Update(fTimeDelta);
}

HRESULT CGameInstance::RenderBegin(const _float4 & vClearColor)
{
	// 241003
	// RTV를 ClearColor로 밀어준다.
	m_pGraphicDevice->Clear_BackBuffer_View(vClearColor);

	// DSV를 1으로 초기화 해준다. 깊이값을 재설정 하기 위해 -> 비교해야하기 때문이다.
	// VS 를 거치고 나면 카메라를 기준으로 한 Z값, NDC로 바뀌는데 0~1 사이의 값을 기록하기 때문이다.
	// 0은 카메라랑 가장 가까이에 있는 값이고 1은 가장 멀리 있는 값이다.
	m_pGraphicDevice->Clear_DepthStencil_View();

	return S_OK;
}

// Rendering과 똑같다.
HRESULT CGameInstance::Draw()
{
	m_pRenderer->DrawRenderObject();

	m_pLevelManager->Render();

	return S_OK;
}

HRESULT CGameInstance::Render_End()
{
	return m_pGraphicDevice->Present();
}

void CGameInstance::Clear(const _int iLevelID)
{
	// 특정 레벨용 객체들을 지운다.
	m_pObjectManager->Clear(iLevelID);
	m_pPrototypeManager->Clear(iLevelID);
	// m_pLightManager->Clear(iLevelID);
}



_float CGameInstance::ComputeRandomNormal()
{
	return (_float)rand() / RAND_MAX;
}

_float CGameInstance::ComputeRandom(_float fMin, _float fMax)
{
	return fMin + (fMax - fMin) * ComputeRandomNormal();
}

_float CGameInstance::GetTimeDelta(const _wstring & strTimerTag)
{
	if (nullptr == m_pTimerManager)
		return 0.f;

	return m_pTimerManager->GetTimeDelta(strTimerTag);
}

HRESULT CGameInstance::AddTimer(const _wstring & strTimerTag)
{
	if (nullptr == m_pTimerManager)
		return E_FAIL;

	return m_pTimerManager->AddTimer(strTimerTag);
}

void CGameInstance::UpdateTimeDelta(const _wstring & strTimerTag)
{
	if (nullptr == m_pTimerManager)
		return;

	return m_pTimerManager->UpdateTimeDelta(strTimerTag);
}


HRESULT CGameInstance::AddRenderObject(CRenderer::RENDER_GROUP eRenderGroup, CGameObject* pRenderObject)
{
	if (nullptr == m_pRenderer)
		return E_FAIL;
	
	return m_pRenderer->AddRenderObject(eRenderGroup, pRenderObject);
}

HRESULT CGameInstance::OpenLevel(_int iLevelIndex, CLevel* pNewLevel)
{
	if (nullptr == m_pLevelManager)
		return E_FAIL;
	
	return m_pLevelManager->OpenLevel(iLevelIndex, pNewLevel);
}

CLevel* CGameInstance::GetCurrentLevel()
{
	if (nullptr == m_pLevelManager)
		return nullptr;

	return m_pLevelManager->GetCurrentLevel();
}

_uint CGameInstance::GetCurrentLevelIndex()
{
	return m_pLevelManager->GetCurrentLevelIndex();
}

HRESULT CGameInstance::AddPrototype(_uint iLevelIdx, const _wstring& strProtoTypeTag, CBase* pProtoType)
{
	if (nullptr == m_pPrototypeManager)
		return E_FAIL;

	return m_pPrototypeManager->AddPrototype(iLevelIdx, strProtoTypeTag, pProtoType);
}

CBase* CGameInstance::ClonePrototype(Engine::PROTOTYPE eType, _uint iLevelIdx, const _wstring& strProtoTypeTag, void* pArg)
{
	if (nullptr == m_pPrototypeManager)
		return nullptr;

	return m_pPrototypeManager->ClonePrototype(eType, iLevelIdx, strProtoTypeTag, pArg);
}

HRESULT CGameInstance::AddGameObjectToLayer(_uint iProtoTypeLevelIdx, const _wstring& strProtoTypeTag, _uint iLevelIdx, const _wstring& strLayerTag, void* pArg)
{
	if (nullptr == m_pObjectManager)
		return E_FAIL;

	return m_pObjectManager->AddGameObjectToLayer(iProtoTypeLevelIdx, strProtoTypeTag, iLevelIdx, strLayerTag, pArg);
}

CLayer* CGameInstance::FindLayer(_uint iLevelIdx, const _wstring& strLayerTag)
{
	if (nullptr == m_pObjectManager)
		return nullptr;

	return m_pObjectManager->FindLayer(iLevelIdx, strLayerTag);
}

CGameObject* CGameInstance::GetPickedGameObject(_uint iLevelID, _fvector vRayOrigin, _fvector vRayDir, _float& fDist, _float4& vPickingPos)
{
	if (nullptr == m_pObjectManager)
		return nullptr;

	return m_pObjectManager->GetPickedGameObject(iLevelID, vRayOrigin, vRayDir, fDist, vPickingPos);
}

_bool CGameInstance::RemoveObject(_uint iLevelID, CGameObject* pRemoveObject)
{
	if (nullptr == m_pObjectManager)
		return false;

	return m_pObjectManager->RemoveObject(iLevelID, pRemoveObject);
}

CComponent* CGameInstance::GetComponent(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iObjectIndex)
{
	if (nullptr == m_pObjectManager)
		return nullptr;

	return m_pObjectManager->GetComponent(iLevelIndex, strLayerTag, strComponentTag, iObjectIndex);
}

CComponent* CGameInstance::GetComponent(_uint iLevelIndex, const _wstring& strLayerTag, _uint iPartObjetIndex, const _wstring& strPartComponentTag, _uint iObjectIndex)
{
	if (nullptr == m_pObjectManager)
		return nullptr;

	return m_pObjectManager->GetComponent(iLevelIndex, strLayerTag, iPartObjetIndex, strPartComponentTag, iObjectIndex);
}

void CGameInstance::ActiveMonsters(_uint iLevelIndex, const _wstring& strLayerTag, const _uint iTriggerID)
{
	if (nullptr == m_pObjectManager)
		return;

	CLayer* pLayer = m_pObjectManager->FindLayer(iLevelIndex, strLayerTag);
	if (nullptr != pLayer)
		pLayer->ActiveMonsters();
}

_bool CGameInstance::CheckIntersectInLayer(_uint iLevelIndex, const _wstring& strLayerTag, const CCollider* pSrc)
{
	if (nullptr == m_pObjectManager)
		return false;

	CLayer* pLayer = m_pObjectManager->FindLayer(iLevelIndex, strLayerTag);
	if (nullptr != pLayer)
		return pLayer->CheckIntersect(pSrc);
	return false;
}

_bool CGameInstance::CheckIntersectInLayer(_uint iLevelIndex, const _wstring& strLayerTag, const CCollider* pSrcAABB, HIT_RESULT& HitResult)
{
	if (nullptr == m_pObjectManager)
		return false;

	CLayer* pLayer = m_pObjectManager->FindLayer(iLevelIndex, strLayerTag);
	if (nullptr != pLayer)
		return pLayer->CheckIntersect(pSrcAABB, HitResult);
	return false;
}

_bool CGameInstance::CheckIntersectInLayerOnlyProjectile(_uint iLevelIndex, const _wstring& strLayerTag, CCollider* pProjectileCol, HIT_RESULT& HitResult)
{
	if (nullptr == m_pObjectManager)
		return false;

	CLayer* pLayer = m_pObjectManager->FindLayer(iLevelIndex, strLayerTag);
	if (nullptr != pLayer)
		return pLayer->CheckIntersectOnlyProjectile(pProjectileCol, HitResult);
	return false;
}

CGameObject* CGameInstance::GetGameObjctInLayer(_uint iProtoTypeLevelIdx, _uint iLevelIdx, const _wstring& strLayerTag, const string& strObjectTag)
{
	if (nullptr == m_pObjectManager)
		return nullptr;

	return m_pObjectManager->GetGameObjectInLayer(iProtoTypeLevelIdx, iLevelIdx, strLayerTag, strObjectTag);
}

CGameObject* CGameInstance::GetGameObjctInLayerBack(_uint iProtoTypeLevelIdx, _uint iLevelIdx, const _wstring& strLayerTag)
{
	if (nullptr == m_pObjectManager)
		return nullptr;

	return m_pObjectManager->GetGameObjectInLayerBack(iProtoTypeLevelIdx, iLevelIdx, strLayerTag);
}

void CGameInstance::SetTransformMatrix(CPipeLine::DTS eState, _fmatrix TransformMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	return m_pPipeLine->SetTransformMatrix(eState, TransformMatrix);
}

_matrix CGameInstance::GetTransformMatrix(CPipeLine::DTS eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->GetTransformMatrix(eState);
}

_float4x4 CGameInstance::GetTransformMatrixFloat4x4(CPipeLine::DTS eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->GetTransformMatrixFloat4x4(eState);
}

const _float4* CGameInstance::GetCameraPosition() const
{
	if (nullptr == m_pPipeLine)
		return nullptr;

	return m_pPipeLine->GetCamPosition();
}

const _float4* CGameInstance::GetCameraState(CAMERA_STATE eState)
{
	if (nullptr == m_pPipeLine)
		return nullptr;

	return m_pPipeLine->GetCameraState(eState);
}

_matrix CGameInstance::GetTransformMatrixInverse(CPipeLine::DTS eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->GetTransformMatrixInverse(eState);
}

_float4x4 CGameInstance::GetTransformMatrixFloat4x4Inverse(CPipeLine::DTS eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();
	
	return m_pPipeLine->GetTransformMatrixFloat4x4Inverse(eState);
}

HRESULT CGameInstance::AddLight(const LIGHT_DESC& LightDesc)
{
	if (nullptr == m_pLightManager)
		return E_FAIL;

	return m_pLightManager->AddLight(LightDesc);
}

const LIGHT_DESC* CGameInstance::GetLightDesc(_uint iIdx) const
{
	if (nullptr == m_pLightManager)
		return nullptr;

	return m_pLightManager->GetLightDesc(iIdx);
}

HRESULT CGameInstance::RenderLights(CShader* pShader, CVIBufferRect* pVIBuffer)
{
	if (nullptr == m_pLightManager)
		return E_FAIL;

	return m_pLightManager->Render(pShader, pVIBuffer);
}

HRESULT CGameInstance::ActivePointLight(const string& strLightName)
{
	if (nullptr == m_pLightManager)
		return E_FAIL;

	m_pLightManager->ActiveLight(strLightName);
	return S_OK;
}

HRESULT CGameInstance::DeActivePointLight(const string& strLightName)
{
	if (nullptr == m_pLightManager)
		return E_FAIL;

	m_pLightManager->DeActiveLight(strLightName);
	return S_OK;
}

HRESULT CGameInstance::UpdatePointLightPos(const string& strLightName, _fvector vWorldPos)
{
	if (nullptr == m_pLightManager)
		return E_FAIL;

	m_pLightManager->UpdateLightPos(strLightName, vWorldPos);
	return S_OK;
}

HRESULT CGameInstance::AddRenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	if (nullptr == m_pRenderTargetManager)
		return E_FAIL;

	return m_pRenderTargetManager->AddRenderTarget(strTargetTag, iWidth, iHeight, ePixelFormat, vClearColor);
}

HRESULT CGameInstance::AddMRT(const _wstring& strMRTTag, const _wstring& strTargetTag)
{
	if (nullptr == m_pRenderTargetManager)
		return E_FAIL;

	return m_pRenderTargetManager->AddMRT(strMRTTag, strTargetTag);
}

HRESULT CGameInstance::BeginMRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV, _bool bIsClear)
{
	if (nullptr == m_pRenderTargetManager)
		return E_FAIL;
	
	return m_pRenderTargetManager->BeginMRT(strMRTTag, pDSV, bIsClear);
}

HRESULT CGameInstance::EndMRT()
{
	if (nullptr == m_pRenderTargetManager)
		return E_FAIL;
	return m_pRenderTargetManager->EndMRT();
}

HRESULT CGameInstance::BindRTShaderResource(CShader* pShader, const _char* pConstantName, const _wstring& strRenderTargetTag)
{
	if (nullptr == m_pRenderTargetManager)
		return E_FAIL;

	return m_pRenderTargetManager->BindShaderResource(pShader, pConstantName, strRenderTargetTag);
}

HRESULT CGameInstance::CopyRTResource(const _wstring& strTargetTag, ID3D11Texture2D* pDest)
{
	if (nullptr == m_pRenderTargetManager)
		return E_FAIL;
	return m_pRenderTargetManager->CopyRTResource(strTargetTag, pDest);
}

#ifdef _DEBUG
HRESULT CGameInstance::ReadyRTVDebug(const _wstring& strTargetTag, _float fScreenX, _float fScreenY, _float fSizeX, _float fSizeY)
{
	if (nullptr == m_pRenderTargetManager)
		return E_FAIL;

	return 	m_pRenderTargetManager->ReadyRTVDebug(strTargetTag, fScreenX, fScreenY, fSizeX, fSizeY);
}

HRESULT CGameInstance::RenderRTVDebug(const _wstring& strMRTTag, CShader* pShader, CVIBufferRect* pVIBuffer)
{
	if (nullptr == m_pRenderTargetManager)
		return E_FAIL;

	return m_pRenderTargetManager->RenderRTVDebug(strMRTTag, pShader, pVIBuffer);
}
void CGameInstance::AdjustBlur()
{
	if (nullptr == m_pRenderer)
		return;

	return m_pRenderer->AdjustBlur();
}
#endif

HRESULT CGameInstance::SpawnObjectAtPickingPos(_uint iProtoTypeLevelIdx, const _wstring& strProtoTypeTag, _uint iLevelIdx, const _wstring& strLayerTag, _float3 vSpawnPos, string strName, void* pArg)
{
	if (nullptr == m_pObjectManager)
		return E_FAIL;

	return m_pObjectManager->SpawnObjectAtPickingPos(iProtoTypeLevelIdx, strProtoTypeTag, iLevelIdx, strLayerTag, vSpawnPos, strName, pArg);
}

HRESULT CGameInstance::SpawnObject(_uint iProtoTypeLevelIdx, const _wstring& strProtoTypeTag, _uint iLevelIdx, const _wstring& strLayerTag, void* pArg)
{
	if (nullptr == m_pObjectManager)
		return E_FAIL;

	return m_pObjectManager->SpawnObject(iProtoTypeLevelIdx, strProtoTypeTag, iLevelIdx, strLayerTag, pArg);
}

HRESULT CGameInstance::AddFont(const _wstring& strFontTag, const _tchar* pFontFilePath)
{
	return m_pFontManager->AddFont(strFontTag, pFontFilePath);
}

HRESULT CGameInstance::RenderFont(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor, _float fRotation, const _float2& vOrigin)
{
	return m_pFontManager->RenderFont(strFontTag, pText, vPosition, vColor, fRotation, vOrigin);
}

_vector CGameInstance::ConvertHitNormalToPivotPos(const _float3& vHitNormal)
{
	// AABB 왼쪽 면 맞은 경우
	if (XMVector3Equal(XMLoadFloat3(&vHitNormal), _vector{ -1.f, 0.f, 0.f }))
		return _fvector{ 1.f, 0.f, 0.f, 1.f };

	// 오른쪽 면 맞은 경우
	else if (XMVector3Equal(XMLoadFloat3(&vHitNormal), _vector{ 1.f, 0.f, 0.f }))
		return _fvector{ -1.f, 0.f, 0.f, 1.f };

	// 후면 맞은 경우, z축이 가장 작은 경우
	else if (XMVector3Equal(XMLoadFloat3(&vHitNormal), _vector{ 0.f, 0.f, -1.f }))
		return _fvector{ 0.f, 0.f, -1.f, 1.f };

	// 전면 맞은 경우
	else if (XMVector3Equal(XMLoadFloat3(&vHitNormal), _vector{ 0.f, 0.f, 1.f }))
		return _fvector{ 0.f, 0.f, 1.f, 1.f };

	// 윗면 맞은 경우 (땅바닥)
	else if (XMVector3Equal(XMLoadFloat3(&vHitNormal), _vector{ 0.f, 1.f, 0.f }))
		return _fvector{ 0.f, -1.f, 0.f, 1.f };

	// 밑면 맞은 경우 (천장)
	else if (XMVector3Equal(XMLoadFloat3(&vHitNormal), _vector{ 0.f, -1.f, 0.f }))
		return _fvector{ 0.f, 1.f, 0.f, 1.f };
}

_bool CGameInstance::IsPicked(_float4* pWorldPos)
{
	return m_pPicking->IsPicked(pWorldPos);
}

const _float4x4* CGameInstance::GetHeightTransform(CHeight::D3D_TRANSFORM_STATE eState)
{
	if (nullptr == m_pHeight)
		return nullptr;
	
	return m_pHeight->GetHeightTransform(eState);
}

void CGameInstance::SetUpHeightTransformMatrix(CHeight::D3D_TRANSFORM_STATE eState, _fmatrix TransformMatrix)
{
	if (nullptr == m_pHeight)
		return;

	m_pHeight->SetUpTransformMatrix(eState, TransformMatrix);
}

_float CGameInstance::ComputeHeight(_fvector vWorldPos)
{
	if (nullptr == m_pHeight)
		return 0.f;

	return m_pHeight->ComputeHeight(vWorldPos);
}

const _float4x4* CGameInstance::GetShadowTransform(CShadow::D3D_TRANSFORM_STATE eState)
{
	if (nullptr == m_pShadow)
		return nullptr;

	return m_pShadow->GetShadowTransform(eState);
}

void CGameInstance::SetUpShadowTransformMatrix(CShadow::D3D_TRANSFORM_STATE eState, _fmatrix TransformMatrix)
{
	if (nullptr == m_pShadow)
		return;

	m_pShadow->SetUpTransformMatrix(eState, TransformMatrix);
}

HRESULT CGameInstance::AddDebugComponent(CComponent* pDebugCom)
{
	if (nullptr == m_pRenderer)
		return E_FAIL;
	return m_pRenderer->AddDebugComponent(pDebugCom);
}

HRESULT CGameInstance::AddPanelUIToManager(string strName, CUIPanel* pUI)
{
	if (m_pUIManager->GetPanelUI(strName))
		return E_FAIL;

	if (FAILED(m_pUIManager->AddPanelUI(strName, pUI, pUI->GetShowType())))
		return E_FAIL;
	return S_OK;
}

HRESULT CGameInstance::AddElementUIToManager(string strName, CUIElement* pUI)
{
	if (m_pUIManager->GetElementUI(strName))
		return E_FAIL;

	if (FAILED(m_pUIManager->AddElementUI(strName, pUI)))
		return E_FAIL;
	return S_OK;
}

HRESULT CGameInstance::AddElementUIToManagerVertical(string strName, CUIElement* pUI)
{
	m_pUIManager->AddElementVertical(strName, pUI);
	return S_OK;
}

CUIElement* CGameInstance::GetVerticalUI()
{
	return m_pUIManager->GetElementVertical();
}

HRESULT CGameInstance::AddElementToPanel(string strPanelName, string strEleName, _float fSX, _float fSY)
{
	CUIPanel* pPanel = m_pUIManager->GetPanelUI(strPanelName);
	if (!pPanel)
	{
		MSG_BOX("pPanel not exist");
		return E_FAIL;
	}

	CUIElement* pEle = m_pUIManager->GetElementUI(strEleName);
	if (!pEle)
	{
		MSG_BOX("pEle not exist");
		return E_FAIL;
	}

	if (FAILED(pPanel->AddChild(pEle, strEleName, fSX, fSY)))
		return E_FAIL;
	return S_OK;
}

CUIElement* CGameInstance::GetEleUIFromPanel(string strPanelName, string strEleName)
{
	if (!m_pUIManager->GetPanelUI(strPanelName))
		return nullptr;
	return m_pUIManager->GetPanelUI(strPanelName)->GetChildUI(strEleName);
}

CUIElement* CGameInstance::GetEleUI(string strEleName)
{
	return m_pUIManager->GetElementUI(strEleName);
}

CUIPanel* CGameInstance::GetPanelUI(string strPanel)
{
	if (m_pUIManager)
	{
		return m_pUIManager->GetPanelUI(strPanel);
	}
	return nullptr;
}

void CGameInstance::SetDamageFont(_int iDamage, _int iColor, _float fSX, _float fSY)
{
	if (!m_pUIManager)
		return;

	m_pUIManager->SetDamageFont(iDamage, iColor, fSX, fSY);
}

void CGameInstance::AddPlayerUI(CUIParent* pUI)
{
	if (!m_pUIManager)
		return;
	m_pUIManager->AddPlayerUI(pUI);
}

void CGameInstance::RenderOffPlayerUI()
{
	if (nullptr == m_pUIManager)
		return;

	m_pUIManager->RenderOffPlayerUI();
}

void CGameInstance::RenderOnPlayerUI()
{
	if (!m_pUIManager)
		return;
	m_pUIManager->RenderOnPlayerUI();
}

_bool CGameInstance::IsInFrustumWorldSpace(_fvector vWorldPos, _float fRange)
{
	if (nullptr == m_pFrustum)
		return false;
	return m_pFrustum->IsInWorldSpace(vWorldPos, fRange);
}

_bool CGameInstance::IsInFrustumLocalSpace(_fvector vLocalPos, _float fRange)
{
	if (nullptr == m_pFrustum)
		return false;
	return m_pFrustum->IsInLocalSpace(vLocalPos, fRange);
}

void CGameInstance::TransformFrustumToLocalSpace(_fmatrix WorldMatrix)
{
	if (nullptr == m_pFrustum)
		return;

	m_pFrustum->TransformToLocalSpace(WorldMatrix);
}

D3D11_VIEWPORT CGameInstance::GetViewPortDesc()
{
	return m_pPicking->GetViewPortDesc();
}


void CGameInstance::ReleaseEngine()
{
	CGameInstance::GetInstance()->Free();
	CGameInstance::GetInstance()->DestroyInstance();
}


void CGameInstance::PlayEffect(const TCHAR* pSoundKey, float fVolume)
{
	m_pSoundManager->PlayEffect(pSoundKey, fVolume);
}

void CGameInstance::PlayBGM(const TCHAR* pSoundKey, float fVolume)
{
	m_pSoundManager->PlayBGM(pSoundKey, fVolume);
}

void CGameInstance::PlayBGM(const TCHAR* pSoundKey, float fVolume, int iUpdate)
{
	m_pSoundManager->PlayBGM(pSoundKey, fVolume, iUpdate);
}

void CGameInstance::Resume()
{
	m_pSoundManager->Resume();
}

void CGameInstance::Pause()
{
	m_pSoundManager->Pause();
}

void CGameInstance::StopBGM()
{
	m_pSoundManager->StopBGM();
}

void CGameInstance::StopAll()
{
	m_pSoundManager->StopAll();
}

void CGameInstance::SetBGMVolume(float fVolume)
{
	m_pSoundManager->SetBGMVolume(fVolume);
}

void CGameInstance::SetBGMPitch(float fPitch)
{
	m_pSoundManager->SetBGMPitch(fPitch);
}

void CGameInstance::UpdateBGM(const _float& fTimeDelta, const _float& fSpeed)
{
	m_pSoundManager->UpdateBGM(fTimeDelta, fSpeed);
}

void CGameInstance::SetBGM(int iUpdate)
{
	m_pSoundManager->SetBGM(iUpdate);
}

void CGameInstance::StopSound(_uint eID)
{
	m_pSoundManager->StopSound(eID);
}

_uint CGameInstance::GetNowEffectID()
{
	return m_pSoundManager->Get_NowEffectID();
}








void CGameInstance::Free()
{
	__super::Free();

	SafeRelease(m_pRenderer);
	SafeRelease(m_pInputDevice);
	SafeRelease(m_pObjectManager);
	SafeRelease(m_pPrototypeManager);
	SafeRelease(m_pLevelManager);
	SafeRelease(m_pTimerManager);
	SafeRelease(m_pGraphicDevice);
	SafeRelease(m_pPipeLine);
	SafeRelease(m_pLightManager);
	SafeRelease(m_pFontManager);
	SafeRelease(m_pPicking);
	SafeRelease(m_pHeight);
	SafeRelease(m_pShadow);
	SafeRelease(m_pFrustum);
	SafeRelease(m_pSoundManager);
}
