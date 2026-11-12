// Answers the font requests of a GDI metrics case list (data/cases_windows11.txt) with the native build's GDI, one
// JSON line per request in the format of data/gdi_metrics_windows11.jsonl (a Windows 11 capture): the TEXTMETRICA,
// the widths of the code page 1252 characters 0x20 to 0xff, and the extent of each string.
//
// Usage: dump_gdi_metrics <cases> <output>
#include <windows.h>

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

static std::vector<std::string> Split(const std::string& inText, char inSeparator)
{
    std::vector<std::string> theParts;
    size_t theStart = 0;
    for (;;) {
        size_t theEnd = inText.find(inSeparator, theStart);
        theParts.push_back(inText.substr(theStart, theEnd - theStart));
        if (theEnd == std::string::npos)
            return theParts;
        theStart = theEnd + 1;
    }
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        fprintf(stderr, "usage: dump_gdi_metrics <cases> <output>\n");
        return 2;
    }
    FILE* theCases = fopen(argv[1], "rb");
    FILE* theOutput = fopen(argv[2], "wb");
    if (!theCases || !theOutput) {
        fprintf(stderr, "cannot open %s or %s\n", argv[1], argv[2]);
        return 2;
    }
    std::vector<std::string> theStrings, theFonts;
    char theLine[1024];
    while (fgets(theLine, sizeof theLine, theCases)) {
        std::string theText = theLine;
        while (!theText.empty() && (theText.back() == '\n' || theText.back() == '\r'))
            theText.pop_back();
        if (theText.rfind("string ", 0) == 0)
            theStrings.push_back(theText.substr(7));
        else if (theText.rfind("font ", 0) == 0)
            theFonts.push_back(theText.substr(5));
    }
    fclose(theCases);
    HDC theDC = GetDC(NULL);
    int theRequests = 0;
    for (const std::string& theSpec : theFonts) {
        std::vector<std::string> theFields = Split(theSpec, ';');
        if (theFields.size() != 4)
            continue;
        for (const std::string& theHeight : Split(theFields[1], ','))
            for (const std::string& theWeight : Split(theFields[2], ',')) {
                int theItalic = atoi(theFields[3].c_str());
                HFONT theFont = CreateFontA(atoi(theHeight.c_str()), 0, 0, 0, atoi(theWeight.c_str()), theItalic,
                                            FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                            DEFAULT_QUALITY, DEFAULT_PITCH, theFields[0].c_str());
                HGDIOBJ theOld = SelectObject(theDC, theFont);
                TEXTMETRICA m;
                GetTextMetricsA(theDC, &m);
                fprintf(theOutput,
                        "{\"face\":\"%s\",\"height\":%s,\"weight\":%s,\"italic\":%d,\"tm\":[%ld,%ld,%ld,%ld,%ld,%ld,%ld,"
                        "%ld,%ld,%ld,%ld,%d,%d,%d,%d,%d,%d,%d,%d,%d],\"widths\":[",
                        theFields[0].c_str(), theHeight.c_str(), theWeight.c_str(), theItalic, (long)m.tmHeight,
                        (long)m.tmAscent, (long)m.tmDescent, (long)m.tmInternalLeading, (long)m.tmExternalLeading,
                        (long)m.tmAveCharWidth, (long)m.tmMaxCharWidth, (long)m.tmWeight, (long)m.tmOverhang,
                        (long)m.tmDigitizedAspectX, (long)m.tmDigitizedAspectY, m.tmFirstChar, m.tmLastChar,
                        m.tmDefaultChar, m.tmBreakChar, m.tmItalic, m.tmUnderlined, m.tmStruckOut,
                        m.tmPitchAndFamily, m.tmCharSet);
                for (int theByte = 0x20; theByte <= 0xff; ++theByte) {
                    char theCharacter = (char)theByte;
                    wchar_t theWide = 0;
                    MultiByteToWideChar(CP_ACP, 0, &theCharacter, 1, &theWide, 1);
                    INT theWidth = 0;
                    GetCharWidthW(theDC, (UINT)theWide, (UINT)theWide, &theWidth);
                    fprintf(theOutput, "%s%d", theByte == 0x20 ? "" : ",", theWidth);
                }
                fprintf(theOutput, "],\"extents\":[");
                for (size_t i = 0; i < theStrings.size(); ++i) {
                    SIZE theSize = {};
                    GetTextExtentPoint32A(theDC, theStrings[i].c_str(), (int)theStrings[i].size(), &theSize);
                    fprintf(theOutput, "%s[%ld,%ld]", i ? "," : "", (long)theSize.cx, (long)theSize.cy);
                }
                fprintf(theOutput, "]}\n");
                SelectObject(theDC, theOld);
                DeleteObject(theFont);
                ++theRequests;
            }
    }
    ReleaseDC(NULL, theDC);
    fclose(theOutput);
    printf("%d requests\n", theRequests);
    return 0;
}
