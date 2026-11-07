// SCRATCH coherent Init cohort, zero new active credit pending root/type/COFF gates.
// Existing six pilot bodies762B plus seven candidate bodies3354B =4116B nominal.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// Full HTTP/DNS/session/default-tracker/context/event/IO declarations required.
// No constructor stand-ins, opaque tails, aliases or new helper markers.
// No whole-TU runtime authority. This script does not compile or promote source.
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyModel.h"
#include "ee/wonapi/WONLobbyPersistence.h"
#include "ee/wonapi/WONLobbyCore.h"
#include "ee/wonapi/WONLobbyEvents.h"
#include "ee/wonapi/WONLobbyAccountLogic.h"
#include "LostDebug.h"
using namespace WONAPI;
// Timeout for the firewall detect and accept operations, in milliseconds.
unsigned long WONAPI::gLobbyTimeout4E0EBC = 20000;

// The three controls the login screen disables while it initialises.
struct LoginControlIds : public ControlIdSet {
    void Add(value_type inA,value_type inB,value_type inC) { insert(inA); insert(inB); insert(inC); }
};

namespace WONAPI {
// FUNCTION: WON 0x7dd100
InitLogic::InitLogic()
{
    mHTTPSession = new HTTPSession;
}
}

namespace WONAPI {
// FUNCTION: WON 0x7dd200
InitLogic::~InitLogic() {}
}

namespace WONAPI {
// FUNCTION: WON 0x7dd340
void InitLogic::InitComponent(ComponentConfig* inConfig)
{
    SafeGetComponent(inConfig, mNetworkAdapterDialog, "ChooseNetworkAdapterDialog", __FILE__, 49);
}
}

namespace WONAPI {
// FUNCTION: WON 0x7dd370
void InitLogic::ChooseNetworkAdapter()
{
    mNetworkAdapterDialog->Reset();
    if (LobbyDoDialog(mNetworkAdapterDialog.get()) == 100001)
        LobbyFn35B9C0(mNetworkAdapterDialog->GetSelectedAddress());
}
}

namespace WONAPI {
// FUNCTION: WON 0x7dd3b0
bool InitLogic::SetError(int inError, const GUIString& inDescription)
{
    mErrors = inError & ~mIgnoredErrors;
    if (mErrors != 0) {
        bool allowIgnore = (mErrors & 5) == mErrors;
        LobbyFn359100(new LoginInitStatusEvent(::LobbyContainer_InitFailed_String, inDescription, allowIgnore, 3), true);
        return true;
    }
    return false;
}
}

namespace WONAPI {
// FUNCTION: WON 0x7dd570
void InitLogic::Start(bool inRetry)
{
    LobbyFn35B500(100004);
    mErrors = 0;
    LobbyConfig* config = GetLobbyConfig();
    if (config != NULL && config->mCheckForInternet && !AsyncSocket::HasInternetConnection()) {
        if (SetError(1, ::LobbyContainer_NoConnection_String))
            return;
    }
    if (!inRetry) {
        ServerContext* account = LobbyFn35B1F0();
        ServerContext* auth = LobbyFn35B250();
        ServerContext* firewall = LobbyFn35B370();
        ServerContext* routing = LobbyFn35B3D0();
        if (account != NULL) account->Clear();
        if (auth != NULL) auth->Clear();
        if (firewall != NULL) firewall->Clear();
        if (routing != NULL) routing->Clear();
    }
    StartDNSTest();
}
}

namespace WONAPI {
// FUNCTION: WON 0x7dd630
void InitLogic::Init()
{
    mIgnoredErrors = 0;
    LobbyFn35E640();
    IPAddr::SetLocalHost(LobbyFn35E3E0(), true);
    LoadTOUTimes();
    LobbyFn35BA00();
    LobbyFn359120(100052, true);
    Container* aRoot = GetLobbyRoot();
    if(aRoot == NULL)
        return;

    LoginControlIds anIds;
    anIds.Add(200001,200002,200003);
    aRoot->Virtual8C(anIds, false);
    ::LobbyGlobal_LobbyMusic_Sound->SetLooping(true);
    if(LobbyFn35E350())
        ::LobbyGlobal_LobbyMusic_Sound->Fn329BD0();
    else
        ::LobbyGlobal_LobbyMusic_Sound->Stop();
    Start(false);
}

// cross-promotion source candidate; preserves admitted17 above byte-for-byte.
}

