
#include "EngineShaderDefine.hlsli"

float4x4        g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4        g_ViewMatrixInv, g_ProjMatrixInv;
float4x4        g_LightViewMatrix, g_LightProjMatrix;
Texture2D       g_Texture;

// MRTGameObjects
Texture2D       g_DiffuseTexture;       // Diffuse 기록
Texture2D       g_NormalTexture;        // Normal 기록    , RenderLights 호출시 해당 텍스쳐를 바인딩
Texture2D       g_DepthTexture;         // Depth 기록     , RenderLights 호출시 해당 텍스쳐를 바인딩


// RenderLights호출시(MRTLightAcc) LightManager에 등록된 Light들이 바인딩하는 값들
vector          g_vLightDir;
vector          g_vLightPos;
float           g_fLightRange;
vector          g_vLightDiffuse;
vector          g_vLightAmbient;
vector          g_vLightSpecular;



Texture2D       g_ShadeTexture;
Texture2D       g_SpecularTexture;
Texture2D       g_LightDepthTexture;
Texture2D       g_FinalTexture;
Texture2D       g_BlurXTexture;

Texture2D       g_WorldPosTexture;

Texture2D       g_OuterTexture;
Texture2D       g_OuterPreTexture;


vector          g_vMtrlAmbient = 1.f;
vector          g_vMtrlSpecular = 1.f;

vector          g_vCamPosition;

Texture2D       g_TargetGlow;
Texture2D       g_TargetGlowCopy;
Texture2D       g_TargetGlowCopyBlurX;
Texture2D       g_TargetGlowCopyBlurY;
Texture2D       g_TargetGlowFinal;
Texture2D       g_TargetDistortion;
Texture2D       g_FinalFinalTexture;


// 

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

/* PixelShader */
PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

	return Out;
}

struct PS_OUT_LIGHT
{
    float4 vShade : SV_TARGET0;
    float4 vSpecular : SV_TARGET1;
};

PS_OUT_LIGHT PS_MAIN_LIGHT_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    
        
    // 클라에서 아래와 같이 연산을 해주었다.
    //  Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
    // x 에는 0 ~ 1사이의 NDC가 들어가 있고, y 에는 0~1로 정규화된 ViewZ가 들어가 있다.
    // - 1000을 곱해서 다시 ViewZ값으로 만들어준다.
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 1000.f;
    
    // -1~1 사이로 정규화 한다.
    // 클라에서 0~1사이로 정규화를 했기때문에 노말벡터를 다시 -1~1로 정규화를 해준다.
    // - 왜 클라에서 0~1사이로 정규화를 했나? 보통 fbx파일에 후처리 쉐이딩을 염두해두고 모델러들이 모델의 노말을 0~1로 미리 설정해두는 경우가 있기 때문이다.
    // - 만약 모델러가 이를 염두해두지 않고 그냥 -1~1로 했다면? 그래도 0~1로 클라쪽 쉐이더에서 정규화를 하니 상관없다.
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    
    float vShade = saturate(dot(normalize(g_vLightDir) * -1.f, vNormal)) + (g_vLightAmbient * g_vMtrlAmbient);
    
    Out.vShade = g_vLightDiffuse * saturate(vShade);

    vector vReflect = reflect(normalize(g_vLightDir), vNormal);
    
    
    // 현재 픽셀의 월드 위치가 필요하다.
    // 현재 픽셀 : 화면에 그려지고 있는 Deferred Shading을 요청하는 객체들(지형, 플레이어, 몬스터 등)을 구성하는 픽셀들 -> 직교 투영으로 그리는 사각형 버퍼의 픽셀과 다르다.
    // 뷰포트, 투영 스페이스는 화면에 그려지는 영역이 고정되어 있다.
    // 뷰포트 상 왼쪽 위에 그려지는 픽셀의 Texcoord (0, 0) == 투영 스페이스의 왼쪽 상단의 위치 (-1.f, 1.f)이다.
    // 이 연산을 통해 투영 스페이스 상의 위치를 구한다.
    vector vPosition = 0.f;
    
    // 로컬 정점 위치 * 월드 행렬 * 뷰 행렬 * 투영 행렬 * 1.f / w
    vPosition.x = In.vTexcoord.x * 2.f - 1.f;   // 현재 픽셀을 NDC x로 변경한다.
    vPosition.y = In.vTexcoord.y * -2.f + 1.f;  // 현재 픽셀을 NDC y로 변경한다.
    vPosition.z = vDepthDesc.x;                 // 현재 픽셀의 zNDC 0~1사이의 값을 넣음.
    vPosition.w = 1.f;
    
    // 로컬 정점 위치 * 월드 행렬 * 뷰 행렬 * 투영 행렬
    vPosition *= fViewZ;    // <- 뷰 스페이스 상의 Z값을 다시 곱해서 Z나누기 전의 값으로 변경한다.
    
    // 로컬 정점 위치 * 월드 행렬 * 뷰 행렬
    vPosition = mul(vPosition, g_ProjMatrixInv); // 투영 변환 행렬의 역행렬을 곱하여 뷰스페이스 상의 위치로 변경한다.
    
    // 로컬 정점 위치 * 월드 행렬
    vPosition = mul(vPosition, g_ViewMatrixInv); // 뷰 스페이스 변환 행렬의 역행렬을 곱하여 월드상의 위치로 변경한다.
    
    vector vLook = vPosition - g_vCamPosition;
    
    float fSpecular = pow(saturate(dot(normalize(vLook) * -1.f, normalize(vReflect))), 10.f);
    
    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;

    return Out;
}

