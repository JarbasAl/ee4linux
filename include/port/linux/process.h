/* The MSVC runtime's thread start and end, for the native Linux build (src/Port/Linux/Win32Thread.cpp). */
#ifndef EE_PORT_LINUX_PROCESS_H
#define EE_PORT_LINUX_PROCESS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The start routine's convention: the i386 form the images use, and Wine's Windows convention where the two differ */
#ifdef __x86_64__
#define EE_THREAD_START __stdcall
#else
#define EE_THREAD_START __attribute__((stdcall))
#endif
uintptr_t _beginthreadex(void* inSecurity, unsigned int inStackSize,
                         unsigned int (EE_THREAD_START * inStart)(void*), void* inArgument,
                         unsigned int inFlags, unsigned int* outThreadID);
void _endthreadex(unsigned int inResult);

#ifdef __cplusplus
}
#endif

#endif
