// Dumps a resource DLL through the native build's module calls (src/Port/Linux/Win32Module.cpp): the string tables'
// language and count from the enumeration calls, then every string LoadStringA returns, hex-encoded. Its output is
// compared with ee-toolkit's own PE reader by tools/test_port_linux.py.
#include <windows.h>

#include <cstdio>

static WORD sLanguage;
static int sTables;

// cdecl and returning bool, as the game's own callbacks (UResourceFile.cpp)
static bool OnLanguage(HMODULE, LPCSTR, LPCSTR, WORD inLanguage, LONG_PTR)
{
    sLanguage = inLanguage;
    return false;
}

static bool OnName(HMODULE inModule, LPCSTR inType, LPSTR inName, LONG_PTR inParam)
{
    ++sTables;
    EnumResourceLanguagesA(inModule, inType, inName, (ENUMRESLANGPROCA)OnLanguage, inParam);
    return true;
}

int main(int inCount, char** inArguments)
{
    if (inCount != 2)
        return 2;
    HMODULE theModule = LoadLibraryA(inArguments[1]);
    if (!theModule)
        return 3;
    if (GetProcAddress(theModule, "PIMGetType") != NULL)     // a PE module has no code here
        return 4;
    EnumResourceNamesA(theModule, RT_STRING, (ENUMRESNAMEPROCA)OnName, 0);
    printf("language %u\ntables %d\n", sLanguage, sTables);
    static char theText[8192];
    for (unsigned int theID = 0; theID < 65536; ++theID) {
        int theLength = LoadStringA(theModule, theID, theText, sizeof theText);
        if (theLength <= 0)
            continue;
        printf("string %u ", theID);
        for (int i = 0; i < theLength; ++i)
            printf("%02x", (unsigned char)theText[i]);
        printf("\n");
    }
    FreeLibrary(theModule);
    return 0;
}
