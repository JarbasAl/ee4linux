// WONAPI GUI / lobby framework classes as the game's lobby glue sees them.
// WONAPI proper (0x29deb0..) is not decompiled: bases come from the RTTI class hierarchy descriptors, sizes from the
// glue's `new` calls; every other name PROVISIONAL (by behaviour); `_gap` arrays are undecoded. Requires <string>,
// ee/EEWonNet.h, ee/wonapi/WONLobbyResource.h.
#ifndef EE_WONAPI_WONLOBBYGUI_H
#define EE_WONAPI_WONLOBBYGUI_H

#include <exception>
#include <deque>
#include <set>
#include "ee/wonapi/WONBuffer.h"
#include "ee/wonapi/WONLobbyBackground.h"
#include "ee/wonapi/WONStringCompare.h"
#include "ee/wonapi/EELobbySound.h"
#include "ee/wonapi/WONLobbyImageDecoder.h"
#include "ee/wonapi/WONAPICore.h"

namespace WONAPI {

// Genuine SDK LobbyTypes enum; original wire values0/1/2/3.
enum LobbyGameType
{
	LobbyGameType_None,
	LobbyGameType_Lan,
	LobbyGameType_Internet,
	LobbyGameType_DirectConnect
};

// Genuine SDK EventTypes.h ControlIdType; native registrar passes100001/100002.
enum ControlIdType
	{
		ControlId_LibMin	= 100000,
		ControlId_Ok,
		ControlId_Cancel,
		ControlId_TreeToggleOpen,
		ControlId_SplitterMin,
		ControlId_SplitterMax = ControlId_SplitterMin + 100,
		ControlId_UserMin	= 200000,
		ControlId_UserMin2	= 300000,
		ControlId_UserMin3	= 400000,
		ControlId_UserMin4	= 500000,
		ControlId_UserMin5	= 600000
	};

// Original nominal int/long identity is provisional; all users share this signed32 ID set.
typedef std::set<int> ControlIdSet;
enum Direction { Direction_Left, Direction_Right, Direction_Up, Direction_Down };
enum HorzAlign { HorzAlign_Left, HorzAlign_Center, HorzAlign_Right };
enum VertAlign { VertAlign_Top, VertAlign_Center, VertAlign_Bottom };

// Pinned SDK nominal event/flag constants; native input1..14 and popup bit tests corroborate.
enum WindowEventType
	{
		WindowEventType_None,

		InputEvent_SizeChange,
		InputEvent_MouseMove,
		InputEvent_MouseDown,
		InputEvent_MouseDoubleClick,
		InputEvent_MouseDownCheckDoubleClick, 
		InputEvent_MouseUp,
		InputEvent_MouseWheel,
		InputEvent_KeyDown,
		InputEvent_KeyUp,
		InputEvent_KeyChar,
		InputEvent_GotFocus,
		InputEvent_LostFocus,
		InputEvent_MouseExit,
		InputEvent_WindowClose,

		ComponentEvent_ButtonPressed,
		ComponentEvent_Scroll,
		ComponentEvent_TextLinkActivated,
		ComponentEvent_HyperLinkActivated,
		ComponentEvent_HyperLinkOver,
		ComponentEvent_InputReturn,
		ComponentEvent_InputTextChanged,

		ComponentEvent_ListSelChanged,
		ComponentEvent_ListItemClicked,
		ComponentEvent_ListItemUpClicked,
		ComponentEvent_MenuItemSelected,
		ComponentEvent_TreeItemOpened,
		ComponentEvent_ComboSelChanged,
		ComponentEvent_TabBarTabSelected,
		ComponentEvent_TabCtrlTabSelected,
		ComponentEvent_EndPopup,
		ComponentEvent_BrowserStatusChange,
		ComponentEvent_BrowserLocationChange,
		ComponentEvent_BrowserButtonChange,
		ComponentEvent_SpinnerValueChanged,

		ComponentEvent_ContextStart	= 10000, 
		ComponentEvent_ContextEnd = 20000,

		Event_User_Start = 100000
	};
enum PopupFlags
{
	PopupFlag_EndOnLeftMouseDown			= 0x0001,
	PopupFlag_EndOnMiddleMouseDown			= 0x0002,
	PopupFlag_EndOnRightMouseDown			= 0x0004,
	PopupFlag_EndOnMouseMove				= 0x0008,
	PopupFlag_EndOnKeyDown					= 0x0010,
	PopupFlag_EndOnEscape					= 0x0020,
	PopupFlag_EndOnFocusChange				= 0x0040,
	PopupFlag_WantInput						= 0x0080,
	PopupFlag_IsModal						= 0x0100,
	PopupFlag_AdjustPosition				= 0x0200,
	PopupFlag_EatMouseDownOnEnd				= 0x0800,
	PopupFlag_Additional					= 0x1000,
	PopupFlag_FireEventOnEnd				= 0x2000,
	PopupFlag_DontWantMouse					= 0x4000,				

	PopupFlag_InOwnWindow				   = 0x10000,
	PopupFlag_NotInOwnWindow			   = 0x20000,

	PopupFlag_EndOnMouseDown				= 0x0007,
	PopupFlag_EndOnInput					= 0x001f,

	PopupFlag_StandardPopup					= 0x23e7,
	PopupFlag_StandardTip					= 0x6247
};

class WONRectangle;
class Window;
class Component;
enum StandardCursor {
    StandardCursor_Arrow,StandardCursor_Hand,StandardCursor_IBeam,
    StandardCursor_EastWest,StandardCursor_NorthSouth,StandardCursor_NorthSouthEastWest,
    StandardCursor_Max
};
// Complete native Cursor RefCount8; native326540 stores only inherited refcount/vptr.
class Cursor : public RefCount {
protected:
    virtual ~Cursor(); //native external
public:
    Cursor(); //326540 complete8
    virtual Cursor* GetNative(); //slot04 native external
    virtual Component* GetComponent(int&,int&); //slot08 native external
    static Cursor* GetStandardCursor(StandardCursor); //326550 borrowed result
    static void SetStandardCursor(StandardCursor,Cursor*); //326560 mutable sharedreference
};
// Complete0c indirection owner: nativeSetStandard326577 new0c and storesownedcursor8.
class CursorRef : public Cursor {
protected:
    SmartPtr<Cursor> mCursor; //08
public:
    CursorRef(Cursor* cursor) : mCursor(cursor) {}
    virtual Cursor* GetNative() { return mCursor->GetNative(); } //326660 support0
    virtual Component* GetComponent(int& x,int& y) { return mCursor->GetComponent(x,y); } //326670 support0
    void SetCursor(Cursor* cursor) { mCursor=cursor; }
};
// Complete10 native platformcursor; original3d0941/3d0271 new10 and vtable45471c.
class MSCursor : public Cursor {
protected:
    virtual ~MSCursor(); //3d0320 conditionalDestroyCursor thenbase
public:
    HCURSOR mNativeCursor; //08
    bool mDoDestroy; //0c, nativeCtor scalarpadding0d..0f
    MSCursor(HCURSOR cursor,bool destroy=false) : mNativeCursor(cursor),mDoDestroy(destroy) {}
    virtual Cursor* GetNative() { return this; } //3d02f0 support0
};
// Borrowed target prefix; native326ce0 reads17DWORDs from+08.
enum StandardColor
{
	StandardColor_3DDarkShadow = 0,
	StandardColor_3DFace,
	StandardColor_3DHilight,
	StandardColor_3DShadow,
	StandardColor_Scrollbar,

	StandardColor_ButtonText,
	StandardColor_GrayText,
	StandardColor_Hilight,
	StandardColor_HilightText,

	StandardColor_ToolTipBack,
	StandardColor_ToolTipText,

	StandardColor_MenuBack,
	StandardColor_MenuText,

	StandardColor_Back,
	StandardColor_Text,

	StandardColor_Link,
	StandardColor_LinkDown,


	StandardColor_Max
};

enum { COLOR_INVALID = 0xffffffff };
class ColorScheme : public RefCount {
public:
    DWORD mColors[17];
    ColorScheme() { for(int i=0;i<17;++i) mColors[i]=0; } //native32b973..86 actual17zero constructor
    static DWORD GetColorRef(StandardColor theColor) { return 0x80000000 + theColor; } // genuine SDK inline
    DWORD GetStandardColor(int i) { return mColors[i]; }
    void SetStandardColor(int i,DWORD color) { mColors[i]=color; }
};
class Font;
class Image;
class NativeImage;
class NativeIcon;
class SelectionColor;
class RawImage32;
class DisplayContext;
struct FontDescriptor;
class Button;
class InputBox;
class ListItem;
class TextListItem;
class Component;
class ChildLayoutInstructionBase;
class Container;
class ComponentConfig;
class ComponentEvent;
class ComponentExtra;
class BackgroundGrabber;
class ListArea;
class MultiListArea;
class ScrollArea;
class ScrollAreaViewport;
class Scroller;
class LobbyClient;
class LobbyPlayer;
class LobbyClientList;
struct RoutingClientInfo;
class ConfigParser;
class StringParser;
class ReadBuffer;
class WriteBuffer;
class ByteBuffer;
class LobbyGroup;

// what a component is initialised from (slot +0x1c); type 2: from its resource configuration
struct ComponentInit {
    int mType;                              // +0x00
};

struct ComponentConfigInit : public ComponentInit {
    ComponentConfigInit(ComponentConfig*); //38aa40 actualexternal18B, type2 and borrowedconfig4
    ComponentConfig* mConfig;               // +0x04
};

class Graphics;
class RootContainer;
class ComponentListener;
struct PopupParams;
extern GUIString gEmptyGUIString;           // 0x966244

// a GUI component (0x30 bytes: EELobbyColorChooser's members start at +0x30; vtable 0x84d71c, 43
// slots). Unknown slots are named by vtable offset. The inline bodies are the COMDATs the first lobby unit
// (EELobbyColorButton.cpp 0x29059d, 0x2905a2) keeps; the other tiny slots are ICF-folded into game code.
class WONRectangle {
public:
    WONRectangle();                         // 0x349300
    WONRectangle(int inX, int inY, int inWidth, int inHeight);    // 0x349310
    void DoIntersect(const WONRectangle&); //349330, native3271b1 call
    void DoUnion(const WONRectangle&); //3493b0 nativeRoot30498b external
    bool Intersects(const WONRectangle&) const; //349450 native complete69B predicate
    int Right() const { return x+width; }
    int Bottom() const { return y+height; }

    int x;                                  // +0x00
    int y;                                  // +0x04
    int width;                              // +0x08
    int height;                             // +0x0c
};

class Component : public RefCount {
public:
    Component();                            // 0x2f49e0 (WONAPI)
    virtual ~Component();                   // 0x2f4a70 (WONAPI)
    virtual void Virtual04();
    virtual void Virtual08();
    virtual void Virtual0C(int& outWidth, int& outHeight);     // (EELobbyColorChooser 0x2908af: 120 x 120)
    virtual bool Virtual10();
    virtual void Virtual14(Graphics& inGraphics);
    virtual void Paint(Graphics& inGraphics);                  // +0x18 (0x2f5020)
    virtual void InitComponent(ComponentInit& inInit) {} //genuine SDK empty InitComponent, native156dd8/3 RET4
    virtual bool Virtual20(int inX, int inY);
    virtual void Virtual24(int inA, int inB);                  // +0x24 (0x2f4cc0)
    virtual void Virtual28();                                  // +0x28 (0x2f4ce0: the mouse left?)
    virtual void Virtual2C(int,int,int) {} //genuine SDK MouseDown empty inline default, native3a6880/3
    virtual void Virtual30(int inA, int inB, int inC) {}       // +0x30 0x2907a3 (emitted with EELobbyColorChooser)
    virtual void Virtual34(int inX, int inY) {}                // +0x34 (the mouse moved); genuine SDK inline empty MouseMove
    virtual void Virtual38(int inX, int inY);
    virtual bool Virtual3C(int inParam) { return false; }      // +0x3c 0x29059d
    virtual void Virtual40(unsigned long inDelta);
    virtual bool Virtual44(int inDelta);
    virtual void Virtual48();
    virtual void Virtual4C();
    virtual bool Virtual50(int inParam) { return false; }      // +0x50 (ICF: 0x29059d)
    virtual bool Virtual54(int inParam) { return false; }      // +0x54 (ICF: 0x29059d)
    virtual bool Virtual58(int inParam) { return false; }      // +0x58 (ICF: 0x29059d)
    virtual bool Virtual5C();
    virtual void Virtual60(WONRectangle& ioRect);
    virtual void Invalidate();                                 // +0x64 (after a visible change; PROVISIONAL)
    virtual void Virtual68();
    virtual void Virtual6C();                                  // +0x6c (0x2f5160: when the popup opens?)
    virtual void Virtual70();
    virtual void Virtual74(bool inFlag);
    virtual void Virtual78(Cursor* inCursor);
    virtual void HandleComponentEvent(ComponentEvent* inEvent);    // +0x7c (0x2f5550)
    virtual void Virtual80(ComponentEvent* inEvent);
    virtual void Virtual84(bool inFlag);
    virtual bool Virtual88(bool inForward, bool inInit);
    virtual void Virtual8C(const ControlIdSet& inIds, bool inEnabled);
    virtual void SetEnabled(bool inEnabled);                   // +0x90 (0x2f5610: clears/sets flag 0x80 of +0x20)
    virtual void SetVisible(bool inVisible);                   // +0x94 (0x2f55b0)
    virtual Component* Virtual98();
    virtual int Virtual9C() { return 0; } //genuine SDK MinWidth INT return, native1ad67e/3
    virtual int VirtualA0() { return 0; } //genuine SDK MinHeight INT return, native1ad67e/3
    virtual int VirtualA4() { return 0xffff; }                 // +0xa4 0x2905a2
    virtual int VirtualA8() { return 0xffff; }                 // +0xa8 (ICF: 0x2905a2)

    void Fn2F59A0(ColorScheme* inScheme); // 0x2f59a0, owns ComponentExtra+4
    ColorScheme* GetOwnColorScheme(); //2f5960, borrowed own scheme
    ColorScheme* GetColorScheme(); //2f5970, borrowed inherited scheme
    void Fn2F4B70(int inX, int inY);          // 0x2f4b70
    void Fn2F4B90(int inWidth, int inHeight); // 0x2f4b90
    void Fn2F4BC0(int inMinWidth, int inMinHeight); // 0x2f4bc0
    void Fn2F4BF0(int inX, int inY, int inWidth, int inHeight); // 0x2f4bf0
    void Fn2F4EE0(int inPadding);            // 0x2f4ee0
    void Fn2F52A0(ComponentEvent* inEvent, bool inFlag); // 0x2f52a0
    void Fn2F52D0(ComponentEvent* inEvent); // 0x2f52d0, retained downward broadcast
    void Fn2F53F0(ComponentEvent* inEvent, bool inSendNow); // 0x2f53f0
    SmartPtr<Font> GetFont(const FontDescriptor&); //2f5a00 actual SDK hiddenSP sret+descriptor/RET8, original external provider
    Font* Fn2F5A30(Font* inFont, int inStyle, int inOptionalSize); // 0x2f5a30
    Font* Fn2F5B20();                        // 0x2f5b20
    Font* GetNamedFont(const std::string&); //2f5b30 actual borrowed manager font lookup
    Font* Fn2F5B50(int inStyle, int inSize);  // 0x2f5b50
    void AddListener(ComponentListener* inListener);           // 0x2f4b10
    void Fn2F4E90(int inA, int inB);        // 0x2f4e90 (on a new popup: 0, 0)
    void Fn2F5380(int inType, int inParam); // 0x2f5380 legacy provisional int API remains external
    void FireEvent(int,bool=false); //2f5380 genuine SDK API; preserves legacy external mapping
    bool Fn2F5600();                        // 0x2f5600, actual bit18 visibility-to-root
    void Fn2F56D0(int& ioX, int& ioY);      // 0x2f56d0 (to the root's coordinates?)
    void Fn2F54E0(ComponentEvent* inEvent); // 0x2f54e0, actual event bubbling
    Component* GetNextSibling(bool,bool=false); //actual2f5230 sibling traversal
    bool WantFocus(); //actual2f5810 can-take-focus predicate
    void RootToChild(int&,int&); //actual2f5720
    void GetMousePos(int&,int&); //actual2f5770
    int GetKeyMod(); //actual2f57a0
    bool CheckDoubleClick(int); //actual2f57b0
    bool Contains(int,int); //2f5080 exact parent-coordinate bounding predicate55
    bool Intersects(Component*); //2f50c0 actual19B original rectangle intersection API
    Cursor* Fn2F4B00(); //2f4b00 external borrowed cursor getter14, support0
    BackgroundGrabber* GetGrabBG(); //2f4af0 borrowed extra+10
    void AllocateExtraStuff(); //2f4ac0 actual original external43B, no source allocation requested
    bool CheckGrabBG(); //2f4db0 actual native149B BG allocation policy
    void EnsureOnTopOf(Component*); //2f4c20 actual external wrapper to parent2f6f60
    void SetDefaultCursor(Cursor*); //2f4c60 actual own defaultCursor setter, DISTINCT virtual78
    void SetDesiredHeight(int); //2f4f20 actual62B, external
    void SetDesiredWidth(int=0); //2f4ee0 actual62B original external
    void SetTranslucentColor(int,int=4); //2f4fb0 actual51B, external
    void SetComponentFlags(int,bool); //2f5560 external40
    void Fn2F5360(bool); //2f5360 actual Component::SubscribeToBroadcast, nonvirtual
    void Fn2F57D0(bool); //2f57d0 external timer request
    void Fn2F5890();                        // 0x2f5890
    Window* GetRoot();               // 0x2f5920, legacy nominal name; true SDKGetWindow
    RootContainer* Fn2F58F0(); //2f58f0 actual SDKGetRoot parentwalk+RTDynamicCast, call-only genuine distinct method
    Window* Fn2F5940();              // 0x2f5940 (root Window or default Window)
    bool ComponentFlagSet(int flags) { return mComponentBits & flags ? true : false; }
    bool IsVisible() { return !ComponentFlagSet(1); }
    bool HasFocus() { return ComponentFlagSet(0x10); }
    bool IsDisabled() const { return (mComponentBits & 0x80) != 0; }      // (EELobbyColorButton 0x29060b: shr 7; test al, 1)

    const WONRectangle& GetBounds() { return mBounds; }
    int Left() { return mBounds.x; }
    int Top() { return mBounds.y; }
    int Width() { return mBounds.width; }
    int Height() { return mBounds.height; }
    int Right() { return mBounds.Right(); }
    int Bottom() { return mBounds.Bottom(); }
    Container* GetParent() { return mParent; } //genuine SDK borrowed accessor, actual Component+08
    Container* mParent;                     // +0x08 (SetEnabled notifies it through its slot +0xbc)
    std::list<SmartPtr<Component> >::iterator mChildListItr; // +0x0c, actual child-list node iterator
protected:
    WONRectangle mBounds; //10..1f, genuine SDK embedded Rectangle16 owner
public:
    int mComponentBits; //20, full DWORD flags (int: the tests compile to shr n; test 1); bit80 disabled
    int mControlId;                         // +0x24 (the component events' id)
    unsigned char mFocusId; //28
    char mInvalidState; //29
    char mTimerFlags; //2a
    ComponentExtra* mExtraStuff; //2c, original external unique ownership
};

// Actual SDK function-template API; surviving LayoutPoint* instantiation3a1da0.
// Call-only; dynamic cast/typed exception remain native, no local lifetime implementation.
template<class T> T SafeConfigGetComponent(Component*,const char*);
// Actual LayoutPoint30 owner: native389ac5 allocation, RTTI44fd9c, no own tail.
// Complete43slot geometry matches Component except true implicit basejmp destructor.
class LayoutPoint : public Component {
public:
    LayoutPoint(int left=0,int top=0,int width=0,int height=0) { Fn2F4BF0(left,top,width,height); }
    // Real SDK inline constructor/native389ae6..afe, ordinary306860 JMPComponent2f4a70.
};

// a component holding others (0x84 bytes: EELobbyGameOptionsCtrl's members start at +0x84; vtable 0x84d7cc, 51 slots)
// Complete20-byte manager: both real owning lists. Original lifetime stays external.
class ChildLayoutManager : public RefCount {
public:
    typedef std::list<SmartPtr<ChildLayoutInstructionBase> > InstructionList;
    virtual ~ChildLayoutManager(); // call-only original normal2f5da0, deleting2f5e60
    void ExecuteInstructions(Component*);
    void AddInstruction(ChildLayoutInstructionBase*);
    void AddLayoutPoint(LayoutPoint*);
    void RemoveComponent(Component*);
    void Clear();
    void EnableInstructions(int, bool);
    InstructionList mInstructions; //08
    std::list<SmartPtr<LayoutPoint> > mLayoutPoints; //14
};
class Container : public Component {
public:
    Container();                            // 0x2f5b70 (WONAPI, /GX)
    virtual ~Container();                   // 0x2f5e80 (WONAPI, /GX)
    // Real inherited overrides: targets in original vtable 0x44d7cc.
    virtual void Virtual08();               // 0x2f62b0
    virtual void Virtual0C(int& outWidth, int& outHeight); // 0x2f62d0
    virtual void Virtual14(Graphics& inGraphics); // 0x2f6340
    virtual void Paint(Graphics& inGraphics); // 0x2f63b0
    virtual bool Virtual20(int inX, int inY); // 0x2f6520
    virtual void Virtual24(int inX, int inY); // 0x2f6700
    virtual void Virtual28();               // 0x2f6730
    virtual void Virtual2C(int inX, int inY, int inButton); // 0x2f6880
    virtual void Virtual30(int inX, int inY, int inButton); // 0x2f6840
    virtual void Virtual34(int inX, int inY); // 0x2f6780
    virtual void Virtual38(int inX, int inY); // 0x2f6810
    virtual bool Virtual3C(int inWheel);      // 0x2f67c0
    virtual void Virtual40(unsigned long inDelta); // 0x2f67f0
    virtual bool Virtual44(int inDelta); // 0x2f7210
    virtual void Virtual48();               // 0x2f7400
    virtual void Virtual4C();               // 0x2f7420
    virtual bool Virtual50(int inKey);       // 0x2f7710
    virtual bool Virtual54(int inKey);       // 0x2f7690
    virtual bool Virtual58(int inChar);      // 0x2f76f0
    virtual bool Virtual5C();               // 0x2f7730
    virtual void Invalidate();              // 0x2f6980
    virtual void Virtual68();               // 0x2f6940
    virtual void Virtual6C();               // 0x2f6990
    virtual void Virtual70();               // 0x2f69a0
    virtual void Virtual78(Cursor* inCursor);    // 0x2f7360
    virtual void Virtual80(ComponentEvent* inEvent); // 0x2f7320
    virtual void Virtual84(bool inFlag);     // 0x2f64d0
    virtual bool Virtual88(bool inForward, bool inInit); // 0x2f75a0
    virtual void Virtual8C(const ControlIdSet& inIds, bool inEnabled); // 0x2f6480
    virtual Component* Virtual98() { return mMouseChild; } //genuine SDK borrowed GetMouseChild, native1c42ce/4          // 0x1c42ce
    // 0x2f6140 (ret 0x20): adds inChild laid out against inRef (EELobbyInterface 0x2972c6: the lobby screen, 3,
    // the root, 0, 0, 0, 0, 0; r24pre-won PROVISIONAL)
    void Fn2F6140(Component* inChild, int inFlags, Component* inRef, int inA, int inB, int inC, int inD, int inE);
    void AddChildLayout(ChildLayoutInstructionBase*,int=0); //2f61d0 actual63B retained instruction; id0 executes first, fullint gate/lowBYTE id
    void Fn2F6120(int inId, bool inEnabled); // 0x2f6120
    void Fn2F6290();                         // 0x2f6290
    void EnterModal(); //2f6210 actual external counter/focus policy
    void LeaveModal(); //2f6260 actual external counter/focus policy
    void RemoveAllChildren(); //2f6d80 actual external, ManagerEndPopup32e663/67c
    void Fn2F6650(int,int); //2f6650 mouse-selection transition99
    void MouseAt(); //2f66c0 actual57B noarg mouse-state refresh, call-only
    std::list<SmartPtr<Component> >::iterator FindTop(); //2f6a40 forwarding27 support0
    std::list<SmartPtr<Component> >::iterator FindTop(std::list<SmartPtr<Component> >::iterator); //2f69d0 real suffix ordering99
    void SetOnlyChildInput(bool v) { mOnlyChildInput=v; } //actual byte7c
    void SetIsolateTabFocus(bool v) { mIsolateTabFocus=v; } //actual SDK inline/native389f3b byte7a
    void AddLayoutPoint(LayoutPoint*); //2f6100 actual19B, external retained child owner
    void AddChildEx(Component*,int); //2f6b50 external266
    void BringToTop(Component*); //2f6dd0 external390
    void SetHasUsedDialogKeys(bool v) { mHasUsedDialogKeys=v; } //real inline byte7b
    unsigned char GetNextFocusId(); //2f6010 actual18B primitiveaccessor support0
    void SetFocusToLastFocusChild(); //2f6030 actual196B
    void EnsureOnTopOf(Component*,Component*); //2f6f60 actual352B
    void Fn2F6A60(Component* inChild);      // 0x2f6a60
    virtual Component* VirtualAC(int inX, int inY); // 0x2f65b0
    virtual Component* VirtualB0(int inX, int inY) { return VirtualAC(inX, inY); } // 0x2909c7
    virtual void VirtualB4(Component* inChild);
    virtual void VirtualB8(Component* inChild);
    virtual void VirtualBC(Component* inChild);
    virtual bool VirtualC0(Component* inChild);
    virtual void VirtualC4(Component* inChild);
    virtual Component* VirtualC8(int inControlId);

