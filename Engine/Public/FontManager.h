#pragma once
#include "Base.h"

BEGIN(Engine)

class CFontManager final : public CBase
{
private:
	CFontManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CFontManager() = default;

public:
	HRESULT				AddFont(const _wstring& strFontTag, const _tchar* pFontFilePath);
	HRESULT				RenderFont(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor, _float fRotation = 0.f, const _float2& vOrigin = _float2(0.f, 0.f));

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	map<const _wstring, class CCustomFont*>			m_Fonts;


private:
	class CCustomFont*		FindFont(const _wstring& strFontTag);



public:
	static CFontManager*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void			Free() override;

};

END