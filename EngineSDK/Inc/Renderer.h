#pragma once
#include "Base.h"

BEGIN(Engine)

// forward declarations...
class CGameObject;
class CGameInstance;
class CShader;
class CVIBufferRect;

class CRenderer final : public CBase
{
public:
	enum RENDER_GROUP { RG_PRIORITY, RG_HEIGHT, RG_SHADOW, RG_NONBLEND, RG_NONPICKING, RG_NONLIGHT, RG_BLEND, RG_UI, RG_GLOW, RG_OUTER, RG_DISTORTION, RG_END, };
	
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() override = default;
	
public:
	HRESULT					Initialize();
	HRESULT					AddRenderObject(RENDER_GROUP eRenderGroup, CGameObject* pRenderObject);
	HRESULT					DrawRenderObject();


#ifdef _DEBUG

public:
	HRESULT AddDebugComponent(class CComponent* pDebugCom) 
	{
		m_DebugComponents.push_back(pDebugCom);
		SafeAddRef(pDebugCom);
		return S_OK;
	}

#endif

public:
	void					AdjustBlur() { m_bAdjustBlur = true; }

private:
	HRESULT					RenderPriority();
	HRESULT					RenderHeight();
	HRESULT					RenderShadow();
	HRESULT					RenderNonBlend();

	// 빛 연산을 수행한다. 결과를 TargetShade에다가 렌더링 해 둔다.
	HRESULT					RenderLights();
	HRESULT					RenderShade();

	HRESULT					RenderBlurX();
	HRESULT					RenderBlurY();

	HRESULT					RenderOuter();

	HRESULT					RenderGlow();
	HRESULT					RenderGlowCopy();
	HRESULT					RenderGlowBlurX();
	HRESULT					RenderGlowBlurY();
	HRESULT					RenderGlowFinal();

	HRESULT					RenderDistortion();


	HRESULT					RenderNonLight();
	HRESULT					RenderBlend();
	HRESULT					RenderUI();
	
private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;
	CGameInstance*			m_pGameInst = nullptr;

private:
	CShader*				m_pShader = nullptr;
	CVIBufferRect*			m_pVIBuffer = nullptr;

private:
	list< CGameObject*>		m_RenderObjects[RG_END];
	_float4x4				m_WorldMatrix = {};
	_float4x4				m_ViewMatrix = {};
	_float4x4				m_ProjMatrix = {};
	_uint					m_iOriginalViewPortWidth;
	_uint					m_iOriginalViewPortHeight;
	ID3D11DepthStencilView* m_pMaxDepthStencilView = nullptr;
	_bool					m_bAdjustBlur = false;

	ID3D11DepthStencilView* m_pGlowDepthStencilView = nullptr;



#ifdef _DEBUG

private:
	list<CComponent*>		m_DebugComponents;
	HRESULT					RenderRTVDebug();

#endif 

private:
	HRESULT					ReadyDepthStencilView(_uint iWidth, _uint iHeight);
	HRESULT					ReadyDepthStencilView(_uint iWidth, _uint iHeight, ID3D11DepthStencilView* pDSV);
	HRESULT					SetUpViewPort(_uint iWidth, _uint iHeight);

	
public:
	static	CRenderer*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void			Free() override;
};

END