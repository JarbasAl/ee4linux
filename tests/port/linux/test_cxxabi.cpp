// Tests of the native build's C++ runtime (src/Port/Linux/CxxAbi.cpp), against the behaviour the original image has:
// MSVC's dynamic_cast judges a pointer by the object's own type, where the Itanium ABI also requires the object to
// derive from the pointer's static type. Built and run by tools/test_port_linux.py, which links these casts the way it
// links the game's (--wrap=__dynamic_cast).
#include <cstdio>
#include <typeinfo>

static int sFailures;
static int sChecks;

#define CHECK(condition)                                                                 \
    do {                                                                                 \
        ++sChecks;                                                                       \
        if (!(condition)) {                                                              \
            ++sFailures;                                                                 \
            fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition);         \
        }                                                                                \
    } while (0)

// The game's shape: two unrelated polymorphic classes, and a list head stored in a pointer of the other one
struct Unit {
    virtual ~Unit() {}
    int mValue = 1;
};

struct Head {
    virtual ~Head() {}
    int mValue = 2;
};

struct SpecialUnit : Unit {
};

static void TestCasts()
{
    Unit theUnit;
    SpecialUnit theSpecial;
    Head theHead;

    // a cast inside one hierarchy resolves as usual
    Unit* theBase = &theSpecial;
    CHECK(dynamic_cast<SpecialUnit*>(theBase) == &theSpecial);
    CHECK(dynamic_cast<SpecialUnit*>(&theUnit) == nullptr);

    // the game's cross-cast, off a pointer to a class the head does not derive from: the object's own type decides
    Unit* theStoredHead = reinterpret_cast<Unit*>(&theHead);
    CHECK(dynamic_cast<Head*>(theStoredHead) == &theHead);

    // a real unit is still not a head, and a null pointer stays null
    CHECK(dynamic_cast<Head*>(&theUnit) == nullptr);
    CHECK(dynamic_cast<Head*>(static_cast<Unit*>(nullptr)) == nullptr);

    // a reference cast either way
    bool theThrew = false;
    try {
        Head& theReference = dynamic_cast<Head&>(*theStoredHead);
        CHECK(&theReference == &theHead);
    } catch (...) {
        theThrew = true;
    }
    CHECK(!theThrew);
    theThrew = false;
    try {
        dynamic_cast<Head&>(theUnit);
        theThrew = false;
    } catch (const std::bad_cast&) {
        theThrew = true;
    }
    CHECK(theThrew);
}

int main()
{
    TestCasts();
    printf("%d checks, %d failed\n", sChecks, sFailures);
    return sFailures != 0;
}
