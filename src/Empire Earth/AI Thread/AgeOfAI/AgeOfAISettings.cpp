// Age of AI's settings reader (AgeOfAISettings.h).
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI),
// dll/settings/settings.c and dll/base/ini.c at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0. Upstream works out
// a value with its script language's constant evaluator (dll/script/expr.c x_const_eval); EvaluateConstant covers the
// part of that language that ini.c documents for the settings.
#include <windows.h>

#include <cctype>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

#include "AgeOfAISettings.h"

namespace AgeOfAI {

Settings gSettings;
SettingsAI gSettingsAI[3];

namespace {

enum Kind { kFlag, kInt, kNum, kShare, kChoice, kStr, kStrP, kFactor, kFlags, kLimit };

// kChoice: choices holds the names, ',' between them. kFlags: choices holds the fields, each name or name=a/b/c and a
// '|' after it, and a their count. kStr, kStrP: a is the buffer's size
struct Row {
    const char* mSection;
    const char* mKey;
    Kind mKind;
    bool mPerDifficulty;   // a row of [ai_base]: a field of SettingsAI
    size_t mOffset;
    int mDefault;
    const char* mDefaultText;
    const char* mChoices;
    int a, b;
};

#define AOAI_ROW_NUMBER(kind, sec, key, st, ai, def, a, b) { #sec, #key, kind, ai, offsetof(st, key), def, nullptr, nullptr, a, b },
#define AOAI_ROW_FLAG(sec, key, st, ai, def, a, b) AOAI_ROW_NUMBER(kFlag, sec, key, st, ai, def, a, b)
#define AOAI_ROW_INT(sec, key, st, ai, def, a, b) AOAI_ROW_NUMBER(kInt, sec, key, st, ai, def, a, b)
#define AOAI_ROW_LIMIT(sec, key, st, ai, def, a, b) AOAI_ROW_NUMBER(kLimit, sec, key, st, ai, def, a, b)
#define AOAI_ROW_NUM(sec, key, st, ai, def, a, b) AOAI_ROW_NUMBER(kNum, sec, key, st, ai, def, a, b)
#define AOAI_ROW_SHARE(sec, key, st, ai, def, a, b) AOAI_ROW_NUMBER(kShare, sec, key, st, ai, def, a, b)
#define AOAI_ROW_FACTOR(sec, key, st, ai, def, a, b) AOAI_ROW_NUMBER(kFactor, sec, key, st, ai, def, a, b)
#define AOAI_ROW_CHOICE(sec, key, st, ai, def, a, b) { #sec, #key, kChoice, ai, offsetof(st, key), 0, def, a, 0, b },
#define AOAI_ROW_STR(sec, key, st, ai, def, a, b) { #sec, #key, kStr, ai, offsetof(st, key), 0, def, nullptr, a, b },
#define AOAI_ROW_STR_P(sec, key, st, ai, def, a, b) { #sec, #key, kStrP, ai, offsetof(st, key), 0, def, nullptr, a, b },
#define AOAI_FIELD_TEXT_BOOL(f) #f "|"
#define AOAI_FIELD_TEXT_ENUM(f, names) #f "=" names "|"
#define AOAI_FIELD_ONE_BOOL(f) +1
#define AOAI_FIELD_ONE_ENUM(f, names) +1
#define AOAI_ROW_FLAGS(sec, key, st, ai, def, a, b) { #sec, #key, kFlags, ai, offsetof(st, key), 0, def, \
    a(AOAI_FIELD_TEXT_BOOL, AOAI_FIELD_TEXT_ENUM), 0 a(AOAI_FIELD_ONE_BOOL, AOAI_FIELD_ONE_ENUM), b },
#define AOAI_ROW_X(kind, sec, key, def, a, b) AOAI_ROW_##kind(sec, key, Settings, false, def, a, b)
#define AOAI_ROW_D(kind, key, def, a, b) AOAI_ROW_##kind(ai_base, key, SettingsAI, true, def, a, b)
const Row kRows[] = { SETTINGS(AOAI_ROW_X, AOAI_ROW_D) };

#define AOAI_ID_X(kind, sec, key, def, a, b) kRow_##key,
#define AOAI_ID_D(kind, key, def, a, b) kRow_##key,
enum { SETTINGS(AOAI_ID_X, AOAI_ID_D) kRowEnd };
static_assert(sizeof kRows / sizeof *kRows == kRowEnd, "a row id per row");

#define AOAI_MEMBER(key, master) { kRow_##key, kRow_##master },
const struct { int mRow, mMaster; } kMembers[] = { SETTING_MEMBERS(AOAI_MEMBER) };

#define AOAI_EASY(key, text) { #key, text },
const struct { const char *mKey, *mText; } kEasy[] = { SETTINGS_EASY(AOAI_EASY) };

enum SectionKind { kSectionPlain, kSectionAll, kSectionDifficulty, kSectionTest };
#define AOAI_SECTION(sec, order, kind, title) { #sec, kind },
#define PLAIN kSectionPlain
#define ALL kSectionAll
#define DIFF kSectionDifficulty
#define TEST kSectionTest
const struct { const char* mName; SectionKind mKind; } kSections[] = { SETTING_SECTIONS(AOAI_SECTION) };
#undef PLAIN
#undef ALL
#undef DIFF
#undef TEST

const char* const kDifficultySection[3] = { "ai_easy", nullptr, "ai_hard" };   // the medium AI plays [ai_base]

// a [master] row's words, in their order in its CHOICE
enum { kMasterDefault, kMasterFile, kMasterOr, kMasterAnd };

bool SameName(const char* inA, size_t inLengthA, const char* inB, size_t inLengthB)
{
    if (inLengthA != inLengthB)
        return false;
    for (size_t i = 0; i < inLengthA; i++)
        if (tolower((unsigned char)inA[i]) != tolower((unsigned char)inB[i]))
            return false;
    return true;
}

bool SameName(const std::string& inA, const char* inB)
{
    return SameName(inA.data(), inA.size(), inB, strlen(inB));
}

std::string Trimmed(const std::string& inText)
{
    size_t theStart = inText.find_first_not_of(" \t\r");
    size_t theEnd = inText.find_last_not_of(" \t\r");
    return theStart == std::string::npos ? std::string() : inText.substr(theStart, theEnd - theStart + 1);
}

// The file as Windows' profile functions read it: [section] lines and key=value lines, names without case, the first
// of a repeated section or key the one that counts, a value's surrounding quotes taken off
struct IniFile {
    struct Section {
        std::string mName;
        std::vector<std::pair<std::string, std::string>> mKeys;
    };
    std::vector<Section> mSections;

