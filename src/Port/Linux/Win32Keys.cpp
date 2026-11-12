// The keys of the native Linux build: each SDL scancode with its Windows virtual-key code and its PC scan code (set
// 1). DirectInput's DIK codes are the scan codes, with 0x80 added for the extended (E0-prefixed) keys.
#include "Win32Internal.h"

#include <SDL.h>

namespace {

struct Entry {
    SDL_Scancode mScancode;
    BYTE mVirtualKey;
    BYTE mScanCode;
    bool mExtended;
};

const Entry kKeys[] = {
    {SDL_SCANCODE_ESCAPE, VK_ESCAPE, 0x01, false},
    {SDL_SCANCODE_1, '1', 0x02, false}, {SDL_SCANCODE_2, '2', 0x03, false}, {SDL_SCANCODE_3, '3', 0x04, false},
    {SDL_SCANCODE_4, '4', 0x05, false}, {SDL_SCANCODE_5, '5', 0x06, false}, {SDL_SCANCODE_6, '6', 0x07, false},
    {SDL_SCANCODE_7, '7', 0x08, false}, {SDL_SCANCODE_8, '8', 0x09, false}, {SDL_SCANCODE_9, '9', 0x0a, false},
    {SDL_SCANCODE_0, '0', 0x0b, false},
    {SDL_SCANCODE_MINUS, VK_OEM_MINUS, 0x0c, false}, {SDL_SCANCODE_EQUALS, VK_OEM_PLUS, 0x0d, false},
    {SDL_SCANCODE_BACKSPACE, VK_BACK, 0x0e, false}, {SDL_SCANCODE_TAB, VK_TAB, 0x0f, false},
    {SDL_SCANCODE_Q, 'Q', 0x10, false}, {SDL_SCANCODE_W, 'W', 0x11, false}, {SDL_SCANCODE_E, 'E', 0x12, false},
    {SDL_SCANCODE_R, 'R', 0x13, false}, {SDL_SCANCODE_T, 'T', 0x14, false}, {SDL_SCANCODE_Y, 'Y', 0x15, false},
    {SDL_SCANCODE_U, 'U', 0x16, false}, {SDL_SCANCODE_I, 'I', 0x17, false}, {SDL_SCANCODE_O, 'O', 0x18, false},
    {SDL_SCANCODE_P, 'P', 0x19, false},
    {SDL_SCANCODE_LEFTBRACKET, VK_OEM_4, 0x1a, false}, {SDL_SCANCODE_RIGHTBRACKET, VK_OEM_6, 0x1b, false},
    {SDL_SCANCODE_RETURN, VK_RETURN, 0x1c, false}, {SDL_SCANCODE_LCTRL, VK_CONTROL, 0x1d, false},
    {SDL_SCANCODE_A, 'A', 0x1e, false}, {SDL_SCANCODE_S, 'S', 0x1f, false}, {SDL_SCANCODE_D, 'D', 0x20, false},
    {SDL_SCANCODE_F, 'F', 0x21, false}, {SDL_SCANCODE_G, 'G', 0x22, false}, {SDL_SCANCODE_H, 'H', 0x23, false},
    {SDL_SCANCODE_J, 'J', 0x24, false}, {SDL_SCANCODE_K, 'K', 0x25, false}, {SDL_SCANCODE_L, 'L', 0x26, false},
    {SDL_SCANCODE_SEMICOLON, VK_OEM_1, 0x27, false}, {SDL_SCANCODE_APOSTROPHE, VK_OEM_7, 0x28, false},
    {SDL_SCANCODE_GRAVE, VK_OEM_3, 0x29, false}, {SDL_SCANCODE_LSHIFT, VK_SHIFT, 0x2a, false},
    {SDL_SCANCODE_BACKSLASH, VK_OEM_5, 0x2b, false}, {SDL_SCANCODE_NONUSHASH, VK_OEM_5, 0x2b, false},
    {SDL_SCANCODE_Z, 'Z', 0x2c, false}, {SDL_SCANCODE_X, 'X', 0x2d, false}, {SDL_SCANCODE_C, 'C', 0x2e, false},
    {SDL_SCANCODE_V, 'V', 0x2f, false}, {SDL_SCANCODE_B, 'B', 0x30, false}, {SDL_SCANCODE_N, 'N', 0x31, false},
    {SDL_SCANCODE_M, 'M', 0x32, false},
    {SDL_SCANCODE_COMMA, VK_OEM_COMMA, 0x33, false}, {SDL_SCANCODE_PERIOD, VK_OEM_PERIOD, 0x34, false},
    {SDL_SCANCODE_SLASH, VK_OEM_2, 0x35, false}, {SDL_SCANCODE_RSHIFT, VK_SHIFT, 0x36, false},
    {SDL_SCANCODE_KP_MULTIPLY, VK_MULTIPLY, 0x37, false}, {SDL_SCANCODE_LALT, VK_MENU, 0x38, false},
    {SDL_SCANCODE_SPACE, VK_SPACE, 0x39, false}, {SDL_SCANCODE_CAPSLOCK, VK_CAPITAL, 0x3a, false},
    {SDL_SCANCODE_F1, VK_F1, 0x3b, false}, {SDL_SCANCODE_F2, VK_F2, 0x3c, false},
    {SDL_SCANCODE_F3, VK_F3, 0x3d, false}, {SDL_SCANCODE_F4, VK_F4, 0x3e, false},
    {SDL_SCANCODE_F5, VK_F5, 0x3f, false}, {SDL_SCANCODE_F6, VK_F6, 0x40, false},
    {SDL_SCANCODE_F7, VK_F7, 0x41, false}, {SDL_SCANCODE_F8, VK_F8, 0x42, false},
    {SDL_SCANCODE_F9, VK_F9, 0x43, false}, {SDL_SCANCODE_F10, VK_F10, 0x44, false},
    {SDL_SCANCODE_NUMLOCKCLEAR, VK_NUMLOCK, 0x45, false}, {SDL_SCANCODE_SCROLLLOCK, VK_SCROLL, 0x46, false},
    {SDL_SCANCODE_KP_7, VK_NUMPAD7, 0x47, false}, {SDL_SCANCODE_KP_8, VK_NUMPAD8, 0x48, false},
    {SDL_SCANCODE_KP_9, VK_NUMPAD9, 0x49, false}, {SDL_SCANCODE_KP_MINUS, VK_SUBTRACT, 0x4a, false},
    {SDL_SCANCODE_KP_4, VK_NUMPAD4, 0x4b, false}, {SDL_SCANCODE_KP_5, VK_NUMPAD5, 0x4c, false},
    {SDL_SCANCODE_KP_6, VK_NUMPAD6, 0x4d, false}, {SDL_SCANCODE_KP_PLUS, VK_ADD, 0x4e, false},
    {SDL_SCANCODE_KP_1, VK_NUMPAD1, 0x4f, false}, {SDL_SCANCODE_KP_2, VK_NUMPAD2, 0x50, false},
    {SDL_SCANCODE_KP_3, VK_NUMPAD3, 0x51, false}, {SDL_SCANCODE_KP_0, VK_NUMPAD0, 0x52, false},
    {SDL_SCANCODE_KP_PERIOD, VK_DECIMAL, 0x53, false}, {SDL_SCANCODE_NONUSBACKSLASH, VK_OEM_102, 0x56, false},
    {SDL_SCANCODE_F11, VK_F11, 0x57, false}, {SDL_SCANCODE_F12, VK_F12, 0x58, false},
    {SDL_SCANCODE_KP_ENTER, VK_RETURN, 0x1c, true}, {SDL_SCANCODE_RCTRL, VK_CONTROL, 0x1d, true},
    {SDL_SCANCODE_KP_DIVIDE, VK_DIVIDE, 0x35, true}, {SDL_SCANCODE_PRINTSCREEN, VK_SNAPSHOT, 0x37, true},
    {SDL_SCANCODE_RALT, VK_MENU, 0x38, true}, {SDL_SCANCODE_PAUSE, VK_PAUSE, 0x45, true},
    {SDL_SCANCODE_HOME, VK_HOME, 0x47, true}, {SDL_SCANCODE_UP, VK_UP, 0x48, true},
    {SDL_SCANCODE_PAGEUP, VK_PRIOR, 0x49, true}, {SDL_SCANCODE_LEFT, VK_LEFT, 0x4b, true},
    {SDL_SCANCODE_RIGHT, VK_RIGHT, 0x4d, true}, {SDL_SCANCODE_END, VK_END, 0x4f, true},
    {SDL_SCANCODE_DOWN, VK_DOWN, 0x50, true}, {SDL_SCANCODE_PAGEDOWN, VK_NEXT, 0x51, true},
    {SDL_SCANCODE_INSERT, VK_INSERT, 0x52, true}, {SDL_SCANCODE_DELETE, VK_DELETE, 0x53, true},
    {SDL_SCANCODE_LGUI, VK_LWIN, 0x5b, true}, {SDL_SCANCODE_RGUI, VK_RWIN, 0x5c, true},
    {SDL_SCANCODE_APPLICATION, VK_APPS, 0x5d, true},
};

} // namespace

