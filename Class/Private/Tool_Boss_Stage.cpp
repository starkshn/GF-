#include "Tool_Boss_Stage.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Transform.h"
#include "Model.h"

CTool_Boss_Stage::CTool_Boss_Stage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CGameObject(pDevice, pContext)
{
}

CTool_Boss_Stage::CTool_Boss_Stage(const CTool_Boss_Stage& Prototype)
    :
    CGameObject(Prototype)
{
}

HRESULT CTool_Boss_Stage::InitializePrototype()
{
    return S_OK;
}

HRESULT CTool_Boss_Stage::Initialize(void* pArg)
{
    if (__super::Initialize(pArg))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;


    return S_OK;
}

void CTool_Boss_Stage::PriorityUpdate(_float fTimeDelta)
{
}

void CTool_Boss_Stage::Update(_float fTimeDelta)
{
}

void CTool_Boss_Stage::LateUpdate(_float fTimeDelta)
{
    m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CTool_Boss_Stage::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->GetNumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        // i 번째에 해당하는 텍스쳐를 Bind 해준다.
        if (FAILED(m_pModelCom->BindMaterial(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
            return E_FAIL;

        // 0 default, 1 wireframe
        m_pShaderCom->Begin(0);

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CTool_Boss_Stage::Ready_Components()
{
    // Com_Shader
    {
        if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_00_Shader_VtxMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
            return E_FAIL;
    }

    // Com_Model
    {
        if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Boss_Stage"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
            return E_FAIL;
    }


    return S_OK;
}

HRESULT CTool_Boss_Stage::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->BindShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    _float4x4 viewMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_VIEW);
    _float4x4 projMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_PROJ);

    if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &viewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &projMatrix)))
        return E_FAIL;

    const LIGHT_DESC* pLightDesc = m_pGameInst->GetLightDesc(0);

    if (FAILED(m_pShaderCom->BindRawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->BindRawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->BindRawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->BindRawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindRawValue("g_vCamPosition", m_pGameInst->GetCameraPosition(), sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

CTool_Boss_Stage* CTool_Boss_Stage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTool_Boss_Stage* pInst = new CTool_Boss_Stage(pDevice, pContext);

    if (FAILED(pInst->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CTool_Boss_Stage");
        SafeRelease(pInst);
    }

    return pInst;
}

CGameObject* CTool_Boss_Stage::Clone(void* pArg)
{
    CTool_Boss_Stage* pInst = new CTool_Boss_Stage(*this);

    if (FAILED(pInst->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CTool_Boss_Stage");
        SafeRelease(pInst);
    }

    return pInst;
}

void CTool_Boss_Stage::Free()
{
    __super::Free();

    SafeRelease(m_pShaderCom);
    SafeRelease(m_pModelCom);
}