    bool Read(const char* inPath)
    {
        FILE* theFile = fopen(inPath, "rb");   // the port maps a Windows path (src/Port/Linux/Win32Path.cpp)
        if (!theFile)
            return false;
        std::string theText;
        char theBuffer[4096];
        for (size_t n; (n = fread(theBuffer, 1, sizeof theBuffer, theFile)) > 0;)
            theText.append(theBuffer, n);
        fclose(theFile);
        Section* theSection = nullptr;
        for (size_t theStart = 0; theStart < theText.size();) {
            size_t theEnd = theText.find('\n', theStart);
            if (theEnd == std::string::npos)
                theEnd = theText.size();
            std::string theLine = Trimmed(theText.substr(theStart, theEnd - theStart));
            theStart = theEnd + 1;
            if (theLine.empty() || theLine[0] == ';')
                continue;
            if (theLine[0] == '[') {
                std::string theName = Trimmed(theLine.substr(1, theLine.find(']') - 1));
                theSection = this->Find(theName.c_str());
                if (!theSection) {
                    this->mSections.push_back(Section{ theName, {} });
                    theSection = &this->mSections.back();
                }
                continue;
            }
            size_t theEquals = theLine.find('=');
            if (!theSection || theEquals == std::string::npos)
                continue;
            std::string theValue = Trimmed(theLine.substr(theEquals + 1));
            if (theValue.size() >= 2 && (theValue[0] == '"' || theValue[0] == '\'') && theValue.back() == theValue[0])
                theValue = theValue.substr(1, theValue.size() - 2);
            theSection->mKeys.emplace_back(Trimmed(theLine.substr(0, theEquals)), theValue);
        }
        return true;
    }

