// lle/ISKeyCode.h (RECONSTRUCTION). Named integer constants; each comment gives the evidence for its name; (provisional
// name) marks an inferred one.
#ifndef EE_ISKEYCODE_H
#define EE_ISKEYCODE_H

// ISKeyboard entity index: the DirectInput DIK_* scan code; 0xe0..0xe2 the either-side ISExtendedKeys; 0x7fff no key
enum ISKeyCode {
    kKeyEscape = 0x01,     // DirectInput 7 dinput.h DIK_ESCAPE = 0x01
    kKey1 = 0x02,          // DirectInput 7 dinput.h DIK_1 = 0x02
    kKey2 = 0x03,          // DirectInput 7 dinput.h DIK_2 = 0x03
    kKey3 = 0x04,          // DirectInput 7 dinput.h DIK_3 = 0x04
    kKey4 = 0x05,          // DirectInput 7 dinput.h DIK_4 = 0x05
    kKey5 = 0x06,          // DirectInput 7 dinput.h DIK_5 = 0x06
    kKey6 = 0x07,          // DirectInput 7 dinput.h DIK_6 = 0x07
    kKey7 = 0x08,          // DirectInput 7 dinput.h DIK_7 = 0x08
    kKey8 = 0x09,          // DirectInput 7 dinput.h DIK_8 = 0x09
    kKeyBack = 0x0e,       // DirectInput 7 dinput.h DIK_BACK = 0x0e
    kKeyTab = 0x0f,        // DirectInput 7 dinput.h DIK_TAB = 0x0f
    kKeyQ = 0x10,          // DirectInput 7 dinput.h DIK_Q = 0x10
    kKeyW = 0x11,          // DirectInput 7 dinput.h DIK_W = 0x11
    kKeyR = 0x13,          // DirectInput 7 dinput.h DIK_R = 0x13
    kKeyT = 0x14,          // DirectInput 7 dinput.h DIK_T = 0x14
    kKeyY = 0x15,          // DirectInput 7 dinput.h DIK_Y = 0x15
    kKeyU = 0x16,          // DirectInput 7 dinput.h DIK_U = 0x16
    kKeyI = 0x17,          // DirectInput 7 dinput.h DIK_I = 0x17
    kKeyLBracket = 0x1a,   // DirectInput 7 dinput.h DIK_LBRACKET = 0x1a
    kKeyRBracket = 0x1b,   // DirectInput 7 dinput.h DIK_RBRACKET = 0x1b
    kKeyReturn = 0x1c,     // DirectInput 7 dinput.h DIK_RETURN = 0x1c
    kKeyA = 0x1e,          // DirectInput 7 dinput.h DIK_A = 0x1e
    kKeyS = 0x1f,          // DirectInput 7 dinput.h DIK_S = 0x1f
    kKeyD = 0x20,          // DirectInput 7 dinput.h DIK_D = 0x20
    kKeyF = 0x21,          // DirectInput 7 dinput.h DIK_F = 0x21
    kKeyG = 0x22,          // DirectInput 7 dinput.h DIK_G = 0x22
    kKeyH = 0x23,          // DirectInput 7 dinput.h DIK_H = 0x23
    kKeyL = 0x26,          // DirectInput 7 dinput.h DIK_L = 0x26
    kKeySemicolon = 0x27,  // DirectInput 7 dinput.h DIK_SEMICOLON = 0x27
    kKeyGrave = 0x29,      // DirectInput 7 dinput.h DIK_GRAVE = 0x29
    kKeyBackslash = 0x2b,  // DirectInput 7 dinput.h DIK_BACKSLASH = 0x2b
    kKeyZ = 0x2c,          // DirectInput 7 dinput.h DIK_Z = 0x2c
    kKeyX = 0x2d,          // DirectInput 7 dinput.h DIK_X = 0x2d
    kKeyC = 0x2e,          // DirectInput 7 dinput.h DIK_C = 0x2e
    kKeyV = 0x2f,          // DirectInput 7 dinput.h DIK_V = 0x2f
    kKeyB = 0x30,          // DirectInput 7 dinput.h DIK_B = 0x30
    kKeyN = 0x31,          // DirectInput 7 dinput.h DIK_N = 0x31
    kKeyM = 0x32,          // DirectInput 7 dinput.h DIK_M = 0x32
    kKeyComma = 0x33,      // DirectInput 7 dinput.h DIK_COMMA = 0x33
    kKeyPeriod = 0x34,     // DirectInput 7 dinput.h DIK_PERIOD = 0x34
    kKeySlash = 0x35,      // DirectInput 7 dinput.h DIK_SLASH = 0x35
    kKeySpace = 0x39,      // DirectInput 7 dinput.h DIK_SPACE = 0x39
    kKeyF1 = 0x3b,         // DirectInput 7 dinput.h DIK_F1 = 0x3b
    kKeyF2 = 0x3c,         // DirectInput 7 dinput.h DIK_F2 = 0x3c
    kKeyF3 = 0x3d,         // DirectInput 7 dinput.h DIK_F3 = 0x3d
    kKeyF4 = 0x3e,         // DirectInput 7 dinput.h DIK_F4 = 0x3e
    kKeyF5 = 0x3f,         // DirectInput 7 dinput.h DIK_F5 = 0x3f
    kKeyF9 = 0x43,         // DirectInput 7 dinput.h DIK_F9 = 0x43
    kKeyF10 = 0x44,        // DirectInput 7 dinput.h DIK_F10 = 0x44
    kKeyNumpad7 = 0x47,    // DirectInput 7 dinput.h DIK_NUMPAD7 = 0x47
    kKeyNumpad8 = 0x48,    // DirectInput 7 dinput.h DIK_NUMPAD8 = 0x48
    kKeySubtract = 0x4a,   // DirectInput 7 dinput.h DIK_SUBTRACT = 0x4a
    kKeyNumpad4 = 0x4b,    // DirectInput 7 dinput.h DIK_NUMPAD4 = 0x4b
    kKeyNumpad6 = 0x4d,    // DirectInput 7 dinput.h DIK_NUMPAD6 = 0x4d
    kKeyAdd = 0x4e,        // DirectInput 7 dinput.h DIK_ADD = 0x4e
    kKeyNumpad1 = 0x4f,    // DirectInput 7 dinput.h DIK_NUMPAD1 = 0x4f
    kKeyNumpad2 = 0x50,    // DirectInput 7 dinput.h DIK_NUMPAD2 = 0x50
    kKeyDecimal = 0x53,    // DirectInput 7 dinput.h DIK_DECIMAL = 0x53
    kKeyF11 = 0x57,        // DirectInput 7 dinput.h DIK_F11 = 0x57
    kKeyNumpadEnter = 0x9c,  // DirectInput 7 dinput.h DIK_NUMPADENTER = 0x9c
    kKeyHome = 0xc7,       // DirectInput 7 dinput.h DIK_HOME = 0xc7
    kKeyUp = 0xc8,         // DirectInput 7 dinput.h DIK_UP = 0xc8
    kKeyPrior = 0xc9,      // DirectInput 7 dinput.h DIK_PRIOR = 0xc9
    kKeyLeft = 0xcb,       // DirectInput 7 dinput.h DIK_LEFT = 0xcb
    kKeyRight = 0xcd,      // DirectInput 7 dinput.h DIK_RIGHT = 0xcd
    kKeyEnd = 0xcf,        // DirectInput 7 dinput.h DIK_END = 0xcf
    kKeyDown = 0xd0,       // DirectInput 7 dinput.h DIK_DOWN = 0xd0
    kKeyInsert = 0xd2,     // DirectInput 7 dinput.h DIK_INSERT = 0xd2
    kKeyDelete = 0xd3,     // DirectInput 7 dinput.h DIK_DELETE = 0xd3
    kKeyControl = 0xe0,    // ISKeyboard ctor: mKeys[0xe0] = ISExtendedKey(DIK_LCONTROL, DIK_RCONTROL); ComputeModifierState bit 2 (provisional name)
    kKeyShift = 0xe1,      // ISKeyboard ctor: mKeys[0xe1] = ISExtendedKey(DIK_LSHIFT, DIK_RSHIFT); ComputeModifierState bit 1; UIForm sShiftKey (provisional name)
    kKeyAlt = 0xe2,        // ISKeyboard ctor: mKeys[0xe2] = ISExtendedKey(DIK_LMENU, DIK_RMENU); ComputeModifierState bit 4 (provisional name)
    kKeyNone = 0x7fff,     // UIForm::AddHotKey sets a modifier only when < 0x7fff; UIHotKey::CreateHotKeys: 0x7fff = no modifier (provisional name)
};

#endif // EE_ISKEYCODE_H
