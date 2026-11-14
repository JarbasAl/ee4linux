// The packed list of unit ids a selection message carries (include/ee/EESelectionPack.h). Expected bytes are worked out
// by hand from the format described there. The decoder is a copy of the game's own unpacker (EEMessage.cpp
// DecompressUnits, EXE 0x4c5fd1), which stays as the game has it: a list that decodes to its ids here reads the same in
// an unmodded game.
#include <cstdio>
#include <string>
#include <vector>

#include "../../../include/ee/EESelectionPack.h"

static int sChecks, sFailures;

static void Check(bool inOk, const std::string& inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat.c_str());
    }
}

// the game's unpacker, as it is
static void DecompressUnits(unsigned char* inBuffer, unsigned char inSize, std::vector<long>& outUnits)
{
    unsigned long theBase = 0;
    unsigned long i = 1;
    outUnits.clear();
    unsigned long theOwner = (unsigned long)inBuffer[0] << 24;
    do {
        unsigned long theCount = inBuffer[i] & 0x3f;
        unsigned long theGroup;
        switch (inBuffer[i] >> 6) {
        case 0: theBase = theOwner; break;
        case 1:
            theBase = ((unsigned long)inBuffer[++i] << 8) | theOwner;
            break;
        case 2:
            theGroup = (unsigned long)inBuffer[++i] << 8;
            theGroup += inBuffer[++i];
            theBase = ((theGroup << 8) & 0x00ffff00) | theOwner;
            break;
        case 3: theCount = 0; break;
        }
        i++;
        for (unsigned long j = 0; j < theCount; j++) {
            long theUnit = (long)(inBuffer[i] | theBase);
            outUnits.push_back(theUnit);
            i++;
        }
    } while (i != inSize);
}

static unsigned long Id(unsigned long inOwner, unsigned long inGroup, unsigned long inLow)
{
    return (inOwner << 24) | (inGroup << 8) | inLow;
}

static bool Bytes(const unsigned char* inBuffer, unsigned inSize, std::initializer_list<int> inExpected)
{
    if (inSize != inExpected.size())
        return false;
    unsigned i = 0;
    for (int b : inExpected)
        if (inBuffer[i++] != b)
            return false;
    return true;
}

static void TestFormat()
{
    unsigned char b[256];
    size_t packed = 99;
    std::vector<unsigned long> ids;
    unsigned n = SelectionPack(ids, 0, 5, b, packed);
    Check(Bytes(b, n, {5, 0xFF}) && packed == 0, "no units: the owner and 0xFF");

    ids = {Id(5, 0, 1), Id(5, 0, 2), Id(5, 0, 3)};
    n = SelectionPack(ids, 0, 5, b, packed);
    Check(Bytes(b, n, {5, 0x03, 1, 2, 3}) && packed == 3, "group 0: count, then the low bytes");

    ids = {Id(5, 0x07, 0x10), Id(5, 0x07, 0x11)};
    n = SelectionPack(ids, 0, 5, b, packed);
    Check(Bytes(b, n, {5, 0x42, 0x07, 0x10, 0x11}) && packed == 2, "group 7: 0x40 + 2, the group byte, the low bytes");

    ids = {Id(5, 0x1234, 0x99)};
    n = SelectionPack(ids, 0, 5, b, packed);
    Check(Bytes(b, n, {5, 0x81, 0x12, 0x34, 0x99}) && packed == 1, "group 0x1234: 0x80 + 1, two group bytes, the low byte");

    ids = {Id(5, 0x2, 1), Id(5, 0x1234, 2), Id(5, 0x2, 3), Id(5, 0, 4)};
    std::vector<unsigned long> ordered;
    SelectionOrder(ids, ordered);
    Check(ordered.size() == 4 && ordered[0] == Id(5, 0, 4) && ordered[1] == Id(5, 2, 1) && ordered[2] == Id(5, 2, 3)
              && ordered[3] == Id(5, 0x1234, 2),
          "the order: groups ascending (0, 2, 0x1234), a group in the order given");
    n = SelectionPack(ordered, 0, 5, b, packed);
    Check(Bytes(b, n, {5, 0x01, 4, 0x42, 0x02, 1, 3, 0x81, 0x12, 0x34, 2}) && packed == 4, "three groups in one list");
    n = SelectionPack(ordered, 1, 5, b, packed);
    Check(Bytes(b, n, {5, 0x42, 0x02, 1, 3, 0x81, 0x12, 0x34, 2}) && packed == 3, "from the second unit on");
}

static void TestBig()
{
    unsigned char b[256];
    size_t packed = 0;
    std::vector<unsigned long> ids;
    for (unsigned long i = 0; i < 100; i++)
        ids.push_back(Id(3, 0, i));
    unsigned n = SelectionPack(ids, 0, 3, b, packed);
    Check(n == 103 && packed == 100, "100 units of one group: runs of 63 and 37, 1 + 64 + 38 = 103 bytes (the game packed 63)");
    Check(b[1] == 63 && b[65] == 37, "the run headers are 63 and 37");

    // 300 units of one group: three runs of 63 (193 bytes), a fourth of 61 to byte 255; then 50 more in 52 bytes
    ids.clear();
    for (unsigned long i = 0; i < 300; i++)
        ids.push_back(Id(3, 0, i & 0xff));
    n = SelectionPack(ids, 0, 3, b, packed);
    Check(n == 255 && packed == 250, "300 units: 255 bytes, 250 packed (3 x 63 + 61)");
    Check(b[193] == 61, "the fourth run holds 61");
    std::vector<long> decoded;
    DecompressUnits(b, (unsigned char)n, decoded);
    bool same = decoded.size() == 250;
    for (size_t i = 0; same && i < 250; i++)
        same = (unsigned long)decoded[i] == ids[i];
    Check(same, "the game's unpacker reads the 250 packed ids");
    n = SelectionPack(ids, 250, 3, b, packed);
    Check(n == 52 && packed == 50, "the rest: 50 units in 52 bytes");

    // scattered ids: every unit its own 3-byte group header, so 63 fit (1 + 63 x 4 = 253)
    ids.clear();
    for (unsigned long i = 0; i < 300; i++)
        ids.push_back(Id(2, 0x100 + i, 7));
    std::vector<unsigned long> ordered;
    SelectionOrder(ids, ordered);
    size_t at = 0;
    int messages = 0;
    bool whole = true;
    std::vector<unsigned long> got;
    while (at < ordered.size() && messages < 20) {
        n = SelectionPack(ordered, at, 2, b, packed);
        if (n > 255 || packed == 0) { whole = false; break; }
        DecompressUnits(b, (unsigned char)n, decoded);
        for (long d : decoded)
            got.push_back((unsigned long)d);
        at += packed;
        messages++;
    }
    Check(whole && messages == 5 && got == ordered, "300 scattered units: 5 messages of 63, 63, 63, 63, 48, every id once, in order");
    n = SelectionPack(ordered, 0, 2, b, packed);
    Check(n == 253 && packed == 63, "scattered: 63 per message in 253 bytes");
}

int main()
{
    TestFormat();
    TestBig();
    printf("test_selection_pack.cpp: %d checks, %d failed\n", sChecks, sFailures);
    return sFailures ? 1 : 0;
}