    Section* Find(const char* inName)
    {
        for (Section& theSection : this->mSections)
            if (SameName(theSection.mName, inName))
                return &theSection;
        return nullptr;
    }

    // a key's text without its inline comment (; to the line's end) and trailing blanks; false: not there or empty
    bool Raw(const char* inSection, const char* inKey, std::string& outText)
    {
        Section* theSection = this->Find(inSection);
        if (!theSection)
            return false;
        for (const auto& theKey : theSection->mKeys) {
            if (!SameName(theKey.first, inKey))
                continue;
            outText = theKey.second.substr(0, theKey.second.find(';'));
            size_t theEnd = outText.find_last_not_of(" \t");
            outText.resize(theEnd == std::string::npos ? 0 : theEnd + 1);
            return !outText.empty();
        }
        return false;
    }
};

// ---- the constant expressions ----

class Evaluator {
public:
    explicit Evaluator(const char* inText) : mAt(inText) {}

    bool Run(double& outValue, bool& outIsBool, std::string& outWhy)
    {
        Value theValue;
        if (!this->Comparison(theValue))
            return this->Failed(outWhy);
        this->Blanks();
        if (*this->mAt) {
            this->mWhy = this->mWhy.empty() ? "not a number or an expression" : this->mWhy;
            return this->Failed(outWhy);
        }
        outValue = theValue.mNumber;
        outIsBool = theValue.mIsBool;
        return true;
    }

private:
    struct Value {
        double mNumber = 0;
        bool mIsBool = false;
    };

    const char* mAt;
    std::string mWhy;

    bool Failed(std::string& outWhy)
    {
        outWhy = this->mWhy.empty() ? "not a number or an expression" : this->mWhy;
        return false;
    }
    bool Fail(const char* inWhy)
    {
        if (this->mWhy.empty())
            this->mWhy = inWhy;
        return false;
    }
    void Blanks()
    {
        while (*this->mAt == ' ' || *this->mAt == '\t')
            this->mAt++;
    }
    bool Take(const char* inToken)
    {
        this->Blanks();
        size_t theLength = strlen(inToken);
        if (strncmp(this->mAt, inToken, theLength))
            return false;
        this->mAt += theLength;
        return true;
    }
    bool Number(const Value& inValue)
    {
        return !inValue.mIsBool || this->Fail("a number expected, not a bool");
    }

    bool Comparison(Value& outValue)
    {
        if (!this->Sum(outValue))
            return false;
        static const char* const kOperators[] = { "<=", ">=", "==", "!=", "<", ">" };
        for (int i = 0; i < 6; i++) {
            if (!this->Take(kOperators[i]))
                continue;
            Value theRight;
            if (!this->Number(outValue) || !this->Sum(theRight) || !this->Number(theRight))
                return false;
            double l = outValue.mNumber, r = theRight.mNumber;
            bool theResult = i == 0 ? l <= r : i == 1 ? l >= r : i == 2 ? l == r : i == 3 ? l != r : i == 4 ? l < r : l > r;
            outValue.mNumber = theResult;
            outValue.mIsBool = true;
            return true;
        }
        return true;
    }

    bool Sum(Value& outValue)
    {
        if (!this->Product(outValue))
            return false;
        for (;;) {
            bool thePlus = this->Take("+");
            if (!thePlus && !this->Take("-"))
                return true;
            Value theRight;
            if (!this->Number(outValue) || !this->Product(theRight) || !this->Number(theRight))
                return false;
            outValue.mNumber += thePlus ? theRight.mNumber : -theRight.mNumber;
        }
    }

    bool Product(Value& outValue)
    {
        if (!this->Unary(outValue))
            return false;
        for (;;) {
            bool theTimes = this->Take("*");
            if (!theTimes && !this->Take("/"))
                return true;
            Value theRight;
            if (!this->Number(outValue) || !this->Unary(theRight) || !this->Number(theRight))
                return false;
            if (!theTimes && theRight.mNumber == 0)
                return this->Fail("a division by zero");
            outValue.mNumber = theTimes ? outValue.mNumber * theRight.mNumber : outValue.mNumber / theRight.mNumber;
        }
    }