    // Actual private prefix: ctor2f5b70, dtor2f5e80, readers2f62d0/2f7770.
    // Remaining private lifetimes stay in the original external ctor/dtor.
    Component* mMouseChild;                // +0x30, borrowed
    Component* mFocusChild;                // +0x34, borrowed
    std::list<SmartPtr<Component> > mChildList; // +0x38, owning values
    typedef std::set<SmartPtr<Component>,SmartPtr<Component>::Comp> ChildTimerSet;
    ChildTimerSet mChildTimerSet; //44..4f actual owning SDKChildSet, ctor/dtor/insert/erase proven
    ChildLayoutManager mChildLayout; //50..6f actual full20 owning manager, original ctor/dtor retained
    unsigned char mNextFocusId; //70, native2f6010 BYTEincrement/zerowrap; natural71..73padding
    SmartPtr<Cursor> mCurrentCursor; //74, ctor2f5c45/dtor2f5ed2/Root304a51
    bool mChildNeedsTimer; //78
    bool mMouseWheelGoesToFocusChild; //79
    bool mIsolateTabFocus; //7a, nativeRoot30420f
    bool mHasUsedDialogKeys; //7b, actual native Tab writes; NOT isolateTabFocus7a
    bool mOnlyChildInput; //7c
    bool mRemovingAllChildren; //7d nativeRemoveAllChildren write/RemoveChild read; natural7e/7f padding
    int mModalCount; //80, actual signed modal-child gate
protected:
    static bool mMouseWheelGoesToFocusChildDef; //original566234 mutable bool, declaration only
};

// Actual ScreenContainer88; external lifetime306a20/306a60, borrowed current screen.
class ScreenContainer : public Container {
public:
    ScreenContainer();
    // Natural implicit destructor: borrowed84 only; actual306a60 JMPContainer2f5e80.
    void ShowScreen(Component* inScreen, bool inTakeFocus = true); //306ab0
    void AddScreen(Component*); //306a70
    Component* mCurScreen; //84, borrowed
};


// Actual layout family; six concrete owners have bounded constructor/cleanup proof.
// Other layout receiver views retain reference-only authority; no blanket allocation grant.
class ChildLayoutInstructionBase : public RefCount {
public:
    ChildLayoutInstructionBase() : mGroupId(0),mEnabled(true) {} //actual SDK inline/native allsix constructors, support0
    virtual void Execute(Component*) {} // genuine SDK inline default, native30a800/3
    virtual bool UsesComponent(Component*) { return false; } // genuine SDK inline default, native3a90d0/5
    unsigned char mGroupId;                 // +08
    bool mEnabled;                         // +09
};
// SOURCE_ONLY_NO_NATIVE_ID: same-snapshot nonpolymorphic value owner, owns nodes and borrows Components.
class ChildLayoutList
{
protected:
	typedef std::list<Component*> ChildList;
	mutable ChildList mChildList;
	mutable ChildList::iterator mItr;

public:
	ChildLayoutList(Component *c1);
	ChildLayoutList(Component *c1, Component *c2);
	ChildLayoutList(Component *c1, Component *c2, Component *c3);
	ChildLayoutList(Component *c1, Component *c2, Component *c3, Component *c4);
	ChildLayoutList(Component *c1, Component *c2, Component *c3, Component *c4, Component *c5);
	ChildLayoutList(Component *c1, Component *c2, Component *c3, Component *c4, Component *c5, Component *c6);
	ChildLayoutList(Component *c1, Component *c2, Component *c3, Component *c4, Component *c5, Component *c6, Component *c7);
	ChildLayoutList(Component *c1, Component *c2, Component *c3, Component *c4, Component *c5, Component *c6, Component *c7, Component *c8);

	void Rewind() const;
	bool HasMore() const;
	Component* GetNext() const;

	void AddTo(ChildList &theList) const;
};
class CenterLayout : public ChildLayoutInstructionBase {
public:
    CenterLayout(Component* box,bool horz,bool vert) : mHorz(horz),mVert(vert),mCenterBox(box) {} //actual388f6d..fb3 full20 owner
    CenterLayout(Component* box,Component* first,bool horz,bool vert) : mHorz(horz),mVert(vert),mCenterBox(box) { Add(first); } //actual SDK/native3624e8..530, cleanup covers first insertion
    CenterLayout(Component* box,const ChildLayoutList& list,bool horz,bool vert) : mHorz(horz),mVert(vert),mCenterBox(box) { Add(list); }
    CenterLayout* Add(const ChildLayoutList& list) { list.AddTo(mComponentList); return this; }
    CenterLayout* Add(Component* component) { mComponentList.push_back(component); return this; }
    static void Center(Component* inParent, Component* inChild, bool inHorz, bool inVert); // 337690
    virtual void Execute(Component*);
    bool mHorz;                            // +0c
    bool mVert;                            // +0d
    Component* mCenterBox;                  // +10, borrowed
    std::list<Component*> mComponentList;   // +14, owns nodes, borrows components
};

// a modal or popup dialog (0x98 bytes: EELobbyConnectDialog's members start at +0x98; its implicit dtor is the
// lobby glue's COMDAT 0x290a26)
class Dialog : public Container {
public:
    virtual void Virtual08();               // 0x2f7a60
    virtual void Virtual28();               // 0x2f7b20
    virtual void Virtual2C(int inX, int inY, int inButton); // 0x2f7a70
    virtual void Virtual30(int inX, int inY, int inButton); // 0x2f7af0
    virtual void Virtual38(int inX, int inY); // 0x2f7b40
    virtual void Virtual48();               // 0x2f7a20
    virtual void Virtual4C();               // 0x2f7a40
    virtual bool Virtual54(int inKey);       // 0x2f7c30
    virtual bool Virtual5C();               // 0x2f7c90
    Dialog();                               // 0x2f7910 (WONAPI)
    void SetEndOnEscape(int); //2f7990 actual20B external
    void SetDontEndOnEscape(); //2f79b0 actual8B external
    void SetDefaultButton(Button*); //2f79c0 real retained Button94 owner external
    bool ActivateDefaultButton(); //2f7ce0 actual51B guarded defaultbutton dispatch, support0
    virtual void HandleComponentEvent(ComponentEvent* inEvent);    // +0x7c 0x2f7d20 (WONAPI)

    int mDragX;                            // +0x84
    int mDragY;                            // +0x88
    bool mB8C;                             // +0x8c, dragging; original ctor does not initialize
    bool mAllowDrag;                       // +0x8d
    bool mKeepInRoot;                      // +0x8e
    bool mEndOnEscape;                     // +0x8f
    int mEscapeId;                         // +0x90
    SmartPtr<Button> mC94;                  // +0x94, actual default Button owner
};

class Button : public Component {
public:
    void Fn2F3440(); //actual enabled/checked click activation, noargs
    Button();                               // 0x2f3070 (WONAPI)
    virtual ~Button();                      // 0x2f3100 (WONAPI)
    virtual bool VirtualAC();               // +0xac (a click; EELobbyColorButton 0x290669)
    virtual void VirtualB0(const GUIString&); //actual SetTextHook2f30d0
    virtual void SetText(const GUIString& inText, bool inFlag);    // +0xb4 (0x2f3180)
    virtual const GUIString& GetText() { return gEmptyGUIString; }   // +0xb8 0x2905a8
    void SetCheck(bool inChecked, bool inNotify = false);      // 0x2f3500
    void Fn2F3370(unsigned long inFlags, bool inSet);          // 0x2f3370 (EELobbyColorButton: 0x40, true)

    union {
        unsigned long mButtonFlags; //30 actual SDK flags
        struct {
    unsigned long mButtonFlag0 : 1;         // +0x30 bits (Button::SetCheck 0x2f3500 tests bit 3)
    unsigned long mButtonFlag1 : 1;
    unsigned long mChecked : 1;             // +0x30 bit 2 (a check box's state)
    unsigned long mButtonFlag3 : 1;
    unsigned long mButtonFlag4 : 1;
    unsigned long mButtonFlag5 : 1;         // +0x30 bit 5 (EELobbyColorButton 0x29060b draws it pressed)
        };
    };
    // the flag tests read one bit through an inline bool getter (shr n; test 1: Button::Fn2F3370 0x2f3370)
    bool IsRepeat() { return (mButtonFlags & 0x02) != 0; }
    bool IsChecked() { return (mButtonFlags & 0x04) != 0; }
    bool IsRadio() { return (mButtonFlags & 0x08) != 0; }
    bool IsPressed() { return (mButtonFlags & 0x20) != 0; }
    bool IsActivateOnPress() { return (mButtonFlags & 0x40) != 0; }
    bool IsNoActivate() { return (mButtonFlags & 0x200) != 0; }
    bool IsDefaultOnFocus() { return (mButtonFlags & 0x400) != 0; }
    bool mKeyDown; //34
    int mDelayAcc; //38
    SmartPtr<Sound> mClickSound; //3c complete external Sound lifetime
    unsigned char mMouseDownState; //40, 0=up 1=downoutside 2=downover
    static SmartPtr<Button> mDefaultButton; //native966224
    static SmartPtr<Button> mDialogDefaultButton; //native966228
    static void SetDialogDefaultButton(Button*); //2f31c0
    static void UnSetDialogDefaultButton(Button*); //2f3230
    static void SetDefaultButton(Button*); //2f3280
    static void UnSetDefaultButton(Button*); //2f3300
    void CopyAttributes(Button*); //2f3310
    void ButtonDown(); //2f3660
    void ButtonUp(bool); //2f36d0
    Button* GetNextRadio(bool); //2f34a0
    Button* GetEndRadio(bool); //2f34e0
    bool IsMouseOver(); //2f3170
    bool IsDefaultButton(); //2f31b0
    void SetClickSound(Sound*); //2f3910
    virtual void Virtual6C(); //2f33c0 AddedToParent
    virtual void Virtual24(int,int); //2f35b0 MouseEnter
    virtual void Virtual28(); //2f3620 MouseExit
    virtual void Virtual2C(int,int,int); //MouseDown
    virtual void Virtual30(int,int,int); //MouseUp
    virtual void Virtual38(int,int); //2f3740 MouseDrag
    virtual void Virtual48(); //2f37a0 GotFocus
    virtual void Virtual4C(); //2f37d0 LostFocus
    virtual bool Virtual58(int); //2f38b0 actual release-key slot58
    virtual bool Virtual54(int); //KeyDown
    virtual bool Virtual44(int); //TimerEvent
    virtual void Virtual40(unsigned long); //2f3900 Hover, forwarding support0
};

// Borrowed native32-slot LineSegment14 prefix; no allocation, construction,
// sizeof or destructor implementation from these partial rich-text contracts.
class MultiLineBuilder;
class ScrollAreaViewport;
struct ElementSelectionInfo;
struct ElementFindInfo;
enum MouseButton { MouseButton_Left=0,MouseButton_Middle=1,MouseButton_Right=2 };
struct ElementPaintInfo {
    int mClipX,mClipY,mClipWidth,mClipHeight;
    int mLineY,mLineHeight;
    int mStartSelSegment,mEndSelSegment,mStartSelSegmentPos,mEndSelSegmentPos;
    ElementPaintInfo() : mLineY(-1),mLineHeight(-1) {}
};
class Table;
class LineSegment : public RefCount {
public:
    bool DrawSelection1(Graphics&,int,int,ElementPaintInfo&); //309570
    void DrawSelection2(Graphics&,int,int); //3095e0
    static SmartPtr<Image> mSelectImageMask; //actual566220 original global owner; no definition here
    static int mCounter; //actual566240 original retained global; no definition here
    static Table* mTableContext; //actual56623c retained original context cell; declaration only
    // Actual SDK natural implicit destructor, inherited RefCount virtual slot0.
    // No owned base fields; TextPiece311480 -> generic338c70 normal cleanup.
    // No fabricated ordinary external LineSegment destructor or empty stub.
    virtual int GetWidth(); //04
    virtual int GetMinWidth(); //08
    virtual int GetMaxWidth(); //0c
    virtual int GetHeight(); //10
    virtual int GetAscent(); //14
    virtual int GetFirstWrapWidth(); //18
    virtual char GetVertAlign(); //1c
    virtual int GetStartSelIndex(); //20
    virtual int GetEndSelIndex(); //24
    virtual void MouseEnter(int,int); //28
    virtual void MouseExit(); //2c
    virtual void MouseDown(int,int,MouseButton); //30
    virtual void MouseUp(int,int,MouseButton); //34
    virtual void MouseMove(int,int); //38
    virtual void MouseDrag(int,int); //3c
    virtual void LinkDown(); //40
    virtual void LinkUp(); //44
    virtual bool CanStartSelection(); //48
    virtual int GetSelChar(int,int); //4c
    virtual bool GetSelCharPos(int&,int&,int,int); //50
    virtual void GetSelText(GUIString&,ElementSelectionInfo&); //54
    virtual bool FindStr(ElementFindInfo&); //58
    virtual void SetSelIndex() { mStartSelIndex=mCounter++; } //5c
    virtual LineSegment* GetSegmentAt(int&,int&,bool); //60
    virtual bool IsInvalid(); //64
    virtual bool AllowModalInput(); //68
    virtual bool PerformInstructions(MultiLineBuilder*); //6c
    virtual void Paint(Graphics&,int,int,ElementPaintInfo&); //70
    virtual void Wrap(MultiLineBuilder*,bool); //74, target excludes later SDK PaintSelection
    virtual void SetMaxWidth(int); //78
    virtual void SetViewport(ScrollAreaViewport*,int,int); //7c
    int mStartSelIndex,mAbsX,mAbsY; //08,0c,10 borrowedprefix
};
class LineElement;
// Actual floating record10, two virtual dimension queries (no virtual dtor).
// Native floating vector stride10 at30cbb5/30cd35/30d7c8; retained segment4.
struct FloatingSegment {
    SmartPtr<LineSegment> mSegment; //04, vptr00
    int mx,my; //08,0c
    FloatingSegment(LineSegment* segment=NULL,int x=0,int y=0):mSegment(segment),mx(x),my(y) {}
    virtual int GetWidth() { return mSegment->GetWidth(); }
    virtual int GetHeight() { return mSegment->GetHeight(); }
};
// Actual complete8 segment-list value, native node payload8/flags0c.
struct SegmentInfo {
    SmartPtr<LineSegment> mSegment;
    unsigned short mFlags;
    SegmentInfo(LineSegment* segment=NULL,unsigned short flags=0):mSegment(segment),mFlags(flags) {}
};
// Actual complete68 MultiLineElement owner; native original constructor/dtor providers.
// Allocation via the genuine external constructor is allowed; no new lifetime bodies or copy authority.
class MultiLineElement : public LineSegment {
public:
    MultiLineElement(); //30c7e0 external actual259B complete68 constructor
    virtual int GetWidth(); //30c8f0 external getter
    void CalcMinMaxWidths(); //genuine SDK explicit empty hook, contextual native4142a0 from actual30d000/30d020
    virtual int GetMinWidth(); //30d000 external getter/support0
    virtual int GetMaxWidth(); //30d020 external getter/support0
    virtual int GetHeight(); //30c900 external getter
    virtual LineSegment* GetSegmentAt(int&,int&,bool); //30ca30
    virtual void SetViewport(ScrollAreaViewport*,int,int); //30cb60
    virtual void GetSelText(GUIString&,ElementSelectionInfo&); //30cc50
    virtual bool GetSelCharPos(int&,int&,int,int); //30ccd0
    virtual bool FindStr(ElementFindInfo&); //30cd70
    virtual void SetSelIndex(); //30cbf0
    virtual int GetEndSelIndex(); //30cc30 external getter/support0
    bool IsLastLineEmpty(); //genuine SDK segment tail flag query; SOURCE_ONLY_NO_NATIVE_ID
    virtual void Paint(Graphics&,int,int,ElementPaintInfo&); //30d6e0
    virtual void SetMaxWidth(int); //30d1d0 orchestration support0; complete80 original builder lifecycle
    void AddSegment(LineSegment*,unsigned short=0); //30d570
    typedef std::vector<FloatingSegment> FloatingVector;
    typedef std::vector<SmartPtr<LineElement> > LineVector;
    typedef std::list<SegmentInfo> SegmentList;
    LineVector::iterator FindLineAt(int); //30d040 actual original binary-search provider
    FloatingSegment* FindFloatingSegmentAt(int,int,bool); //30d0c0 actual original provider
    FloatingVector mFloatingItems[2]; //14..33
    LineVector mLineVector; //34..43
    SegmentList mSegmentList; //44..4f
    SmartPtr<LineElement> mSearchLine; //50 retained original-owned search sentinel
    int mWidth,mHeight,mMinWidth,mCurLineWidth,mMaxWidth; //54,58,5c,60,64
};
// Complete target30 WrappedText owner; preserves earlier public8/c fields.
class WrappedText : public RefCount {
public:
    GUIString mText; //08
    SmartPtr<Font> mFont; //0c
    SmartPtr<MultiLineElement> mRichText; //10; native external virtual lifetime
protected:
    struct BreakInfo {
        unsigned short mBreakPos,mNextStartPos,mWidth;
        BreakInfo(int p,int n,int w) : mBreakPos((unsigned short)p),mNextStartPos((unsigned short)n),mWidth((unsigned short)w) {}
    };
    typedef std::list<BreakInfo> BreakList;
    BreakList mBreakList; //14..1f
    int mWidth,mHeight,mMaxWidth; //20,24,28
    bool mWrap,mIsCentered; //2c,2d complete30
    void Init(); //34a7c0
    void CalcWrap(); //34a9b0
public:
    WrappedText(); //34a7e0
    WrappedText(const GUIString&); //genuine SDK explicit text constructor; SOURCE_ONLY_NO_NATIVE_ID
    void SetText(const GUIString&); //34acc0
    void SetWrap(bool); //34ad20
    void SetCentered(bool); //34ad40
    void SetFont(Font*); //34ad50
    void SetMaxWidth(int); //34adb0
    void Paint(Graphics&,int,int); //34adf0
    int GetWidth() { return mWidth; }
    const GUIString& GetText() { return mText; } //genuine SDK inline, borrowed reference to owned08
    int GetHeight(); //actual SDK inline, defined after complete Font
};
// Complete actualMSButtonBase5c owner: inheritedButton44, WrappedSP44, scalar48..58/pad59..5b.
// Separate derived MS controls remain borrowed; no by-value copy implementation.
class MSButtonBase : public Button {
public:
    MSButtonBase(); //2fe6e0 actualfullconstructor, newWrappedText30
    virtual ~MSButtonBase(); //normal2fe910, original external lifetime
    virtual void Virtual08(); //2fea10
    virtual void Virtual0C(int&,int&); //2fe8c0
    virtual void VirtualB0(const GUIString&); //2fe880 real SetTextHook
    virtual const GUIString& GetText(); //2fe850 real B8
    void SetTransparent(bool); //2fe8a0
    void SetAdjustHeightToFitText(bool); //2fe9f0
    void AdjustHeightToFitText(); //2fe980
    void DrawButtonText(Graphics&,int,int); //2fea40
    void SetFont(Font* f) { mText->SetFont(f); }
    void SetTextColor(int c) { mTextColor=c; }
    void SetStandardTextColor(StandardColor c) { mStandardTextColor=c;mTextColor=-1; }
    void SetWrap(bool b) { mText->SetWrap(b); }
    void SetHorzAlign(HorzAlign h) { mHAlign=(unsigned char)h;mText->SetCentered(mHAlign==HorzAlign_Center); }
    void SetVertAlign(VertAlign v) { mVAlign=(unsigned char)v; }
    enum FocusStyle { FocusStyle_None,FocusStyle_AroundWindow,FocusStyle_AroundText };
    void SetFocusStyle(FocusStyle s) { mFocusStyle=(unsigned char)s; }
    void SetHorzTextPad(int l,int r) { mLeftTextPad=(unsigned char)l;mRightTextPad=(unsigned char)r; }
    void SetVertTextPad(int t,int b) { mTopTextPad=(unsigned char)t;mBottomTextPad=(unsigned char)b; }
    void IncrementHorzTextPad(int l,int r) { mLeftTextPad=mLeftTextPad+(unsigned char)l;mRightTextPad=mRightTextPad+(unsigned char)r; }
    void IncrementVertTextPad(int t,int b) { mTopTextPad=mTopTextPad+(unsigned char)t;mBottomTextPad=mBottomTextPad+(unsigned char)b; }
    SmartPtr<WrappedText> mText; //44, original external owner lifetime
    StandardColor mStandardTextColor; //48
    int mTextColor; //4c
    unsigned char mHAlign,mVAlign,mFocusStyle; //50/51/52
    unsigned char mTopTextPad,mBottomTextPad,mLeftTextPad,mRightTextPad; //53..56
    bool mIsTransparent,mAdjustHeightToFitText; //57/58
};
// Actual SDK utility class: static call contracts only, no instances/lifetime.
class MSGraphicsUtil {
public:
    static void DrawArrow(Graphics&,Direction,int,int,int,int,bool); //2fdfb0
    static void DrawFrame3(Graphics&,int,int,int,int,bool); //2fdee0
    static void DrawTab(Graphics&,int,int,int,int); //2fe4d0
    static void DrawCheck(Graphics&,int,int,int,int,bool,bool); //2fe250
    static void DrawRadio(Graphics&,int,int,int,int,bool,bool); //2fe3b0
    static void DrawCloseX(Graphics&,int,int,int,int); //2fe5e0
};
// Complete actual MSButton60; genuine explicit SDK constructors now declared.
// Ordinary reviewed setup Source may construct this owner; destructor remains external.
class MSButton : public MSButtonBase {
public:
    MSButton(); //2fec30 actual complete60 default constructor, original external
    MSButton(const GUIString&); //2feca0 actual complete60 text constructor, RET4, original external
    virtual ~MSButton(); //2fed10 original external
    virtual void Paint(Graphics&); //2fed20
    void SetUseFrame2(bool useFrame2) { mUseFrame2=useFrame2; } //genuine SDK inline API
protected:
    void Init(); //2febf0 genuine SDK explicit setup helper
public:
    bool mUseFrame2; //5c, native2fec1e/2fed75; complete size60 externally
};
// Borrowed actual5c control; original external lifetime, no constructors/allocation/copy/sizeof.
class MSCheckbox : public MSButtonBase {
public:
    MSCheckbox(); //2ff3e0 genuine explicit SDK constructor, fieldless5c
protected:
    void Init(); //2ff3b0 genuine SDK helper
public:
    virtual ~MSCheckbox(); //normal2ff420 originalexternal
    virtual void Paint(Graphics&); //2ff470 actual18
    typedef void (*CheckboxPainterFunc)(Graphics&,MSCheckbox*);
    static CheckboxPainterFunc CheckboxPainter;
    static int mCheckboxWidth,mCheckboxHeight;
    static void DefaultCheckboxPainterFunc(Graphics&,MSCheckbox*); //2ff430 support0
};
// Complete actual fieldless5c MSRadioButton owner through original external ctor.
// No new constructor/destructor/copy source bodies.
class MSRadioButton : public MSButtonBase {
public:
protected:
    void Init(); //genuine SDK helper, naturally expanded native2ff530
public:
    MSRadioButton(); //2ff530 actual95B external constructor
    virtual ~MSRadioButton(); //normal2ff5b0 originalexternal
    virtual void Paint(Graphics&); //2ff600 actual18
    typedef void (*RadioPainterFunc)(Graphics&,MSRadioButton*);
    static RadioPainterFunc RadioPainter;
    static int mRadioWidth,mRadioHeight;
    static void DefaultRadioPainterFunc(Graphics&,MSRadioButton*); //2ff5c0 support0
};

// Complete actual MSArrowButton48, full Button44 +Direction44; external lifetime only.
class MSArrowButton : public Button {
public:
    MSArrowButton(Direction=Direction_Up); //2ff0c0 actual external86B/RET4
    virtual ~MSArrowButton(); //original external lifetime
    virtual void Paint(Graphics&); //actual18
    void SetDirection(Direction d) { mDirection=d; }
    Direction mDirection; //44
};
// Complete fieldless SDK owner48; native ctor3c9590, Paint3c9620,
// VT4541f4, RTTI MaxSplitterButton>MSArrowButton>Button>Component>RefCount.
// Destructor is genuinely implicit; native3c9610 tails MSArrowButton2ff140.
class MaxSplitterButton : public MSArrowButton {
public:
    MaxSplitterButton(Direction=Direction_Up);
    virtual void Paint(Graphics&);
};
// Complete actual MSTabButton60, full MSButtonBase5c +intLowerAmount5c; external lifetime only.
class MSTabButton : public MSButtonBase {
public:
    MSTabButton(const GUIString& = "",bool=true,int=3); //genuine SDK defaults; existing2fee20/135 textctorRET12