namespace WONAPI {
// 3dd810: all four accessors are nullable. This callback owns no returned context.
// FUNCTION: WON 0x7dd810
void InitLogic::DNSCompletion(AsyncOp* inOp)
{
    LobbyConfig* config = GetLobbyConfig();
    ServerContext* dir = LobbyFn35B2B0();
    ServerContext* roomDir = LobbyFn35B310();
    ServerContext* auth = LobbyFn35B250();
    if (config == NULL || dir == NULL || roomDir == NULL || auth == NULL)
        return;
    if (inOp->GetStatus() == 0) {
        // Original constructs this address, sets byte0=true, and destroys its host
        // after both asynchronous setup calls. It is distinct from loop temporaries.
        IPAddr address;
        address.mB00 = true;
        dir->RemoveAllAddresses();
        std::list<std::string>::iterator it = config->mL54.begin();
        while (true) {
            if (it == config->mL54.end())
                break;
            {
                IPAddr anAddr(*it);
                dir->AddAddress(anAddr);
            }
            ++it;
        }
        it = config->mL60.begin();
        while (true) {
            if (it == config->mL60.end())
                break;
            {
                IPAddr anAddr(*it);
                roomDir->AddAddress(anAddr);
            }
            ++it;
        }
        it = config->mL6C.begin();
        while (true) {
            if (it == config->mL6C.end())
                break;
            {
                IPAddr anAddr(*it);
                auth->AddAddress(anAddr);
            }
            ++it;
        }
        if (roomDir->GetNumAddresses() < 1)
            LobbyFn35B430();
        StartHTTPDocuments();
        StartDirectoryQuery();
    } else {
        SetError(2, ::LobbyContainer_DNSLookupFailed_String);
    }
}
}

namespace WONAPI {
// FUNCTION: WON 0x7dda10
void InitLogic::CrossPromotionImageCompletion(AsyncOp* inOp)
{
    if (inOp->mStatus != 0)
        return;
    HTTPGetOp* op = static_cast<HTTPGetOp*>(inOp);
    SmartPtr<Image> image = Window::GetDefaultWindow()->DecodeImage(op->GetLocalPath().c_str());
    LobbyFn359100(new SetCrossPromotionEvent(image.get(),mCrossPromotion),true);
}
}

namespace WONAPI {
// FUNCTION: WON 0x7ddc00
void InitLogic::ParseCrossPromotion(const ByteBuffer* inDocument)
{
    WriteBuffer storage(0,true);
    storage.AppendBytes(inDocument != NULL ? inDocument->data() : NULL,
                        inDocument != NULL ? inDocument->length() : 0);
    storage.AppendByte(0);
    char* token = storage.mData;
    char delimiters[] = " \t\n=";
    std::string imageURL("");
    mCrossPromotion = "";
    token = strtok(token,delimiters);
    while (token != NULL) {
        if (_stricmp(token,"link") == 0) {
            token = strtok(NULL,"\n");
            if (token == NULL)
                break;
            mCrossPromotion = token;
        } else if (_stricmp(token,"image") == 0) {
            token = strtok(NULL,"\n");
            if (token == NULL)
                break;
            imageURL = token;
        }
        if (token == NULL)
            break;
        token = strtok(NULL,delimiters);
    }
    if (!imageURL.empty()) {
        SmartPtr<HTTPGetOp> op = new HTTPGetOp(imageURL);
        op->SetHTTPCache(HTTPCache::GetGlobalCache());
        op->mCompletion = new LogicCompletion<InitLogic>(&InitLogic::CrossPromotionImageCompletion,this);
        mHTTPSession->AddOp(op.get());
    }
}


// directory cohort SCRATCH ONLY; whole typed operation gate still pending.
}