PS_OUT_LIGHT PS_MAIN_LIGHT_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 1000.f;

    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	/* 현재 픽셀의 월드위치가 필요하다. */
	/* 현재 픽셀 : 화면에 그려지고 있는 디퍼드 셰이딩을 요하는 객체들(지형, 프렐이어 몬스터)을 구성하는 픽셀들 <-> 직교투영으로 그리느느 사각형 버퍼의 픽셀(x)*/
	/* 뷰포트, 투영스페이스는 화면에 그려지는 영역이 고정되어있다. */
	/* 뷰포트 상 왼쪽 위에그려지는 픽셀의 텍스쿠드 (0, 0) == 투영스페이스 상의 왼쪽위 픽셀의 위치(-1.f, 1.f) */
	/* 이 연산을 통해 투영스페이스 상의 위치를 구하낟.*/

    vector vPosition = 0.f;

	/* 로컬정점위치 * 월드행렬 * 뷰행렬 * 투영행렬 * 1.f / w */
    vPosition.x = In.vTexcoord.x * 2.f - 1.f;
    vPosition.y = In.vTexcoord.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;

	/* 로컬정점위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
    vPosition *= fViewZ;

	/* 로컬정점위치 * 월드행렬 * 뷰행렬 */
    vPosition = mul(vPosition, g_ProjMatrixInv);

	/* 로컬정점위치 * 월드행렬 */
    vPosition = mul(vPosition, g_ViewMatrixInv);

    vector vLightDir = vPosition - g_vLightPos;

    float fDistance = length(vLightDir);

	/* 빛의 감쇄값*/
	/* 픽셀과 빛이 가까우면 1에 가까운값으로 멀면 0에 가까운 값으로 범위를 벗어나면 다 0으로. */
    float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);

    vector vShade = fAtt * (saturate(dot(normalize(vLightDir) * -1.f, vNormal)) + (g_vLightAmbient * g_vMtrlAmbient));

    Out.vShade = g_vLightDiffuse * saturate(vShade);

    vector vReflect = reflect(normalize(vLightDir), vNormal);

    vector vLook = vPosition - g_vCamPosition;


    float fSpecular = fAtt * pow(saturate(dot(normalize(vLook) * -1.f, normalize(vReflect))), 10.f);

    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;

    return Out;
}