    virtual void Paint(Graphics&); //actual18
    virtual void Virtual0C(int&,int&); //actual0c
    void SetLowerAmount(int n) { mLowerAmount=n; }
    int mLowerAmount; //5c
};
// Complete actual MS3DFrame38, Component30 +bool30/int34; external lifetime only.
class MS3DFrame : public Component {
public:
    MS3DFrame(int=3,bool=false); //300780 actual original48B/RET8
    virtual void Paint(Graphics&); //actual18,3007b0
    void SetIsUp(bool v) { mIsUp=v; }
    void SetFrameType(int t) { mType=t; }
    bool mIsUp; //30
    int mType; //34
};

class Label : public Component {
public:
    Label(); //2f9d80, complete actual68 owner, native new68 at300908
    virtual ~Label(); //2f9f50, actual Background+WrappedText owner cleanup
    virtual void Virtual08(); //2fa020 SizeChanged
    virtual void Virtual0C(int&,int&); //2fa140 GetDesiredSize
    virtual void Paint(Graphics&); //2fa1a0
    void SetMaxWidth(); //2f9f20
    void AdjustHeightToFitText(); //2fa090
    void SetText(const GUIString& inText, bool inFlag = true); // 0x2fa060
    void SetHorzAlign(HorzAlign); //2fa040 native external
    void SetWrap(bool); //2fa120 native external
    void SetAdjustHeightToFitText(bool); //2fa100 native external
    void SetVertPad(int inTop, int inBottom) { mTopPad=inTop; mBottomPad=inBottom; }
    void SetFont(Font* theFont) { mWrappedText->SetFont(theFont); }
    void SetTextColor(int theColor) { mColor=theColor; }
    void SetBackground(const Background& theBackground) { mBackground=theBackground; }
    void SetVertAlign(VertAlign theAlign) { mAlign3C=theAlign; }
    SmartPtr<WrappedText> mWrappedText;      // +30
    int mColor;                            // +34
    int mAlign38;
    int mAlign3C;
    int mLeftPad;                          // +40
    int mRightPad;                         // +44
    int mTopPad;                           // +48
    int mBottomPad;                        // +4c
    bool mAdjustHeightToFitText; //50, naturalpad51..53
    Background mBackground; //54..67 borrowed actual tail; no lifetime implementation
};




// Actual SelectionColor0c, native read-only color helpers; no new lifetime bodies.
enum ForegroundSelectionMode {
    ForegroundSelectionMode_UseOriginalColor,
    ForegroundSelectionMode_InvertOriginalColor,
    ForegroundSelectionMode_UseForegroundColor
};
class SelectionColor {
public:
    SelectionColor(); //3494a0, back80000007/fore80000008/mode2
    SelectionColor(DWORD,ForegroundSelectionMode = ForegroundSelectionMode_InvertOriginalColor); //3494c0, foreFFFFFF
    SelectionColor(DWORD,DWORD); //3494e0, mode2
    unsigned long GetBackgroundColor(Graphics&) const; //349500
    unsigned long GetForegroundColor(Graphics&, unsigned long) const; //349510
    unsigned long mBackgroundColor;
    unsigned long mForegroundColor;
    ForegroundSelectionMode mMode;
};

// Complete actual InputBoxec owner: true Background14 values c4/d8 endec.
// Original constructor2f7fd0/destructor2f8270 retain lifecycle bodies.
// Allocation through original ctor is proven; no new copy or lifetime implementation.
class InputBox : public Component {
public:
    typedef bool (*InputFilter)(InputBox*,GUIString&);
    static bool NumericFilter(InputBox*,GUIString&); //2f7f70 cdecl
    InputBox(); //external2f7fd0; no definition in this cohort
    // Genuine SDK implicit virtual destructor2f8270; automatic real owned members and base cleanup.
    const GUIString& GetText() const; //2f9960 actual SDK const
    void SetText(const GUIString&,bool); //2f9990
    void Clear(bool); //2f99c0
    void Fn2F9140(const GUIString&,bool); //actual AddInputHistory2f9140; preserved existing spelling
    void SetInputHistory(const GUIString&,bool=false); //2f92f0
    void Fn2F84B0(bool); //actual SetAllowSelection2f84b0, existing spelling
    void SelectRange(int,int); //actual SetSel2f8aa0, existing spelling
    void AdjustTextRect(); //2f8400
    void SetIcon(Image*); //2f8450
    void SetCharWidth(int); //2f8560
    void CheckSel(int,int,bool); //2f8600
    void SetCursorBlinkRate(int,int=0); //2f87d0
    void AdjustPosition(); //2f8a20
    void CopySelection(); //external2f8b10 until Clipboard contract
    void SetSelText(const GUIString&,bool=true); //2f8e10
    int MoveCursor(bool,bool,const GUIString&,int); //2f90c0
    void SetPasswordChar(char); //2f9970
    void SetTextPrv(const GUIString&,bool=true); //external2f99e0 until Fill helper
    virtual void Virtual08(); //2f8510
    virtual void Virtual0C(int&,int&); //2f8530
    virtual void Virtual48(); //2f8590
    virtual void Virtual4C(); //2f85d0
    virtual void Virtual2C(int,int,int); //2f8750
    virtual void Virtual30(int,int,int); //2f8790
    virtual void Virtual28(); //2f87a0
    virtual void Virtual38(int,int); //2f87b0
    virtual bool Virtual44(int); //2f8800
    virtual bool Virtual50(int); //external KeyChar2f8c30 until Clipboard contract
    virtual bool Virtual54(int); //2f93b0
    virtual void Paint(Graphics&); //external2f9ac0 until Background owner contract
    GUIString mText; //30 display
    GUIString mPasswordText; //34 actual logicaltext when password
    char mPassword; //38 actual passwordCHAR; previous bool was incomplete
    SmartPtr<Image> mIcon; //3c
    InputFilter mInputFilter; //40 actual mutable transform
    std::deque<GUIString> mInputHistory; //44..57; actual Dinkum23320B allocator/map/mapsize/myoff/mysize
    int mInputHistoryPos; //58
    int mMaxInputHistory; //5c
    bool mHaveTraversedHistory; //60
    bool mBackwardsSel; //61
    int mSelStartChar; //64
    int mSelEndChar; //68
    int mHorzOffset; //6c
    bool mSelecting; //70
    int mLeftPad,mTopPad,mRightPad,mBottomPad; //74,78,7c,80
    int mCursorDelay,mCursorOnTime,mCursorOffTime; //84,88,8c
    bool mCursorOn,mUnderlineCursor,mAllowSelection,mSelectOnFocus,mUseDisabledColors; //90..94
    int mMaxChars; //98
    SmartPtr<Font> mFont; //9c
    unsigned long mTextColor; //a0 existing gameconsumer
    int mDisabledTextColor; //a4
    SelectionColor mSelColor; //a8..b3 complete scalarcolor contract
    WONRectangle mTextRect; //b4..c3 complete real rectangle
    Background mBackground; //c4..d7 actual external-owned Background20
    Background mDisabledBackground; //d8..eb actual external-owned Background20
};

class StretchImage;
// Actual nonpolymorphic14 record: four border ints then ImageSP10.
// Complete actual nonpolymorphic14 value: borders0..c and sole retained ImageSP10.
// Natural destructor releases ImageSP10. Original real constructors; genuine SDK out-of-line deep-copy constructor declared below.
class WONFrame {
public:
    int mLeft,mTop,mRight,mBottom;
    SmartPtr<Image> mImage;
    WONFrame(Image* =NULL,int=0); //3175f0 actual uniform-border ret8
    WONFrame(Image*,int,int,int,int); //317690 actual five-arg ret14
    WONFrame(StretchImage*); //317550 actual Stretch ret4
    WONFrame(const WONFrame&); //genuine SDK out-of-line deep-copy constructor; support0 source, no guessed native entry
    WONFrame& operator=(const WONFrame&); //3177a0 genuine original assignment provider
    void Copy(const WONFrame&); //3177c0 actual deep image Duplicate, not SP sharing
    void SetImage(Image*); //genuine SDK assignment; shared349ab0 already mapped
    void SetImage(Image*,int,int,int,int); //317730 genuine SDK assignment and borders
    void Paint(Graphics&,Component*); //3178c0
    void Paint(Graphics&,Component*,int,int,int,int); //317910
};
// Full native WONFrameComponent44, actual Component30 plus genuine WONFrame14.
// F allocation authority is bounded to reviewed WONMSSkin composition using original constructor.
class WONFrameComponent : public Component {
public:
    WONFrameComponent(Image* =NULL,int=0,int=0,int=0,int=0); //31a5b0 actual RET14, original external
    WONFrameComponent(const WONFrame&); //31a740 genuine SDK frame-copy constructor RET4
    const WONFrame& GetFrame(void) { return mFrame; } //genuine SDK inline accessor
    // Genuine SDK implicit virtual destructor; full Frame/ImageSP40 then Component cleanup.
    virtual void Paint(Graphics& inGraphics); // actual inherited override
    virtual void VirtualAC(Image*, int, int, int, int);
    virtual void VirtualB0(Image*);
    virtual void VirtualB4(int, int, int, int);
    virtual void VirtualB8(int);
    virtual void VirtualBC(int);
    virtual void VirtualC0(int);
    virtual void VirtualC4(int);
    virtual int VirtualC8();
    virtual int VirtualCC();
    virtual int VirtualD0();
    virtual int VirtualD4();
    WONFrame mFrame; //30..43 real14 aggregate; native31a5d6/31a6ef, retained ImageSP40
};
// Actual complete ImageComponent38 owner,45 slots and ImageSP30/tile34/stretch35.
// Constructor/destructor remain genuine originals; new authority only fully-proven callers.
class ImageComponent : public Component {
public:
    ImageComponent(Image* = NULL,bool = false); //306b10 actual full38 original external constructor
    // Genuine SDK implicit virtual destructor: owned ImageSP30 then Component base; no authored lifetime body.
    virtual void Virtual0C(int&, int&);      // 306cf0
    virtual void Paint(Graphics&);          // 306d30
    virtual void VirtualAC(Image*, bool);   // 306c80, ret8
    virtual void VirtualB0(Image*);          // 306c10, ret4
    void SetTile(bool doTile) { mTile = doTile; }
    void SetStretch(bool doStretch) { mStretch = doStretch; }
    bool GetTile() { return mTile; }
    bool GetStretch() { return mStretch; }
    Image* GetImage() { return mImage.get(); }
    SmartPtr<Image> mImage; //30 real retained owner, borrowed control facade only
    bool mTile,mStretch; //34,35 actual config394da6/394dd8
};

// a drop-down list (vtable 0x84d638; its list at +0x90)
// Scratch complete actual TabBar a0 / TabCtrl d8; GUI owner must review/migrate before use.
class TabCtrl;
class LayoutPoint;
class TabBar : public Container {
public:
    TabBar(TabCtrl*);
    // Natural implicit normal3081f0: no own-vptr store, vector owns Button refs;0quota.
    virtual void HandleComponentEvent(ComponentEvent*);
    void PositionTab(Button*);
    void AddTab(Button*);
    void RemoveTab(int); //genuine SDK remove/reindex/reparent algorithm
    void SetCurTab(int,bool=true);
    void SetCurTabByName(const GUIString&,bool=true);
    void EnableTab(int,bool=true);
    bool Disabled(int);
    int Fn308280(); // protected canonical call spelling; native borrowed getter18/support0
    std::vector<SmartPtr<Button> > mTabVector; //84, data88/end8c/cap90
    Button* mCurTab; //94, borrowed
    TabCtrl* mTabCtrl; //98, borrowed
    int mOverlap; //9c; completea0
};
class TabCtrl : public Container {
public:
    TabCtrl(); // native308670/718; standalone Container/LayoutPoint new gates, no definition yet
    // Natural implicit normal301340 (shared physical cleanup),0quota; no own-vptr store.
    virtual void Virtual08(); // SizeChanged308f50
    virtual void Virtual0C(int&,int&); // GetDesiredSize3089e0
    virtual bool Virtual54(int); // KeyDown309240
    virtual void Virtual6C(); // AddedToParent308cb0
    virtual void HandleComponentEvent(ComponentEvent*); //3090e0
    virtual bool Virtual88(bool,bool); //TakeTabFocus309140
    void CopyAttributes(TabCtrl*);
    void SetTabBarOnRight(bool);
    void SetArrowsOnLeft(bool);
    void SetTabArrows(Button*,Button*);
    void NotifyTabChange(bool);
    void KeepTabButtonInView();
    void PositionTabContainer(Container*);
    void PositionArrows();
    void HorzPositionTabBar();
    void VertPositionTabBar();
    void CalcContainerLayout();
    bool NeedScrollButtons();
    void AdjustTabBarScroll(int=0);
    void Fn309350(int,bool); // SetCurTab protected canonical24/support0
    void Fn309370(const GUIString&,bool); // SetCurTabByName protected canonical24/support0
    int Fn309390(Container*); // GetTabIdForContainer55
    void Fn3093D0(int,bool); // EnableTab protected canonical24/support0
    void AddTab(Button*,Container*);
    void RemoveTab(int); //genuine SDK remove/reselect algorithm
    SmartPtr<TabBar> mBottomTabBar; //84
    SmartPtr<Container> mTopTabBar; //88
    SmartPtr<Container> mBottomTabBarContainer; //8c
    SmartPtr<Container> mTopTabBarContainer; //90
    SmartPtr<LayoutPoint> mContainerLayout; //94
    int mLeftPad,mRightPad,mTopPad,mBottomPad; //98..a4
    int mBottomTabLeftOffset,mBottomTabTopOffset; //a8/ac
    int mTabBarHeight,mTabBarOverlap; //b0/b4
    SmartPtr<Button> mTabBarLeft,mTabBarRight; //b8/bc
    bool mArrowsOnLeft,mTabBarOnRight,mTabBarOnBottom,mUserHasInteracted; //c0..c3
    // Native constructor deliberately leaves mUserHasInteracted uninitialized.
    std::vector<Container*> mTabVector; //c4, nonowning values, datac8/endcc/capd0
    Container* mCurTab; //d4, borrowed; completed8
};

// Complete actual Scrollbarb8 owner; native305e10/305eb0 and pinned SDK full fields.
// Original external base constructor/destructor only; no own base lifecycle definitions.
class Scrollbar : public Container {
public:
    Scrollbar(bool=true); //305e10 actual external base constructor114B/RET4
    virtual ~Scrollbar(); //305eb0 external normal lifetime
    virtual void Virtual08(); //306380
    virtual void Virtual2C(int,int,int); //3064c0
    virtual void Virtual38(int,int); //306510
    virtual void Virtual6C(); //305fe0
    virtual void HandleComponentEvent(ComponentEvent*); //306690
    virtual void SetEnabled(bool); //3067e0 actual90 override
    void CopyAttributes(Scrollbar*); //305fa0 support0
    void AddControls(); //306000
    void LayoutThumb(bool); //3061d0
    void S_SetPos(Component*,int,int); //306100 support0
    void S_SetSize(Component*,int,int); //306140 support0
    void S_SetPosSize(Component*,int,int,int,int); //306180 support0
    int S_Left(Component* c) { return mVertical?c->Left():c->Top(); }
    int S_Right(Component* c) { return mVertical?c->Left()+c->Width():c->Top()+c->Height(); }
    int S_Top(Component* c) { return mVertical?c->Top():c->Left(); }
    int S_Bottom(Component* c) { return mVertical?c->Top()+c->Height():c->Left()+c->Width(); }
    int S_Height(Component* c) { return mVertical?c->Height():c->Width(); }
    int S_Width(Component* c) { return mVertical?c->Width():c->Height(); }
    SmartPtr<Button> mUpArrow,mDownArrow,mScrollRect; //84/88/8c
    SmartPtr<Component> mThumb; //90
    bool mResizeThumb,mVertical; //94/95
    unsigned char mMinThumbSize; //96
    int mPosition,mPageSize,mTotalSize; //98/9c/a0
    int mLineUpScrollSize,mLineDownScrollSize,mPageScrollSize; //a4/a8/ac
    int mDragX,mDragY; //b0/b4
};

// Complete actual Scroller88; sole owner ScrollAreaSP84, original external lifetime.
class Scroller : public Container {
public:
    Scroller(); //305540 actual external constructor28B
    // Genuine SDK implicit virtual destructor3001e0; automatic real owned members and base cleanup.
    virtual void Virtual2C(int,int,int); //305670
    virtual bool Virtual3C(int); //3056c0
    virtual void NotifyScrollChange(bool) {} //genuine SDK NotifyScrollChange empty inline default, native30a800/3
    ScrollArea* GetScrollArea() { return mScrollArea.get(); }
    void SetScrollArea(ScrollArea*); //305580 actual external owner/backlink method
    void SetOffsets(int,int); //3055e0 horizontal thenvertical
    void SetVertOffset(int); //305630
    void SetHorzOffset(int); //305650
    SmartPtr<ScrollArea> mScrollArea; //84, actual owner; implicit normal3001e0
};
enum ScrollbarCondition { ScrollbarCondition_Disable=0,ScrollbarCondition_Enable=1,ScrollbarCondition_Conditional=2 };
// Complete actual ScrollbarScroller9c; external base lifetime, no own lifecycle source.
class ScrollbarScroller : public Scroller {
public:
    ScrollbarScroller(); //305700 actual external constructor55B
    // Genuine SDK implicit virtual destructor300060; automatic real owned members and base cleanup.
    virtual void Virtual6C(); //305840 AddedToParent
    virtual void HandleComponentEvent(ComponentEvent*); //305a00
    virtual void NotifyScrollChange(bool); //305a50 actualCC
    void SetScrollbar(Scrollbar*); //305760
    void AddControls(); //305860
    Scrollbar* GetScrollbar(bool vertical) { return vertical?mVertScrollbar.get():mHorzScrollbar.get(); } //genuine SDK inline; native88/8c
    bool EnableScrollbar(Scrollbar*,ScrollbarCondition); //3058d0
    void SetScrollbarConditions(ScrollbarCondition,ScrollbarCondition); //305990 original external
    SmartPtr<Scrollbar> mHorzScrollbar,mVertScrollbar; //88/8c actual owners, external lifetime
    ScrollbarCondition mVertCondition,mHorzCondition; //90/94
    bool mSettingScrollbarConditions; //98, natural99..9b
};
enum ScrollButtonCondition {
    ScrollButtonCondition_Disable=0, ScrollButtonCondition_Enable=1,
    ScrollButtonCondition_Conditional=2, ScrollButtonCondition_PartialConditional=3
};
// Genuine same-SDK complete owner; Native identity unresolved, no layout/coverage claim.
class ButtonScroller : public Scroller {
protected:
    SmartPtr<Button> mLeftButton,mRightButton,mUpButton,mDownButton;
    ScrollButtonCondition mVertCondition,mHorzCondition;
    bool mSettingScrollButtonConditions;
    bool EnableScrollButtons(Button*,Button*,ScrollButtonCondition);
public:
    virtual void Virtual08();
    virtual void NotifyScrollChange(bool);
    virtual void HandleComponentEvent(ComponentEvent*);
    virtual void Virtual6C();
    ButtonScroller();
    void SetButton(Button*,Direction);
    void AddControls();
    void SetScrollButtonConditions(ScrollButtonCondition,ScrollButtonCondition);
};
// Complete actual ComboBox a4 proposal; GUI owner must install after native/peer proof.
class Scroller;
class ComboBox : public Container {
public:
    ComboBox();
    virtual ~ComboBox(); // own-vptr normal2f3a50, real listener-detach body
    virtual void Virtual08(); //2f4010 SizeChanged
    virtual void Virtual2C(int,int,int); //2f4600 MouseDown
    virtual bool Virtual3C(int); //2f4650 MouseWheel
    virtual void Virtual48(); //2f4090 GotFocus
    virtual void Virtual4C(); //2f40b0 inherited LostFocus forwarding0
    virtual bool Virtual54(int); //2f4680 KeyDown
    virtual void Virtual6C(); //2f3ef0 AddedToParent
    virtual void HandleComponentEvent(ComponentEvent*); //2f44e0
    virtual void SetEnabled(bool); //2f4800
    ListItem* AddString(const GUIString&,int=-1); //existing protected spelling2f3b70
    void InsertItem(TextListItem*,int=-1);
    void DeleteItem(int);
    void UpdateItem(TextListItem*);
    void Clear(bool=true);
    int GetSelItem(); //existing index-return spelling2f3d40; getter/support0
    void SetSelItem(int,bool=false);
    void SetListBox(Scroller*);
    void AddControls();
    void CalcListBoxSize();
    void SetListBoxDimensions(int,int);
    void SetListBoxWidth(int);
    void SetListBoxHeight(int);
    void SetInputBoxToListSelection(bool=false,bool=true);
    void SetText(const GUIString&,bool=false);
    void SetTextAndSel(const GUIString&,bool);
    void ShowList();
    void SetNoType(bool);
    SmartPtr<InputBox> mEdit; //84
    SmartPtr<Button> mButton; //88
    SmartPtr<Scroller> mListBox; //8c
    SmartPtr<ListArea> mListArea; //90
    bool mListBoxVisible; //94
    bool mNoType; //95
    bool mShowButtonWhenDisabled; //96; natural padding97
    int mListBoxWidth; //98
    int mListBoxHeight; //9c
    GUIString mUndoStr; //a0; actual complete a4
};

// one local network address (a std::list element, 0x14 bytes; struct and member names PROVISIONAL)
struct LobbyNetAddress {
    std::string mName;                      // +0x00
    long mHandle;                           // +0x10, numeric IPv4 adapter address
};
// Actual ConfigObject54 declarations are provided by WONConfigObject.h via Resource.
// Complete78-byte external-lifetime base. Private24-byte tail remains opaque;
// no copy/by-value use or own base constructor/destructor implementation.
class ComponentConfig : public ConfigObject {
public:
    static ComponentConfig* CfgFactory(); //39aab0 genuine SDK static API, original external provider only
    static Component* CompFactory(); //396960 genuine SDK external API; call-only
    bool ReadConfig_Unsafe(Component*,const char*,ResourceConfigTable*,ConfigParser*); //3828c0 actualRET10
    void ReadConfig(Component*,const char*,ResourceConfigTable* = NULL,ConfigParser* = NULL);
    Component* GetChildComponent(const std::string&);
    ComponentConfig* GetChildConfig(const std::string&);
    void InitComponents(); //382cc0 recursive guarded component initialization
    Direction ReadDirection(); //3856f0
    HorzAlign ReadHorzAlign(); //385950
    VertAlign ReadVertAlign(); //385b10
    static void SetResourceFolder(const std::string&); //387ca0, native original-owned storage only
    static std::string mResourceFolder; //566b18, external original owner
    SmartPtr<Sound> ReadSound(); //3855f0, true owner result
    void EnsureComma(); //382e00; SkipComma is382dd0
    int GetIntValue(); //382f40, actual native until source next cohort
    int ParseExpression(int); //383560
    int ReadChar(); //383930
    GUIString ReadFlag(bool* = NULL); //383a90
    GUIString ReadRawString(bool); //383b00
    SmartPtr<Font> ReadFont(); //384480 native external result
    SmartPtr<Image> ReadImage(bool); //384fa0 actual owned hidden-result API
    SelectionColor ReadSelectionColor(); //385cd0 native external12B result
    ComponentConfig* GetComponentConfig(const GUIString&,bool); //386380 native external borrowed result
    ComponentConfig* ReadComponentConfig(); //3868d0 actual123B lookup reader, support0
    bool HandleMapComponent(); //386960 actual709B;386950 is separate9B wrapper
    bool HandleModifyGroup(ConfigParser&); //386d80 actual410B
    bool HandleResourceGroup(ConfigParser&); //386f20 actual384B, call-only pending realResource prefix gate
    bool HandleModify(); //3870a0 actual423B
    bool BaseHandleInstruction(ConfigParser&,const std::string&,StringParser&); //387370 actual27B forwarding support0
    void SetDesiredSizeIfNecessary(); //3860b0 actual131B, call-only
    SmartPtr<Cursor> ReadCursor(); //385380 actual owned4B hidden result; RefCount8 prefix proof, no Cursorallocation
    int GetIntResource(const std::string&); //384210
    static SmartPtr<ResourceConfigTable> mResourceConfigTable; //original566b28 actualcollection storage only, no definition
    SmartPtr<RawImage> ReadRawImage(); //385050 native owned result
    SmartPtr<Image> GetImageFromStr(const std::string&,bool); //384890
    SmartPtr<RawImage> GetRawImageFromStr(const std::string&); //384f10, cohesive support0
    StandardCursor ReadStandardCursor(); //385120
    void AddChildToConfigMap(const GUIString&,ComponentConfig*); //386c30 parent retains child
    GUIString GetStringResource(const std::string&); //383bd0
    Image* GetImageResource(const std::string&); //383d60 borrowed
    Font* GetFontResource(const std::string&); //383e50 borrowed
    SmartPtr<Sound> GetSoundResource(const std::string&); //383f40 owned result
    DWORD GetColorResource(const std::string&); //384050
    void GetBackgroundResource(const std::string&,Background&); //384140
    Background ReadBackground(); //385ea0 real20B value
    int ReadInt(); //3837e0/8, actual no-argument parser call
    Component* ReadComponent(); //386950/9, actual borrowed result
    bool EndOfString(); //382db0
    void SkipComma(); //382e00
    static std::string GetResourceFile(const std::string&); //387e50

