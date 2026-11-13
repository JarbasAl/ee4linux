// Scripted input for a headless run: EE_PORT_INPUT_SCRIPT names a file of key and mouse events, which the thread that
// pumps feeds to the game at fixed times. The events travel Translate's path like a real key press or click, so the
// layer and the game handle them as they handle a person's.
//
// One event per line, times in milliseconds from the first pump; '#' starts a comment; a blank line does nothing:
//
//   1000 mouse move 512 384
//   1500 mouse down left 512 384
//   1560 mouse up left 512 384
//   3000 key down F4
//   3060 key up F4
//
// A key name is SDL's name for its scancode ("F4", "Return", "Left Shift", "1"). A button is "left", "middle" or
// "right". A line that cannot be read is logged with its number and skipped, so one typo does not cancel the events
// that follow it.
#include "Win32Internal.h"

#include <SDL.h>

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

namespace {

enum class Kind { Key, MouseMove, MouseButton };

struct ScriptEvent {
    Uint32 mTime = 0;           // ms from the first pump
    Kind mKind = Kind::Key;
    int mWhich = 0;             // the scancode, or SDL's button number
    bool mDown = false;
    int mX = 0, mY = 0;
};

std::vector<ScriptEvent> gEvents;
size_t gNext = 0;
bool gRead = false;
bool gStarted = false;
Uint32 gStart = 0;
int gCursorX = 0, gCursorY = 0;

// The modifier keys the script has pressed in its first inCount events
Uint16 HeldModifiers(size_t inCount)
{
    Uint16 theMod = KMOD_NONE;
    for (size_t i = 0; i < inCount && i < gEvents.size(); ++i) {
        const ScriptEvent& theEvent = gEvents[i];
        if (theEvent.mKind != Kind::Key)
            continue;
        switch ((SDL_Scancode)theEvent.mWhich) {
        case SDL_SCANCODE_LSHIFT: theMod = theEvent.mDown ? (theMod | KMOD_LSHIFT) : (theMod & ~KMOD_LSHIFT); break;
        case SDL_SCANCODE_RSHIFT: theMod = theEvent.mDown ? (theMod | KMOD_RSHIFT) : (theMod & ~KMOD_RSHIFT); break;
        case SDL_SCANCODE_LCTRL: theMod = theEvent.mDown ? (theMod | KMOD_LCTRL) : (theMod & ~KMOD_LCTRL); break;
        case SDL_SCANCODE_RCTRL: theMod = theEvent.mDown ? (theMod | KMOD_RCTRL) : (theMod & ~KMOD_RCTRL); break;
        case SDL_SCANCODE_LALT: theMod = theEvent.mDown ? (theMod | KMOD_LALT) : (theMod & ~KMOD_LALT); break;
        case SDL_SCANCODE_RALT: theMod = theEvent.mDown ? (theMod | KMOD_RALT) : (theMod & ~KMOD_RALT); break;
        default: break;
        }
    }
    return theMod;
}

bool Read(const char* inPath)
{
    FILE* theFile = fopen(inPath, "r");
    if (!theFile) {
        eeport::Log("input script \"%s\" is not readable", inPath);
        return false;
    }
    char theLine[1024];
    int theNumber = 0;
    int theSkipped = 0;
    while (fgets(theLine, sizeof theLine, theFile)) {
        ++theNumber;
        char* theComment = strchr(theLine, '#');
        if (theComment)
            *theComment = '\0';
        unsigned theTime = 0;
        char theKind[32];
        int theUsed = 0;
        if (sscanf(theLine, "%u %31s%n", &theTime, theKind, &theUsed) != 2)
            continue;   // blank or a comment alone
        ScriptEvent theEvent;
        theEvent.mTime = theTime;
        const char* theRest = theLine + theUsed;
        if (strcmp(theKind, "wait") == 0)
            continue;
        if (strcmp(theKind, "key") == 0) {
            char theState[32];
            int theUsedState = 0;
            if (sscanf(theRest, "%31s%n", theState, &theUsedState) != 1 ||
                (strcmp(theState, "down") != 0 && strcmp(theState, "up") != 0)) {
                eeport::Log("input script line %d: expected \"key down|up <name>\"; skipped", theNumber);
                ++theSkipped;
                continue;
            }
            // A key's name is the rest of the line: SDL's names have spaces in them ("Left Shift", "Keypad 4"),
            // and reading a single word would turn "Left Shift" into "Left", the arrow key
            std::string theName(theRest + theUsedState);
            while (!theName.empty() && isspace((unsigned char)theName.back()))
                theName.pop_back();
            size_t theFirst = theName.find_first_not_of(" \t");
            if (theFirst == std::string::npos) {
                eeport::Log("input script line %d: no key name; skipped", theNumber);
                ++theSkipped;
                continue;
            }
            theName.erase(0, theFirst);
            SDL_Scancode theScancode = SDL_GetScancodeFromName(theName.c_str());
            if (theScancode == SDL_SCANCODE_UNKNOWN) {
                eeport::Log("input script line %d: no key named \"%s\"; skipped", theNumber, theName.c_str());
                ++theSkipped;
                continue;
            }
            theEvent.mKind = Kind::Key;
            theEvent.mWhich = (int)theScancode;
            theEvent.mDown = strcmp(theState, "down") == 0;
        } else if (strcmp(theKind, "mouse") == 0) {
            char theAction[32];
            if (sscanf(theRest, "%31s", theAction) != 1) {
                eeport::Log("input script line %d: expected \"mouse move|down|up ...\"; skipped", theNumber);
                ++theSkipped;
                continue;
            }
            if (strcmp(theAction, "move") == 0) {
                if (sscanf(theRest, "%*s %d %d", &theEvent.mX, &theEvent.mY) != 2) {
                    eeport::Log("input script line %d: expected \"mouse move <x> <y>\"; skipped", theNumber);
                    ++theSkipped;
                    continue;
                }
                theEvent.mKind = Kind::MouseMove;
            } else if (strcmp(theAction, "down") == 0 || strcmp(theAction, "up") == 0) {
                char theButton[32];
                if (sscanf(theRest, "%*s %31s %d %d", theButton, &theEvent.mX, &theEvent.mY) != 3) {
                    eeport::Log("input script line %d: expected \"mouse down|up <button> <x> <y>\"; skipped",
                                theNumber);
                    ++theSkipped;
                    continue;
                }
                theEvent.mKind = Kind::MouseButton;
                theEvent.mDown = strcmp(theAction, "down") == 0;
                if (strcmp(theButton, "left") == 0)
                    theEvent.mWhich = SDL_BUTTON_LEFT;
                else if (strcmp(theButton, "middle") == 0)
                    theEvent.mWhich = SDL_BUTTON_MIDDLE;
                else if (strcmp(theButton, "right") == 0)
                    theEvent.mWhich = SDL_BUTTON_RIGHT;
                else {
                    eeport::Log("input script line %d: no button named \"%s\"; skipped", theNumber, theButton);
                    ++theSkipped;
                    continue;
                }
            } else {
                eeport::Log("input script line %d: expected \"mouse move|down|up ...\"; skipped", theNumber);
                ++theSkipped;
                continue;
            }
        } else {
            eeport::Log("input script line %d: no command \"%s\"; skipped", theNumber, theKind);
            ++theSkipped;
            continue;
        }
        gEvents.push_back(theEvent);
    }
    fclose(theFile);
    // times are a script's own order: sort stably so an out-of-order line still runs
    std::stable_sort(gEvents.begin(), gEvents.end(),
                     [](const ScriptEvent& a, const ScriptEvent& b) { return a.mTime < b.mTime; });
    eeport::Log("input script: %zu events from %s%s", gEvents.size(), inPath,
                theSkipped ? " (some lines were skipped)" : "");
    return true;
}

// Hands one due event to SDL's queue, in the shape a window's own events have
void Send(const ScriptEvent& inEvent, Uint16 inMod)
{
    Uint32 theWindow = eeport::FocusNativeID();
    SDL_Event theSDL;
    SDL_zero(theSDL);
    switch (inEvent.mKind) {
    case Kind::Key:
        theSDL.type = inEvent.mDown ? SDL_KEYDOWN : SDL_KEYUP;
        theSDL.key.type = theSDL.type;
        theSDL.key.timestamp = SDL_GetTicks();
        theSDL.key.windowID = theWindow;
        theSDL.key.state = inEvent.mDown ? SDL_PRESSED : SDL_RELEASED;
        theSDL.key.repeat = 0;
        theSDL.key.keysym.scancode = (SDL_Scancode)inEvent.mWhich;
        theSDL.key.keysym.sym = SDL_GetKeyFromScancode((SDL_Scancode)inEvent.mWhich);
        theSDL.key.keysym.mod = inMod;
        break;
    case Kind::MouseMove:
        theSDL.type = SDL_MOUSEMOTION;
        theSDL.motion.type = theSDL.type;
        theSDL.motion.timestamp = SDL_GetTicks();
        theSDL.motion.windowID = theWindow;
        theSDL.motion.x = inEvent.mX;
        theSDL.motion.y = inEvent.mY;
        theSDL.motion.xrel = inEvent.mX - gCursorX;
        theSDL.motion.yrel = inEvent.mY - gCursorY;
        theSDL.motion.state = 0;
        break;
    case Kind::MouseButton:
        theSDL.type = inEvent.mDown ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP;
        theSDL.button.type = theSDL.type;
        theSDL.button.timestamp = SDL_GetTicks();
        theSDL.button.windowID = theWindow;
        theSDL.button.which = 0;    // the mouse that Windows reports
        theSDL.button.button = (Uint8)inEvent.mWhich;
        theSDL.button.state = inEvent.mDown ? SDL_PRESSED : SDL_RELEASED;
        theSDL.button.clicks = 1;
        theSDL.button.x = inEvent.mX;
        theSDL.button.y = inEvent.mY;
        break;
    }
    if (inEvent.mKind != Kind::Key) {
        gCursorX = inEvent.mX;
        gCursorY = inEvent.mY;
    }
    if (!SDL_PushEvent(&theSDL))
        eeport::Log("input script: SDL does not take the event at %u ms: %s", inEvent.mTime, SDL_GetError());
}

} // namespace

namespace eeport {

void FeedInputScript()
{
    if (!gRead) {
        gRead = true;
        const char* thePath = getenv("EE_PORT_INPUT_SCRIPT");
        if (thePath && *thePath)
            Read(thePath);
    }
    if (gEvents.empty())
        return;
    if (!gStarted) {
        gStarted = true;
        gStart = SDL_GetTicks();
    }
    Uint32 theNow = SDL_GetTicks() - gStart;
    while (gNext < gEvents.size() && gEvents[gNext].mTime <= theNow) {
        const ScriptEvent& theEvent = gEvents[gNext];
        Send(theEvent, HeldModifiers(gNext));
        ++gNext;
        // A pushed event does not move SDL's own modifier state, which the layer's GetKeyState answers from, so the
        // state the next event sees is set here
        SDL_SetModState((SDL_Keymod)HeldModifiers(gNext));
    }
}

} // namespace eeport
