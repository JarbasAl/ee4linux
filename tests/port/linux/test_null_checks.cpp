// The port compiles the game with the null tests that VC6 kept. The game calls members that ignore `this` through a
// pointer that may be null, and tests that pointer afterwards: EEIsLineToUnitBlocked calls theList->Scale(...), then
// tests theList != NULL. Clang otherwise infers from the call that the pointer is not null and deletes the test, and
// the read the test guards faults. This test is compiled with the game's own unit flags.
#include <cstdio>

struct TagPointList {
    void Scale(float inScale) { sLastScale = inScale; }   // does not read `this`
    int mCount;
    static float sLastScale;
};
float TagPointList::sLastScale;

__attribute__((noinline)) static int CountAfterScale(TagPointList* inList)
{
    inList->Scale(2.0f);
    return inList != NULL ? inList->mCount : -1;
}

static TagPointList* volatile sNoList = NULL;

int main()
{
    TagPointList theList = {3};
    int theChecks = 2, theFailures = 0;
    if (CountAfterScale(&theList) != 3) {
        ++theFailures;
        fprintf(stderr, "FAIL: a list's count after Scale\n");
    }
    if (CountAfterScale(sNoList) != -1) {   // without the null test, the read of mCount faults here
        ++theFailures;
        fprintf(stderr, "FAIL: a null list after Scale\n");
    }
    if (theFailures) {
        printf("%d of %d checks failed\n", theFailures, theChecks);
        return 1;
    }
    printf("%d checks passed\n", theChecks);
    return 0;
}