    ComponentConfig();                     // 0x3825b0
    virtual ~ComponentConfig();            // 0x382690, own VT451f54
    virtual bool Virtual04(ConfigParser*, const std::string&);
    virtual bool Virtual08(ConfigParser*, const std::string&, StringParser&);
    virtual bool Virtual0C(ConfigParser*);
    virtual bool Virtual10(ConfigParser*);
    virtual bool Virtual24(const std::string&);
    virtual bool Virtual28(const std::string&, ComponentConfig*&, Component*&);
    virtual bool Virtual2C(const std::string&, int&);
    virtual bool Virtual30(const std::string&, const std::vector<int>&, int&);
    virtual void Virtual34(Component*);
    GUIString Fn3842F0(bool);               // ReadString
    int Fn385510();                        // ReadColor
    Component* Fn386860(const std::string& inName); // 0x386860, borrowed component lookup
    bool Fn3837F0();                       // 0x3837f0, native boolean parser
    SmartPtr<Component> mComponent; //54, native3825dc, external-lifetime base
    ConfigParser* mConfigParser; //58, borrowed
    StringParser* mStringParser; //5c, native scalar reader access
    GUIString mConfigError; //60, native3825ff
    bool mHasSetWidth,mHasSetHeight,mHaveInitedComponents; //64/65/66, naturalpad67
    std::map<std::string,SmartPtr<ComponentConfig>,StringLessNoCase> mConfigMap; //68..73 actual12B SDK/native ownership
    SmartPtr<ResourceConfig> mCurResourceConfig; //74 actualConfigObject-derived resource owner, no pointer/layout change
};

// Actual FontConfig declaration follows the complete FontDescriptor below.
class FontConfig;

// Complete target1c exception, implicit copy/destructor are real original helpers.
class ConfigObjectException : public std::exception {
public:
    ConfigObjectException(const std::string& inText, bool inFlag = true)
        : mText() { mText = inText; mFlag = inFlag; }
    std::string mText;                     // +08
    bool mFlag;                            // +18
};

// Genuine SDK checked borrowed component cast; natural existing consumers only.
template<class T>
T SafeConfigGetComponent(Component *theComponent, const char *theName)
{
	T aComp = dynamic_cast<T>(theComponent);
	if(aComp==NULL)
		throw ConfigObjectException(std::string("Invalid ") + theName);

	return aComp;
}


// 0x38aa60 (cdecl): the component the resource names inName, 0x38ab30: reports a missing one
Component* ComponentConfigGet(ComponentConfig* inConfig, const char* inName);
void ComponentConfigMissing(const std::string& inName, const char* inFile, int inLine);

// fetches a named component of the lobby's resources into outPtr (WONAPI header template; COMDATs 0x296948,
// 0x2969c8 in EELobbyInit.cpp)
template <class T> void SafeGetComponent(ComponentConfig* inConfig, SmartPtr<T>& outPtr, const char* inName,
                                         const char* inFile, int inLine)
{
    T* aComponent = dynamic_cast<T*>(ComponentConfigGet(inConfig, inName));
    if (aComponent == NULL)
        ComponentConfigMissing(inName, inFile, inLine);
    outPtr = aComponent;
}

template <class T> void SafeGetComponent(ComponentConfig* inConfig, SmartPtr<T>& outPtr,
    const char* inName, int inControlId, const char* inFile, int inLine)
{
    SafeGetComponent(inConfig, outPtr, inName, inFile, inLine);
    outPtr->mControlId = inControlId;
}

class ButtonConfig : public ComponentConfig {
public:
    void HandleButtonFlags(); //381880
    void HandleFitText(); //381d00
    ButtonConfig();                         // 0x381860, actual7c
    virtual bool Virtual24(const std::string&); //381e80
    virtual void Virtual34(Component*);     //38f8d0 ICF target
    Button* mButton;                        // +78, borrowed
};

class ContainerConfig : public ComponentConfig {
public:
    static Component* LayoutPointFactory(); //39ab10 genuine SDK static API, original external provider only
    static ComponentConfig* CfgFactory(); //295842 genuine SDK static API, original external provider only
    static Component* CompFactory(); //365120 genuine SDK static API, original external provider only
    bool HandleAdd(); //388840 actual43B
    bool HandleAddLayoutPoint(); //388870 actual57B
    bool HandleLayout(bool chain=false,bool repeat=false); //3888b0 actual1379B
    bool HandleSameSizeLayout(bool,bool); //388e20 actual269B, genuine external helper
    bool HandleCenterLayout(bool,bool); //388f30 actual334B, genuine external helper
    bool HandleClearRightLayout(); //389080 actual344B, genuine external helper
    bool HandleSpaceEvenlyLayout(bool); //389280 actual397B, genuine external helper
    bool HandleNoOverlapLayout(); //3894b0 actual777B, genuine external helper
    bool HandleSizeAtLeastLayout(); //389860 actual365B, genuine external helper
    bool HandleNewLayoutPoint(); //389a70 actual523B, genuine external helper
    ContainerConfig();                      // 0x387f90, actual98
    // Actual implicit normal38ef70 tail-jumps external ComponentConfig destructor.
    virtual bool Virtual04(ConfigParser*, const std::string&); //3885d0
    virtual bool Virtual24(const std::string&); //389c80
    virtual void Virtual34(Component*);     //38f8d0
    Container* mContainer;                  // +78, borrowed
    Component* mLayoutRef;                  // +7c, borrowed
    int mLayoutInstruction;                // +80
    int mLayoutParams[4];                  // +84..90
    int mCurLayoutId;                      // +94
};

class DialogConfig : public ContainerConfig {
public:
    static ComponentConfig* CfgFactory(); //29585c genuine SDK static API, original external provider only
    static Component* CompFactory(); //39ac50 genuine SDK static API, original external provider only
    virtual bool Virtual24(const std::string&); //389f80
    virtual void Virtual34(Component*);     //38f960
    Dialog* mDialog;                        // +98, borrowed
};

class LobbyConfig : public ConfigObject {
public:
    LobbyConfig();                          // 0x358090
    typedef LobbyConfig* (*ConfigAllocator)();
    static ConfigAllocator mAllocator; // true SDK class-static, native4d954c; existing storage
    static LobbyConfig* DefaultAllocator(); //357f30 existing genuine body, renamed only
    static std::string mDefaultFileName; //566310 existing genuine string storage
    static void SetAllocator(ConfigAllocator allocator) { mAllocator=allocator; } // genuine SDK inline setter
    virtual bool Virtual08(ConfigParser* inParser, const std::string& inKey, StringParser& inValue);
    virtual bool Virtual10(ConfigParser* inParser);

    std::list<std::string> mL54;            // +0x54; ConfigListValue RTTI, node size 0x18
    std::list<std::string> mL60;            // +0x60
    std::list<std::string> mL6C;            // +0x6c
    std::string mS78;                       // +0x78
    std::wstring mW88;                      // +0x88
    std::string mS98;                       // +0x98
    std::string mSA8;                       // +0xa8
    std::string mSB8;                       // +0xb8
    std::string mSC8;                       // +0xc8
    bool mCRCCheck;                         // +0xd8
    bool mCDKeyCheck;                       // +0xd9
    bool mPublic;                           // +0xda
    unsigned short mLanBroadcastPort;      // +0xdc
    bool mUseLocalIP;                       // +0xde
    bool mNoPingInProgress;                 // +0xdf
    unsigned short mLobbyPort;             // +0xe0
    bool mCheckForInternet;                 // +0xe2
    unsigned short mPingCeiling;           // +0xe4
    bool mAllowDuplicateNames;              // +0xe6
    bool mAppendNumberToDefaultLanName;      // +0xe7
    bool mAllowJoinGameWithoutPing;          // +0xe8
    std::map<std::string, std::string> mMEC;               // +0xec (node value 0x20; pair dtor 0x296a48)
};

// 0x358710: the lobby's configuration object (an EELobbyConfig: EELobbyInit.cpp's factory)
LobbyConfig* GetLobbyConfig();

// one item of a list (0x18 bytes: TextListItem's members start at +0x18; vtable 0x84dba4, 14 slots; the inline
// bodies are the COMDATs EELobbyConnectDialog.cpp keeps, 0x290bb4..0x290bc4)
class ListItem : public RefCount {
protected:
    virtual ~ListItem() {} //genuine SDK protected inline explicit destructor, inherited RefCount cleanup
public:
    ListItem();                             // 0x2fa280
    virtual void Virtual04(Graphics&, ListArea*);
    virtual void Virtual08(ListArea*);
    virtual void Virtual0C(ListArea*, Container*);
    virtual int Virtual10(const ListItem*, ListArea*) const;
    virtual bool Virtual14(ListArea*, int, int) { return mB09; }
    virtual void Virtual18(int, int inTop) { mTop = inTop; }
    virtual bool Virtual1C(int, int inY) { return inY >= mTop && inY < mTop + mHeight; }
    virtual void Virtual20(ListArea*, int, int);
    virtual void Virtual24(ListArea*);
    virtual void Virtual28(ListArea*, int, int);
    virtual void Virtual2C(ListArea*, int, int);
    virtual void Virtual30(ListArea*, int, int, int);
    virtual void Virtual34(ListArea*, int, int, int);
    bool mB08;                              // +08
    bool mB09;                              // +09
    int mTop;                               // +0c
    int mL10;                               // +10
    int mHeight;                            // +14
};

// the root of the lobby's components (runs the popups and modal dialogs; RTTI WONAPI::RootContainer; its base
// Container: r24pre-won PROVISIONAL)
// Complete target C8 RootContainer owner; lifetime remains original external.
// Borrowed RefCount8 callback interface; nativeRoot3049bd uses slot04(key,type).
class KeyboardListenerBase : public RefCount {
public:
    virtual void KeyEvent(int,int)=0;
};
// Complete native38 record. RootContainer borrows; Common owns producer source.
// Native33613f/148 stores 64bit order30/34; defaultpack8 padding2c..2f.
class BackgroundGrabber {
public:
    SmartPtr<NativeImage> mBGImage; //00
    int mImageX,mImageY; //04/08
    WONRectangle mClipRect; //0c..1b
    bool mNeedPutBG,mHaveBG; //1c/1d
    int mTranslucentColor,mTranslucentLevel; //20/24
    SmartPtr<Window> mDebugWindow; //28
    unsigned __int64 mImageOrder; //30, actual64bit targetcounter
    static unsigned __int64 mImageOrderCounter; //native5662e0/e4
    BackgroundGrabber(); //336000, complete producer lifetime
    void PrePaint(Component*,Graphics&); //336070
    void Paint(Component*,Graphics&); //3360f0
    void SetTranslucentColor(int,int); //3360d0
    void Clear(); //336060 original external, bothflagsfalse
    void SizeChanged(Component*); //2fa500 actual external empty RET4, support0
    void Invalidate(Component*); //336680 actual external14B haveBG -> needPutBG
    bool NeedDrawBG() const { return mHaveBG && mNeedPutBG; }
};
class RootContainer : public Container {
public:
    RootContainer(Window*); //3040d0; original complete constructor
    virtual void Virtual08(); //304730 SizeChanged
    virtual void Virtual14(Graphics&); //304760 PrePaint
    virtual void Paint(Graphics&); //304900
    virtual void Virtual24(int,int); //304c40 MouseEnter
    virtual void Virtual28(); //304c60 MouseExit
    virtual void Virtual2C(int,int,int); //304bd0 MouseDown
    virtual void Virtual30(int,int,int); //304c90 MouseUp
    virtual void Virtual34(int,int); //304a30 MouseMove
    virtual void Virtual38(int,int); //304ac0 MouseDrag
    virtual bool Virtual50(int); //304a00 KeyChar
    virtual bool Virtual54(int); //3049a0 KeyDown
    virtual bool Virtual58(int); //3049d0 KeyUp
    virtual void Virtual60(WONRectangle&); //304980 InvalidateRect
    virtual void Virtual78(Cursor*); //304cf0 SetCursor
    virtual Component* VirtualAC(int,int); //304710 FindChildAt
    virtual bool VirtualC0(Component*); //304970 InvalidateUp
    virtual void VirtualC4(Component*); //304cc0 RemoveChild
    void AddBG(BackgroundGrabber*); //304510

    struct BGStruct {
        int mImageX,mImageY;
        WONRectangle mClipRect;
        SmartPtr<NativeImage> mImage;
        BGStruct() : mImageX(0),mImageY(0) {}
        BGStruct(int x,int y,NativeImage* image,const WONRectangle& clip)
          : mImageX(x),mImageY(y),mClipRect(clip),mImage(image) {}
    }; //1c, native map node30
    WONRectangle mInvalidRect; //84..93
    Component* mModalComponent; //94
    Window* mWindow; //98
    Component* mCursorComponent; //9c
    int mDragX,mDragY; //a0/a4
    bool mDragging,mAllowDrag; //a8/a9
    SmartPtr<KeyboardListenerBase> mKeyboardListener; //ac
    std::set<Component*> mPopupSet; //b0..bb
    std::map<int,BGStruct> mBGStructMap; //bc..c7
};

// how a popup is opened (0x2c bytes, on the stack; members PROVISIONAL)
struct PopupParams {
    PopupParams(Component* inPopup,int inFlags,Component* inOwner,int inX=-1,int inY=-1); //328540 external
    Window* mParent; //00, borrowed
    Component* mPopup; //04, borrowed
    Component* mInputComponent; //08, borrowed
    Component* mOwnerComponent; //0c, borrowed
    int mFlags; //10
    int mx,my; //14/18
    WONRectangle mAvoidRect; //1c..2b, actual embedded POD; full2c
};
// gets a component's events (vtable 0x84d...; 2 slots)
class ComponentListener : public RefCount {
public:
    virtual void Virtual04(ComponentEvent*);
};

// Actual complete0c callback owner, RTTI451ad8 and allocation359089.
// Existing base virtual spelling retained; original slot4 callback3590d0.
class ComponentListenerFunc : public ComponentListener {
public:
    typedef void (*Callback)(ComponentEvent*);
    ComponentListenerFunc(Callback callback) : mCallback(callback) {}
    virtual void Virtual04(ComponentEvent* event) { mCallback(event); }
    Callback mCallback; //08; implicit real base/member cleanup
};

// Actual ComponentExtra1c, RTTI4d5eb8/vtable44d714, sole virtual destructor.
// Complete native1c owner; native allocator2f4ac0 and SDK prove construction and unique lifetime.
// Four SmartPtr members retain/release naturally; mGrabBG is uniquely owned. No copy authority.
class ComponentExtra {
public:
    ComponentExtra(); //2f4890 original external constructor
    virtual ~ComponentExtra(); //ordinary2f48d0/deleting2f48b0, external native lifetime
    SmartPtr<ColorScheme> mColorScheme; //04
    SmartPtr<ComponentListener> mListener; //08, AddListener2f4b10 /Deliver2f54e0
    SmartPtr<Cursor> mDefaultCursor; //0c
    BackgroundGrabber* mGrabBG; //10 unique original-owned pointer, external destructor deletes real38 owner
    SmartPtr<Component> mToolTip; //14
    bool mDontDoTip; //18, natural19..1b alignment
};

// a listener that hands the events on to a component (0x0c bytes; vtable 0x84c134: ??_G ICF-folded 0x291f6f,
// +0x04 0x337bf0)
class ComponentListenerComp : public ComponentListener {
public:
    ComponentListenerComp(Component* inComponent) : mComponent(inComponent) {}
    virtual void Virtual04(ComponentEvent*);               // 0x337bf0 (WONAPI)