// 법선 기반 엣지 검출 함수
float DetectNormalEdges(float2 uv, float2 PixelSize)
{ 
    // 1. 중심 픽셀의 법선 값을 가져온다.
    // 'g_NormalTexture'는 법선 텍스처이며, 'uv'는 현재 픽셀의 텍스처 좌표입니다.
    // 'Sample(LinearSampler, uv)'로 법선을 샘플링하고, 0에서 1 사이의 범위로 나오기 때문에, (어심프에서 로드한 모델의 정점의 노말값은 0~1사이로 먼저 읽기 때문에 변환 화는 과정이다)
    // 이를 -1에서 1 사이의 범위로 변환하기 위해 (2.f * 법선 - 1.f) 계산을 합니다.
    vector vCenterNormal = g_NormalTexture.Sample(LinearSampler, uv) * 2.f - 1.f;
    
    // 2. 현재 픽셀의 상하 좌우 주변 픽셀의 법선 값을 샘플링한다.
    // 중심 픽셀을 기준으로 상하좌우 4방향으로 'PixelSize'만큼 오프셋을 줘서 샘플링합니다.
    // LinearSamplerClamp를 사용하여 클램핑을 적용합니다.
    vector NormalOffsets[4];
    NormalOffsets[0] = g_NormalTexture.Sample(LinearSamplerClamp, uv + float2(-PixelSize.x, 0.f)) * 2.f - 1.f;
    NormalOffsets[1] = g_NormalTexture.Sample(LinearSamplerClamp, uv + float2(PixelSize.x, 0.f)) * 2.f - 1.f;
    NormalOffsets[2] = g_NormalTexture.Sample(LinearSamplerClamp, uv + float2(0.f, -PixelSize.y)) * 2.f - 1.f;
    NormalOffsets[3] = g_NormalTexture.Sample(LinearSamplerClamp, uv + float2(0.f, PixelSize.y)) * 2.f - 1.f;
  
    // 3. 상하좌우 법선 차이로 엣지 강도 계산
    // 각 주변 픽셀의 법선 벡터와 중심 픽셀의 법선 벡터의 차이를 계산하여, 그 길이를 구합니다.
    // 법선 벡터의 차이를 계산할 때는 벡터의 길이를 사용하여 크기를 측정합니다.
    // 'length' 함수는 두 벡터 간의 유클리드 거리(차이의 크기)를 반환합니다. 
    float fEdgeStrength = 0.0f;
    for (int i = 0; i < 4; ++i)
    {
        fEdgeStrength += length(vCenterNormal - NormalOffsets[i]);
    }
    
    // 4. 엣지 강도에 대한 밝기 조정
    // 강도가 클수록 엣지가 더 선명해집니다.
    // 'fEdgeStrength' 값이 커지면 외곽선이 더 뚜렷해지므로 이를 조정하기 위해 2.f를 곱합니다.
    return saturate(fEdgeStrength * 2.f);
}

