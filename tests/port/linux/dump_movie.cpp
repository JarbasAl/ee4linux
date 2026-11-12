// Plays a movie through the native build's Bink calls (src/Port/Linux/Bink.cpp) in the DX7 plug-in's order
// (DX7Movie::PlayBink): wait, decode, copy into a 32-bit buffer, test for the end, next frame. It prints the movie's
// size and frame count, the frames played, the time taken and the decode rate, and writes the frames named on the
// command line as raw BGRA files. tools/test_port_linux.py compares them with FFmpeg's own decode.
//
//   dump_movie <movie name> <wait: 0|1> <output folder> [frame number...]
#include <windows.h>
#include <bink/bink.h>

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <set>
#include <vector>

int main(int inCount, char** inArguments)
{
    if (inCount < 4)
        return 2;
    bool theWait = atoi(inArguments[2]) != 0;
    std::set<unsigned long> theWanted;
    for (int i = 4; i < inCount; ++i)
        theWanted.insert(strtoul(inArguments[i], nullptr, 10));

    HBINK theBink = BinkOpen(inArguments[1], 0);
    if (!theBink)
        return 3;
    printf("size %lu %lu\nframes %lu\n", theBink->Width, theBink->Height, theBink->Frames);
    const S32 kPitch = theBink->Width * 4 + 64;             // a pitch wider than the row, as a surface may have
    std::vector<unsigned char> theBuffer(kPitch * theBink->Height);
    unsigned long thePlayed = 0;
    auto theStart = std::chrono::steady_clock::now();
    for (bool theFinished = false; !theFinished;) {
        if (theWait)
            while (BinkWait(theBink))
                Sleep(1);
        BinkDoFrame(theBink);
        if (!BinkCopyToBuffer(theBink, theBuffer.data(), kPitch, theBink->Height, 0, 0, 3))
            return 4;
        ++thePlayed;
        if (theWanted.count(theBink->FrameNum)) {
            char thePath[4096];
            snprintf(thePath, sizeof thePath, "%s/frame-%lu.bgra", inArguments[3], theBink->FrameNum);
            FILE* theFile = fopen(thePath, "wb");
            for (unsigned long y = 0; y < theBink->Height; ++y)
                fwrite(&theBuffer[y * kPitch], 4, theBink->Width, theFile);
            fclose(theFile);
        }
        theFinished = theBink->FrameNum == theBink->Frames;
        if (!theFinished)
            BinkNextFrame(theBink);
    }
    double theSeconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - theStart).count();
    printf("played %lu\nlast %lu of %lu\nseconds %.3f\nrate %.1f\n", thePlayed, theBink->FrameNum, theBink->Frames,
           theSeconds, thePlayed / theSeconds);
    BinkClose(theBink);
    return 0;
}
