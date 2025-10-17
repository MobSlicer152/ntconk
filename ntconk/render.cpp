#include "ntconk.h"

HANDLE g_renderThread;

[[noreturn]] static void QuitRenderThread(NTSTATUS status);
static bool InitRender();

void RenderThread(void* context)
{
    UNREFERENCED_PARAMETER(context);

    ConLog("Render thread started\n");

   

    while (g_running)
    {
    }
}

[[noreturn]] static void QuitRenderThread(NTSTATUS status)
{
    // Call DriverUnload and quit this thread with the right status
    ZwUnloadDriver(&g_driverObj->DriverName);
    PsTerminateSystemThread(status);
}

static bool InitRender()
{
    D3DKMT_ENUMADAPTERS adapters = {};
    auto status = D3DKMTEnumAdapters(&adapters);
    if (!NT_SUCCESS(status))
    {
        ConLog("D3DKMTEnumAdapters failed: 0x%08lX\n", status);
        QuitRenderThread(status);
    }

    D3DKMT_CREATEDEVICE createDev = {};
    status = D3DKMTCreateDevice(&createDev);
}