// 중심 픽셀과 그 주변 픽셀의 깊이 값 차이를 계산하여 엣지 강도를 측정하고, 이를 바탕으로 2D 깊이 맵에서 외곽선 효과를 구현하는 데 사용
float DetectDepthEdges(float2 uv, float2 PixelSize)
{ 
    // 1. 중심 픽셀의 깊이 값을 가져온다.
    // 'g_DepthTexture'깊이 텍스텨(깊이 버퍼)이며, 'uv'는 해당 텍스쳐 내의 현재 픽셀의 좌표이다.
    // Sample(LinearSampler, uv)'로 깊이를 샘플링하고, 'y' 값만 사용하여 깊이 값을 얻는다.
    // 이후 깊이를 1000배하여 실제 뷰 공간의 Z 값을 얻는다.
    vector vCenterDepth = g_DepthTexture.Sample(LinearSampler, uv);
    float fViewZ = vCenterDepth.y * 1000.f;
    
    // 2. 현재 픽셀의 상하 좌우 주변 픽셀의 깊이 값을 샘플링한다.
    // 상하좌우 4방향에 대한 깊이를 각각 샘플링하여 배열에 저장.
    // 각 방향으로 'PixelSize'만큼 오프셋을 주어서 샘플링한다.
    // 샘플링할 때 LinearSamplerClamp를 사용하여 클램핑을 적용하고, 깊이를 1000배하여 실제 뷰 공간의 Z 값을 얻는다.
    float DepthOffsets[4];
    DepthOffsets[0] = g_DepthTexture.Sample(LinearSamplerClamp, uv + float2(-PixelSize.x, 0.f)).y * 1000.f;
    DepthOffsets[1] = g_DepthTexture.Sample(LinearSamplerClamp, uv + float2(PixelSize.x, 0.f)).y * 1000.f;
    DepthOffsets[2] = g_DepthTexture.Sample(LinearSamplerClamp, uv + float2(0.f, -PixelSize.y)).y * 1000.f;
    DepthOffsets[3] = g_DepthTexture.Sample(LinearSamplerClamp, uv + float2(0.f, PixelSize.y)).y * 1000.f;
  
    // 3. 상하좌우 깊이 변화로 엣지 강도를 계산한다.
    // 주변 픽셀의 깊이 값들과 현재 픽셀의 깊이를 비교하여 엣지 강도를 계산한다.
    // 'fViewZ'는 현재 픽셀의 깊이 값이고, 'DepthOffsets[i]'는 주변 픽셀의 깊이 값이다.
    // 두 깊이 값의 차이를 절댓값으로 구하여, 엣지 강도를 누적한다.
    float fEdgeStrength = 0.0f;
    for (int i = 0; i < 4; ++i)
    {
        fEdgeStrength += abs(fViewZ - DepthOffsets[i]);
    }
    
    // 4. 엣지 강도에 대한 밝기 조정 (강도가 클수록 검은 외곽선이 더 뚜렷해짐)
    // 강도가 큰 곳일수록 외곽선이 뚜렷하게 나타나도록 하기 위해, 'fIntensity'를 곱한다.
    // 이 값은 외곽선의 강도를 조정하는 파라미터로, 높을수록 외곽선이 더 선명하게 보인다.
    float fIntensity = 15.f;
    
    
    // 5. 최종 결과 값 반환
    // 'saturate' 함수는 값이 0과 1 사이로 클램핑되도록 하여, 결과값이 그 범위를 벗어나지 않도록 한다.
    return saturate(fEdgeStrength * fIntensity);
}

float g_fWeights[13] =
{
    0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 1.f, 0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561
};