namespace WONAPI {
// 3ddef0: requires the reviewed complete WONFile70 scalar/bool tail.
// FUNCTION: WON 0x7ddef0
void InitLogic::LoadTOUTimes()
{
    WONFile file(LobbyFn35B630());
    long systemTime = file.GetModifyTime();
    file.SetFilePath(LobbyFn35B650());
    long gameTime = file.GetModifyTime();
    LobbyFn35B5D0(systemTime, gameTime);
}
}

namespace WONAPI {
// 3de090..3de272, one body with two FileWriterException& catch continuations.
// FUNCTION: WON 0x7de090
void InitLogic::HandleTOU(GetHTTPDocumentOp* inOp)
{
    long systemTime;
    long gameTime;
    LobbyFn35B600(systemTime, gameTime);
    bool changed = false;
    if (inOp->GetDocumentStatus(HTTPDocOwner_Sys) == 0 && inOp->GetModifiedTime(HTTPDocOwner_Sys) > systemTime) {
        FileWriter writer;
        try {
            if (writer.Open(LobbyFn35B630().c_str(), true)) {
                ByteBufferPtr document = inOp->GetDocument(HTTPDocOwner_Sys);
                if (document.get() != NULL && document->length() > 1)
                    writer.WriteBytes(document->data(), document->length() - 1);
            }
            // Original sets this even if Open returns false, but not on an exception.
            changed = true;
        } catch (FileWriterException&) {}
    }
    if (inOp->GetDocumentStatus(HTTPDocOwner_Game) == 0 && inOp->GetModifiedTime(HTTPDocOwner_Game) > gameTime) {
        FileWriter writer;
        try {
            if (writer.Open(LobbyFn35B650().c_str(), true)) {
                ByteBufferPtr document = inOp->GetDocument(HTTPDocOwner_Game);
                if (document.get() != NULL && document->length() > 1)
                    writer.WriteBytes(document->data(), document->length() - 1);
            }
            changed = true;
        } catch (FileWriterException&) {}
    }
    if (changed)
        LoadTOUTimes();
}
}

namespace WONAPI {
// 3de280: original always broadcasts the document event before type dispatch.
// FUNCTION: WON 0x7de280
void InitLogic::HTTPDocumentCompletion(AsyncOp* inOp)
{
    GetHTTPDocumentOp* op = static_cast<GetHTTPDocumentOp*>(inOp);
    LobbyFn359100(new GotHTTPDocumentEvent(op), true);
    switch (op->mDocumentType) {
    case 1:
        HandleTOU(op);
        break;
    case 2:
        {
            ByteBufferPtr document = op->GetDocument(HTTPDocOwner_Game);
            if (document.get() != NULL && document->length() > 1) {
                ParseCrossPromotion(document.get());
                return;
            }
            LobbyFn359100(new SetCrossPromotionEvent(NULL, std::string("")), true);
            return;
        }
    }
}


#include "ee/wonapi/EELobbySound.h"
// Existing admitted13 code above preserved verbatim; final entry4/761 below SCRATCH only.
}

