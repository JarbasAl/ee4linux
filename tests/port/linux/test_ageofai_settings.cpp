// Age of AI's settings reader (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAISettings.cpp): the constant expressions, a
// file's sections, kinds, mistakes and unknown keys, and the shipped config/ageofai.ini. The expected values come from
// the rows as written in AgeOfAISettingsRows.h and from arithmetic done by hand, not from the reader.
#include <windows.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAISettings.cpp"
#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAI.h"

static int sChecks, sFailures;

static void Check(bool inOk, const std::string& inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat.c_str());
    }
}

static void CheckNumber(const char* inText, double inExpected, bool inIsBool)
{
    double theValue = -12345;
    bool theIsBool = !inIsBool;
    std::string theWhy;
    bool theOk = AgeOfAI::EvaluateConstant(inText, theValue, theIsBool, theWhy);
    Check(theOk && fabs(theValue - inExpected) < 1e-9 && theIsBool == inIsBool,
          std::string(inText) + " = " + std::to_string(theValue) + (theOk ? "" : " (" + theWhy + ")"));
}

static void CheckRefused(const char* inText)
{
    double theValue;
    bool theIsBool;
    std::string theWhy;
    Check(!AgeOfAI::EvaluateConstant(inText, theValue, theIsBool, theWhy) && !theWhy.empty(),
          std::string(inText) + " is refused with a reason");
}

static bool Has(const std::vector<std::string>& inLines, const char* inPart)
{
    for (const std::string& theLine : inLines)
        if (theLine.find(inPart) != std::string::npos)
            return true;
    return false;
}

static std::string Write(const char* inName, const char* inText)
{
    const char* theFolder = getenv("EE_PORT_TEST_DIR");
    std::string thePath = std::string(theFolder ? theFolder : ".") + "/" + inName;
    FILE* theFile = fopen(thePath.c_str(), "wb");
    fputs(inText, theFile);
    fclose(theFile);
    return thePath;
}

// every row's value at inA and inB is the same (texts compared as texts)
static std::string FirstDifference(const unsigned char* inA, const unsigned char* inB, bool inPerDifficulty)
{
    using namespace AgeOfAI;
    for (const Row& theRow : kRows) {
        if (theRow.mPerDifficulty != inPerDifficulty)
            continue;
        const unsigned char* a = inA + theRow.mOffset;
        const unsigned char* b = inB + theRow.mOffset;
        bool theSame;
        if (theRow.mKind == kStr)
            theSame = !strcmp((const char*)a, (const char*)b);
        else if (theRow.mKind == kStrP) {
            theSame = true;
            for (int p = 0; p < 16; p++)
                theSame = theSame && !strcmp((const char*)a + p * theRow.a, (const char*)b + p * theRow.a);
        } else if (theRow.mKind == kFlags)
            theSame = !memcmp(a, b, sizeof(int) * (size_t)theRow.a);
        else
            theSame = !memcmp(a, b, 4);
        if (!theSame)
            return theRow.mKey;
    }
    return std::string();
}

