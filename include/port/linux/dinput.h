/* Wine's dinput.h, with the ANSI names that the Windows SDK header gives the interfaces when UNICODE is not set */
#ifndef EE_PORT_LINUX_DINPUT_H
#define EE_PORT_LINUX_DINPUT_H

#include_next <dinput.h>

#define IDirectInput IDirectInputA
#define IDirectInput2 IDirectInput2A
#define IDirectInput7 IDirectInput7A
#define IDirectInputDevice IDirectInputDeviceA
#define IDirectInputDevice2 IDirectInputDevice2A
#define IDirectInputDevice7 IDirectInputDevice7A

#endif
