// The GUIDs of the DirectDraw, Direct3D, DirectInput and DirectSound headers, and IID_IUnknown, defined once for the
// whole game, as dxguid.lib and uuid.lib define them on Windows. No game unit defines a GUID.
#include <windows.h>
#include <initguid.h>

#include <ddraw.h>
#include <d3d.h>
#include <dinput.h>
#include <dsound.h>

// windows.h declared these before initguid.h
DEFINE_GUID(IID_IUnknown, 0x00000000, 0x0000, 0x0000, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46);
DEFINE_GUID(GUID_NULL, 0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
