// Age of AI's settings: the rows of AgeOfAISettingsRows.h as gSettings and, for the rows of [ai_base], gSettingsAI (a
// value per AI difficulty), read from ageofai.ini in the game folder. A key's text is checked by its row's kind; a
// mistake keeps the row's default and is reported. What each key means is written above it in ageofai.ini.
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI),
// dll/settings/settings.c and dll/base/ini.c at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0.
#ifndef AGEOFAI_AGEOFAISETTINGS_H
#define AGEOFAI_AGEOFAISETTINGS_H

#include <string>
#include <vector>

#include "AgeOfAISettingsRows.h"   // MAX_PATH: <windows.h> first

#define AOAI_FIELD_FLAG(name, a) int name;
#define AOAI_FIELD_INT(name, a) int name;
#define AOAI_FIELD_NUM(name, a) int name;
#define AOAI_FIELD_LIMIT(name, a) int name;
#define AOAI_FIELD_SHARE(name, a) float name;
#define AOAI_FIELD_CHOICE(name, a) int name;
#define AOAI_FIELD_STR(name, a) char name[a];
#define AOAI_FIELD_STR_P(name, a) char name[16][a];
#define AOAI_FIELD_FACTOR(name, a) float name;
#define AOAI_FLAGS_BOOL(f) int f;
#define AOAI_FLAGS_ENUM(f, names) int f;
#define AOAI_FIELD_FLAGS(name, a) struct { a(AOAI_FLAGS_BOOL, AOAI_FLAGS_ENUM) } name;   // an int per field
#define AOAI_FIELD_X(kind, sec, key, def, a, b) AOAI_FIELD_##kind(key, a)
#define AOAI_FIELD_D(kind, key, def, a, b) AOAI_FIELD_##kind(key, a)
#define AOAI_NONE(...)

namespace AgeOfAI {

// SHARE and FACTOR rows keep a fraction (35% is 0.35); CHOICE rows keep the index of the name; FLAGS rows an int per
// field (an enum field: the index of its value)
struct Settings {
    SETTINGS(AOAI_FIELD_X, AOAI_NONE)
};
struct SettingsAI {
    SETTINGS(AOAI_NONE, AOAI_FIELD_D)
};

enum { kDifficultyEasy, kDifficultyMedium, kDifficultyHard };

extern Settings gSettings;
extern SettingsAI gSettingsAI[3];   // by difficulty; the medium AI plays [ai_base] itself

// Reads inPath into gSettings and gSettingsAI: each row from its section (a row of [ai_base] for the easy and hard AI
// from [ai_easy] / [ai_hard] first), else its default. A missing file leaves every row at its default. Returns the
// mistakes, the values out of their range, and the keys and sections that are no setting, one line each.
std::vector<std::string> ReadSettings(const char* inPath);

// A constant expression of an ini value: numbers, % (35% is 0.35), + - * / ( ), min, max, abs, round, floor, and the
// bools true, false and comparisons. False and outWhy set when inText is none.
bool EvaluateConstant(const char* inText, double& outValue, bool& outIsBool, std::string& outWhy);

}

#endif