namespace eeport {

bool KeyOf(int inScancode, BYTE* outVirtualKey, BYTE* outScanCode, bool* outExtended)
{
    for (const Entry& theKey : kKeys)
        if (theKey.mScancode == inScancode) {
            *outVirtualKey = theKey.mVirtualKey;
            *outScanCode = theKey.mScanCode;
            *outExtended = theKey.mExtended;
            return true;
        }
    return false;
}

// Whether a virtual key is down now: the generic and the left/right codes of Shift, Ctrl and Alt included
bool IsKeyDown(int inVirtualKey)
{
    SDL_Keymod theModifiers = SDL_GetModState();
    switch (inVirtualKey) {
    case VK_SHIFT: return theModifiers & KMOD_SHIFT;
    case VK_LSHIFT: return theModifiers & KMOD_LSHIFT;
    case VK_RSHIFT: return theModifiers & KMOD_RSHIFT;
    case VK_CONTROL: return theModifiers & KMOD_CTRL;
    case VK_LCONTROL: return theModifiers & KMOD_LCTRL;
    case VK_RCONTROL: return theModifiers & KMOD_RCTRL;
    case VK_MENU: return theModifiers & KMOD_ALT;
    case VK_LMENU: return theModifiers & KMOD_LALT;
    case VK_RMENU: return theModifiers & KMOD_RALT;
    case VK_LBUTTON: return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_LMASK;
    case VK_RBUTTON: return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_RMASK;
    case VK_MBUTTON: return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_MMASK;
    }
    const Uint8* theState = SDL_GetKeyboardState(NULL);
    for (const Entry& theKey : kKeys)
        if (theKey.mVirtualKey == inVirtualKey && theState[theKey.mScancode])
            return true;
    return false;
}

} // namespace eeport

// The high bit is set while the key is down; the low bit is the toggle state of Caps Lock, Num Lock and Scroll Lock
SHORT WINAPI GetKeyState(INT inVirtualKey)
{
    SHORT theState = eeport::IsKeyDown(inVirtualKey) ? (SHORT)0x8000 : 0;
    SDL_Keymod theModifiers = SDL_GetModState();
    if ((inVirtualKey == VK_CAPITAL && (theModifiers & KMOD_CAPS)) ||
        (inVirtualKey == VK_NUMLOCK && (theModifiers & KMOD_NUM)) ||
        (inVirtualKey == VK_SCROLL && (theModifiers & KMOD_SCROLL)))
        theState |= 1;
    return theState;
}
