/* MSVC's debug-runtime header. The port builds the release configuration, where its macros do nothing. */
#ifndef EE_PORT_LINUX_CRTDBG_H
#define EE_PORT_LINUX_CRTDBG_H
#define _CRT_WARN 0
#define _RPT0(type, text) ((void)0)
#define _RPT1(type, text, a) ((void)0)
#define _RPT2(type, text, a, b) ((void)0)
#define _ASSERT(expression) ((void)0)
#define _ASSERTE(expression) ((void)0)
#endif
