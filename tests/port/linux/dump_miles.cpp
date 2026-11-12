// Plays a sound file through the native build's Miles calls (src/Port/Linux/Miles.cpp) as the mixer plug-in loads
// one: AIL_file_size, AIL_file_read, AIL_set_named_sample_file. The driver runs at the sample's own rate with full
// volume, centred, so the mix is the decoded PCM itself. It prints the rate, the granularity, the length in
// milliseconds, the frames played and the position at the end, and writes the frames as 16-bit stereo PCM.
// tools/test_port_linux.py compares them with FFmpeg's own decode.
//
//   dump_miles <Windows path> <output file>
#include "../../../src/Port/Linux/Win32Internal.h"

#include <mss/mss.h>

#include <cstdio>
#include <vector>

int main(int inCount, char** inArguments)
{
    if (inCount < 3)
        return 2;
    eeport::MilesWithoutDevice();
    (AIL_startup)();
    S32 theSize = AIL_file_size(inArguments[1]);
    void* theImage = AIL_file_read(inArguments[1], NULL);
    if (theSize <= 0 || !theImage)
        return 3;

    HDIGDRIVER theProbe = AIL_open_digital_driver(22050, 16, 2, 0);
    HSAMPLE theSample = AIL_allocate_sample_handle(theProbe);
    if (!AIL_set_named_sample_file(theSample, inArguments[1], theImage, theSize, 0))
        return 4;
    S32 theRate = AIL_sample_playback_rate(theSample);
    AIL_close_digital_driver(theProbe);

    HDIGDRIVER theDriver = AIL_open_digital_driver(U32(theRate), 16, 2, 0);
    theSample = AIL_allocate_sample_handle(theDriver);
    AIL_init_sample(theSample);
    if (!AIL_set_named_sample_file(theSample, inArguments[1], theImage, theSize, 0))
        return 5;
    S32 theTotal = 0, theCurrent = 0;
    AIL_sample_ms_position(theSample, &theTotal, &theCurrent);
    printf("rate %ld\ngranularity %ld\nms %ld\n", theRate, AIL_sample_granularity(theSample), theTotal);

    FILE* theOut = fopen(inArguments[2], "wb");
    if (!theOut)
        return 6;
    AIL_start_sample(theSample);
    // one frame a call, so the frames counted end where the sample does
    std::vector<short> thePCM;
    short theFrame[2];
    while (AIL_sample_status(theSample) == 4) {
        eeport::MixMiles(theDriver, theFrame, 1);
        if (AIL_sample_status(theSample) == 4 || theFrame[0] || theFrame[1])
            thePCM.insert(thePCM.end(), theFrame, theFrame + 2);
    }
    unsigned long theFrames = thePCM.size() / 2;
    fwrite(thePCM.data(), 4, theFrames, theOut);
    fclose(theOut);
    printf("frames %lu\nend %lu of %ld\n", theFrames, AIL_sample_position(theSample), theSize);
    AIL_mem_free_lock(theImage);
    AIL_shutdown();
    return 0;
}