namespace WONAPI {
// FUNCTION: WON 0x007de4c0
void InitLogic::DirectoryCompletion(AsyncOp* inOp)
{
    ServerContext* account = LobbyFn35B1F0();
    ServerContext* auth = LobbyFn35B250();
    ServerContext* firewall = LobbyFn35B370();
    ServerContext* versions = LobbyFn35B3D0();
    ServerContext* directory = LobbyFn35B2B0();
    LobbyConfig* config = GetLobbyConfig();
    if (config == NULL || account == NULL || auth == NULL ||
        firewall == NULL || directory == NULL)
        return;
    WONStatus status = inOp->GetStatus();
    if (status == 0 || status == -1223) {
    GetMultiDirOp* op = static_cast<GetMultiDirOp*>(inOp);
    const GetEntityRequestList& requests = op->GetRequestList();
    GetEntityRequestList::const_iterator i = requests.begin();
    int index = 0;
    for (; i != requests.end(); ++i,++index) {
        GetEntityRequest* request = (*i).get();
        if (request->GetStatus() != 0)
            continue;
        ServerContext* selected = NULL;
        switch (index) {
        case 0:
            if (config->mL6C.empty())
                selected = auth;
            break;
        case 1: selected = account; break;
        case 2: selected = firewall; break;
        case 3: selected = versions; break;
        }
        if (selected != NULL)
            selected->AddAddressesFromDir(request->GetDirEntityList(),NULL);
    }
    if (auth->GetNumAddresses() == 0 || account->GetNumAddresses() == 0 ||
        versions->GetNumAddresses() == 0) {
        directory->NotifyFailed(op->GetCurAddr());
        GUIString names;
        int errors = 0;
        if (auth->GetNumAddresses() == 0) {
            names.append(::LobbyContainer_AuthServers_String);
            errors = 8;
        }
        if (account->GetNumAddresses() == 0) {
            if (names.length() != 0)
                names.append(GUIString(", "));
            names.append(::LobbyContainer_AccountServers_String);
            errors |= 4;
        }
        if (versions->GetNumAddresses() == 0) {
            if (names.length() != 0)
                names.append(GUIString(", "));
            names.append(::LobbyContainer_VersionServers_String);
            errors |= 0x10;
        }
        GUIString description(::LobbyContainer_NoServers_String);
        description.append(names);
        if (SetError(errors,description))
            return;
    }
    // LOST-DEBUG: none: exact only
    EE_LOST_DEBUG(2);
    StartVersionCheck();
    StartFirewallTest();
    }
    else {
        GUIString description = LobbyFormatString(::LobbyContainer_DirLookupFailure_String,LobbyFn366170(status));
        SetError(0x20,description);
    }
}


// remaining Version/Firewall cohort; original-backed source, normal capture pending.
}

namespace WONAPI {
// FUNCTION: WON 0x007de8f0
void InitLogic::VersionCompletion(AsyncOp* inOp)
{
    Container* root = GetLobbyRoot();
    if (root == NULL)
        return;
    WONStatus status = inOp->GetStatus();
    LobbyFn35B500(status);
    if (status != 0 && status != -2816 && status != -2814) {
        if (status == -2815 || status == -2800) {
            ServerContext* versions = LobbyFn35B3D0();
            if (versions != NULL)
                versions->NotifyFailed(static_cast<CheckValidVersionOp*>(inOp)->GetCurAddr());
        }
        GUIString description = LobbyFormatString(::LobbyContainer_VersionCheckFailure_String,LobbyFn366170(status));
        SetError(0x40,description);
        return;
    }
    LoginControlIds ids;
    ids.Add(200001,200002,200003);
    root->Virtual8C(ids,true);
    LobbyFn359100(new LoginInitStatusEvent(::LobbyContainer_InitComplete_String,gEmptyGUIString,false,2),true);
}
}

namespace WONAPI {
// FUNCTION: WON 0x007deb50
void InitLogic::AcceptCompletion(AsyncOp* inOp)
{
    AcceptOp* op = mAcceptOp.get();
    if (inOp != op)
        return;
    mAcceptOp = NULL;
    if (op->GetStatus() != 0) {
        LobbyFn361770(true);
        LobbyFn359120(100055,true);
        return;
    }
    if (op->mAcceptedSocket->GetDestAddr().Fn2B2FB0() == mFirewallAddress.Fn2B2FB0()) {
        LobbyFn361770(false);
        LobbyFn359120(100055,true);
        return;
    }
    mAcceptOp = new AcceptOp(op->mSocket.get());
    RunOp(mAcceptOp.get(),new LogicCompletion<InitLogic>(&InitLogic::AcceptCompletion,this),
          gLobbyTimeout4E0EBC);
}
}

namespace WONAPI {
// FUNCTION: WON 0x007ded20
void InitLogic::FirewallCompletion(AsyncOp* inOp)
{
    DetectFirewallOp* op = static_cast<DetectFirewallOp*>(inOp);
    if (op->mStatus == 0) {
        LobbyFn3615E0(op->GetPublicAddr());
        if (mFirewallSocket.get() != NULL) {
            mFirewallAddress = op->GetCurAddr();
            mAcceptOp = new AcceptOp(mFirewallSocket.get());
            RunOp(mAcceptOp.get(),new LogicCompletion<InitLogic>(&InitLogic::AcceptCompletion,this),
                  gLobbyTimeout4E0EBC);
            mFirewallSocket = NULL;
        }
    } else {
        LobbyFn361770(true);
    }
    LobbyFn359120(100055,true);
}
}

