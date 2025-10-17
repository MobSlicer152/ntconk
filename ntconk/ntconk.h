#pragma once

#include "extratypes.h"

#include <ntifs.h>
#include <d3dkmthk.h>

#define PHNT_MODE PHNT_MODE_KERNEL
#include "phnt.h"

#include "condrv.h"

#define ConLog(...) KdPrint(("NTCONK: " __VA_ARGS__))

EXTERN_C DRIVER_INITIALIZE DriverEntry;
extern DRIVER_UNLOAD DriverUnload;

extern KSTART_ROUTINE RenderThread;

extern PDRIVER_OBJECT g_driverObj;
extern HANDLE g_renderThread;
extern bool g_running;
