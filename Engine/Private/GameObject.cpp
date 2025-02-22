#include "GameObject.h"
#include "GameInstance.h"

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    m_pDevice(pDevice), 
    m_pContext(pContext),
    m_pGameInst(CGameInstance::GetInstance())
{
    SafeAddRef(m_pDevice);
    SafeAddRef(m_pContext);
    SafeAddRef(m_pGameInst);
}

CGameObject::CGameObject(const CGameObject& ProtoType)
    :
    m_pDevice(ProtoType.m_pDevice),
    m_pContext(ProtoType.m_pContext),
    m_pGameInst(ProtoType.m_pGameInst),
    m_strObjectTag(ProtoType.m_strObjectTag)
{
    SafeAddRef(m_pDevice);
    SafeAddRef(m_pContext);
    SafeAddRef(m_pGameInst);
}

HRESULT CGameObject::InitializePrototype()
{
    return S_OK;
}

// 게임 내에서 직접 사용되기 위한 사본 객체가 생성된다.
// 원본 객체는 Loader가 원본들을 로드함.
HRESULT CGameObject::Initialize(void* pArg)
{
    // transform com의 경우 프로토 타입 안 만들고 그냥 바로 생성한다.
   // 원형 초기화 함수 호출인 Initialize_Prototype를 안거치고 Initialize를 거치게 한다.
    m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == m_pTransformCom)
        return E_FAIL;

    m_Components.emplace(TEXT("Com_Transform"), m_pTransformCom);

    // 임시 코드
    if (nullptr != pArg)
    {
        GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);
        m_strObjectTag = pDesc->strObjectTag;

        // SpawnPos 사용한다면 아래 조건문 실행한다.
        if (pDesc->bUseSpawnPos)
        {
            m_vSpawnPos = pDesc->vSpawnPos;
            m_pTransformCom->SetTransformState(CTransform::STATE_POS, XMVectorSetW(XMLoadFloat3(&m_vSpawnPos), 1.f));
        }

        m_iLevelIndex = pDesc->iLevelIndex;
    }

    if (FAILED(m_pTransformCom->Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CGameObject::PriorityUpdate(_float fTimeDelta)
{
    // TODO
}

void CGameObject::Update(_float fTimeDelta)
{
    
}

void CGameObject::LateUpdate(_float fTimeDelta)
{
    // TODO
}

HRESULT CGameObject::Render()
{
    return S_OK;
}

// Add_Comp함수를 통해 원본 컴포넌트 객체를 찾아서 사본을 만들어서 return해주고, 리턴받은 사본객체를 CGameObject 멤버 변수인 map 컨테이너에 추가를 해준다.
// - 이렇게 한 이유는 다른 객체의 컴포넌트를 참조하는 일이 발생할 수 있기 때문이다. 예를 들어 상대방의 위치를 알고싶다거나 하는 경우 CTransformCom에 접근하여 3행의 정보를 얻어와야 하는데 이를 CGameObject나 특정 객체에 컴포넌트를 리턴하는 기능을 만들어 버리면 컴포넌트 개수가 많아질 수록 컴포넌트 객체를 리턴하는 함수도 따라서 증가해야한다.
// - ppOut을 포인터롤 받아주고 있는데 해당 함수의 의도는 매개 변수로 들어온 iPrototypeLevelIdx, strPrototypeTag, strComponentTag로 부터 원형을 찾아 사본을 만들어서 이를 std::map에 저장하여 다른 객체들이 현재 객체의 컴포넌트를 이용하고 싶을 때 빠르고 쉽게 참조를 할 수 있게 하기 위함의 의도를 둔 함수이다.
HRESULT CGameObject::AddComponent(_uint iPrototypeLevelIdx, const _wstring& strPrototypeTag, const _wstring& strComponentTag, CComponent** ppOut, void* pArg)
{
    if (nullptr != GetComponent(strComponentTag))
        return E_FAIL;

    // 이미 CComponent를 상속받는 것을 알기 때문에 static_cast를 한다(다이나믹도 노상관)
    CComponent* pComp = static_cast<CComponent*>(m_pGameInst->ClonePrototype(PROTOTYPE::PROTO_COMPONENT, iPrototypeLevelIdx, strPrototypeTag, pArg));
    if (nullptr == pComp)
        return E_FAIL;

    // 컨테이너에 추가 해준다.
    m_Components.emplace(strComponentTag, pComp);

    // 이중 포인터이기 때문에 *ppOut으로 이중 포인터의 주소에다가 pComp(일중 포인터) 대입.
    *ppOut = pComp;
    
    // 참조하는 카운트가 ppOut에 늘어났으므로 ref cnt증가 시킨다.
    SafeAddRef(pComp);

    return S_OK;
}

CComponent* CGameObject::GetComponent(const _wstring& strCompTag)
{
    auto iter = m_Components.find(strCompTag);

    if (iter != m_Components.end())
        return iter->second;

    return nullptr;
}

_bool CGameObject::IsObjInCam(_uint iGlobal)
{
    if (!m_pGameInst)
        return false;

    _uint iLevel = m_pGameInst->GetCurrentLevelIndex();

    auto pPlayer = m_pGameInst->GetGameObjctInLayerBack(iGlobal, iLevel, TEXT("A"));
    if (!pPlayer)
        return false;

    CTransform* pCom = static_cast<CTransform*>(pPlayer->GetComponent(TEXT("Com_Transform")));
    if (!pCom)
        return false;

    _vector vPlayerPos = pCom->GetTransformState(CTransform::STATE_POS);
    _vector vPlayerLook = XMVector3Normalize(pCom->GetTransformState(CTransform::STATE_LOOK));

    _vector vUIWorldPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);

    _vector vMontoPlayerDir = -XMVector3Normalize(vPlayerPos - vUIWorldPos);
    _float fDot = XMVectorGetX(XMVector3Dot(vPlayerLook, vMontoPlayerDir));

    if (fDot >= 0.f)
        return true;
    else
        return false;
}


void CGameObject::Free()
{
    __super::Free();

    SafeRelease(m_pDevice);
    SafeRelease(m_pContext);
    SafeRelease(m_pGameInst);
    SafeRelease(m_pTransformCom);

    for (auto& Pair : m_Components)
        SafeRelease(Pair.second);

    m_Components.clear();
}
