#pragma once

#include "extratypes.h"

#include <ntifs.h>
#include <d3dkmthk.h>

#define PHNT_MODE PHNT_MODE_KERNEL
#include "phnt.h"

#include "condrv.h"

#define ConLog(...) KdPrint(("NTCONK: " __VA_ARGS__))

// entry
EXTERN_C DRIVER_INITIALIZE DriverEntry;
extern DRIVER_UNLOAD DriverUnload;

// render
extern NTSTATUS InitRender();
extern KSTART_ROUTINE RenderThread;

// util
extern NTSTATUS QueryAdapterInfo(D3DKMT_HANDLE adapter, KMTQUERYADAPTERINFOTYPE type, PVOID data, UINT size);

extern PDRIVER_OBJECT g_driverObj;
extern HANDLE g_renderThread;
extern bool g_running;
