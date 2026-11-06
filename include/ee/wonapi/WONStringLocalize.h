// Genuine stateless StringLocalize core API; wrappers remain original-owned.
#ifndef EE_WONAPI_WONSTRINGLOCALIZE_H
#define EE_WONAPI_WONSTRINGLOCALIZE_H
#include "ee/wonapi/WONLobbyResource.h"
namespace WONAPI {
class StringLocalize {
public:
    static GUIString GetStr(const GUIString&,const GUIString**,int); //349f10 actual cdecl owning sret
};
}
#endif