namespace WONAPI {
// Complete original-backed next candidates. Scratch only; canonical ABI gates pending.
// 3def30: DNSTestOp and LogicCompletion allocation require their full class contracts.
// FUNCTION: WON 0x7def30
void InitLogic::StartDNSTest()
{
    LobbyConfig* config = GetLobbyConfig();
    if (config == NULL || config->mL54.empty())
        return;
    LobbyFn359100(new LoginInitStatusEvent(::LobbyContainer_InitStart_String, gEmptyGUIString, false, 1), true);
    SmartPtr<DNSTestOp> op = new DNSTestOp(config->mL54.front());
    RunTrackedOp(op.get(), new LogicCompletion<InitLogic>(&InitLogic::DNSCompletion, this), 0);
}
}

namespace WONAPI {
// 3df0c0: all three op locals survive until return, preserving original ownership/EH.
// FUNCTION: WON 0x7df0c0
void InitLogic::StartHTTPDocuments()
{
    LobbyConfig* config = GetLobbyConfig();
    if (config == NULL)
        return;
    SmartPtr<GetHTTPDocumentOp> motd = new GetHTTPDocumentOp(config->mS78, HTTPDocType_MOTD);
    motd->SetExtraConfig(config->mSB8);
    motd->SetHTTPSession(mHTTPSession.get());
    RunTrackedOp(motd.get(), new LogicCompletion<InitLogic>(&InitLogic::HTTPDocumentCompletion, this), 0);
    SmartPtr<GetHTTPDocumentOp> tou = new GetHTTPDocumentOp(config->mS78, HTTPDocType_TOU);
    tou->mPath = config->mSB8;
    tou->mSession = mHTTPSession.get();
    RunTrackedOp(tou.get(), new LogicCompletion<InitLogic>(&InitLogic::HTTPDocumentCompletion, this), 0);
    SmartPtr<GetHTTPDocumentOp> adv = new GetHTTPDocumentOp(config->mS78, HTTPDocType_ADV);
    adv->SetExtraConfig(config->mSB8);
    adv->mSession = mHTTPSession.get();
    RunTrackedOp(adv.get(), new LogicCompletion<InitLogic>(&InitLogic::HTTPDocumentCompletion, this), 0);
}
}

namespace WONAPI {
// FUNCTION: WON 0x007df410
void InitLogic::StartDirectoryQuery()
{
    ServerContext* context = LobbyFn35B2B0();
    if (context == NULL)
        return;
    LobbyFn359100(new LoginInitStatusEvent(::LobbyContainer_QueryingServers_String, gEmptyGUIString, false, 0), true);
    SmartPtr<GetMultiDirOp> op = new GetMultiDirOp(context);
    op->SetFlags(0x04000002);
    op->AddPath(L"/TitanServers/Auth");
    op->AddPath(L"/TitanServers/DBProxy");
    op->AddPath(L"/TitanServers/Firewall");
    LobbyConfig* config = GetLobbyConfig();
    if (config != NULL)
        op->AddPath(config->mW88 + L"/Patch");
    RunTrackedOp(op.get(), new LogicCompletion<InitLogic>(&InitLogic::DirectoryCompletion, this), 0);
    // LOST-DEBUG: none: exact only
    EE_LOST_TRACE(op.get());
}
}

namespace WONAPI {
// FUNCTION: WON 0x007df6c0
void InitLogic::StartVersionCheck()
{
    ServerContext* versions = LobbyFn35B3D0();
    LobbyConfig* config = GetLobbyConfig();
    if (versions == NULL || config == NULL)
        return;
    LobbyFn359100(new LoginInitStatusEvent(::LobbyContainer_CheckingVersion_String,gEmptyGUIString,false,0),true);
    SmartPtr<CheckValidVersionOp> op = new CheckValidVersionOp(config->mS78,versions);
    op->SetConfigName(config->mSB8);
    op->SetVersion(config->mS98);
    RunTrackedOp(op.get(),new LogicCompletion<InitLogic>(&InitLogic::VersionCompletion,this),0);
}
}