    Component* mComponent;                  // +0x08
};

// WONAPI's drawing context (methods PROVISIONAL)
class Font;
// Replace existing Graphics reference contract. Native vtable450fe8;
// field accesses326ad0/326ce0/327140 prove complete target size38.
class Graphics {
public:
    virtual void ClearClipRectHook() {}
    virtual void SetClipRectHook(int, int, int, int) {}
    virtual DWORD GetPixelHook(int, int) { return 0; }
    virtual void SetPixelHook(int, int, DWORD) {}
    virtual void SetColorHook() {}
    virtual void DrawLineHook(int, int, int, int) {}
    virtual void DrawRectHook(int, int, int, int);
    virtual void DrawDottedRectHook(int, int, int, int);
    virtual void FillRectHook(int, int, int, int);
    virtual void DrawPolygonHook(POINT*, int) {}
    virtual void FillPolygonHook(POINT*, int) {}
    virtual void DrawEllipseHook(int, int, int, int) {}
    virtual void FillEllipseHook(int, int, int, int) {}
    virtual void DrawArcHook(int, int, int, int, int, int) {}
    virtual void FillRectBlendHook(int, int, int, int, int) {}
    Graphics(); //326ad0
    virtual ~Graphics(); //326b90
    virtual void Attach(DisplayContext*); //326c70
    virtual void Detach() {}
    virtual bool BlendSupported() { return false; }
    void SetColor(DWORD); //326ce0
    DWORD Fn326D20(int); // preserve existing caller-facing identity
    void SetSysColor(int);
    void SetFont(Font*);
    void Translate(int, int);
    void DoTranslate(int&, int&);
    void SetPixel(int, int, DWORD);
    void DrawImage(Image*, int, int);
    void DrawImage(Image*, int, int, int, int, int, int);
    void DrawLine(int, int, int, int);
    void DrawRect(int, int, int, int);
    void FillRect(int, int, int, int);
    void FillRectBlend(int, int, int, int, int);
    void DrawDottedRect(int, int, int, int);
    void DrawEllipse(int, int, int, int);
    void FillEllipse(int, int, int, int);
    void DrawArc(int, int, int, int, int, int);
    void DrawString(const GUIString&, int, int);
    void DrawString(const GUIString&, unsigned short, unsigned short, int, int);
    void PerformClip();
    void PushClipRect(int, int, int, int);
    void PopClipRect();
    SmartPtr<DisplayContext> mDisplayContext; //04
    int mTransX, mTransY; //08,0c
    SmartPtr<Font> mFont; //10
    DWORD mColor; //14
    typedef std::list<WONRectangle> ClipStack;
    ClipStack mClipStack; //18..23
    WONRectangle mClipRect; //24..33
    ColorScheme* mColorScheme; //34, borrowed
};

// 0x2fdcd0 / 0x2fddf0 (cdecl): a 3D frame (inPressed: sunken) and a flat frame (names PROVISIONAL)
void LobbyDraw3DFrame(Graphics& inGraphics, int inX, int inY, int inWidth, int inHeight, bool inPressed);
void LobbyDrawFrame(Graphics& inGraphics, int inX, int inY, int inWidth, int inHeight, bool inFlag);

// a game of the lobby (0x68 bytes: original factory 0x3a6730; vtable 26 slots, the game's overrides
// named by offset)
class LobbyGame : public RefCount {
public:
    void Fn3A6C90();
    bool Fn3A6D00(LobbyGame* inOther);
    void Fn3A6E00(LobbyGroup* inGroup);
    bool Fn3A7200(ByteBuffer* inBuffer);
    SmartPtr<ByteBuffer> Fn3A72C0();
    void Fn3A7360(LobbyClient* inClient);
    bool Fn3A7400();
    bool Fn3A7490();
    short Fn3A7570(ReadBuffer& inBuffer, LobbyClient* inClient, WriteBuffer& outBuffer);
    SmartPtr<ByteBuffer> Fn3A7830(ReadBuffer& inBuffer, LobbyClient* inClient);
    short Fn3A78E0(ReadBuffer& inBuffer);
    GUIString Fn3A7A90(short inStatus);
    LobbyGame();                            // 0x3a6790 (WONAPI)
    virtual ~LobbyGame();                   // 0x3a68f0 (WONAPI)
    virtual bool Virtual04() { return true; } //genuine SDK inline PreNetJoinGameHook
    virtual bool Virtual08() { return true; } //genuine SDK inline PostNetJoinGameHook
    virtual bool Virtual0C() { return true; } //genuine SDK inline PreNetCreateGameHook
    virtual bool Virtual10() { return true; } //genuine SDK inline PostNetCreateGameHook
    virtual void Virtual14() {} //genuine SDK inline PreNetLeaveGameHook
    virtual void Virtual18() {} //genuine SDK inline PostNetLeaveGameHook
    virtual void Virtual1C() {} //genuine SDK inline NotifyStartGameHook
    virtual bool Virtual20() { return true; } //genuine SDK inline TryStartGameHook               // (EELobbyGame 0x292719: can the game start)
    virtual long Virtual24() { return 1; } // the derived EELobbyGame 0x2926f9 returns through eax (long, as main had it); the SDK default folds to a `mov al, 1` body 0x2f30d0: open
    virtual bool Virtual28() { return true; } //genuine SDK inline CanReadyHook
    virtual short Virtual2C();
    // (EELobbyGame's overrides): +0x30 a new game hosted by inClient, +0x34 the game's version
    virtual void Virtual30(LobbyClient* inClient) {} //genuine SDK inline HandleHostGameHook
    virtual void Virtual34(WriteBuffer& outBuffer) {} //genuine SDK inline GetJoinGameRequestHook
    // +0x38 a join request's check
    virtual short Virtual38(ReadBuffer& inBuffer, LobbyClient* inClient) { return 0; } //genuine SDK HandleJoinGameRequestHook default
    // +0x3c the game's state for a joining client
    virtual void Virtual3C(LobbyClient* inClient, WriteBuffer& outBuffer) {} //genuine SDK inline GetJoinGameReplyHook
    virtual void Virtual40(LobbyGame* inGame) {} //genuine SDK inline CopyFromHook
    virtual void Virtual44(ReadBuffer& inBuffer) {} //genuine SDK inline HandleJoinGameReplyHook     // +0x44 a joined game's state read
    //: +0x48 a client left (inPlayer: its player), +0x4c a client joined
    virtual void Virtual48(LobbyClient* inClient, LobbyPlayer* inPlayer) {} //genuine SDK inline HandlePlayerLeftHook
    virtual void Virtual4C(LobbyClient* inClient) {} //genuine SDK inline HandlePlayerJoinedHook
    //: +0x50 the summary (three options), +0x54 the details (options and the slots' clients), +0x58 the summary
    // read back, +0x64 a join status' text
    virtual void Virtual50(WriteBuffer& outBuffer) {} //genuine SDK inline WriteSummaryHook
    virtual void Virtual54(WriteBuffer& outBuffer) {} //genuine SDK inline WriteDetailsHook
    virtual bool Virtual58(ReadBuffer& inBuffer) { return true; } //genuine SDK inline ReadSummaryHook
    virtual bool Virtual5C(ReadBuffer& inBuffer) { return true; } //genuine SDK inline ReadDetailsHook     // +0x5c the details read back
    // +0x60 a game message (inType 0x64..0x6a) from inFrom
    virtual void Virtual60(unsigned char inType, ReadBuffer& inBuffer, LobbyClient* inFrom) {} //genuine SDK inline HandleGameMessageHook
    virtual GUIString Virtual64(short inStatus) { return gEmptyGUIString; } //genuine SDK GetStatusStringHook default, owned result
    void CopyFrom(LobbyGame* inGame);       // 0x3a69d0
    void SetClients(LobbyClientList* inClients); // 0x3a6c10
    LobbyClient* FindClient(unsigned short inId); // 0x3a6c70
    bool Fn3A6D40(long inFlags);
    bool Fn3A6D70();
    bool Fn3A6D90();
    bool Fn3A6DA0();
    bool Fn3A6DB0();
    bool Fn3A6DC0();
    bool Fn3A6DD0();
    void Fn3A6DE0();
    void Fn3A6DF0();
    void Fn3A7240(WriteBuffer& outBuffer);
    bool Fn3A74B0();
    bool Fn3A74C0();
    void Fn3A74D0(LobbyClient* inClient);
    void Fn3A79E0(unsigned char inType, ReadBuffer& inBuffer, LobbyClient* inFrom);
    bool IsFull();
    bool Fn3A7A20();
    short GetJoinStatus();
    void WriteSummary(WriteBuffer& outBuffer);
    bool ReadSummary(ReadBuffer& inBuffer);
    void RemoveClient(LobbyClient* inClient);
    bool ReadDetails(ReadBuffer& inBuffer);

    GUIString mName;                       // +0x08
    GUIString mPassword;                   // +0x0c
    bool mB10;
    bool mB11;
    bool mB12;
    bool mB13;
    bool mB14;
    bool mIsHost;
    bool mB16;
    bool mB17;
    bool mB18;
    long mL1C;                             // +0x1c
    bool mIsFiltered;                     // +0x20, actual filter writers; natural padding through23
    long mL24;                             // +0x24
    long mNumOpenSlots;                    // +0x28
    long GetGameType() { return mL2C; }
    long mL2C;                             // +0x2c
    long mL30;                             // +0x30
    SmartPtr<LobbyClientList> mClients;     // +0x34 (owned; constructor/destructor/copy evidence)
    IPAddr mAddress;                       // +0x38
    unsigned short mL60;
    unsigned short mL62;
    unsigned short GetPing() { return mL64; }
    unsigned short mL64;                   // base allocation 0x68 at 0x3a6730
protected:
    typedef LobbyGame* (*LobbyGameFactory)();
    static LobbyGameFactory mGameFactory;
    static LobbyGame* DefaultGameFactory() { return new LobbyGame; }
public:
    static LobbyGame* CreateGame(LobbyGameType inType);
    static void SetGameFactory(LobbyGameFactory factory) { mGameFactory=factory; }

public:
    unsigned short GetL62() const { return mL62; }
    bool GetB12() const { return mB12; }
    bool GetB13() const { return mB13; }
    bool GetIsFiltered() const { return mIsFiltered; }
};



// 0x361350: the game the local player is in (NULL when none)
LobbyGame* GetMyLobbyGame();

// WONAPI's message buffers (names PROVISIONAL)


class ByteBuffer;
template <class T> class SmartPtr;

// 0x361b60 / 0x361b80 (cdecl): a game message to the lobby
void LobbyFn361B60(ByteBuffer* inMessage);
void LobbyFn361B80(ByteBuffer* inMessage);
void LobbyFn361BA0(LobbyClient* inClient, ByteBuffer* inMessage);  // 0x361ba0
void LobbyFn361290();                       // 0x361290

// a client of the lobby's room (its name at +0x08; class PROVISIONAL)
class LobbyPlayer;

class LobbyClient : public RefCount {
public:
    LobbyClient();                         // 0x3a51a0; new 0x2c
    virtual ~LobbyClient();                // 0x3a5260
    void Fn3A52F0(unsigned short inId, bool inMode);
    bool Fn3A5330();
    bool IsFriend(); //actual native physicalICF2b31d0, SDK nonconst
    bool Fn3A5340();
    void Fn3A5350(RoutingClientInfo* inInfo);
    void Fn3A5450();
    bool Fn3A5470();
    bool Fn3A5480(bool inMode);
    bool Fn3A54B0();
    bool Fn3A54C0();
    bool Fn3A54D0();
    bool Fn3A54E0(bool inFlag);
    void Fn3A5500(bool inValue, bool inMode);
    void Fn3A5520(LobbyPlayer* inPlayer);
    bool Fn3A5580();
    bool Fn3A5590();
    unsigned short Fn3A55A0();
    bool Fn3A55C0(LobbyClient* inOther);

    GUIString mName;                        // +0x08
    unsigned short GetClientId() { return mClientId; }
    unsigned short mClientId;              // +0x0c
    unsigned long mU10;
    unsigned long mU14;
    unsigned long mU18;
    unsigned char mFlags1C;
    unsigned char mFlags1D;
    unsigned short mGroupId1E;
    unsigned short mGroupId20;
    bool mB22;
    bool mB23;
    bool mB24;
    bool mB25;
    bool mB26;
    bool mB27;
    SmartPtr<LobbyPlayer> mPlayer;          // +0x28; setter and destructor own the reference
public:
    unsigned long GetU14() const { return mU14; }
    unsigned short GetGroupId1E() const { return mGroupId1E; }
    unsigned short GetGroupId20() const { return mGroupId20; }
    bool GetB22() const { return mB22; }
public:
    const GUIString& GetName() const { return mName; }
};



// RTTI LobbyPlayer; vtable 0x852c54, four slots; size 0x0c.
class LobbyPlayer : public RefCount {
public:
    LobbyPlayer();
    virtual ~LobbyPlayer();
    virtual bool ReadData(ReadBuffer& inBuffer) { return true; } //genuine SDK ReadDataHook default, native387ff0
    virtual void WriteData(WriteBuffer& outBuffer) {} //genuine SDK WriteDataHook default, native30a800
    virtual bool IsTeammate(LobbyPlayer* inOther);
    bool ReadFromBuffer(ReadBuffer& inBuffer); // 0x3a90f0 (typed exception handler/tail separately audited)
    void WriteToBuffer(WriteBuffer& outBuffer); // 0x3a9170
    bool IsSameTeam(LobbyPlayer* inOther);     // 0x3a91a0

    unsigned short mPlayerId;              // +0x08
    bool mBA;                              // +0x0a
protected:
    typedef LobbyPlayer* (*LobbyPlayerFactory)();
    static LobbyPlayerFactory mPlayerFactory;
    static LobbyPlayer* DefaultPlayerFactory() { return new LobbyPlayer; }
public:
    static LobbyPlayer* CreatePlayer();
    static void SetPlayerFactory(LobbyPlayerFactory factory) { mPlayerFactory=factory; }

};

// a rectangle

// The lobby's core, window manager and window (EELobbyInterface.cpp's globals 0x565898, 0x565894, 0x565890). Class
// names and bases from RTTI: WONAPICoreEx : WONAPICore; MSWindowManager : WindowManager : RefCount;
// MSWindow : Window : RefCount; MSNativeIcon : NativeIcon : RefCount; MultiImageDecoder, DisplayContext. Sizes from
// EELobbyInterface 0x2972c6's `new`s; member and slot names PROVISIONAL.
class WindowEventBase;
class InputEvent;
class MouseEvent;
class KeyEvent;
class SizeEvent;
// a window's drawing context (RTTI: DisplayContext : RefCount)
// Borrowed target prefix only: native326c70 stores mGraphics at+c.
// Complete native10B DisplayContext: new10/fieldstores3cfab4..3cfad4; TD4e0ab0.
class DisplayContext : public RefCount {
public:
    DisplayContext(HDC dc=NULL) : mDC(dc),mGraphics(NULL) {}
    virtual ~DisplayContext() {}
    HDC mDC; //08; borrowed; owningimage/window deletesitsDC
    Graphics* mGraphics; //0c borrowed
    SmartPtr<NativeImage> CreateImage(int,int); //3cedd0, nonvirtual
};
class SoundDecoder;
class RawImage;

// Complete logical decoder contracts are in WONLobbyImageDecoder.h.
// 0x3d2a20 / 0x3d24e0 (cdecl): add the JPG decoder (4 extensions) / the PNG decoder to a multi-decoder (PROVISIONAL)

// Actual core ownership/API is defined in WONAPICore.h.

class WindowManager : public RefCount {
protected:
    static WindowManager* mDefaultWindowManager; //native566270, genuine borrowed default manager
    static DWORD mThreadId; //native566274, genuine GUI owner thread id
    bool CheckModalValid(); //genuine SDK protected modal-state validation
public:
    static WindowManager* GetDefaultWindowManager() { return mDefaultWindowManager; }
    static DWORD GetThreadId() { return mThreadId; }
    WindowManager(); //32b660, complete148
    virtual ~WindowManager(); //32bff0
    virtual void GetNextEvent(); //04 32e7c0
    virtual bool SetScreenResolutionHook(int,int); //08 32c380
    virtual void GetScreenResolutionHook(int&,int&); //0c 327440
    virtual void Signal(); //10 326b60
    virtual bool AddWindow(Window*); //14 32d6a0
    virtual bool RemoveWindow(Window*); //18 32d840
    virtual void GetScreenRect(WONRectangle&); //1c 30a800
    virtual void GetSystemColorScheme(); //20 326b60; target no argument, MS3d0b70 reads own20
    void InitColorScheme(); //32c230
    void NotifyLostFocus(); //32c2d0
    void NotifyGotFocus(Window*); //32c300
    void SetScreenResolution(int,int); //32c320
    void RestoreScreenResolution(); //32c390
    Window* GetPopupWindow(); //32c440
    void SetNamedFont(const std::string&,Font*); //32c470
    void SetDefaultFont(Font* font); //actual SDK inline definition follows completeFont below
    Font* GetNamedFont(const std::string&); //32c4d0
    Font* GetDefaultFont(); //32c540
    Font* GetDefaultFontMod(int,int=0); //32c700
    Font* GetNamedFontMod(const std::string&,int,int=0);
    static SmartPtr<Font> GetDefaultFont(const FontDescriptor&);
    static DisplayContext* GetDefaultDisplayContext();
    void RemoveAllTimerEvents();
    void SetCacheImages(bool); //32c7f0, actual conditional image-cache count and clear operation
    SmartPtr<NativeImage> DecodeImage(DisplayContext*,const char*); //32c950
    SmartPtr<NativeImage> DecodeImage(const char*); //32ccb0
    SmartPtr<Image> DecodeDelayImage(const char*); //32ccf0
    SmartPtr<RawImage> DecodeImageRaw(const char*); //32d160
    void SetSoundDecoder(SoundDecoder*); //32d490
    SmartPtr<Sound> DecodeSound(const SoundDescriptor&); //32d4f0
    void Fn32D5B0(); //32d5b0 CloseAllWindows, existing caller identity
    void CalcModality(); //32da30
    bool NotModalWindow(Window*); //32dc30 actual26B member; native MSWindowProc3d1730
    //32dbe0 is the separate actual static SetModal(Component*,bool) helper, not a member.
    int DoDialog(Window*,Component*); //32dc50
    void Fn32DDB0(int,Window*,Component*); //32ddb0 existing EndDialog identity
    void ExitMessageLoop(); //32ddf0
    void AdjustPopupPosition(Window*,Component*,bool,const WONRectangle&); //32de00
    void DoPopup(const PopupParams&); //32e040
    void EndPopup(Component*); //32e4d0
    void CheckEndPopup(Component*); //32e790
    int MessageLoop(); //32e7d0
    void PumpOnce(); //32e840
    void PostEvent(WindowEventBase*,bool=true); //32e850
    void AddTimerEvent(WindowEventBase*); //32e960
    void RemoveTimerEvent(WindowEventBase*); //32ea70
    void FindMouseWindow(MouseEvent*); //32ec60
    bool CheckPopupClick(MouseEvent*); //32ee40
    void DeliverEvent(WindowEventBase*); //32ef20
    void SubscribeToBroadcast(Component*,bool); //32f3f0
    void Fn32F680(ComponentEvent*); //32f680 existing BroadcastEvent identity
    void ClearSubscriptions(); //32f5d0
    WindowEventBase* PeekNextEvent(); //32f700
    void ProcessEvents(); //32f760

    typedef std::map<std::string,SmartPtr<Font>,StringLessNoCase> NamedFontMap;
    typedef std::map<std::string,SmartPtr<NativeImage>,StringLessNoCase> NativeImageMap;
    typedef std::map<std::string,SmartPtr<RawImage>,StringLessNoCase> RawImageMap;
    typedef std::map<std::string,SmartPtr<Image>,StringLessNoCase> DelayImageMap;
    typedef std::list<Component*> PopupList;
    typedef std::set<SmartPtr<Window>,SmartPtr<Window>::Comp> WindowSet;
    typedef std::list<SmartPtr<WindowEventBase> > WindowEventList;
    typedef std::set<SmartPtr<WindowEventBase> > WindowEventSet;
    typedef std::set<SmartPtr<Component> > BroadcastSet;
    CriticalSection mDataCrit; //08..1f
    SmartPtr<ColorScheme> mColorScheme; //20
    MultiImageDecoder mImageDecoder; //24..57 complete34 from skins C packet
    SmartPtr<SoundDecoder> mSoundDecoder; //58
    SmartPtr<Font> mDefaultFont; //5c
    SmartPtr<NativeIcon> mDefaultWindowIcon; //60
    NamedFontMap mNamedFontMap; //64
    NativeImageMap mNativeImageMap; //70
    RawImageMap mRawImageMap; //7c
    DelayImageMap mDelayImageMap; //88
    int mCacheImageCount; //94
    DWORD mLastTimerTick,mLastInputTick; //98/9c
    bool mEndPopup,mIsValid,mInDoPopup,mInEndPopup,mPopupIsUp,mBA5; //a0..a5, mBA5=popup-own-window default
    int mPopupFlags; //a8; target has no later SDK PauseEvents field
    bool mDoMouseCapturePopupSupport,mIsFullScreen; //ac/ad
    int mFullScreenWidth,mFullScreenHeight,mOriginalScreenWidth,mOriginalScreenHeight; //b0..bc
    DWORD mLastLoseFocusTick; //c0
    int mCurMsgLoopDepth,mTargetMsgLoopDepth,mDialogResult; //c4/c8/cc
    Window* mModalWindow; //d0
    Component* mModalComponent; //d4
    SmartPtr<Window> mPopupWindowSave; //d8
    Window* mPopupOwnerWindow; //dc
    SmartPtr<Component> mPopupInputComponent,mPopupOwnerComponent; //e0/e4
    PopupList mPopupList; //e8
    Window* mFocusWindow; //f4
    Window* mMouseDownWindow; //f8
    Window* mDefaultWindow; //fc
    DWORD mWindowCount; //100
    WindowSet mWindowSet; //104
    WindowEventList mWindowEventList; //110
    WindowEventSet mTimerEvents,mRemoveTimerEvents; //11c/128
    bool mCallingTimers; //134
    SmartPtr<WindowEventBase> mCloseEvent; //138
    BroadcastSet mBroadcastSet; //13c..147
};
// 0x361250: the default window manager, out of line (EELobbyInit 0x295ce3 calls it; a 6-byte body that reads
// gDefaultWindowManager; its WON-side source is not written yet)
WindowManager* GetLobbyWindow();

// (0x14c bytes; vtable 0x85472c)
class MSWindowManager : public WindowManager {
public:
    HINSTANCE GetHinstance() { return mInstance; }
    MSWindowManager(HINSTANCE inInstance);  // 0x3d0900 (NULL: GetModuleHandle(NULL))
    virtual ~MSWindowManager(); //3d0b60 actualcomplete11B baseforwarding
    virtual void GetNextEvent(); //04 3d0c40
    virtual bool SetScreenResolutionHook(int,int); //08 3d0cf0
    virtual void GetScreenResolutionHook(int&,int&); //0c 3d0d50
    virtual void Signal(); //10 3d0cc0
    virtual void GetScreenRect(WONRectangle&); //1c 3d0c90
    virtual void GetSystemColorScheme(); //3d0b70 target noarg override

    HINSTANCE mInstance;                    // +0x148
};

// a window's icon (0x0c bytes; vtable 0x85107c)
class NativeIcon : public RefCount {
public:
    NativeIcon(DisplayContext* inContext);  // 0x328990
    virtual ~NativeIcon(); //328a00 actual normal; SDK protected lifetime, existing public reconstruction access retained

    SmartPtr<DisplayContext> mContext;      // +0x08
};

// (0x10 bytes; vtable 0x84c6d4; ??_G 0x297531 is EELobbyInterface.cpp's COMDAT)
class MSNativeIcon : public NativeIcon {
public:
    MSNativeIcon(DisplayContext* inContext, HICON inIcon) : NativeIcon(inContext), mIcon(inIcon) {}
    virtual ~MSNativeIcon();                // 0x3cf9f0

    HICON mIcon;                            // +0x0c
};

enum FontStyles {FontStyle_Plain=0,FontStyle_Bold=1,FontStyle_Italic=2,FontStyle_Underline=4};
struct FontDescriptor {
    std::string mFace;
    int mStyle;
    int mSize;
    FontDescriptor() : mStyle(0), mSize(0) { } // genuine SDK default; complete18 descriptor
    FontDescriptor(const std::string& theFace, int theStyle, int theSize) : mFace(theFace), mStyle(theStyle), mSize(theSize) { }
    bool operator<(const FontDescriptor&) const; // immutable primary comparator contract
    bool operator==(const FontDescriptor&) const; //326840
    bool operator!=(const FontDescriptor&) const; //326880
};

// Complete typed targetB8 storage; only original external constructor/destructor
// implement this lifetime. No private implementation or implicit copying is proposed.
class Window : public RefCount {
public:
    SmartPtr<Font> GetFont(const FontDescriptor&); //32a230
    static void SetSkipDecodeImages(bool); //32a640 actualstatic byte switch
    SmartPtr<RawImage> DecodeImageRaw(const char*); //32a800 owned raw8 fallback/native manager decoder
    SmartPtr<NativeImage> CreateImage(int,int); //32a8c0, original external wrapper
    bool CanClose(); //32ab80
    void CheckResize(DWORD); //32ac90
    void SuspendRootResize(bool suspend);
    NativeImage* GetOffscreenImage();
    Graphics& GetGraphics();
    void RemoveComponent(Component*);
    bool IsMouseDown(MouseButton);
    void ShowNativeCursor(bool); //32afc0, original external wrapper
    void CheckSetCursor(bool); //32afd0, original external behavior
    bool CheckDoubleClick(MouseButton); //32b110 genuine SDK, unchecked button index reads38
    void HandleMouseExit(); //32b120
    void HandleKeyEvent(KeyEvent*); //32b170
    void HandleMouseDown(MouseEvent*); //32b1c0
    void HandleMouseEvent(MouseEvent*); //32b260
    void HandleInputEvent(InputEvent*); //32b390