int main()
{
    using AgeOfAI::gSettings;
    using AgeOfAI::gSettingsAI;

    // the expressions ini.c documents
    CheckNumber("35%", 0.35, false);
    CheckNumber("2 + 3 * 4", 14, false);
    CheckNumber("(2 + 3) * 4", 20, false);
    CheckNumber("10 / 4", 2.5, false);
    CheckNumber("-3 + 1", -2, false);
    CheckNumber("min(7, 2, 5) + max(1, 9)", 11, false);
    CheckNumber("abs(-4) * round(2.5) + floor(1.9)", 13, false);
    CheckNumber("round(150 * 35%)", 53, false);
    CheckNumber(".5", 0.5, false);
    CheckNumber("true", 1, true);
    CheckNumber("1 < 2", 1, true);
    CheckNumber("3 <= 2", 0, true);
    CheckRefused("");
    CheckRefused("abc");
    CheckRefused("1 / 0");
    CheckRefused("2 +");
    CheckRefused("(1");
    CheckRefused("min(1)");
    CheckRefused("true + 1");
    CheckRefused("1 2");

    // no file: every row's default; [general] enabled is 0 here, 1 upstream
    std::vector<std::string> theLines = AgeOfAI::ReadSettings(Write("none.ini", "").c_str());
    Check(theLines.empty(), "an empty file tells nothing");
    theLines = AgeOfAI::ReadSettings("/nonexistent/ageofai.ini");
    Check(theLines.empty(), "a missing file tells nothing");
    Check(gSettings.enabled == 0, "enabled defaults to 0 (the classic AI)");
    Check(gSettings.scenario_ai == 1, "scenario_ai defaults to 1");
    Check(fabs(gSettingsAI[AgeOfAI::kDifficultyMedium].epoch_share - 0.40f) < 1e-6, "epoch_share defaults to 40%");
    // epoch_pacing: "classic,timer,rush,follow,independent", default independent (4); the easy AI's built-in text is
    // classic (0)
    Check(gSettingsAI[AgeOfAI::kDifficultyMedium].epoch_pacing == 4, "epoch_pacing defaults to independent");
    Check(gSettingsAI[AgeOfAI::kDifficultyEasy].epoch_pacing == 0, "the easy AI's built-in epoch_pacing is classic");
    Check(gSettingsAI[AgeOfAI::kDifficultyEasy].pop_follow_human == 1, "the easy AI's built-in pop_follow_human is 1");

    // a file with every kind, a mistake of each sort, and keys that are no setting
    std::string thePath = Write("ageofai.ini",
        "; a comment\n"
        "[General]\n"
        "Enabled = 1 ; on\n"
        "scenario_ai=2\n"
        "auto_victory=true\n"
        "answer_fire=Computers\n"
        "chat=loud\n"
        "personality_P3=\"rusher\"\n"
        "personality_P16=nobody\n"
        "bogus=1\n"
        "[ai_base]\n"
        "epoch_share=35%\n"
        "food_floor=7.5\n"
        "food_plan=hunt_near forage_near=1\n"
        "mine_fill=all\n"
        "citizen_plan=nothing\n"
        "[ai_hard]\n"
        "epoch_share=round(100 / 3) / 100\n"
        "epoch_pacing=rush\n"
        "[nope]\n"
        "x=1\n");
    theLines = AgeOfAI::ReadSettings(thePath.c_str());
    Check(gSettings.enabled == 1, "Enabled = 1 with an inline comment, names without case");
    Check(gSettings.scenario_ai == 1 && Has(theLines, "[general] scenario_ai=2: 0 or 1 expected - the default instead"),
          "a FLAG of 2 is a mistake and keeps its default");
    Check(gSettings.auto_victory == 1, "true for a 0 / 1 key");
    Check(gSettings.answer_fire == 2, "a CHOICE by name without case (game,all,computers: 2)");
    Check(gSettings.chat == 0 && Has(theLines, "chat=loud: one of off, debug, status expected"),
          "a CHOICE that is none of its names keeps its default");
    Check(!strcmp(gSettings.personality_P[3], "rusher") && gSettings.personality_P[2][0] == 0,
          "a STR_P key per player slot, its quotes taken off");
    Check(Has(theLines, "[general] personality_P16: no such setting"), "a STR_P slot past 15 is no setting");
    Check(Has(theLines, "[general] bogus: no such setting"), "an unknown key is told");
    Check(Has(theLines, "[nope]: no such section"), "an unknown section is told");
    const AgeOfAI::SettingsAI& theMedium = gSettingsAI[AgeOfAI::kDifficultyMedium];
    const AgeOfAI::SettingsAI& theEasy = gSettingsAI[AgeOfAI::kDifficultyEasy];
    const AgeOfAI::SettingsAI& theHard = gSettingsAI[AgeOfAI::kDifficultyHard];
    Check(fabs(theMedium.epoch_share - 0.35f) < 1e-6 && fabs(theEasy.epoch_share - 0.35f) < 1e-6,
          "[ai_base] for the medium and the easy AI");
    Check(fabs(theHard.epoch_share - 0.33f) < 1e-6, "[ai_hard] over [ai_base]: round(100 / 3) / 100 = 0.33");
    Check(theHard.epoch_pacing == 2 && theMedium.epoch_pacing == 4, "[ai_hard] epoch_pacing=rush (2)");
    Check(theMedium.food_floor == 18 && Has(theLines, "food_floor=7.5: a whole number expected"),
          "a NUM that is no whole number keeps its default (18)");
    // FOOD_PLAN_FIELDS: count_trained back_to_food forage_near hunt_near no_fields_wild wild_settlement
    const int theFood[6] = { 0, 0, 1, 1, 0, 0 };
    Check(!memcmp(&theMedium.food_plan, theFood, sizeof theFood), "FLAGS by name and name=1, every other field 0");
    const int theMines[3] = { 1, 1, 1 };   // MINE_FILL_FIELDS: all on
    Check(!memcmp(&theMedium.mine_fill, theMines, sizeof theMines), "FLAGS all");
    // CITIZEN_PLAN_FIELDS default "mine_slots, food_pays": mine_slots food_pays mine_settlement before_reserve
    const int theCitizens[4] = { 1, 1, 0, 0 };
    Check(!memcmp(&theMedium.citizen_plan, theCitizens, sizeof theCitizens)
              && Has(theLines, "citizen_plan=nothing: nothing: no field"),
          "a FLAGS word that is no field keeps the default");

    // the shipped config/ageofai.ini: every key a setting, every value usable, and every value the row's default
    // ([ai_easy] and [ai_hard] aside, which differ from [ai_base] on purpose)
    std::string theSource = __FILE__;
    std::string theShipped = theSource.substr(0, theSource.rfind("/tests/")) + "/config/ageofai.ini";
    AgeOfAI::ReadSettings("/nonexistent/ageofai.ini");
    static AgeOfAI::Settings sDefaults;
    static AgeOfAI::SettingsAI sDefaultsAI;
    sDefaults = gSettings;
    sDefaultsAI = gSettingsAI[AgeOfAI::kDifficultyMedium];
    theLines = AgeOfAI::ReadSettings(theShipped.c_str());
    for (const std::string& theLine : theLines)
        fprintf(stderr, "shipped: %s\n", theLine.c_str());
    Check(theLines.empty(), "the shipped ageofai.ini reads without a message");
    Check(gSettings.enabled == 0, "the shipped ageofai.ini selects the classic AI");
    std::string theDifference = FirstDifference((const unsigned char*)&gSettings, (const unsigned char*)&sDefaults, false);
    Check(theDifference.empty(), "the shipped file's general rows are their defaults; " + theDifference + " is not");
    theDifference = FirstDifference((const unsigned char*)&gSettingsAI[AgeOfAI::kDifficultyMedium],
                                    (const unsigned char*)&sDefaultsAI, true);
    Check(theDifference.empty(), "the shipped file's [ai_base] rows are their defaults; " + theDifference + " is not");

    // the scenario flag's scope
    bool theFlag = true;
    {
        AgeOfAI::ScenarioFlagScope theScope(theFlag, true);
        Check(!theFlag, "the scope clears the flag");
    }
    Check(theFlag, "the flag is back after the scope");
    {
        AgeOfAI::ScenarioFlagScope theScope(theFlag, false);
        Check(theFlag, "a scope that does not clear leaves the flag");
    }

    printf("%d checks, %d failed\n", sChecks, sFailures);
    return sFailures ? 1 : 0;
}