// 수업 코드의 PS_MAIN_FINAL과 동일
PS_OUT PS_MAIN_SHADE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    
    vector vDistortionColor = g_TargetDistortion.Sample(LinearSampler, In.vTexcoord);
    vector vDiffuse;
    
    if (vDistortionColor.r > 0.05f && vDistortionColor.g > 0.05f && vDistortionColor.b > 0.05f)
    {
         // TODO?
        // g_NormalTexture
        // 노이즈 값 스케일링
        float fDisplacementAmount = 3.f;
        float fDisplacement = vDistortionColor.r * fDisplacementAmount;
    
        float3 vNormal = g_NormalTexture.Sample(LinearSampler, In.vTexcoord) * 2.f - 1.f;
        
        // 왜곡된 텍스쳐 좌표 계산
        float2 vDistortedTexcoord = In.vTexcoord + vNormal.xy * fDisplacement;
        
        // 텍스쳐 좌표를 랩핑 
        vDistortedTexcoord = frac(vDistortedTexcoord); // 좌표를 0~1반복
        
        // 왜곡된 텍스쳐 좌표로 디퓨즈 텍스쳐 샘플링
        vDiffuse = g_DiffuseTexture.Sample(LinearSampler, vDistortedTexcoord);
    }
    else
    {
        vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    }
    
    if (0.f == vDiffuse.a)
        discard;

    vector vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);
    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor = vDiffuse * vShade + vSpecular;
    
    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexcoord);
    
    // 1000을 왜 곱하나? 
    // 클라 Shader_VtxAnimMesh에서 아래처럼 계산하고 있다.
    // Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
    // In.vProjPos.w == ClipSpace.w == ViewZ이다.
    float fViewZ = vDepthDesc.y * 1000.f;
    
    vector vPos = 0.f;
    
    // NDC로 만든다.
    vPos.x = In.vTexcoord.x * 2.f - 1.f;
    vPos.y = In.vTexcoord.y * -2.f + 1.f;
    vPos.z = vDepthDesc.x;
    vPos.w = 1.f;
    
    // w나누기 했던것에 다시 w를 곱한다. NCD -> Clip
    vPos *= fViewZ;
    
    // 투영의 역 -> 뷰의 역 -> world로 만든다.
    vPos = mul(vPos, g_ProjMatrixInv);
    vPos = mul(vPos, g_ViewMatrixInv);
    
    // 광원 카메라 기준의 뷰, 투영을 곱하여 ClipSpace 상에 위치하게 한다.
    vPos = mul(vPos, g_LightViewMatrix);
    vPos = mul(vPos, g_LightProjMatrix);
    
    // 광원 기준으로의 ClipSpace상에 있는 vPos를 w값(ViewZ)으로 나누어 NDC로 만들고 NDC좌표를 뷰포트 좌표로 매핑한다.
    float2 vTex = 0.f;
    vTex.x = (vPos.x / vPos.w) * 0.5f + 0.5f;
    vTex.y = (vPos.y / vPos.w) * -0.5f + 0.5f;
    
    float4 vLightDepth = g_LightDepthTexture.Sample(LinearSampler, vTex);
    
    // NDC상의 비교는 하지 않고 실제 ViewZ값으로 비교를 한다.
    // -1? 야매
    if ((vLightDepth.x * 1000.f) < vPos.w - 1.f)
    {
        Out.vColor = vector(Out.vColor.rgb * 0.5f, 1.f);
    }
    
    
    // 툰 쉐이딩 적용 코드
    // 중심 픽셀과 주변 픽셀의 깊이 값차이가 0.9보다 크고, 중심 픽셀의 노말 벡터와 주변 픽셀의 노말 벡터의 차이가 0.5보다 크거나 같으면 현재 픽셀(중심 픽셀)의 색을 검은색으로 칠한다.
    if (0.9f <= DetectDepthEdges(In.vTexcoord, float2((1.f / 1280.f), (1.f / 720.f))) && 0.5f <= DetectNormalEdges(In.vTexcoord, float2((1.f / 1280.f), (1.f / 720.f))))
    {
        Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    }
    
    vector vOuterColor = g_OuterTexture.Sample(LinearSampler, In.vTexcoord);
    if (vOuterColor.r > 0.1f)
    {
        Out.vColor = vOuterColor;
    }
    else
    {
        //vector vFinal = g_FinalFinalTexture.Sample(LinearSampler, In.vTexcoord);
        //if (vFinal.b < 0.01f && vFinal.r < 0.01f && vFinal.g < 0.01f)
        //{
        //    Out.vColor = vFinal;
        //}
        //else
        //{
        //    // 최종 Glow 값을 추가
        //    vector vGlow = g_TargetGlowFinal.Sample(LinearSampler, In.vTexcoord); // Glow 텍스처 샘플링
        //    Out.vColor += vGlow * 2.f; // Glow를 기존 색상에 추가
        //}
        
        vector vGlow = g_TargetGlowFinal.Sample(LinearSampler, In.vTexcoord); // Glow 텍스처 샘플링
        Out.vColor += vGlow * 2.f; // Glow를 기존 색상에 추가
    }
    
    return Out;
}