    Window();                              // 0x329c30
    virtual ~Window();                     // 0x32a020, complete external normal destructor
    virtual SmartPtr<Font> Virtual04(const FontDescriptor&);
    virtual bool Virtual08();
    virtual void Virtual0C(DWORD inElapsed);
    virtual void Virtual10();
    virtual void Virtual14();
    virtual void Virtual18(Cursor*);
    virtual void Virtual1C(bool);
    virtual void Virtual20(bool);
    virtual void Virtual24(bool);
    virtual void Virtual28(bool);
    virtual void Virtual2C();
    virtual void Create(const WONRectangle&);
    virtual void SetPosSize(int, int, int, int);
    virtual void GetRect(WONRectangle&, bool);
    virtual void Virtual3C();
    virtual void Show(bool);
    void Minimize(bool minimize); //genuine SDK nonvirtual wrapper, native32afb0/13
    DisplayContext* GetDisplayContext() { return mDisplayContext.get(); } //genuine borrowed getter at28; no standalone native attribution
    virtual void Virtual44(bool);
    virtual void Virtual48(WindowEventBase*);
    virtual bool Virtual4C();
    static Window* GetDefaultWindow();      // 0x32a220
    SmartPtr<NativeImage> DecodeImage(const char*); // 0x32a650
    int RunDialog(Component*);              // 0x32b080
    void AddComponent(Component*); //32b040 actual external root add forwarding
    void SetModal(bool); //32b060 native dispatch to actual ContainerEnter/LeaveModal
    bool Close();                          // 0x32ac30, actual AL false/true paths
    void SetClientSize(int, int);           // 0x32af30
    void Center(bool);                     // 0x32ae80
    void Fn32B0A0(int, Component*);          // 0x32b0a0
    void OpenPopup(PopupParams&);           // 0x32b0c0
    void ClosePopup(Component*);            // 0x32b0e0
    void Fn32A1E0();                        // 0x32a1e0
    void SetParent(Window*);                // 0x32a210, borrowed parent80
    void SetTitle(const GUIString&);        // 0x32adc0
    void SetIcon(NativeIcon*);              // 0x32ae20
    int GetKeyMod() const { return mKeyMod; }
    enum WindowState { WindowState_Open, WindowState_Closing, WindowState_Closed };
    typedef bool (*AllowCloseFunc)(Window*);
    WindowState mWindowState; // +08
    SmartPtr<NativeImage> mOffscreenImage; // +0c
    int mMouseX; // +10
    int mMouseY; // +14
    int mKeyMod; // +18
    bool mMouseIsDown[3]; // +1c
    bool mMouseIsInClientArea; // +1f
    Cursor* mCurrentCursor; // +20
    bool mNativeCursorShowing; // +24
    SmartPtr<DisplayContext> mDisplayContext; // +28
    DWORD mButtonDownTick[3]; // +2c
    bool mButtonDoubleClick[3]; // +38
    bool mDetectDoubleClick; // +3b
    int mDoubleClickX; // +3c
    int mDoubleClickY; // +40
    DWORD mLastMoveTick; // +44
    int mMinWidth; // +48
    int mMinHeight; // +4c
    int mMaxWidth; // +50
    int mMaxHeight; // +54
    bool mNeedResize; // +58
    int mNewWidth; // +5c
    int mNewHeight; // +60
    DWORD mResizeDelay; // +64
    DWORD mLastResizeTick; // +68
    DWORD mLastCheckCursorTick; // +6c
    int mSuspendRootResize; // +70
    int mStyle; // +74
    bool mIsPopup; // +78
    bool mIsOneComponentWindow; // +79
    bool mMinimizeOnResolutionRestore; // +7a
    WindowManager* GetWindowManager() { return mManager; } //genuine SDK borrowed manager getter
    WindowManager* mManager; // +7c
    Window* mParent; // +80
    std::set<Window*> mChildWindowSet; // +84
    SmartPtr<WindowEventBase> mCloseEvent; // +90
    AllowCloseFunc mAllowCloseFunc; // +94
    SmartPtr<NativeIcon> mIcon; // +98
    GUIString mTitle; // +9c
    bool mIsShowing; // +a0
    SmartPtr<RootContainer> mRoot; // +a4
    std::map<FontDescriptor, SmartPtr<Font> > mFontMap; // +a8
    SmartPtr<Font> mDefaultFont; // +b4
};

// Exact target C8-byte derived class; SDK's later extra key-mod field is absent.
class MSWindow : public Window {
public:
    HWND GetHWND() { return mLB8; }
    void SetMinimizeOnAltTab(bool minimize) { mBC4=minimize; }
    void SetForwardKeyMsgToParent(bool forward) { mBC5=forward; }
    MSWindow();                            // 0x3d0d90, calls original Window constructor
    void CalcCodePage(); //3d0e20; actual private nonvirtual helper
    void MyGetKeyState(); //3d15a0; writes targetcurKeyModbc
    void PostMouseEvent(WindowEventType,int,LPARAM); //3d1600 ret0c, targetactualenum
    void DoMinimizeOnAltTab(); //3d16a0
    static LRESULT CALLBACK StaticWindowProc(HWND,UINT,WPARAM,LPARAM); //3d1530 actualstdcall callback
    LRESULT WindowProc(HWND,UINT,WPARAM,LPARAM); //3d16e0 actualthiscall
    virtual ~MSWindow();                   // 0x3d0de0, owns native HWND cleanup
    virtual SmartPtr<Font> Virtual04(const FontDescriptor&);
    virtual bool Virtual08();
    virtual void Virtual0C(DWORD);
    virtual void Virtual10();
    virtual void Virtual14();
    virtual void Virtual18(Cursor*);
    virtual void Virtual1C(bool);
    virtual void Virtual20(bool);
    virtual void Virtual24(bool);
    virtual void Virtual28(bool);
    virtual void Virtual2C();
    virtual void Create(const WONRectangle&);
    virtual void SetPosSize(int, int, int, int);
    virtual void GetRect(WONRectangle&, bool);
    virtual void Virtual3C();
    virtual void Virtual44(bool);
    virtual bool Virtual4C();
    virtual void Virtual50();               // OnPaint3d23f0, no arguments
    virtual void Virtual54();               // OnCreate3d22c0, no arguments
    HWND mLB8;                             // +b8, target HWND
    DWORD mLBC;                            // +bc
    int mCodePage;                         // +c0, GetLocaleInfoA/atoi producer3d0e20
    bool mBC4;                             // +c4
    bool mBC5;                             // +c5
};
// 0x3d08c0 (cdecl): the parent window of the MS windows created next (PROVISIONAL)
void LobbyFn3D08C0(HWND inParent);

// WONAPI lobby state functions the game's lobby glue calls
int LobbyFn362760();                        // 0x362760 (how the lobby was left: 1, 2 or 3)
void LobbyFn362830();                       // 0x362830
void LobbyFn362880();                       // 0x362880
void LobbyFn362170();                       // 0x362170
Container* LobbyFn362250();                 // 0x362250
void LobbyFn362740();                       // 0x362740
void LobbyGetNetAddresses(std::list<LobbyNetAddress>& outAddresses, bool inFlag);   // 0x2b32b0 (cdecl)

// 0x361390: the local player (NULL when none)
LobbyPlayer* GetMyLobbyPlayer();
// 0x34a100 (cdecl): a text with its %s-style argument filled in (name PROVISIONAL)
GUIString LobbyFormatString(const GUIString& inFormat, const GUIString& inArg);
void LobbyFn3612B0(bool inFlag);            //0x3612b0 genuine SDK SetGameInProgress(bool)
bool LobbyFn361310(bool inFlag);             // 0x361310
bool LobbyFn3613C0();                       // 0x3613c0
void LobbyFn361260();                       // 0x361260
void LobbyFn3613E0();                       // 0x3613e0

class ServerListCtrl;
class ServerDetailsItem;
typedef std::list<ServerDetailsItem*> ServerDetailsList;
class ServerDetailsItem : public ListItem {
public:
    ServerDetailsItem();
    virtual ~ServerDetailsItem();
    virtual void Virtual04(Graphics&,ListArea*);
    virtual void Virtual08(ListArea*);
    virtual int Virtual10(const ListItem*,ListArea*) const;
    virtual bool Virtual14(ListArea*,int,int);
    virtual void Virtual30(ListArea*,int,int,int);
    virtual void PaintDetailsHook(Graphics&,MultiListArea*) {} //genuine SDK inline empty details hook, slot38
    virtual int GetDetailsHeightHook(MultiListArea*) { return 0; } //genuine SDK inline zero-height hook, slot3c
    void SetGame(LobbyGame*);
    void CalcIcon();
    static bool IsStartOfNumber(char);
    static bool ParseIntFilter(const GUIString&,int&,int&);
    static void PaintIcon(Graphics&,ServerDetailsItem*,ServerListCtrl*);
    static void PaintSkill(Graphics&,ServerDetailsItem*,ServerListCtrl*);
    static void PaintGameName(Graphics&,ServerDetailsItem*,ServerListCtrl*);
    static void PaintAddress(Graphics&,ServerDetailsItem*,ServerListCtrl*);
    static void PaintNumPlayers(Graphics&,ServerDetailsItem*,ServerListCtrl*);
    static void PaintPing(Graphics&,ServerDetailsItem*,ServerListCtrl*);
    static int CompareIcon(const ServerDetailsItem*,const ServerDetailsItem*,ServerListCtrl*);
    static int CompareSkill(const ServerDetailsItem*,const ServerDetailsItem*,ServerListCtrl*);
    static int CompareGameName(const ServerDetailsItem*,const ServerDetailsItem*,ServerListCtrl*);
    static int CompareAddress(const ServerDetailsItem*,const ServerDetailsItem*,ServerListCtrl*);
    static int CompareNumPlayers(const ServerDetailsItem*,const ServerDetailsItem*,ServerListCtrl*);
    static int ComparePing(const ServerDetailsItem*,const ServerDetailsItem*,ServerListCtrl*);
    static void FilterSkill(const GUIString&,ServerDetailsList&,ServerListCtrl*);
    static void FilterGameName(const GUIString&,ServerDetailsList&,ServerListCtrl*);
    static void FilterAddress(const GUIString&,ServerDetailsList&,ServerListCtrl*);
    static void FilterNumPlayers(const GUIString&,ServerDetailsList&,ServerListCtrl*);
    static void FilterPing(const GUIString&,ServerDetailsList&,ServerListCtrl*);
    SmartPtr<LobbyGame> mGame; //18
    GUIString mIPAddrStr; //1c
    int mIconId; //20
    unsigned char mSummaryHeight; //24
protected:
    typedef ServerDetailsItem* (*ItemFactoryFunc)();
    static ItemFactoryFunc mItemFactoryFunc;
    static ServerDetailsItem* DefaultItemFactory() { return new ServerDetailsItem; }
public:
    static ServerDetailsItem* CreateItem() { return mItemFactoryFunc(); }
    static void SetItemFactory(ItemFactoryFunc factory) { mItemFactoryFunc=factory; }

};

class WindowEventBase : public RefCount {
public:
    WindowEventBase(long inType = 0) : mL08(inType), mWindow(0) {}
    virtual void Deliver() = 0;             // +0x04: original vtable 0x44df64 -> _purecall

    long mL08;                              // +0x08
    Window* mWindow;                        // +0x0c
};

// an event that calls a function with one argument when it is delivered
class UserEvent : public WindowEventBase {
public:
    typedef void (*Func)(int inParam);

    UserEvent(Func inFunc, int inParam) : mFunc(inFunc), mParam(inParam) {}
    virtual void Deliver() { mFunc(mParam); }   // +0x04 0x295d30

    Func mFunc;                             // +0x10
    int mParam;                             // +0x14
};

// a component's event (slot +0x7c; type at +0x14: WONAPI's own ids, 100000 up for the game's)
// Actual ComponentEvent: complete size0x20, owning sender reference +0x10.
// Complete native events: allocation16 at32ac45; borrowedMouse32/Key24/Size32.
class InputEvent : public WindowEventBase {
public:
    InputEvent(int type) : WindowEventBase(type) {}
    virtual void Deliver(); //32b520
};
class MouseEvent : public InputEvent {
public:
    MouseEvent(int type,int x,int y,int mod,int button=0)
      : InputEvent(type),mx(x),my(y),mKeyMod(mod),mButtonOrAmount(button) {}
    virtual void Deliver(); //32b630
    int mx,my,mKeyMod,mButtonOrAmount; //10,14,18,1c
};
class KeyEvent : public InputEvent {
public:
    KeyEvent(int type,int key,int mod) : InputEvent(type),mKey(key),mKeyMod(mod) {}
    virtual void Deliver(); //32b640
    int mKey,mKeyMod; //10,14
};
class SizeEvent : public InputEvent {
public:
    SizeEvent(int x,int y,int width,int height)
      : InputEvent(1),mx(x),my(y),mWidth(width),mHeight(height) {}
    int mx,my,mWidth,mHeight; //10,14,18,1c
};

class ComponentEvent : public WindowEventBase {
public:
    ComponentEvent(Component* inComponent, int inType); // 0x32b530
    virtual void Deliver();                // 0x32b610
    void SetComponent(Component* inComponent); // 0x32b5a0
    int GetControlId() const;               // 0x32b600
    SmartPtr<Component> mComponentRef;      // +0x10
    int mType;                              // +0x14
    bool mBroadcast;                        // +0x18
    Component* mComponent;                  // +0x1c
};

// Actual ListItemClickedEvent: RTTI0x44dd78, allocation0x34.
class ListItemClickedEvent : public ComponentEvent {
public:
    ListItemClickedEvent(Component* component,ListItem* item,MouseButton button,bool dbl,bool up,int x,int y)
      :ComponentEvent(component,up?ComponentEvent_ListItemUpClicked:ComponentEvent_ListItemClicked),
       mButton(button),mDoubleClick(dbl),mItem(item),mX(x),mY(y) {}
    bool IsSelectedItem(); //genuine SDK ListBox.cpp; native2fa550
    int mButton;                            // +0x20
    bool mDoubleClick;                      // +0x24
    SmartPtr<ListItem> mItem;               // +0x28
    int mX;                                 // +0x2c
    int mY;                                 // +0x30
};

// Actual LobbyEvent adds no payload to ComponentEvent (size0x20).
// Derived lobby events are declared only in WONLobbyEvents.h, owned by core.
class LobbyEvent : public ComponentEvent {
public:
    LobbyEvent(int inType) : ComponentEvent(NULL, inType) {}
};

// Actual RTTI receiver of GetClient 0x3aa370 is LobbyServer (complete contract in WONLobbyModel.h).
class LobbyServer;
LobbyServer* LobbyFn35B6F0();               // 0x35b6f0; NULL outside a server

// RTTI LobbyClientList : RefCount; original constructor 0x3a55f0, allocation 0x14.
typedef std::map<unsigned short, SmartPtr<LobbyClient> > LobbyClientMap;
class LobbyClientList : public RefCount {
public:
    LobbyClientList();
    virtual ~LobbyClientList();
    void Fn3A5770();
    void Fn3A5820(const std::map<unsigned short, SmartPtr<RoutingClientInfo> >& inMap);
    void Fn3A5C50();
    void Fn3A5CC0(LobbyClient* inClient);
    LobbyClient* FindClient(unsigned short inId);
    SmartPtr<LobbyClient> Fn3A5D90(unsigned short inId);
    LobbyClient* Fn3A5EA0(unsigned long inId);
    void Fn3A5F10();
    LobbyClientMap mClients;               // +0x08
};

// Generic non-owning client iterator: 0x3a5f70..0x3a6140; size 0x08.
class LobbyClientIterator {
public:
    LobbyClientIterator(LobbyClientList* inList);
    bool HasMore();
    LobbyClient* Next();
    void Rewind();
    LobbyClientMap* mClients;
    LobbyClientMap::iterator mItr;
};

// Existing provisional iterator spelling retained. It filters clients with players; size 0x0c.
class LobbyClientSetIterator {
public:
    LobbyClientSetIterator(LobbyClientList* inList);
    bool HasMore();
    LobbyPlayer* Next(LobbyClient** outClient);
    void Rewind();
    LobbyClientMap* mClients;
    LobbyClientMap::iterator mItr;
    bool mReady;
};
bool LobbyFn35D010(const GUIString& inName); // persistent ignore predicates; cdecl
bool LobbyFn35D090(unsigned long inAddress);
unsigned short GetLobbyProductId();        // 0x35b580 (LobbyGame summary wire product id)

// 0x361370 (cdecl): the local client
LobbyClient* LobbyFn361370();
// 0x361b20 (cdecl): EELobbyInterface 0x296d1e passes it as the connection request's flag
bool LobbyFn361B20();

// 0x359120 (cdecl): fires a component event of inType
void LobbyFn359120(int inType, bool inFlag);
// 0x359100 (cdecl): delivers inEvent to the lobby's components
void LobbyFn359100(ComponentEvent* inEvent, bool inFlag);
// 0x361bc0
void LobbyFn361BC0(LobbyClient* inClient, bool inFlag);

// 0x361250: default GUI manager (global 0x566270, assigned by WindowManager constructor).

// Reference contracts: no Font/Image/NativeImage allocation or private implementation.

class Font : public RefCount {
public:
    Font(DisplayContext*, const FontDescriptor&); //3268a0
    const FontDescriptor& GetDescriptor() { return mDescriptor; }
    int GetSize() { return mDescriptor.mSize; }
    int GetStyle() { return mDescriptor.mStyle; }
    const std::string& GetFace() { return mDescriptor.mFace; }
    virtual ~Font();                        // 0x326a60, complete external descriptor lifetime
    virtual void Virtual04(DisplayContext*);
    virtual void Virtual08(DisplayContext*, const GUIString&, unsigned short, unsigned short, int, int);
    int GetStringWidth(const GUIString& theStr) { return GetStringWidth(theStr,0,(unsigned short)theStr.length()); }
    virtual int GetStringWidth(const GUIString&, unsigned short, unsigned short);
    virtual int Virtual10(int inChar);
    virtual int GetHeight();
    virtual int GetAscent();
    virtual int GetDescent();
    FontDescriptor mDescriptor;             // +08..1f
};
inline void WindowManager::SetDefaultFont(Font* font) {mDefaultFont=font;} //actualSDK0quota, completeFont lookup

// Complete actual FontConfig98: nativefactory38a160 alloc98, ctor38a1c0/implicitdtor38a250.
class FontConfig : public ComponentConfig {
public:
    static ComponentConfig* CfgFactory(); //38a160 genuine SDK static API, original external provider only
    static Component* CompFactory(); //2c7b10 genuine SDK static API, original external provider only
    FontConfig(); //38a1c0 original external, completeactual98 lifetime
    Font* GetFont(); //38a300 borrowed result, original lazy recalculation
    void SetFont(Font*); //38a3e0
    virtual bool Virtual10(ConfigParser*); //PostParse3099f0 true sharedhook/support0
    virtual bool Virtual24(const std::string&); //38a5f0
    virtual bool Virtual2C(const std::string&,int&); //38a580
    SmartPtr<Font> mFont; //78
    FontDescriptor mDescriptor; //7c..93
    bool mNeedRecalcFont; //94, natural95..97; full98
    // Implicit destructor, native38a250 has no own-vptr store.
};
inline int WrappedText::GetHeight() { return mHeight==0 ? mFont->GetHeight() : mHeight; }
class Image : public RefCount {
public:
    void DrawEntire(DisplayContext*,int,int); //3285a0 genuine SDK nonvirtual whole-image draw adapter
    virtual ~Image();
    virtual void Draw(DisplayContext*, int, int, int, int, int, int);
    virtual void SetSize(int, int);
    virtual SmartPtr<Image> Duplicate();
    virtual SmartPtr<NativeImage> GetNative(DisplayContext*);
    unsigned short mWidth;                  // +08
    unsigned short mHeight;                 // +0a
};
// Complete native mask0c: ctor3288b0 and actual NativeMask RTTI4d6e80.
class NativeMask : public RefCount {
public:
    NativeMask(DisplayContext*); //3288b0
    virtual ~NativeMask(); //328920
    SmartPtr<DisplayContext> mDisplayContext; //08
};
// Complete abstract18B NativeImage owner; native328a70/328b20 fields/lifetimes.
class NativeImage : public Image {
public:
    NativeImage(DisplayContext*); //328a70
    Graphics& GetGraphics(); //328c20
    void SetTransparentMask(NativeMask* mask) { mTransparentMask=mask; }
    virtual ~NativeImage(); //328b20
    virtual void Draw(DisplayContext*,int,int,int,int,int,int) = 0;
    virtual SmartPtr<NativeImage> GetNative(DisplayContext*); //328bf0
    virtual Graphics* Virtual14() = 0;
    virtual SmartPtr<NativeIcon> GetIcon();
    virtual SmartPtr<RawImage32> GetRaw();
    Graphics* mGraphics; //0c uniqueowner
    SmartPtr<DisplayContext> mDisplayContext; //10
    SmartPtr<NativeMask> mTransparentMask; //14
};

// Concrete50B native MSGraphics, original854688 vtable and factory3cfce0 new50.
class MSGraphics : public Graphics {
public:
    MSGraphics(); //3cf0f0
    virtual ~MSGraphics(); //3cf130
    virtual void Attach(DisplayContext*); //3cf180
    virtual void Detach(); //3cf210
    virtual void SetColorHook(); //3cf290
    virtual DWORD GetPixelHook(int,int); //3cf310
    virtual void SetPixelHook(int,int,DWORD); //3cf330
    virtual void DrawLineHook(int,int,int,int); //3cf370
    virtual void DrawRectHook(int,int,int,int); //3cf3c0
    virtual void DrawDottedRectHook(int,int,int,int); //3cf410
    virtual void FillRectHook(int,int,int,int); //3cf430
    virtual bool BlendSupported(); //3cf460
    virtual void FillRectBlendHook(int,int,int,int,int); //3cf500
    virtual void DrawPolygonHook(POINT*,int); //3cf790
    virtual void FillPolygonHook(POINT*,int); //3cf7d0
    virtual void DrawEllipseHook(int,int,int,int); //3cf7f0
    virtual void FillEllipseHook(int,int,int,int); //3cf840
    virtual void DrawArcHook(int,int,int,int,int,int); //3cf870
    virtual void ClearClipRectHook(); //3cf970
    virtual void SetClipRectHook(int,int,int,int); //3cf980
    HDC mDC; //38
    HBRUSH mOriginalBrush; //3c
    HPEN mOriginalPen; //40
    HBRUSH mSolidBrush; //44
    HBRUSH mHollowBrush; //48
    HPEN mPen; //4c
};
// Actual10B bitmap owner; native GetMask3d06b0 alloc10 and initializesbase0c+handle0c.
class MSNativeMask : public NativeMask {
public:
    MSNativeMask(DisplayContext* context,HBITMAP mask) : NativeMask(context),mMask(mask) {}
    virtual ~MSNativeMask(); //3cf9b0
    HBITMAP mMask; //0c
};
// Target28B native bitmap owner. Later SDK mImageData28/useDibSection are absent.
class MSNativeImage : public NativeImage {
public:
    MSNativeImage(DisplayContext*,HBITMAP,MSNativeMask* = NULL); //3cfb70
    MSNativeImage(DisplayContext*,int,int); //3cfbf0
    virtual ~MSNativeImage(); //3cfc60
    void Init(DisplayContext*,HBITMAP,MSNativeMask*); //3cfa50
    void SelectBitmap(bool); //3cfa10
    virtual Graphics* Virtual14(); //3cfce0, real PlatformGetGraphics
    virtual void Draw(DisplayContext*,int,int,int,int,int,int); //3cfd40
    virtual SmartPtr<RawImage32> GetRaw(); //3cfe50
    virtual SmartPtr<Image> Duplicate(); //3d0130
    virtual SmartPtr<NativeIcon> GetIcon(); //3d0350
    virtual SmartPtr<Cursor> GetCursor(int,int); //20,3d0230; external fornow
    HDC mDC; //18
    HBITMAP mBitmap; //1c
    HBITMAP mOldBitmap; //20
    bool mBitmapIsSelected; //24
};

// Target concrete MSFont: native3cee60 stores HFONT20,HDC24,TEXTMETRICA28.
// Add Font(DisplayContext*, const FontDescriptor&) to existing Font.
class MSFont : public Font {
public:
    MSFont(DisplayContext*, const FontDescriptor&); //3cee60
    virtual ~MSFont(); //3cef30
    virtual void Virtual04(DisplayContext*); //3cef60 PrepareContext
    virtual void Virtual08(DisplayContext*,const GUIString&,unsigned short,unsigned short,int,int); //3cef80
    virtual int GetStringWidth(const GUIString&,unsigned short,unsigned short); //3cf040
    virtual int Virtual10(int); //3cf090 GetCharWidth
    virtual int GetHeight(); //3cf0c0
    virtual int GetAscent(); //3cf0d0
    virtual int GetDescent(); //3cf0e0
    HFONT mFont; //20
    HDC mDC; //24
    TEXTMETRICA mTextMetrics; //28
};

// Target classes merge platform behavior into the concrete RawImage classes.
// Six native vtable slots at4510a8/4510c4/4510e0; later SDK platform
// virtuals and GetRawImage32 virtual are absent from this binary.
class Palette : public RefCount {
public:
    Palette(int n) : mNumColors(n) { mColors=new DWORD[n]; } //actual SDK inline constructor, native336828 full10 allocation
    SmartPtr<Palette> Duplicate(); //349040 actual owned palette duplicate, external native
    void SetColor(int thePos, unsigned long theVal) { mColors[thePos] = theVal; }
    unsigned long GetColor(int thePos) { return mColors[thePos]; }
    int GetNumColors() { return mNumColors; }
    virtual ~Palette(); //external real lifetime; no local ownership implementation
    DWORD* mColors; //08
    int mNumColors; //0c
}; // complete10 owner, SDK/native exact two owned fields; destructor original external
class NativeMask;
class RawImage : public Image {
public:
    RawImage(int type) : mType(type) {}
    virtual void Draw(DisplayContext*,int,int,int,int,int,int); //328c50
    virtual SmartPtr<NativeImage> GetNative(DisplayContext*) = 0;
    virtual bool HasTransparency() { return false; }
    int mType; //0c; 0=1bit,1=8bit,2=32bit
};
class RawImage32 : public RawImage {
public:
    RawImage32(int,int); //328cf0; complete24B owner
    virtual ~RawImage32(); //328da0
    virtual SmartPtr<Image> Duplicate(); //328df0
    virtual SmartPtr<NativeImage> GetNative(DisplayContext*); //328f30
    virtual bool HasTransparency() { return mDoTransparency; }
    SmartPtr<NativeImage> Fn3D0410(DisplayContext*); //real native platform method
    DWORD GetPixel(int,int); //328ee0
    DWORD* StartRowTraversal(int); //328f00