namespace WONAPI {
// FUNCTION: WON 0x007df880
void InitLogic::StartFirewallTest()
{
    ServerContext* firewall = LobbyFn35B370();
    if (firewall == NULL)
        return;
    KillFirewallTest();
    LobbyFn361440();
    SmartPtr<DetectFirewallOp> op = new DetectFirewallOp(firewall);
    op->SetListenPort(LobbyFn361400());
    op->SetDoListen(false);
    op->SetWaitForConnect(false);
    RunTrackedOp(op.get(),new LogicCompletion<InitLogic>(&InitLogic::FirewallCompletion,this),0);
    LobbyFn359120(100055,true);
    mFirewallSocket = new AsyncSocket(0);
    WONStatus aStatus = mFirewallSocket->Fn2B0D90(LobbyFn361400(),0);
    if (aStatus == 0) {
        aStatus = mFirewallSocket->Fn2B0FC0(1000);
        if (aStatus == 0)
            return;
    }
    mFirewallSocket = NULL;
}
}

namespace WONAPI {
// FUNCTION: WON 0x7dfa80
void InitLogic::KillFirewallTest()
{
    if (mAcceptOp.get() != NULL) {
        mAcceptOp->Kill(true);
        if (mAcceptOp->mSocket.get() != NULL)
            mAcceptOp->mSocket->Fn2B1AD0();
        mAcceptOp = NULL;
    }
    mFirewallSocket = NULL;
}
}

namespace WONAPI {
// FUNCTION: WON 0x7dfb00
void InitLogic::KillHook()
{
    mHTTPSession->KillAll();
    KillFirewallTest();
}
}

namespace WONAPI {
// FUNCTION: WON 0x7dfb20
void InitLogic::HandleControlEvent(int inControlId)
{
    switch (inControlId) {
    case 200006: LobbyFn3627F0(0); break;
    case 200008: LobbyFn360F00(); break;
    case 200023: ChooseNetworkAdapter(); break;
    }
}
}

namespace WONAPI {
// FUNCTION: WON 0x7dfb50
void InitLogic::HandleComponentEvent(ComponentEvent* inEvent)
{
    if (inEvent->mType != 100045) {
        if (inEvent->mType != 100025)
            return;
        mIgnoredErrors |= mErrors;
    }
    Start(true);
}
}

// TEMPLATE: WON 0x007f20a0

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007dd1e0
// (public: virtual void * __thiscall WONAPI::InitLogic::`scalar deleting destructor'(unsigned int)    [??_GInitLogic@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007df3f0
// (public: __thiscall WONAPI::LogicCompletionBase::LogicCompletionBase(void)    [??0LogicCompletionBase@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x007f36d0

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007dd4a0
// (public: virtual void * __thiscall WONAPI::LoginInitStatusEvent::`scalar deleting destructor'(unsigned int)    [??_GLoginInitStatusEvent@WONAPI@@UAEPAX)
// TEMPLATE: WON 0x007dd4c0
// (public: virtual __thiscall WONAPI::LoginInitStatusEvent::~LoginInitStatusEvent(void)    [??1LoginInitStatusEvent@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x007ddb50
// (public: virtual void * __thiscall WONAPI::SetCrossPromotionEvent::`scalar deleting destructor'(unsigned int)    [??_GSetCrossPromotionEvent@WONAPI@@UA)
// TEMPLATE: WON 0x007ddb70
// (public: virtual __thiscall WONAPI::SetCrossPromotionEvent::~SetCrossPromotionEvent(void)    [??1SetCrossPromotionEvent@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007f1430
// (public: virtual void * __thiscall WONAPI::LogicCompletion<class WONAPI::InitLogic>::`scalar deleting destructor'(unsigned int)    [??_G?$LogicCompleti)
// TEMPLATE: WON 0x007f1510
// (public: virtual __thiscall WONAPI::LogicCompletion<class WONAPI::InitLogic>::~LogicCompletion<class WONAPI::InitLogic>(void)    [??1?$LogicCompletion@)
