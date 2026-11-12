// Prints what the native build's text calls (src/Port/Linux/Win32System.cpp) return, for tools/test_port_linux.py to
// compare with Python's cp1252 and UTF-8 codecs and with exact arithmetic:
//   byte B U   the code point MultiByteToWideChar gives Windows-1252 byte B
//   back B R   the byte WideCharToMultiByte gives that code point back, and whether it used the default
//   utf8 U...  the UTF-16 units of a UTF-8 sample
//   muldiv A B C R
#include <windows.h>

#include <cstdio>

int main()
{
    for (int i = 1; i < 256; ++i) {
        char theByte = (char)i;
        WCHAR theWide = 0;
        if (MultiByteToWideChar(1252, 0, &theByte, 1, &theWide, 1) != 1)
            return 3;
        printf("byte %d %d\n", i, (int)theWide);
        char theBack = 0;
        BOOL theDefault = TRUE;
        if (WideCharToMultiByte(1252, 0, &theWide, 1, &theBack, 1, NULL, &theDefault) != 1)
            return 4;
        printf("back %d %d %d\n", i, (unsigned char)theBack, (int)theDefault);
    }
    const char kSample[] = "caf\xc3\xa9 \xe2\x82\xac \xf0\x9f\x8f\xb0";    // café € 🏰
    WCHAR theUTF16[32];
    int theCount = MultiByteToWideChar(CP_UTF8, 0, kSample, -1, theUTF16, 32);
    printf("utf8");
    for (int i = 0; i < theCount; ++i)
        printf(" %d", (int)theUTF16[i]);
    printf("\n");
    static const int kCases[][3] = {{10, 96, 72}, {-10, 96, 72}, {7, 3, 2}, {-7, 3, 2}, {7, 3, -2}, {-7, -3, -2},
                                    {5, 1, 2}, {-5, 1, 2}, {3, 1, 2}, {1, 1, 3}, {2, 1, 3}, {0x7fffffff, 2, 2},
                                    {0x7fffffff, 2, 1}, {12, 72, 96}, {123456789, 1000, 999}, {1, 1, 0}};
    for (const int* c : kCases)
        printf("muldiv %d %d %d %d\n", c[0], c[1], c[2], MulDiv(c[0], c[1], c[2]));
    return 0;
}
