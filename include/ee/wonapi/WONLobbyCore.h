#ifndef EE_WONAPI_WONLOBBYCORE_H
#define EE_WONAPI_WONLOBBYCORE_H
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONLobbyResource.h"
namespace WONAPI {
class Component;
int LobbyDoDialog(Component* inDialog); //358e50; genuine SDK borrowed generic Component contract
// Individually original-backed call contracts only. No object-ownership grouping inferred.
WONStatus LobbyFn35B520(); //35b520; code70 or100004 when no lobby
int LobbyFn358EF0(const GUIString& inTitle, const GUIString& inText, bool inPlaySound); //358ef0; all5directcallerspass1,calleeconsumesbyte
void LobbyFn3627F0(int inScreen); //3627f0
void LobbyFn3610D0(); //3610d0
void LobbyFn3610F0(); //3610f0
void LobbyFn361110(); //361110
void LobbyFn35B500(WONStatus inStatus); //35b500
ServerContext* LobbyFn35B1F0(); //35b1f0
ServerContext* LobbyFn35B250(); //35b250
ServerContext* LobbyFn35B370(); //35b370
ServerContext* LobbyFn35B3D0(); //35b3d0
void LobbyFn35B430(); //35b430
// Reviewed original-backed shared call contracts; definitions remain with their actual families.
class Container;
class ChatCommandParser;
class UDPManager;
class LobbyGame;
class LobbyClient;
Container* GetLobbyRoot();
void LobbyFn358DF0(Component* inPopup, int inFlags, int inX, int inY);
void LobbyFn35B9A0(Sound* inSound);
void LobbyFn35B900(LobbyGame* inGame, bool inForce);
void LobbyFn35B920(LobbyGame* inGame);
void LobbyFn35B960(int inUnused);
void LobbyFn361020();
ChatCommandParser* LobbyFn35B710();
UDPManager* LobbyFn35B8F0();
AuthContext* GetLobbyAuthContext();
const std::string& LobbyFn35B630();
const std::string& LobbyFn35B650();
void LobbyFn35B600(long& outSystemTime, long& outGameTime);
GUIString LobbyFn366170(WONStatus inStatus);
IPAddr LobbyFn361990();
bool LobbyFn361170();
bool LobbyFn361150();
bool LobbyFn361B40();
void LobbyFn358E70(AsyncOp* inOp, const GUIString& inTitle, const GUIString& inStatus, int inStatusMode, unsigned long inTimeout);
int LobbyFn35B940();
void LobbyFn3612D0(LobbyGame* inGame);
void LobbyFn3612F0();
class ComponentEvent;
void LobbyFn359260(ComponentEvent* inEvent, Component* inTarget);
ChatCommandParser* LobbyFn35B730(int inRoomSpec);
void LobbyFn361E50(bool inShowError);
void LobbyFn35B850(LobbyClient* inClient, bool inFlag, int inRoomSpec);
void LobbyFn35B7B0(LobbyClient* inClient, bool inFlag, int inRoomSpec);
bool LobbyFn3BA6A0(const std::string& inEmail);
void LobbyFn361BC0(LobbyClient* inClient, bool inIsBan);
int LobbyFn361190();
class Window;
void LobbyFn358ED0(const GUIString& inTitle, WONStatus inStatus);
void LobbyFn359300(ComponentEvent* inEvent);
bool LobbyFn35B540();
bool LobbyFn35B560(); //35b560 native19B private90 flag getter, call-only
bool LobbyFn35B670(long,long); //35b670 native49B signed by-value terms timestamps check, call-only
void LobbyFn35B980();
void LobbyFn360F90();
Window* LobbyFn361230();
void LobbyFn361ED0(bool inJoinDefaultRoom);
void LobbyFn361DD0();
bool LobbyFn361C30(LobbyGame* inGame);
bool LobbyFn361C60(LobbyGame* inGame);
LobbyClient* LobbyFn361C90();
IPAddr LobbyFn3617A0(int inRoomSpec);
void LobbyFn361E10(ByteBuffer* inData, LobbyClient* inClient);
void LobbyFn361E70(LobbyClient* inClient);
void LobbyFn361E90(LobbyClient* inClient, bool inIsBan);
void LobbyFn3610B0();
bool LobbyFn361130();
// Individually closed dialog/window contracts.
bool LobbyFn3628B0(); //private suspended byte54, nullfalse
class ColorScheme;
ColorScheme* LobbyFn361210(); //actual Component GetColorScheme2f5970, not a listener
void LobbyFn359080(Component* inComponent); //actual AddListener2f4b10 wrapper
void LobbyFn361E30(ReadBuffer& inData, LobbyClient* inClient); //actual Staging3efb50 forwarding
unsigned short LobbyFn361400(); //actual privateLobbyPortb8, nullzero
Component* LobbyFn360EA0(); //original360ea0, actual zero-offset component return
void LobbyFn35B9C0(long inAddress); //original35b9c0, numeric IPv4 address
ServerContext* LobbyFn35B2B0(); //original35b2b0, borrowed ServerContext at private60
ServerContext* LobbyFn35B310(); //original35b310, borrowed ServerContext at private64
void LobbyFn35B5D0(long inSysTime, long inGameTime); //original35b5d0, stores privatebc/c0
void LobbyFn35E640(); //original singleton guard then tail Load35f7b0
void LobbyFn35BA00(); //original no-argument UDP/config-port setup; caller ignores incidental EAX
void LobbyFn361420(unsigned short inPort); //361420 actual cdecl plainRET; nullguard and private LobbyPortb8 wordstore
void LobbyFn360F00(); //original no-argument screen/event wrapper, no incoming receiver
void LobbyFn359040(); //359040 genuine namespace event/timer teardown, cdecl void/noargs
bool LobbyFn358F50(); //358f50 actual DialogLogic active-state byte getter, call-only
bool LobbyFn361330(bool); //361330 actual StagingLogic Fn3F0110 bool forward, call-only
struct LobbyUserInfo;
LobbyUserInfo* LobbyFn35CDE0(); //borrowed current user record; null when no matching file ID
GUIString LobbyFn34A170(const GUIString& inFormat, const GUIString& inFirst,
    const GUIString& inSecond, const GUIString& inThird); //cdecl hidden GUIString result plus four references
// Borrowed current Routing fields; native19B wrappers, no retain/new/body quota.
class LobbyServerList;
class LobbyGroup;
LobbyServerList* LobbyFn35B6B0(); //35b6b0: Routing.mServerList54
LobbyGroup* LobbyFn35B6D0(); //35b6d0: Routing.mChatGroup6c
// R3 actual caller/callee contracts; no wrapper implementations.
GUIString LobbyFn34A130(const GUIString& inFormat, const GUIString& inFirst, const GUIString& inSecond);
void LobbyFn361440();
void LobbyFn3615E0(const IPAddr& inAddress);
void LobbyFn361770(bool inBehindFirewall);
}
#endif