    bool Unary(Value& outValue)
    {
        if (this->Take("-")) {
            if (!this->Unary(outValue) || !this->Number(outValue))
                return false;
            outValue.mNumber = -outValue.mNumber;
            return true;
        }
        if (this->Take("+"))
            return this->Unary(outValue) && this->Number(outValue);
        if (!this->Primary(outValue))
            return false;
        if (this->Take("%")) {
            if (!this->Number(outValue))
                return false;
            outValue.mNumber /= 100;
        }
        return true;
    }

    bool Primary(Value& outValue)
    {
        this->Blanks();
        if (this->Take("(")) {
            if (!this->Comparison(outValue))
                return false;
            return this->Take(")") || this->Fail("a ) expected");
        }
        if (isdigit((unsigned char)*this->mAt) || (*this->mAt == '.' && isdigit((unsigned char)this->mAt[1]))) {
            char* theEnd = nullptr;
            outValue.mNumber = strtod(this->mAt, &theEnd);
            this->mAt = theEnd;
            return true;
        }
        const char* theStart = this->mAt;
        while (isalpha((unsigned char)*this->mAt) || *this->mAt == '_')
            this->mAt++;
        std::string theName(theStart, this->mAt);
        if (theName.empty())
            return this->Fail("not a number or an expression");
        if (theName == "true" || theName == "false") {
            outValue.mNumber = theName == "true";
            outValue.mIsBool = true;
            return true;
        }
        bool theMinMax = theName == "min" || theName == "max";
        if (!theMinMax && theName != "abs" && theName != "round" && theName != "floor")
            return this->Fail("no names in a setting: numbers, %, + - * / ( ), min, max, abs, round, floor");
        if (!this->Take("("))
            return this->Fail("a ( expected");
        std::vector<double> theArguments;
        do {
            Value theArgument;
            if (!this->Comparison(theArgument) || !this->Number(theArgument))
                return false;
            theArguments.push_back(theArgument.mNumber);
        } while (this->Take(","));
        if (!this->Take(")"))
            return this->Fail("a ) expected");
        if (theMinMax ? theArguments.size() < 2 : theArguments.size() != 1)
            return this->Fail(theMinMax ? "min and max take two numbers or more" : "abs, round and floor take one number");
        double& x = outValue.mNumber;
        x = theArguments[0];
        if (theName == "abs")
            x = fabs(x);
        else if (theName == "round")
            x = round(x);
        else if (theName == "floor")
            x = floor(x);
        for (size_t i = 1; i < theArguments.size(); i++)
            x = theName == "min" ? (theArguments[i] < x ? theArguments[i] : x) : (theArguments[i] > x ? theArguments[i] : x);
        return true;
    }
};

// ---- the rows ----

class Reader {
public:
    IniFile mFile;
    std::vector<std::string> mMessages;

    void Mistake(const char* inSection, const char* inKey, const std::string& inValue, const std::string& inWhy)
    {
        this->Tell(std::string("ageofai.ini [") + inSection + "] " + inKey + "=" + inValue.substr(0, 40) + ": " + inWhy
                   + " - the default instead");
    }
    void Warning(const char* inSection, const char* inKey, const std::string& inValue, const std::string& inWhy)
    {
        this->Tell(std::string("ageofai.ini [") + inSection + "] " + inKey + "=" + inValue.substr(0, 40) + ": " + inWhy
                   + " - used anyway");
    }
    void Tell(const std::string& inLine)
    {
        for (const std::string& theLine : this->mMessages)
            if (theLine == inLine)   // a row of [ai_base] is read once per difficulty
                return;
        this->mMessages.push_back(inLine);
    }

    // a number; a bool only where inBoolAllowed (a 0 / 1 key)
    bool Evaluate(const char* inSection, const char* inKey, const std::string& inValue, double& outValue, bool inBoolAllowed)
    {
        bool theIsBool = false;
        std::string theWhy;
        if (!EvaluateConstant(inValue.c_str(), outValue, theIsBool, theWhy)) {
            this->Mistake(inSection, inKey, inValue, theWhy);
            return false;
        }
        if (theIsBool && !inBoolAllowed) {
            this->Mistake(inSection, inKey, inValue, "a number expected (true and false only for a 0 / 1 key)");
            return false;
        }
        return true;
    }