struct PS_OUT_BLUR
{
    vector vColor : SV_TARGET0;
};

PS_OUT_BLUR PS_MAIN_BLUR_X(PS_IN In)
{
    PS_OUT_BLUR Out = (PS_OUT_BLUR) 0;

    float2 vTex = 0.f;
    
    for (int i = -6; i < 7; i++)
    {
        vTex = float2(In.vTexcoord.x + (1.f / 1280.f) * i, In.vTexcoord.y);
        Out.vColor += g_fWeights[i + 6] * g_FinalTexture.Sample(LinearSamplerClamp, vTex);
    }
    
    Out.vColor /= 6.f;
    return Out;
}

PS_OUT_BLUR PS_MAIN_BLUR_Y(PS_IN In)
{
    PS_OUT_BLUR Out = (PS_OUT_BLUR) 0;

    float2 vTex = 0.f;
    vector vBlur = 0.f;
    for (int i = -6; i < 7; i++)
    {
        vTex = float2(In.vTexcoord.x, In.vTexcoord.y + (1.f / 720.f) * i);
        vBlur += g_fWeights[i + 6] * g_BlurXTexture.Sample(LinearSamplerClamp, vTex);
    }
    
    vBlur /= 6.f;
    vector vFinalColor = g_FinalTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor = vFinalColor + vBlur;
    return Out;
}

struct PS_OUT_OUTER
{
    float4 vColor : SV_TARGET0;
};

PS_OUT_OUTER PS_MAIN_OUTER(PS_IN In)
{
    PS_OUT_OUTER Out = (PS_OUT_OUTER) 0;
    float4 vOuterPreColor = g_OuterPreTexture.Sample(LinearSampler, In.vTexcoord); // 초록색
    
    // 해상도 기반 UV 오프셋 (1280x720 예시)
    float2 UVOffset = float2(1.0f / 1280.0f, 1.0f / 720.0f);
    
    // Sobel 필터로 경계 검출
    float4 left = g_OuterPreTexture.Sample(LinearSampler, In.vTexcoord - float2(UVOffset.x, 0.0f));
    float4 right = g_OuterPreTexture.Sample(LinearSampler, In.vTexcoord + float2(UVOffset.x, 0.0f));
    float4 up = g_OuterPreTexture.Sample(LinearSampler, In.vTexcoord - float2(0.0f, UVOffset.y));
    float4 down = g_OuterPreTexture.Sample(LinearSampler, In.vTexcoord + float2(0.0f, UVOffset.y));
    
    // Sobel 연산 계산 (경계 강도)
    float gx = abs(right.g - left.g); // X축 차이
    float gy = abs(down.g - up.g); // Y축 차이
    float edgeStrength = sqrt(gx * gx + gy * gy); // 경계 강도 계산

    // 경계선 기준 값 설정
    float edgeThreshold = 0.1f; // 경계선 강도 기준

    // 검은색(배경)과 초록색(몬스터) 간 경계를 계산
    bool isBackground = (vOuterPreColor.r + vOuterPreColor.g + vOuterPreColor.b) < 0.1f;
    bool isGreen = vOuterPreColor.g > 0.9f && vOuterPreColor.r < 0.5f && vOuterPreColor.b < 0.5f;

    // 경계선 픽셀 결정
    if (isGreen)
    {
        if (edgeStrength > edgeThreshold)
        {
            Out.vColor = float4(1.0f, 0.0f, 0.0f, 1.0f); // 경계선은 빨간색
        }
        else
        {
            Out.vColor = float4(0.0f, 0.0f, 0.0f, 1.0f); // 경계가 아닌 초록색 내부는 검은색
            // Out.vColor = blurredColor; // 경계가 아닌 초록색 내부는 Blur 적용
        }
    }
    else
    {
        discard; // 배경은 그대로 검은색 유지
    }
    return Out;
}