    void SetDoTransparency(bool value) { mDoTransparency=value; }
    void SetRowPixel(DWORD value) { *mRowPtr=value; }
    void NextRowPixel() { ++mRowPtr; }
    void NextRow() { mRowPtr=mRowEnd; mRowEnd+=mWidth; }
    DWORD* mImageData; //10
    DWORD* mImageEnd; //14
    DWORD* mRowPtr; //18
    DWORD* mRowEnd; //1c
    bool mDoTransparency; //20
};
class RawImage8 : public RawImage {
public:
    RawImage8(int,int,Palette* =NULL); //329170; complete30B owner
    virtual ~RawImage8(); //329280
    virtual SmartPtr<Image> Duplicate(); //329300
    virtual SmartPtr<NativeImage> GetNative(DisplayContext*); //329460
    virtual bool HasTransparency() { return mTransparentColor>=0; }
    void Fn3D04F0(); //real nonvirtual platform row initialization
    SmartPtr<NativeImage> Fn3D0520(DisplayContext*);
    unsigned char GetPixel(int,int); //329420
    unsigned char* StartRowTraversal(int); //329440
    unsigned char* mImageData; //10
    unsigned char* mImageEnd; //14
    unsigned char* mRowPtr; //18
    unsigned char* mRowEnd; //1c
    int mRowPad; //20
    int mBytesPerRow; //24
    SmartPtr<Palette> mPalette; //28
    int mTransparentColor; //2c
};
class RawImage1 : public RawImage {
public:
    RawImage1(int,int); //329690; complete3cB owner
    virtual ~RawImage1(); //329790
    virtual SmartPtr<NativeImage> GetNative(DisplayContext*); //3d07e0
    void Fn3D0690(); //real nonvirtual platform row initialization
    SmartPtr<NativeMask> GetMask(DisplayContext*); //3d06b0, nonvirtual
    unsigned char* StartRowTraversal(int); //3297e0
    int GetBytesPerRow() { return mBytesPerRow; }
    unsigned char* GetImageData() { return mImageData; }
    void NextRowPixel() { mRowMask>>=1; if(mRowMask==0) { mRowPtr++; mRowMask = 128; } }
    void NextRowPixel(int theAmount) { for(int i=0; i<theAmount; i++) NextRowPixel(); }
    void SetRowPixel(bool on) { if(on) *mRowPtr |= mRowMask; else *mRowPtr &= ~mRowMask; }
    bool GetRowPixel() { return (*mRowPtr)&mRowMask?true:false; }
    unsigned char* mImageData; //10
    unsigned char* mImageEnd; //14
    unsigned char* mRowPtr; //18
    unsigned char* mRowEnd; //1c
    int mRowMask; //20
    int mRowPad; //24
    int mEndRowMask; //28
    int mBytesPerRow; //2c
    int mBytesPerRowMinusPad; //30
    int mCompleteBytesPerRow; //34
    int mWidthMod8; //38
};

class TextListItem : public ListItem {
public:
    TextListItem() : mL1C(-1) {}
    TextListItem(const GUIString& text) : mText(text),mL1C(-1) {} //genuine SDK inline, nativeMenuCtor2fcd54..7a
    TextListItem(const GUIString& inText, Image* inImage) : mText(inText), mL1C(-1), mImage(inImage) {}
    TextListItem(const GUIString& inText, Font* inFont) : mText(inText), mL1C(-1), mFont(inFont) {}
    virtual ~TextListItem() {}              // original explicit inline lifetime
    virtual void Virtual04(Graphics&, ListArea*); // 0x2fa380
    virtual void Virtual08(ListArea*);       // 0x2fa2c0
    virtual int Virtual10(const ListItem*, ListArea*) const; // 0x2fa360
    GUIString mText;                        // +18
    long mL1C;                              // +1c
    SmartPtr<Font> mFont;                   // +20
    SmartPtr<Image> mImage;                 // +24
};
// Actual producer2f4133/type27, full28, implicit normal2f41c0 has no own-vptr.
class ComboSelChangedEvent : public ComponentEvent {
public:
    ComboSelChangedEvent(Component* c,TextListItem* item,bool active)
        :ComponentEvent(c,27),mItem(item),mActiveSel(active) {}
    SmartPtr<TextListItem> mItem; //20
    bool mActiveSel; //24
};

// Complete actual MenuItem38, native2fd49c allocation; full TextListItem28 base.
// Original constructor and implicit ordinary lifetime stay external; no partialowner.
class PopupMenu;
class MenuItem : public TextListItem {
public:
    // Genuine SDK implicit virtual destructor2fcde0; automatic real owned members and base cleanup.
    MenuItem(const GUIString&,ComponentListener*); //2fcd30 originalexternal constructor, full38
    virtual void Virtual04(Graphics&,ListArea*); //2fcf10 actualPaint04slot
    virtual void Virtual08(ListArea*); //2fcee0 actualCalcDimensions08slot
    SmartPtr<PopupMenu> mSubMenu;            // +28
    int mMenuId;                            // +2c
    bool mDisabled; //30 proven2fcda4write/2fd018read, naturalpadding31..33
    SmartPtr<ComponentListener> mListener; //34 nativector/dtor completeownerfield
};
// All image globals require independent ResourceConfig producer/lifetime proof.

// Reference-only ScrollArea prefix: original external lifetime, no allocation.
class ScrollArea : public Container {
public:
    ScrollArea(); //304df0 actual387B operationalviewport setup
    // Genuine SDK implicit virtual destructor2fa790; automatic real owned members and base cleanup.
    virtual void VirtualCC(bool);           // 305030
    virtual void VirtualD0();               // 4142a0
    virtual void VirtualD4(Graphics&);       // 2fa500
    virtual void Virtual08(); //3051c0 SizeChanged
    virtual void Virtual0C(int&,int&); //3054a0
    virtual bool Virtual3C(int); //305410 MouseWheel
    virtual void Paint(Graphics&); //3052d0 actual inherited18 override
    void SetDelegatePos(); //304fd0
    bool OnBottom(); //305190
    void SetBorderPadding(int,int,int,int); //305270
    void ScrollTranslate(int&,int&); //305390
    void ScrollUntranslate(int&,int&); //3053d0
    virtual void HandleComponentEvent(ComponentEvent*); //305180 actual7c override, originalexternal
    void SetSizeEqualToTotal(int,int); //305450
    bool IsRectVisible(const WONRectangle&); //3054f0
    ScrollAreaViewport* GetViewport() { return mViewport.get(); }
    void SetScrollDimension(int,int); //genuine SDK content-dimension update
    void SetScrollbar(Scrollbar*); //genuine SDK typed scroller selection
    void SetScrollButton(Button*,Direction); //genuine SDK typed button-scroller selection
    void SetScroller(Scroller*); //305010, borrowedScroller d4
    int GetTotalHeight() { return mTotalHeight; }
    int GetTotalWidth() { return mL0A4; }
    int GetLeftPad() { return mLeftPad; }
    int GetRightPad() { return mRightPad; }
    void SetVertOffset(int); //305130
    void SetOffsets(int, int); //305150, horizontal then vertical
    SmartPtr<ScrollAreaViewport> mViewport; //84 original external lifetime
    WONRectangle mInvalidRect; //88..97
    int mVertOffset; //98
    int mHorzOffset; //9c
    int mTotalHeight; //a0
    int mL0A4;                              // actual total width
    int mAVertOffset,mAHorzOffset,mATotalHeight,mATotalWidth; //a8..b7
    int mOldScrollAreaWidth,mOldScrollAreaHeight; //b8..bf
    int mLeftPad; //c0
    int mRightPad; //c4
    int mTopPad,mBottomPad; //c8,cc
    bool mUserHasScrolled; //d0
    Scroller* mScroller; //d4 borrowed
    WONRectangle mScrollArea; //d8, visible height e4
    Background mBackground; //e8..fb actual external-owned Background20
    int mExtraHeight; //fc
    int mUpScrollAmount,mDownScrollAmount,mLeftScrollAmount,mRightScrollAmount; //100..10f
};

// Genuine same-SDK complete owner; Native identity unresolved, no layout/coverage claim.
class ComponentScrollArea : public ScrollArea {
protected:
    SmartPtr<Component> mComponent;
    bool mHorzCenter,mVertCenter;
    void PositionComponent();
public:
    virtual void Virtual08();
    ComponentScrollArea(Component*);
    void SetComponentSize(int,int);
    void UpdateComponentSize();
    Component* GetComponent() { return mComponent.get(); }
};

enum ListSortType { ListSortType_Ascending, ListSortType_Descending, ListSortType_None };
// Complete actualListAreaToolTip38: native2fbbb6 new38->2fb7b0 ctor,
// Component30 base +rawListArea30/owningListItemSP34, actual43 inheritedslots.
class ListAreaToolTip : public Component {
public:
    ListAreaToolTip(ListArea*); //2fb7b0 actual129B lifecycle0
    virtual void Paint(Graphics&); //2fb8d0 actual18
    void DoTip(ListItem*); //2fb9b0
    void EndTip(); //2fbab0
    ListArea* mListArea; //30 borrowed
    SmartPtr<ListItem> mItem; //34 actualretainedowner
};
// Complete actual ListArea164 owner; allocation through existing actual ctor/dtor permitted.
// Full ScrollArea110 base/real vector+SP owners already proved; no new lifecycle source.
class ListArea : public ScrollArea {
public:
    ListArea(); //2fa570 actual508B search/font/focus setup
    virtual ~ListArea(); //2fa860 actual413B EndTip then realowningcleanup,0quota
    void SetSortType(ListSortType); //2facd0 native external
    typedef int (*SortFunc)(ListArea*, const ListItem*, const ListItem*);
    static int NumberSortFunc(ListArea*,const ListItem*,const ListItem*); //2faaf0 actual external
    struct LineSortPred {
        ListArea* mListArea; SortFunc mSortFunc; bool mAscending;
        LineSortPred(ListArea* area,SortFunc func,int type):mListArea(area),mSortFunc(func),mAscending(type==0) {}
        bool operator()(const ListItem* a,const ListItem* b) { return mAscending ? mSortFunc(mListArea,a,b)<0 : mSortFunc(mListArea,a,b)>0; }
        bool operator()(const SmartPtr<ListItem>& a,const SmartPtr<ListItem>& b) { return (*this)(a.get(),b.get()); }
    };
    static int DefaultSortFunc(ListArea* area,const ListItem* a,const ListItem* b) { return a->Virtual10(b,area); }
    SortFunc GetSortFunc(); //2fab20 support0
    int FindInsertPos(ListItem*,int); //2fabe0
    void DeleteItemPrv(int); //2fca50
    void CheckMouseSel(ListItem*,int,int,bool=false); //2fb060
    void ItemMouseAt(int&,int&); //2fb6a0
    ListItem* FindSelectableItem(int,bool,bool=true); //2fc0a0
    virtual void VirtualCC(bool); //2fadf0 AdjustPosition
    virtual void Virtual08(); //2faef0 SizeChanged
    virtual void VirtualD4(Graphics&); //2fafb0 ScrollPaint
    virtual void Virtual24(int,int); //2fc010 MouseEnter
    virtual void Virtual28(); //2fc040 MouseExit
    virtual void Virtual2C(int,int,int); //2fbd00 MouseDown
    virtual void Virtual30(int,int,int); //2fbef0 MouseUp
    virtual void Virtual34(int,int); //2fbad0 MouseMove
    virtual void Virtual38(int,int); //2fbc70 MouseDrag
    virtual bool Virtual54(int); //2fc160 KeyDown
    // Actual appended slots D8/DC/E0: ListArea has57, TreeArea overrides D8/E0.
    virtual void NotifyMouseItemGone();     // 0x2fcad0, no args
    virtual void ListSelChangedHook();      // 0x4142a0, no args
    virtual bool Sort();                   // 0x2fab50, no args, AL result
    void Fn2FAA00();                        // 0x2faa00 (before the list is refilled)
    void Fn2FAA10();                        // 0x2faa10 (after it)
    void Fn2FAA50(bool inFlag);             // 0x2faa50 (clears it?)
    ListItem* GetItem(int inIndex);         // 0x2fc4e0
    ListItem* InsertItem(ListItem* inItem, int inPos = -1);         // 0x2fc510
    void Fn2FC730(ListItem* inItem);        // 0x2fc730 (an item changed)
    void DeleteItem(int); //2fcae0 index deletion
    void Fn2FB600(unsigned long,bool); //2fb600 actual listflags mask
    void RemoveItem(ListItem* inItem);      // 0x2fcb00
    ListItem* InsertString(const GUIString& inText, int inPos = -1);    // 0x2fcc30
    int GetNumItems() const { return mItems.size(); }         // 0x290d70 (EELobbyConnectDialog.cpp's COMDAT)