    static int Choice(const char* inChoices, const char* inValue, size_t inLength)
    {
        int i = 0;
        for (const char* c = inChoices;; i++) {
            const char* e = strchr(c, ',');
            size_t n = e ? (size_t)(e - c) : strlen(c);
            if (SameName(c, n, inValue, inLength))
                return i;
            if (!e)
                return -1;
            c = e + 1;
        }
    }

    // a FLAGS row's field k: its name, its values (a/b/c, empty for a bool); false: no field k
    static bool Field(const Row& inRow, int k, std::string& outName, std::string& outValues)
    {
        const char* c = inRow.mChoices;
        for (; k > 0 && c && *c; k--) {
            c = strchr(c, '|');
            if (c)
                c++;
        }
        if (!c || !*c)
            return false;
        std::string theField(c, strcspn(c, "|"));
        size_t theEquals = theField.find('=');
        outName = theField.substr(0, theEquals);
        outValues = theEquals == std::string::npos ? std::string() : theField.substr(theEquals + 1);
        return true;
    }

    // inText as a FLAGS row's fields into outFields (every one 0 first): words apart by commas, '|' or blanks,
    // name=value without blanks; empty, else what is wrong
    static std::string Flags(const Row& inRow, const char* inText, int* outFields)
    {
        std::string theName, theValues;
        for (int k = 0; k < inRow.a; k++)
            outFields[k] = 0;
        for (const char* p = inText; *p;) {
            while (*p == ',' || *p == '|' || *p == ' ' || *p == '\t')
                p++;
            const char* e = p + strcspn(p, ",| \t");
            if (e == p)
                continue;
            const char* theEquals = (const char*)memchr(p, '=', (size_t)(e - p));
            size_t n = (size_t)((theEquals ? theEquals : e) - p);
            const char* theValue = theEquals ? theEquals + 1 : nullptr;
            size_t theValueLength = theValue ? (size_t)(e - theValue) : 0;
            if (!theValue && (SameName(p, n, "all", 3) || SameName(p, n, "none", 4))) {
                for (int k = 0; Field(inRow, k, theName, theValues); k++)
                    if (theValues.empty())
                        outFields[k] = n == 3;
                p = e;
                continue;
            }
            int k = 0;
            while (Field(inRow, k, theName, theValues) && !SameName(theName.data(), theName.size(), p, n))
                k++;
            if (k >= inRow.a) {
                std::string theWhy = std::string(p, n) + ": no field - all, none or";
                for (int j = 0; Field(inRow, j, theName, theValues); j++)
                    theWhy += " " + theName;
                return theWhy;
            }
            if (theValues.empty()) {
                if (theValue && !(theValueLength == 1 && (*theValue == '0' || *theValue == '1')))
                    return theName + ": 0 or 1 expected";
                outFields[k] = !theValue || *theValue == '1';
            } else {
                std::string theList = theValues;   // an enum: name=one of a/b/c
                for (char& c : theList)
                    if (c == '/')
                        c = ',';
                int i = theValue ? Choice(theList.c_str(), theValue, theValueLength) : -1;
                if (i < 0)
                    return theName + "=" + theValues + " expected";
                outFields[k] = i;
            }
            p = e;
        }
        return std::string();
    }

    static void Default(const Row& inRow, unsigned char* outAt)
    {
        switch (inRow.mKind) {
        case kStr:
        case kStrP:
            snprintf((char*)outAt, (size_t)inRow.a, "%s", inRow.mDefaultText);
            break;
        case kFlags:
            Flags(inRow, inRow.mDefaultText, (int*)outAt);
            break;
        case kChoice:
            *(int*)outAt = Choice(inRow.mChoices, inRow.mDefaultText, strlen(inRow.mDefaultText));
            break;
        case kShare:
        case kFactor:
            *(float*)outAt = (float)(inRow.mDefault / 100.0);
            break;
        default:
            *(int*)outAt = inRow.mDefault;
        }
    }

