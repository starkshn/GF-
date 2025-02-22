#include "Shader.h"

CShader::CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CComponent(pDevice, pContext)
{
}

CShader::CShader(const CShader& Prototype)
    :
    CComponent(Prototype),
    m_pEffect(Prototype.m_pEffect),
    m_InputLayouts(Prototype.m_InputLayouts),
    m_iNumLayouts(Prototype.m_iNumLayouts)
{
    for (auto& pInputLayout : m_InputLayouts)
        SafeAddRef(pInputLayout);

    SafeAddRef(m_pEffect);
}

HRESULT CShader::InitializePrototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
    _uint iHLSLFlag = 0;
    
#ifdef _DEBUG
    iHLSLFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    iHLSLFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

    ID3DBlob* pBlob = nullptr;

    // D3DX11CompileEffectFromFile로 pshaerFilePath에 해당하는 쉐이더 프로그램 컴파일 진행한다.
    // 쉐이더를 컴파일할 때, ID3DX11Effect 라는 객체를 만들어야한다.(Effects11.lib 외부 정적 라이브러리로 컴파일 한다)
    // ID3DX11Effect객체는 쉐이더 객체를 의미한다
    // - (부연설명하자면 Shader를 컴파일해서 얻은 바이트 코드 스트림을 의미하는듯 하다.)
    // HLSL 파일에 정의된 technique11과 pass를 포함한 쉐이더 코드가 컴파일 된다.
    // 이때, 파일에서 컴파일된 Effect객체(m_pEffect)가 생성된다.
    if (FAILED(D3DX11CompileEffectFromFile(pShaderFilePath, 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHLSLFlag, 0, m_pDevice, &m_pEffect, &pBlob)))
        return E_FAIL;

    // 컴파일 된 쉐이더 코드에서 특정 technique를 가져온다. (현재 1개 밖에 없으니까 아마 0을 넣어준듯 하다)
    ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
    if (!pTechnique)
        return E_FAIL;

    // CreateInputLayout함수의 세번째 인자를 보면 const void *pShaderBytecodeWithInputSignature 를 받아주고 있다.
    // 뜻 그대로 해석해 보면 바이트 코드(쉐이더 프로그램을 컴파일 해서 얻은)의 입력 시그니처 라고볼 수 있는듯하다.
    // 수업에서는 이해하기 쉽게하기 위해 '어떤 형태의 정점을 받아주고 있는데요?'를 요구한다고 한다.
    // 이 입력 시그니처는 쉐이더 객체(Effect객체)의 technique로 부터 어떤 pass로 부터 어떤 정점을 받는 다는 식으로 정의한다.
    // 그래서 technique안에 pass가 몇개 있는지 알기위해서 아래 technique 디스크립션을 가져와서 Passes로 pass 개수를 가져온다.
    D3DX11_TECHNIQUE_DESC TechniqueDesc{};
    pTechnique->GetDesc(&TechniqueDesc);
    m_iNumLayouts = TechniqueDesc.Passes; // technique에 있는 pass의 개수를 가져옴

    // m_iNumLayouts는 technique안에 pass개수라 봐도 된다.
    for (_uint i = 0; i < m_iNumLayouts; ++i)
    {
        // i번째에 해당하는 pass로 부터 IAInputSignatureSize를 얻어와 바이트 코드 사이즈를 4번재 인자에 넣어주어 InputLayout을 만들어 낸다.
        ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(i);
        if (!pPass)
            return E_FAIL;

        D3DX11_PASS_DESC PassDesc = {};
        pPass->GetDesc(&PassDesc);

        ID3D11InputLayout* pInputLayout = nullptr;
        
        if (FAILED(m_pDevice->CreateInputLayout(pElements, iNumElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout)))
            return E_FAIL;

        m_InputLayouts.push_back(pInputLayout);
    }
    
    return S_OK;
}

HRESULT CShader::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CShader::Begin(_uint iPassIdx)
{
    // if (iPassIdx >= m_iNumLayouts)
    //     return E_FAIL;

    ID3DX11EffectPass* pPass = m_pEffect->GetTechniqueByIndex(0)->GetPassByIndex(iPassIdx);

    // 어떤 Pass를 사용할 것인지 적어주어야 한다. (어떤 쉐이더 함수를 호출할 것인지)
    // GPU 메모리에 저장된 상수 버퍼를 새 변수 값들로 갱신하고, 패스에 설정된 쉐이더 프로그램들을 파이프 라인에 묶고, 패스에 설정된 렌더 상태들을 적용한다.
    pPass->Apply(0, m_pContext);

    // 버퍼를 Draw하기 전에 반드시 호출되어야 하는 함수이다.
    m_pContext->IASetInputLayout(m_InputLayouts[iPassIdx]);

    return S_OK;
}