    int GetItemPos(ListItem* inItem);        // 0x2fc450
    void SetSelItem(int inIndex, bool inFireEvent = true); // 0x2fb410, ret8
    void EnsureVisible(ListItem* inItem);    // 0x2faf60, ret4
    void FindNextSelection(); //2fb4d0 native selected-item iterator scan
    int ClearSelectionPrv(bool); //2fb560 actual removed-selection count
    void ClearSelection(bool = true); //2fb5f0 forwarding support0
    bool SetSelItem(ListItem*,bool,bool); //2fb200 actual selection mutation
    void SelectRange(int,int); //2fb2e0
    std::vector<SmartPtr<ListItem> >::iterator FindLineAt(int); //2facf0
    ListItem* FindItemAt(int); //2fad80
    void FindTopLine(); //2fadc0
    bool IsItemVisible(ListItem*); //2faf10
    void EnforceInvariants(); //2fc3c0
    void DeleteRange(int,int); //2fcb20
    void RewindSelections();               // 0x2fb480
    bool HasMoreSelections();              // 0x2fb490
    ListItem* GetNextSelection();           // 0x2fb530
    int GetTextColor(); //2fb520
    int GetItemPosAtYPos(int); //2fc4b0
    bool ListFlagSet(int flags) { return mListFlags & flags ? true : false; }
    ListItem* GetSelItem() { return mSelItem; } //genuine SDK borrowed selected-item getter
    const SelectionColor& GetSelColor() { return mSelColor; } //actual nominal inline API, quota0
    void SetFont(Font* theFont) { mFont=theFont; }
    std::vector<SmartPtr<ListItem> > mItems;          // +0x110
    int mTopLineVectorPos; //120 native2fadc0
    int mListFlags; //124
    int mDoingMultiChange; //128 native2faa00/2faa10
    int mL12C;                              // actual layout readback
    int mSortType; //130 actual ListSortType value; no change to remaining external-owned134
    SmartPtr<ListAreaToolTip> mItemToolTip; //134 actualownedSP, originalListArea lifetime remains external
    SmartPtr<Font> mFont;                            // +0x138
    int mTextColor; //13c
    SelectionColor mSelColor; //140
    ListItem* mSelItem;                     // +0x14c (a combo box's list: ComboBox::GetSelItem 0x2f3d40)
    ListItem* mKeyItem; //150, borrowed
    ListItem* mMouseItem; //154, borrowed
    int mSelItemItr; //158
    SmartPtr<ListItem> mSearchLine; //15c
    SortFunc mSortFunc; //160; actual base extent164
};


// Actual complete TreeItem3c, native3150b2 new3c/3140f0 ctor/314170 lifetime.
// Constructor/destructor stay original external; TreeArea borrowed174 is never allocated.
class TreeArea;
class TreeItem : public ListItem {
public:
    TreeItem(); //3140f0 complete original external constructor
    // Genuine SDK implicit virtual destructor314170; automatic real owned members and base cleanup.
    virtual void Virtual04(Graphics&,ListArea*); //314b60
    virtual void Virtual08(ListArea*); //314df0
    virtual void Virtual0C(ListArea*,Container*); //314dd0
    virtual int Virtual10(const ListItem*,ListArea*) const; //314e20
    virtual bool Virtual14(ListArea*,int,int); //314e40
    virtual void Virtual18(int,int); //314b40
    virtual void Virtual20(ListArea*,int,int); //3142c0
    virtual void Virtual24(ListArea*); //3142e0
    virtual void Virtual28(ListArea*,int,int); //314310
    virtual void Virtual2C(ListArea*,int,int); //314340
    virtual void Virtual30(ListArea*,int,int,int); //3143b0
    virtual void Virtual34(ListArea*,int,int,int); //314370
    TreeItem* Fn314440(int); //314440
    TreeItem* GetNextNonChild(); //314470
    int GetNextNonChildPos(TreeArea*); //3144b0 support0
    int GetInsertPos(TreeArea*,TreeItem*,int); //3144d0
    void InsertChild(TreeArea*,TreeItem*,int=-1); //314570
    void RemoveAllChildren(TreeArea*); //314710
    void RemoveChild(TreeArea*,TreeItem*); //314770
    void RemoveFromParent(TreeArea*); //314830
    void Sort(TreeArea*,bool); //genuine SDK recursive children ordering; SOURCE_ONLY_NO_NATIVE_ID
    void CloseAll(); //314850
    void InsertChildren(TreeArea*,int); //314880
    void SetOpen(TreeArea*,bool,bool=false,bool=true); //3148e0
    void ToggleOpen(TreeArea*,bool=false,bool=true); //314af0 support0
    bool IsLastChild(); //314b10
    ListItem* GetMouseItem(ListArea*,int&,int&); //314210
    bool Fn314E60(int,int); //314e60
    TreeItem* mParent; //18 borrowed tree parent; actual raw parent
    bool IsOpen() { return mOpen; } //actual scalar getter,0quota
    int mX,mIndex; //1c/20
    bool mOpen; //24
    SmartPtr<ListItem> mItem; //28 owns actual wrapped item
    std::vector<SmartPtr<TreeItem> > mChildren; //2c..3b complete16B Dinkum vector
};
class TreeItemOpenedEvent : public ComponentEvent {
public:
    TreeItemOpenedEvent(Component* inComponent,TreeItem* inItem)
        :ComponentEvent(inComponent,26),mItem(inItem) {}
    // Implicit normal314a60: no own-vptr write; support only.
    SmartPtr<TreeItem> mItem; //20, complete event24
};
// Actual named TreeArea174; newctor314ea0 only. No receiverallocation/sizeof/copy/dtor body authority.
class TreeArea : public ListArea {
public:
    TreeArea(); //314ea0 actual255B ownedroot/open/listflags setup
    // Genuine SDK implicit virtual destructor314fc0; automatic real owned members and base cleanup.
    virtual void Virtual2C(int,int,int); //315260 forwarding support0
    virtual void HandleComponentEvent(ComponentEvent*); //315280
    virtual void NotifyMouseItemGone(); //315060
    virtual bool Sort(); //315050 false support0
    void SetSortType(ListSortType); //315030
    void Sort(TreeItem*,bool=false); //genuine SDK visible-subtree sorting; SOURCE_ONLY_NO_NATIVE_ID
    TreeItem* InsertString(TreeItem*,const GUIString&,Image* =NULL,int=-1);
    void CloseItem(TreeItem*,bool=false,bool=true);
    void SetTreeFlags(int,bool);
    TreeItem* Fn315080(TreeItem*,ListItem*,int=-1); //315080
    void Fn315190(TreeItem*); //315190
    void Fn3151A0(bool=true); //3151a0
    void Fn3151B0(TreeItem*); //3151b0
    void Fn315220(TreeItem*,bool=true); //315220
    void ToggleItem(TreeItem*,bool=false,bool=true); //315240
    bool TreeFlagSet(int flags) { return (mTreeFlags&flags)!=0; }
    SmartPtr<TreeItem> mRoot; //164
    int mTreeFlags; //168
    ListItem* mMouseColItem; //16c borrowed current wrappedmouseitem
    int mTreeSortType; //170
};

// Complete actual PopupMenu180: fullListArea164 plus two SPs/fourints/bool below.
// NativeCtor2fd150/full57slots prove real scalar allocation; no by-value copy authority.
// Existing external implicit destructor2fd340 owns reverseSP cleanup thenListArea.
class PopupMenu : public ListArea {
public:
    PopupMenu(); //2fd150 actual458B default configuration, support0
    // Genuine SDK implicit virtual destructor2fd340; automatic real owned members and base cleanup.
    virtual void Virtual2C(int,int,int); //2fdb70
    virtual void Virtual30(int,int,int); //2fdc10
    virtual void Virtual34(int,int); //2fdb10
    virtual bool Virtual44(int); //2fd820
    virtual void HandleComponentEvent(ComponentEvent*); //2fd7d0
    virtual void ListSelChangedHook(); //2fd860
    MenuItem* AddItem(const GUIString&,int); //2fd440,ret8
    MenuItem* AddItem(const GUIString&,ComponentListener*,int); //2fd460 genuine SDK overload,RET0c
    MenuItem* AddItem(const GUIString&,NativeImage*,ComponentListener*,int); //2fd480,ret10
    void AddSeperator(); //2fd8b0,ret0
    void EndSubMenu(); //2fd580 actual custom submenu cleanup
    void CheckSubMenu(); //2fd610 actual custom submenu activation
    bool TryKeyDown(int); //genuine SDK recursive submenu keyboard policy, not TimerEvent
    void Reset(); //2fd400 originalexternal, clears submenu then selectedindex
protected:
    SmartPtr<PopupMenu> mCurSubMenu; //164, original owns and releases
    SmartPtr<ListItem> mCurSubMenuItem; //168, original owns and releases
    int mMouseUpCount; //16c
    int mTimerAcc; //170
    int mLastX,mLastY; //174,178
    bool mKeyboardMode; //17c; naturalpadding17d..17f, complete180; mLastX/Y ctor intentionally untouched
};

// Complete genuine same-SDK menu owners. Native identities remain unlocated.
enum MenuBarState { MenuBarState_None,MenuBarState_Up,MenuBarState_Down };
class MenuBarItem : public Component {
protected:
    SmartPtr<PopupMenu> mPopup;
    SmartPtr<Font> mFont;
    GUIString mText;
    MenuBarState mState;
public:
    virtual void Paint(Graphics&);
    MenuBarItem();
    void SetMenuBarState(MenuBarState);
    void SetText(const GUIString&);
    void SetPopup(PopupMenu*);
    PopupMenu* GetPopup() { return mPopup.get(); }
};
class MenuItemSelectedEvent : public ComponentEvent {
public:
    SmartPtr<MenuItem> mItem;
    virtual void Deliver();
    MenuItemSelectedEvent(Component* menu,MenuItem* item)
      :ComponentEvent(menu,ComponentEvent_MenuItemSelected),mItem(item) {}
};
class MenuBar : public Container {
protected:
    typedef std::vector<MenuBarItem*> ItemVector;
    ItemVector mItemVector;
    MenuBarState mState;
    MenuBarItem* mCurItem;
    int mLastX,mLastY;
    Background mBackground;
    void CheckItem(Component*);
public:
    virtual void Virtual38(int,int);
    virtual void Virtual34(int,int);
    virtual void Virtual2C(int,int,int);
    virtual void Virtual30(int,int,int);
    virtual void Virtual28();
    virtual bool Virtual54(int);
    virtual void HandleComponentEvent(ComponentEvent*);
    virtual void Paint(Graphics&);
    MenuBar();
    void AddItem(const GUIString&,PopupMenu*);
    void AddSeperator(Component* =NULL);
};

class ScrollAreaViewport : public Container {
public:
    ScrollAreaViewport(ScrollArea* area) : mScrollArea(area) {} //actualSDKinline, native304e70..e83
    void Reset(); //305c70 actual original contents/popup reset, no args
    void InvalidateIfRectVisible(WONRectangle&); //305c10 genuine SDK nonvirtual nested viewport geometry policy
    // Genuine SDK implicit virtual destructor306a60; automatic real owned members and base cleanup.
    ScrollArea* mScrollArea; //84 borrowedbacklink; actualcomplete88 new304e49, no own cleanup
    virtual void Paint(Graphics&); //18 actual305e00
    virtual bool Virtual44(int); //44 actual305dd0
    virtual void Virtual78(Cursor*); //78 actual305de0
    virtual void VirtualB4(Component*); //b4 actual305db0
    virtual void VirtualB8(Component*); //b8 actual305dc0
    virtual void VirtualBC(Component*); //bc actual2fa500
    virtual bool VirtualC0(Component*); //c0 actual305d70
};

class MultiListItem : public ListItem {
protected:
    typedef std::vector<SmartPtr<ListItem> > ItemVector;
    ItemVector mItems; //18, allocator18/first1c/end20/cap24
    ListItem* GetMouseItem(ListArea*,int&);
    ListItem* GetMouseItem(ListArea*,int&,int&);
public:
    // Genuine SDK implicit default constructor3fa110; ListItem and ItemVector construct automatically.
    // Genuine SDK implicit virtual destructor303db0; automatic owning ItemVector/ListItem cleanup.
    void InsertItem(ListItem*); //genuine SDK owning cell append
    void SetItem(ListItem*,int); //actual302480, native cell setter/resizer support0
    ListItem* GetItem(int) const; //external actual302500; bounds getter0
    virtual void Virtual04(Graphics&,ListArea*);
    virtual void Virtual08(ListArea*);
    virtual void Virtual0C(ListArea*,Container*);
    virtual int Virtual10(const ListItem*,ListArea*) const;
    virtual bool Virtual14(ListArea*,int,int);
    virtual void Virtual18(int,int);
    virtual bool Virtual1C(int,int inY) { return inY>=mTop && inY<mTop+mHeight; }
    virtual void Virtual20(ListArea*,int,int);
    virtual void Virtual24(ListArea*);
    virtual void Virtual28(ListArea*,int,int);
    virtual void Virtual2C(ListArea*,int,int);
    virtual void Virtual30(ListArea*,int,int,int);
    virtual void Virtual34(ListArea*,int,int,int);
};

// Complete actual MultiListArea1a4 owner. Bounded GetListCtrl allocation calls original constructor; no lifetime source or copy authority.
class MultiListHeader;
class MultiListArea : public ListArea {
public:
    MultiListArea(int=1); //3033a0 actual original ctor RET4, complete1a4
    virtual void NotifyMouseItemGone(); //3035d0 actual d8 override, inherited ListArea hook
    void SetNumColumns(int); //303600/393
    void SetColumnSizeMode(int,unsigned char); //genuine SDK partner-column size policy, SOURCE_ONLY_NO_NATIVE_ID
    MultiListItem* InsertRow(int=-1); //genuine SDK new blank-row insertion
    void SetColumnWidth(int,int,bool); //3037e0/382
    // Genuine SDK implicit virtual destructor303540; automatic ColumnInfo vector/ListArea cleanup.
    int GetColumnPaintContext() { return mColumnPaintContext; }
    int GetSortColumn() { return mSortColumn; }
    struct ColumnInfo {
        SmartPtr<ScrollAreaViewport> mContainer;
        int mx, mWidth;
        ListArea::SortFunc mSortFunc;
        unsigned char mSizeMode;
        int mPartnerColumn;
        unsigned char mHAlign;
        ColumnInfo() : mx(0),mWidth(0),mSortFunc(NULL),mSizeMode(1),mPartnerColumn(-1),mHAlign(0) {} //actual native303627..303652 default value; support0
    }; //actual complete1c value including owning SmartPtr; native SetNumColumns resizes this vector
    std::vector<ColumnInfo> mColumnInfoVector; //164..173
    ListArea::SortFunc mSortColFunc; //174
    int mColumnPaintContext; //178
    int mTotalColumnWidth; //17c
    int mSortColumn; //180
    int mSortColumnPrev; //184
    int mColumnBorderWidth; //188
    int mDynamicSizeColumn; //18c
    bool mAutoGrowColumns; //190
    ListItem* mMouseColItem; //194, borrowed
    int mMouseColNum; //198
    int mMaxTotalColumnWidth; //19c native3038ac, SDK exact field
    MultiListHeader* mListHeader; //1a0 borrowed, native3035f0/303b24
    void NotifyListHeader(); //3035f0 original external support0
    void SetSortColumn(int,ListSortType); //303ac0
    void SetSortColumnFunc(int,SortFunc); //303b50 support0
    void SetColumnHAlign(int,HorzAlign); //303790 actual external
    virtual void VirtualCC(bool); //303bb0 AdjustPosition support0
    virtual void Virtual08(); //303be0 SizeChanged
    virtual void VirtualD4(Graphics&); //303c70 ScrollPaint
    MultiListItem* UpdateItem(MultiListItem*); //303e50
    MultiListItem* SetCell(int,int,ListItem*); //303ee0
    int GetColumnAt(int); //3039c0
    int GetColumnPos(int); //303a60
    int GetColumnWidth(int); //303960
    MultiListItem* InsertItem(MultiListItem*, int = -1); //303e30
    MultiListItem* SetString(int, int, const GUIString&, Image* = NULL); //303f60
};

// Complete actual ac Container51 descendant; field graph below matches native302a90.
// Genuine constructor declaration permits automatic base construction; no copy authority.
// TD4d64a0/COL476030/VT44f170; all51 targets/ten actual overrides independently
// tabulated in MULTILISTHEADER_OVERRIDE_SURFACE.json (other41 inherit Container).
class MultiListHeader : public Container {
public:
    MultiListHeader(MultiListArea* =NULL); //302a90 genuine external ctor RET4, fullac
    // Genuine SDK implicit virtual destructor302b50; automatic ColumnVector/Container cleanup.
    virtual void Virtual08(); //08=302fb0,SizeChanged
    virtual void Paint(Graphics&); //18=305e00
    virtual void Virtual28(); //28=303390,MouseExit
    virtual void Virtual2C(int,int,int); //2c=303260,MouseUp
    virtual void Virtual30(int,int,int); //30=303350,MouseDown
    virtual void Virtual34(int,int); //34=303290,MouseMove
    virtual void Virtual38(int,int); //38=3032f0,MouseDrag
    virtual void HandleComponentEvent(ComponentEvent*); //7c=302be0
    virtual Component* VirtualAC(int,int); //ac=304710,FindChildAt
    void SetColumnItem(int,Component*); //302cf0/283,ret8
    void SetMultiListArea(MultiListArea*); //302bc0 original external, ret4
    void NotifyColumnChange(); //302e10 actual external
    void SetSortColumn(int,ListSortType); //302cb0 actual external
    static ListSortType GetSortTypeForButton(Component*); //303050 actual original provider
protected:
    struct ColumnInfo {
        Component* mComponent; //borrowed child, Container owns actual child
        ListSortType mSortType;
        ColumnInfo(Component* c=NULL) : mComponent(c),mSortType(ListSortType_None) {} //302d1c..302d29 actual resize default
    }; //complete8 value; no owning pointer
    typedef std::vector<ColumnInfo> ColumnVector;
    ColumnVector mColumnVector; //84 allocator/88first/8cend/90cap, native ctor/dtor and all accessors
    MultiListArea* mListArea; //94 borrowed,302bc0 setter/backlink
    int mDragColumn; //98
    Component* mDragComponent; //9c borrowed
    int mDragX; //a0
    int mCurSortCol; //a4
    bool mDrawSortArrow; //a8, natural a9..ab alignment gives fullac
    Component* GetDragComponent(int); //3030b0 actual425B
public:
    int GetColumnIndex(Component*); //302fd0 actual55B
    ListSortType GetSortType(int); //303010 actual64B
};


struct GUIStringLessNoCase {
    bool operator()(const GUIString& inLeft, const GUIString& inRight) const
    { return inLeft.CompareNoCase(inRight) < 0; }
};

// Actual borrowed Table140 receiver, native VT4501d0 /54slots; ScrollArea110 base.
// No construction/new/sizeof/copy or lifetime-source authority. Existing original
// AnchorMap owner134 permits proven mutation only; all children remain borrowed/retained as below.
class Table : public ScrollArea {
public:
    Table(LineSegment* =NULL); //30f9d0 genuine SDK ctor; complete140, original base/owned members
    // Genuine SDK implicit destructor: native normal30fbb0/304, scalar30fad0; real owned AnchorMap/contents/base cleanup.
    virtual void Virtual24(int,int); //3100b0 MouseEnter
    virtual void Virtual28(); //3100e0 MouseExit
    virtual void Virtual2C(int,int,int); //310100 MouseDown, preserve canonical existing slot signature
    virtual void Virtual30(int,int,int); //310250 MouseUp
    virtual void Virtual34(int,int); //3102b0 MouseMove
    virtual void Virtual38(int,int); //310300 MouseDrag
    virtual bool Virtual44(int); //310450 TimerEvent
    virtual bool Virtual50(int); //30fdc0 KeyChar
    virtual bool Virtual54(int); //30fe00 KeyDown
    virtual Component* VirtualB0(int,int); //310040 WMFindChildAt
    virtual void VirtualD0(); //310660 ScrollSizeChanged
    virtual void VirtualD4(Graphics&); //310670 ScrollPaint
    void MouseAt(int&,int&,bool=false); //30ff20 actual RET0c
    void CopySelection(); //30fce0
    void SetContents(LineSegment*); //310500
    void SetMaxWidth(bool=true); //3105b0 genuine nonvirtual bool argument
    void AddAnchor(const GUIString&,int,int); //310700
    bool GotoAnchor(const GUIString&); //310730
    bool FindStr(const GUIString&,bool=true,bool=false); //3107b0 actual RET0c
    SmartPtr<LineSegment> mContents; //110 retained original owner
    LineSegment* mMouseSegment; //114 borrowed current hit
    int mStartSelSegment,mEndSelSegment; //118,11c
    int mStartSelSegmentPos,mEndSelSegmentPos; //120,124
    bool mBackwardsSel,mSelecting; //128,129; natural12a..12b alignment
    int mMouseSegmentDX,mMouseSegmentDY; //12c,130
    struct AnchorCoordinate {
        int mx,my;
        AnchorCoordinate(int x=0,int y=0) : mx(x),my(y) {}
    };
    typedef std::map<GUIString,AnchorCoordinate,GUIStringLessNoCase> AnchorMap;
    AnchorMap mAnchorMap; //134 allocator/compare bytes,138 native head,13c size; actual12 map
};
class HTMLTag;
class TextLinkParam : public RefCount {
public:
    GUIString mLinkParam; //08
    std::list<LineSegment*> mLinkList; //0c..17, borrowed line pointers
    bool mLinkIsDown,mLaunchBrowser; //18,19, complete1c
    void LinkDown(); //30ab50
    bool LinkUp(); //30ab80
    TextLinkParam(const GUIString& p,bool launch) : mLinkParam(p),mLinkIsDown(false),mLaunchBrowser(launch) {}
    virtual ~TextLinkParam(); //actual ordinary33a170/deleting33a150, external lifetime
};
// Complete actual HyperLinkEvent24; native30ac35/30adbd/30ae5f allocate24.
// Genuine SDK inline constructor retains actual param20 after original ComponentEvent20.
class HyperLinkEvent : public ComponentEvent {
public:
    HyperLinkEvent(Component* component,int type,TextLinkParam* param)
        :ComponentEvent(component,type),mLinkParam(param) {} //native30ac51..30ac82; support0
    SmartPtr<TextLinkParam> mLinkParam; //20 actual retained owner in original event
};
// Genuine pinned Keyboard.h modifiers; target native tests bits1/2.
enum { KEYMOD_SHIFT=0x0001,KEYMOD_ALT=0x0002,KEYMOD_CTRL=0x0004 };
class ImageMap : public RefCount {
public:
    struct ShapeLink {
        SmartPtr<TextLinkParam> mLinkParam;
        WONRectangle mShape;
        ShapeLink(TextLinkParam* p,WONRectangle& r) : mLinkParam(p),mShape(r) {}
    }; //complete14 value: SP4 + rectangle10
    std::list<ShapeLink> mShapeList; //08..13, complete14 owner
    virtual ~ImageMap(); //actual ordinary33df20/deleting33df00 external lifetime
    TextLinkParam* GetLink(int,int); //30aed0 borrowed result
    void AddLink(TextLinkParam* p,WONRectangle& r) { mShapeList.push_back(ShapeLink(p,r)); } //actual SDK/native support0
};
enum HTMLListType
{
	HTMLListType_None,
	HTMLListType_Disc,
	HTMLListType_Circle,
	HTMLListType_Square,
	HTMLListType_Digit,
	HTMLListType_LowerLetter,
	HTMLListType_UpperLetter,
	HTMLListType_LowerRoman,
	HTMLListType_UpperRoman
};
class HTMLBodyTag;
class HTMLImageTag;
class HTMLInputTag;
class HTMLImageMapTag;
class HTMLAreaTag;
class HTMLListTag;
class HTMLListItemTag;
// Complete actual HTMLDocument80 member contract. Original lifetime remains external.
// Native ctor33c610 and Parser34461e allocation80 prove fields; no new lifetime body here.
class HTMLDocument : public RefCount {
protected:
    GUIString mBasePath; //08
    Window* mWindow; //0c borrowed
    bool mDelayImages; //10
    std::list<SmartPtr<HTMLTag> > mTagList; //14..1f
    std::list<SmartPtr<HTMLTag> >::iterator mTagItr,mBodyStart,mBodyEnd; //20,24,28
public:
    struct LoadedImageStruct {
        SmartPtr<RawImage> mRawImage; //00
        SmartPtr<NativeImage> mImage; //04
        SmartPtr<Animation> mAnimation; //08
        bool mHaveUsedAnimation; //0c, complete10
        LoadedImageStruct(NativeImage* =NULL,Animation* =NULL); //external original
    };
    typedef std::map<GUIString,LoadedImageStruct,GUIStringLessNoCase> LoadedImageMap;
protected:
    LoadedImageMap mLoadedImageMap,mScaledImageMap; //2c,38
    bool mDecodingImages; //44
    std::list<SmartPtr<HTMLTag> > mImageTagList; //48..53
    SmartPtr<ImageMap> mCurImageMap; //54
    typedef std::map<GUIString,SmartPtr<ImageMap>,GUIStringLessNoCase> ImageMapMap;
    ImageMapMap mImageMapMap; //58..63
    struct ListInfo { bool mOrdered; HTMLListType mType; int mCount; }; //0c
    std::vector<ListInfo> mListStack; //64..73
    typedef std::multimap<GUIString,SmartPtr<HTMLTag>,GUIStringLessNoCase> ImageTagMap;
    ImageTagMap mImageTagMap; //74..7f
    LoadedImageStruct* GetImage(const GUIString&,HTMLTag*); //33d010
    NativeImage* CheckScaleImage(LoadedImageStruct*,const GUIString&,int,int); //33d560
    void HandleBody(HTMLBodyTag*); //33e410 external original
    void HandleImage(HTMLImageTag*); //33d9e0 external original
    void HandleInput(HTMLInputTag*); //33dfa0 external original
    void HandleImageMap(HTMLImageMapTag*); //33e020 external original
    void HandleArea(HTMLAreaTag*); //33e270 external original
    void HandleList(HTMLListTag*); //33e680 external original
    void HandleListItem(HTMLListItemTag*); //33e7b0 external original
public:
    std::list<SmartPtr<HTMLTag> >& GetTagList() { return mTagList; } //genuine SDK/native33c43e actual list14
    HTMLDocument(const GUIString&); //33c610 external full80 owner constructor
    void Clear(); //33cbd0 external actual clear
    void DecodeImages(bool); //33edb0
    bool DoDecodeImage(LoadedImageStruct*,Window*,const std::string&); //33ce10 genuine public SDK API
    LoadedImageMap& GetLoadedImageMap() {return mLoadedImageMap;}
    virtual ~HTMLDocument(); //native deleting33c780, actual sole slot, ordinary target external
    void AddTag(HTMLTag*); //33ebd0
    void AddDocument(HTMLDocument*); //33eb80
    void Rewind(bool=true); //genuine SDK document iterator mode; SOURCE_ONLY_NO_NATIVE_ID
    HTMLTag* GetNextTag(bool=true); //genuine SDK body/document terminal bounds; SOURCE_ONLY_NO_NATIVE_ID
    void SetBasePath(const GUIString&); //33ccf0
    std::list<SmartPtr<HTMLTag> >::iterator GetBodyStart(); //33ef70 helper0
    std::list<SmartPtr<HTMLTag> >::iterator GetBodyEnd(); //33efa0 helper0
};
// Genuine cached SDK enum; native Browser ctor stores Medium=2 at8c.
enum HTMLFontSize {
    HTMLFontSize_Smallest,HTMLFontSize_Small,HTMLFontSize_Medium,
    HTMLFontSize_Large,HTMLFontSize_Largest
};
class HTTPCache;
class HTTPSession;
class BrowserComponent : public Container {
public:
    BrowserComponent(); //3fad10 explicit original constructor, completee0
    void SetTransparent(bool); //3fd210 original olderSDK target API
    virtual ~BrowserComponent(); //native normal3faf30; deleting3faf10, 51slot reference contract
    virtual void Virtual6C(); //AddedToParent3fb0e0, no args
    virtual void HandleComponentEvent(ComponentEvent*); //3fd350, ret4
    bool StopButtonEnabled(); //3fb230
    bool BackButtonEnabled(); //3fb250
    bool ForwardButtonEnabled(); //3fb280
    void Reset(); //3fc480
    void Refresh(); //3fc4e0
    void Stop(); //3fc510
    void Go(HTMLDocument*); //3fc590, ret4
    void Go(const GUIString&,bool=false,bool=false,int=-1); //3fc800, ret10
    void Back(); //3fcf60
    void Forward(); //3fcfc0
    bool FindStr(const GUIString&,bool=true,bool=false); //3fd1f0, ret0c
    Table* GetHTMLView() { return mHTMLView.get(); }
    void SetScroller(Scroller*); //3fb1a0 actual original ownership/layout policy
    const GUIString& GetCurStatus() { return mCurStatus; }
    void SetFontSize(HTMLFontSize size) { mFontSize=size; }
    const GUIString& GetCurLocation() { return mCurLocation; }
    void SetLaunchBrowserOnLink(bool inLaunch) { mLaunchBrowserOnLink=inLaunch; }
    // Actual external-owned/private Table,Scroller,fontSize,cache,session,document,background region.
    // No construction or own lifetime implementation; this is not a padding-size claim.
    SmartPtr<Table> mHTMLView; //84, actual external-owned Table
    SmartPtr<Scroller> mScroller; //88, native3fb1a9..3fb1e5 actual retained owner
    HTMLFontSize mFontSize; //8c native32-bit enum, default2=Medium
    SmartPtr<HTTPCache> mHTTPCache; //90, original retained cache; no local allocation
    SmartPtr<HTTPSession> mHTTPSession; //94, original retained session; no local allocation
    SmartPtr<HTMLDocument> mHTMLDocument; //98, original retained document
    bool mHaveBackground; //9c, native byte; natural9d..9f alignment before a0
    GUIString mCurStatus; //a0, GetEmpty+SmartPtr ctor/dtor and Label consumer closed
    GUIString mCurPath; //a4
    GUIString mCurAnchor; //a8
    std::string mHTMLDocumentPath; //ac
    bool mLaunchBrowserOnLink; //bc, byte ctor/read and consumers write true
    int mNumPendingOps; //c0, natural bd..bf padding before this field
    int mVertOffset; //c4
    struct PageLocation {
        GUIString mLocation; //00 actual retained GUIString value
        int mVertOffset; //04, complete8 value
        PageLocation() :mVertOffset(0) {} //genuine SDK inline, support0
        PageLocation(const GUIString& location,int offset) :mLocation(location),mVertOffset(offset) {}
    };
    typedef std::vector<PageLocation> PageVector;
    PageVector mPageVector; //c8, data cc/end d0/cap d4 actual stride8
    int mCurPage; //d8
    GUIString mCurLocation; //dc
protected:
    void GotoAnchor(); //3fc540
    void RememberVertOffset(); //3fc7c0
    void GetLinkPath(const std::string&,std::string&); //3fd030
    void SetStatus(const GUIString&); //3fb2c0 support0
    void FinishImages(); //3fb6a0
    struct ImageParam : public RefCount {
        SmartPtr<BrowserComponent> mBrowser; //08 retained
        HTMLDocument::LoadedImageStruct* mImageStruct; //0c borrowed, full10
        ImageParam(BrowserComponent* browser,HTMLDocument::LoadedImageStruct* image) :mBrowser(browser),mImageStruct(image) {}
    };
    typedef SmartPtr<ImageParam> ImageParamPtr;
    static void DownloadImageCompletion(AsyncOpPtr,ImageParamPtr); //3fb930 actual cdecl byvalue owners
    bool DownloadImages(); //3fbc10 actual bool/RET0 original external provider
    void ParseDocument(); //3fc0d0 actual void/RET0 original external provider
    static void DownloadDocumentCompletion(AsyncOpPtr,SmartPtr<RefCount>); //3fb320 actual cdecl two-byvalueSP callback
    // Borrowed-only e0 prefix, no receiver construction/new/sizeof/copy/lifecycle.
};
// Full14 value-lifetime class, no virtual slots. Native ctor33efc0/implicitdtor3c0fb0.
class HTMLDocumentGen {
public:
protected:
    bool AddDocPrv(const GUIString&,int,bool); //33f020
public:
    HTMLDocumentGen(HTMLDocument* =NULL);
    void SetDocument(HTMLDocument*); //33f440 genuine SDK owned document replacement
    bool AddDoc(const GUIString&,int=0); //33f1f0 forwarding20 support0
    bool AddDocParam(const GUIString&,int=0); //33f210 ->AddDocPrv(...,true)
    void printf(const GUIString&); //33f230
    void AddTag(HTMLTag*); //genuine SDK document delegate; SOURCE_ONLY_NO_NATIVE_ID
    void AddTagParam(HTMLTag*); //genuine SDK owned parameter document insertion; SOURCE_ONLY_NO_NATIVE_ID
    HTMLDocument* GetDocument() { return mDocument.get(); }
    SmartPtr<HTMLDocument> mDocument; //0
    std::vector<SmartPtr<HTMLDocument> > mDocumentVector; //4, allocator4/data8/endc/cap10
    // Keep natural implicit destructor, matching native full owner graph; no empty external stand-in.
};

// Original-owned HTMLDisplayGen; actual borrowed prefix/API in WONLobbyHTMLDisplayGen.h.
class HTMLDisplayGen;

// 0x3c5ff0: the sort/filter handlers of a server-list column


// 0x8dec84 / 0x8dec88 / 0x8df4d0 / 0x8d954c: the lobby's object factories


}

#endif