    // inValue as the row's value into outAt (inSection, inKey: where it was read). A mistake (what cannot be used at
    // all) keeps the default; a value outside an INT, SHARE or FACTOR row's range is a warning and is used
    void Value(const Row& inRow, const char* inSection, const char* inKey, const std::string& inValue, unsigned char* outAt)
    {
        Default(inRow, outAt);
        if (inRow.mKind == kStr || inRow.mKind == kStrP) {
            if ((int)inValue.size() >= inRow.a)
                this->Mistake(inSection, inKey, inValue, "at most " + std::to_string(inRow.a - 1) + " characters");
            else
                snprintf((char*)outAt, (size_t)inRow.a, "%s", inValue.c_str());
            return;
        }
        if (inRow.mKind == kFlags) {
            std::string theWhy = Flags(inRow, inValue.c_str(), (int*)outAt);
            if (!theWhy.empty()) {
                this->Mistake(inSection, inKey, inValue, theWhy);
                Default(inRow, outAt);
            }
            return;
        }
        if (inRow.mKind == kChoice) {
            int i = Choice(inRow.mChoices, inValue.c_str(), inValue.size());
            if (i < 0) {
                std::string theNames;
                for (const char* c = inRow.mChoices; *c; c++)
                    theNames += *c == ',' ? std::string(", ") : std::string(1, *c);
                this->Mistake(inSection, inKey, inValue, "one of " + theNames + " expected");
                return;
            }
            *(int*)outAt = i;
            return;
        }
        double x;
        if (inRow.mKind == kShare || inRow.mKind == kFactor) {
            if (!this->Evaluate(inSection, inKey, inValue, x, false))
                return;
            if (!(x >= inRow.a / 100.0 && x <= inRow.b / 100.0)) {
                char theWhy[96];
                snprintf(theWhy, sizeof theWhy, "a share %g%% to %g%% expected", (double)inRow.a, (double)inRow.b);
                this->Warning(inSection, inKey, inValue, theWhy);
            }
            *(float*)outAt = (float)x;
            return;
        }
        bool theFlag = inRow.mKind == kFlag;
        if (!this->Evaluate(inSection, inKey, inValue, x, theFlag))
            return;
        if (x != (double)(long long)x || x < -2147483647.0 || x > 2147483647.0) {
            this->Mistake(inSection, inKey, inValue, "a whole number expected (a share of a whole number: round(...))");
            return;
        }
        int n = (int)x;
        int theLow = theFlag ? 0 : inRow.a, theHigh = theFlag ? 1 : inRow.b;
        if (inRow.mKind != kNum && (n < theLow || n > theHigh)) {
            std::string theWhy = theFlag ? std::string("0 or 1 expected")
                                         : std::to_string(theLow) + " to " + std::to_string(theHigh) + " expected";
            if (theFlag || inRow.mKind == kLimit) {
                this->Mistake(inSection, inKey, inValue, theWhy);
                return;
            }
            this->Warning(inSection, inKey, inValue, theWhy);
        }
        *(int*)outAt = n;
    }

    // one key of the row (difficulty inDifficulty) into outAt; true: read from the file
    bool Read(const Row& inRow, int inDifficulty, const char* inKey, unsigned char* outAt)
    {
        std::string theText;
        Default(inRow, outAt);
        const char* theSection = inRow.mSection;
        bool theFound = false;
        if (inRow.mPerDifficulty && inDifficulty != kDifficultyMedium)
            theFound = this->mFile.Raw(theSection = kDifficultySection[inDifficulty], inKey, theText);
        if (!theFound)
            theFound = this->mFile.Raw(theSection = inRow.mSection, inKey, theText);
        if (theFound) {
            this->Value(inRow, theSection, inKey, theText, outAt);
            return true;
        }
        for (const auto& theEasy : kEasy)
            if (inRow.mPerDifficulty && inDifficulty == kDifficultyEasy && !strcmp(theEasy.mKey, inKey))
                this->Value(inRow, "ai_easy", inKey, theEasy.mText, outAt);
        return false;
    }

    // a member of a [master] group, as its master's word says (read, in the file: the member was in the file)
    void Member(const Row& inRow, int inMasterWord, bool inRead, unsigned char* inBase, const char* inMasterKey)
    {
        unsigned char* theAt = inBase + inRow.mOffset;
        if (inMasterWord == kMasterFile)
            return;
        if (inMasterWord == kMasterDefault) {
            if (inRead)
                this->Tell(std::string(inRow.mKey) + " is set, but " + inMasterKey + "=default: ignored");
            Default(inRow, theAt);
            return;
        }
        if (inRow.mKind != kFlag)
            return;
        int theRead = *(int*)theAt;
        Default(inRow, theAt);
        *(int*)theAt = inMasterWord == kMasterOr ? theRead || *(int*)theAt : theRead && *(int*)theAt;
    }