// 다운 샘플링 하기 전, Glow를 그린 렌더링 타겟
PS_OUT PS_MAIN_GLOW(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    if (Out.vColor.a < 0.1f)
        discard;

    return Out;
}


// g_TargetGlow에 그린 횟불이나 이펙트의 특정 알파값은 폐기하고 픽셀들을 그대로 반환한다.
PS_OUT PS_MAIN_GLOW_COPY(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_TargetGlow.Sample(LinearSampler, In.vTexcoord);
    if (Out.vColor.a < 0.1f)
        discard;

    return Out;
}


//PS_OUT PS_MAIN_GLOW_COPY_BLUR_X(PS_IN In)
//{
//    PS_OUT Out;

//    // 가우시안 커널 (5 샘플, 가중치)
//    float weights[5] = { 0.06136f, 0.24477f, 0.38774f, 0.24477f, 0.06136f }; // 가우시안 분포

//    // 텍셀 크기 (X축 방향으로 1픽셀 크기 계산)
//    float2 texelSize = float2(1.0f / (1280.f * 0.5f), 0.0f);

//    // 초기화
//    Out.vColor = float4(0, 0, 0, 0);

//    // X축 블러: -2, -1, 0, +1, +2 위치 샘플링
//    for (int i = -2; i <= 2; ++i)
//    {
//        // 텍스처 좌표 계산 (X축 이동)
//        float2 sampleCoord = In.vTexcoord + float2(float(i), 0) * texelSize;

//        // 텍스처 샘플링
//        float4 sampledColor = g_TargetGlowCopy.Sample(LinearSampler, sampleCoord);

//        // 가중치 곱하여 누적
//        // abs(i) 대칭 가중치 사용 2 1 0 1 2
//        Out.vColor += sampledColor * weights[abs(i)];
//    }

//    // 알파 값이 너무 낮으면 픽셀 폐기
//    if (Out.vColor.a < 0.1f)
//        discard;

//    return Out;
//}


// 샘플링 오프셋:
//블러는 중심 픽셀 주변의 여러 픽셀을 샘플링하고, 이 값을 가중치(weight)를 곱하여 평균을 내는 방식으로 이루어집니다.
//X축 블러는 텍스처의 좌우 방향으로 여러 픽셀을 샘플링합니다.
//가우시안 분포:
// - 가우시안 분포 기반의 커널을 사용하여 가중치를 계산합니다. 중심에 가까울수록 높은 가중치를 부여하고, 멀어질수록 낮은 가중치를 부여합니다.
//결과 합산:
// - 샘플링된 값에 가중치를 곱하고 합산한 뒤, 총 가중치로 나누어 평균 값을 계산
PS_OUT PS_MAIN_GLOW_COPY_BLUR_X(PS_IN In)
{
    PS_OUT Out;
    
    // 가우시안 커널 (5 샘플, 가중치)
    float weights[5] = { 0.06136f, 0.24477f, 0.38774f, 0.24477f, 0.06136f }; // 가우시안 분포

    // 텍셀 크기 (X축 방향으로 1픽셀 크기 계산)
    float2 texelSize = float2(1.0f / (1280.f * 0.5f), 0.0f);

    // 초기화
    Out.vColor = float4(0, 0, 0, 0);

    // X축 블러: -2, -1, 0, +1, +2 위치 샘플링
    for (int i = -2; i <= 2; ++i)
    {
        // 텍스처 좌표 계산 (X축 이동)
        float2 sampleCoord = In.vTexcoord + float2(float(i), 0) * texelSize;

        // 텍스처 샘플링
        float4 sampledColor = g_TargetGlowCopy.Sample(LinearSampler, sampleCoord);

        // 가중치 곱하여 누적
        // abs(i) 대칭 가중치 사용 2 1 0 1 2
        Out.vColor += sampledColor * weights[abs(i)];
    }

    // 알파 값이 너무 낮으면 픽셀 폐기
    if (Out.vColor.a < 0.1f)
        discard;

    return Out;
}


