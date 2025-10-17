#include "ntconk.h"

PDRIVER_OBJECT g_driverObj;
bool g_running;

_Use_decl_annotations_ extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT driverObj, PUNICODE_STRING regPath)
{
    ConLog("Initializing NTCONK\n");

    g_driverObj = driverObj;
    g_driverObj->DriverUnload = DriverUnload;

    ConLog("Initializing render resources\n");
    auto status = InitRender();
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    ConLog("Creating render thread\n");
    g_running = true;
    status = PsCreateSystemThread(&g_renderThread, THREAD_ALL_ACCESS, nullptr, nullptr, nullptr, RenderThread, nullptr);
    if (!NT_SUCCESS(status))
    {
        ConLog("Failed to create render thread: 0x%08lX\n", status);
        return status;
    }

    return STATUS_SUCCESS;
}

_Use_decl_annotations_
void DriverUnload(PDRIVER_OBJECT driverObj)
{
    ConLog("Unloading NTCONK\n");
    g_running = false; // next iteration of the render thread loop will quit
    // wait for the render thread before unloading
    ZwWaitForSingleObject(g_renderThread, FALSE, nullptr);
}
