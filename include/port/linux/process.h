/* The MSVC runtime's thread start and end, for the native Linux build (src/Port/Linux/Win32Thread.cpp). */
#ifndef EE_PORT_LINUX_PROCESS_H
#define EE_PORT_LINUX_PROCESS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uintptr_t _beginthreadex(void* inSecurity, unsigned int inStackSize,
                         unsigned int (__attribute__((stdcall)) * inStart)(void*), void* inArgument,
                         unsigned int inFlags, unsigned int* outThreadID);
void _endthreadex(unsigned int inResult);

#ifdef __cplusplus
}
#endif

#endif
