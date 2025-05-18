#pragma once
#include "Base.h"
#include "Model.h"
#include "AsTypes.h"
#include <array>
#include "tinyxml2.h"
#include <filesystem>
#include "Tool_Defines.h"


class CConverter final : public Engine::CBase
{
	DECLARE_SINGLETON(CConverter);

private:
	CConverter();
	virtual ~CConverter() = default;
	
public:
	void		ExportAssetFile(const _char* pModelFilePath, const _char* pModelExportPath, CModel::TYPE eModelType, _fmatrix PreTransformMatrix);

	void		ExportAssetBonesToXML(const _char* pModelFilePath, const _char* pModelExportPath, CModel::TYPE eModelType);


	void		ExportAnimToXML(const _char* pModelFilePath, const _char* pModelExportPath);


private:
	void		ReadBones(const aiNode* pAINode, _int iParentIndex, _fmatrix PreTransformMatrix);
	void		ReadBones(tinyxml2::XMLElement* pParent, const aiNode* pAINode, _int iParentIndex);

	_int		Get_BoneIndex(const _char* pBoneName);

	

private:
	const aiScene*			m_pAIScene = nullptr;
	Assimp::Importer		m_pImporter;

private:
	_float4x4				m_PreTransformMatrix = {};

private:
	wstring		m_ModelPath = L"../Bin/Resources/Assets/Models/";
	wstring		m_TexturePath = L"../Bin/Resources/Assets/Textures/";


private:
	vector<array<_char, MAX_PATH>>		m_Bones;
	vector<AI_BONE_INFO>				m_BoneInfos;


private:
	vector<_int>			m_BonesSizeCheck;
	tinyxml2::XMLDocument*	m_pDocBone = nullptr;
	tinyxml2::XMLElement*	m_pBoneRoot = nullptr;


private:
	tinyxml2::XMLDocument* m_pDocAnim = nullptr;
	tinyxml2::XMLElement* m_pRootAnim = nullptr;
	


private:
	_bool		m_bTrans = false;

private:
	virtual void				Free() override;
};