PS_OUT PS_MAIN_GLOW_COPY_BLUR_Y(PS_IN In)
{
    PS_OUT Out;
    
    // 가우시안 커널 (5 샘플, 가중치)
    float weights[5] = { 0.06136f, 0.24477f, 0.38774f, 0.24477f, 0.06136f }; // 가우시안 분포

    // 텍셀 크기 (Y축 방향으로 1픽셀 크기 계산)    
    float2 texelSize = float2(0.0f, 1.0f / (720.f * 0.5f)); // Y축 크기 설정 (다운샘플링된 높이 기준)

    // 초기화
    Out.vColor = float4(0, 0, 0, 0);

    // X축 블러: -2, -1, 0, +1, +2 위치 샘플링
    for (int i = -2; i <= 2; ++i)
    {
        // 텍스처 좌표 계산 (X축 이동)
        float2 sampleCoord = In.vTexcoord + float2(0.f, float(i)) * texelSize;

        // 텍스처 샘플링
        float4 sampledColor = g_TargetGlowCopyBlurX.Sample(LinearSampler, sampleCoord);

        // 가중치 곱하여 누적
        // abs(i) 대칭 가중치 사용 2 1 0 1 2
        Out.vColor += sampledColor * weights[abs(i)];
    }
    
    // 알파 값이 너무 낮으면 픽셀 폐기
    if (Out.vColor.a < 0.1f)
        discard;

    return Out;
}

// 최종 합성
// PS_MAIN_GLOW_FINAL
PS_OUT PS_MAIN_GLOW_FINAL(PS_IN In)
{
    PS_OUT Out;
    
    Out.vColor = g_TargetGlowCopyBlurY.Sample(LinearSampler, In.vTexcoord);
    
    // 알파 값이 너무 낮으면 픽셀 폐기
    if (Out.vColor.a < 0.1f)
        discard;
    
    return Out;
}


technique11			DefaultTechnique
{
	/* 우리가 수행해야할 정점, 픽셀 셰이더의 진입점 함수를 지정한다. */

// 0
	pass Debug
	{
		SetRasterizerState(RSDefault);
		SetDepthStencilState(DSSDefault, 0);
		SetBlendState(BSDefault, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
	}


// 1
    pass LightDirectional
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDisable, 0);
        SetBlendState(BSOneBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_DIRECTIONAL();
    }
	

// 2
    pass LightPoint
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDisable, 0);
        SetBlendState(BSOneBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_POINT();
    }
    
// RenderShade(Renderfinal)에서 사용하는 pass이다.
// g_DiffuseTexture, g_ShadeTexture, g_SpecularTexture, g_LightDepthTexture, g_DepthTexture를 바인딩 한 상태이다.
    pass Shade
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDisable, 0);
        SetBlendState(BSDefault, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHADE();
    }
    

// 4
    pass BlurX
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDisable, 0);
        SetBlendState(BSDefault, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_X();
    }

// 5
    pass BlurY
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDisable, 0);
        SetBlendState(BSDefault, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_Y();
    }

// 6
    pass Outer
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDisable, 0);
        SetBlendState(BSDefault, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_OUTER();
    }


// 7
    pass Glow
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDisable, 0);
        SetBlendState(BSDefault, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_GLOW();
    }
    
// 8
    pass GlowCopy
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDisable, 0);
        SetBlendState(BSDefault, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_GLOW_COPY();
    }


// 9
    pass GlowCopyBlurX
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDisable, 0);
        SetBlendState(BSDefault, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_GLOW_COPY_BLUR_X();
    }

// 10
    pass GlowCopyBlurY
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDisable, 0);
        SetBlendState(BSDefault, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_GLOW_COPY_BLUR_Y();
    }

// 11
    pass GlowFinal
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDisable, 0);
        SetBlendState(BSDefault, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_GLOW_FINAL();
    }
}