    bool Known(const IniFile::Section& inSection, const std::string& inKey)
    {
        bool theDifficulty = SameName(inSection.mName, kDifficultySection[kDifficultyEasy])
                             || SameName(inSection.mName, kDifficultySection[kDifficultyHard]);
        for (const Row& theRow : kRows) {
            if (!SameName(inSection.mName, theRow.mSection) && !(theDifficulty && theRow.mPerDifficulty))
                continue;
            if (theRow.mKind != kStrP) {
                if (SameName(inKey, theRow.mKey))
                    return true;
                continue;
            }
            size_t theLength = strlen(theRow.mKey);   // key0 .. key15
            if (inKey.size() <= theLength || !SameName(inKey.data(), theLength, theRow.mKey, theLength))
                continue;
            std::string theNumber = inKey.substr(theLength);
            if (theNumber.find_first_not_of("0123456789") == std::string::npos && theNumber.size() <= 2
                && std::stoi(theNumber) < 16)
                return true;
        }
        return false;
    }

    void Unknown()
    {
        for (const IniFile::Section& theSection : this->mFile.mSections) {
            const char* theName = nullptr;   // the section's own name
            for (const auto& theListed : kSections)
                if (theListed.mKind != kSectionTest && SameName(theSection.mName, theListed.mName))
                    theName = theListed.mName;
            if (!theName) {
                this->Tell("ageofai.ini [" + theSection.mName.substr(0, 60)
                           + "]: no such section (a misspelt or an old one?) - ignored");
                continue;
            }
            for (const auto& theKey : theSection.mKeys)
                if (!this->Known(theSection, theKey.first))
                    this->Tell(std::string("ageofai.ini [") + theName + "] " + theKey.first.substr(0, 60)
                               + ": no such setting (a misspelt key?) - ignored");
        }
    }
};

}

bool EvaluateConstant(const char* inText, double& outValue, bool& outIsBool, std::string& outWhy)
{
    return Evaluator(inText).Run(outValue, outIsBool, outWhy);
}

std::vector<std::string> ReadSettings(const char* inPath)
{
    Reader theReader;
    theReader.mFile.Read(inPath);
    bool theRead[kRowEnd][3] = {};   // a row was in the file, per difficulty
    for (int i = 0; i < kRowEnd; i++) {
        const Row& theRow = kRows[i];
        if (theRow.mPerDifficulty) {
            for (int d = 0; d < 3; d++)
                theRead[i][d] = theReader.Read(theRow, d, theRow.mKey, (unsigned char*)&gSettingsAI[d] + theRow.mOffset);
            continue;
        }
        unsigned char* theAt = (unsigned char*)&gSettings + theRow.mOffset;
        if (theRow.mKind != kStrP) {
            theRead[i][0] = theReader.Read(theRow, 0, theRow.mKey, theAt);
            continue;
        }
        theRead[i][0] = false;
        for (int p = 0; p < 16; p++) {
            std::string theKey = theRow.mKey + std::to_string(p);
            theRead[i][0] = theReader.Read(theRow, 0, theKey.c_str(), theAt + p * theRow.a) || theRead[i][0];
        }
    }
    for (const auto& theMember : kMembers) {   // every master is read: its members now
        const Row& theRow = kRows[theMember.mRow];
        const Row& theMaster = kRows[theMember.mMaster];
        if (!theRow.mPerDifficulty) {
            unsigned char* theBase = (unsigned char*)&gSettings;
            theReader.Member(theRow, *(int*)(theBase + theMaster.mOffset), theRead[theMember.mRow][0], theBase, theMaster.mKey);
            continue;
        }
        for (int d = 0; d < 3; d++) {
            unsigned char* theBase = (unsigned char*)&gSettingsAI[d];
            theReader.Member(theRow, *(int*)(theBase + theMaster.mOffset), theRead[theMember.mRow][d], theBase, theMaster.mKey);
        }
    }
    theReader.Unknown();
    return theReader.mMessages;
}

}