HRESULT CShader::BindRawValue(const _char* pConstantName, const void* pData, _uint iLenth)
{
    ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
    if (nullptr == pVariable)
        return E_FAIL;

    return pVariable->SetRawValue(pData, 0, iLenth);
}

// Bind_Matrix의 pMatrix를 ref가 아니라 pointer로 받는 이유는 레퍼런스를 const pointer타입으로 형변환을 못하기 때문이다.
HRESULT CShader::BindMatrix(const _char* pConstantName, const _float4x4* pMatrix)
{
    // Shader에 선언된 전역변수의 HANDLE을 가져온다.
    // - ID3DX11EffectVariable도 컴객체.
    ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
    if (nullptr == pVariable)
        return E_FAIL;

    // 얻어온 변수가 실제 어떤 타입인지 AsMatrix로 확인하고 캐스팅한다.
    // Matrix 타입이 아니라면 nullptr을 반환한다.
    ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
    if (nullptr == pMatrixVariable)
        return E_FAIL;

    return pMatrixVariable->SetMatrix(reinterpret_cast<const _float*>(pMatrix));
}

HRESULT CShader::BindSRV(const _char* pConstantName, ID3D11ShaderResourceView* pSRV)
{
    ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
    if (!pVariable)
        return E_FAIL;

    ID3DX11EffectShaderResourceVariable* pSRVVariable = pVariable->AsShaderResource();
    if (!pSRVVariable)
        return E_FAIL;

    return pSRVVariable->SetResource(pSRV);
}

HRESULT CShader::BindSRVs(const _char* pConstantName, ID3D11ShaderResourceView** ppSRV, _uint iNumSRVs)
{
    ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
    if (nullptr == pVariable) 
        return E_FAIL;

    ID3DX11EffectShaderResourceVariable* pSRVVariable = pVariable->AsShaderResource();
    if (nullptr == pSRVVariable)
        return E_FAIL;

    return pSRVVariable->SetResourceArray(ppSRV, 0, iNumSRVs);
}

HRESULT CShader::BindMatrices(const _char* pConstantName, const _float4x4* pMatrix, _uint iNumMatrices)
{
    ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
    if (nullptr == pVariable)
        return E_FAIL;

    ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
    if (nullptr == pMatrixVariable)
        return E_FAIL;

    return pMatrixVariable->SetMatrixArray(reinterpret_cast<const _float*>(pMatrix), 0, iNumMatrices);
}

HRESULT CShader::Bind_Val(const _char* pConstantName, const _float fVal)
{
    ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
    if (!pVariable)
        return E_FAIL;

    ID3DX11EffectScalarVariable* pScalarVariable = pVariable->AsScalar();
    if (nullptr == pScalarVariable)
        return E_FAIL;

    return pScalarVariable->SetFloat(fVal);
}

HRESULT CShader::Bind_Val_Test(const _char* pConstantName, _fvector vDir)
{
    ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
    if (!pVariable)
        return E_FAIL;

    ID3DX11EffectVectorVariable* pScalarVariable = pVariable->AsVector();
    if (nullptr == pScalarVariable)
        return E_FAIL;

    _float3 float3Val;
    XMStoreFloat3(&float3Val, vDir);
    
    return pScalarVariable->SetFloatVector(reinterpret_cast<const float*>(&float3Val));
}


CShader* CShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
    CShader* pInstance = new CShader(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype(pShaderFilePath, pElements, iNumElements)))
    {
        MSG_BOX("Failed to Created : CShader");
        SafeRelease(pInstance);
    }
    return pInstance;
}

CComponent* CShader::Clone(void* pArg)
{
    CShader* pInstance = new CShader(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CShader");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CShader::Free()
{
    __super::Free();

    for (auto& pInputLayout : m_InputLayouts)
        SafeRelease(pInputLayout);

    m_InputLayouts.clear();

    SafeRelease(m_pEffect);
}
