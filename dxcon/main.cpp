#include "dxcon.h"

KERNEL_CALLBACK_TABLE g_userDispatch;

NTSTATUS DummyKernelCallback(UINT_PTR a1, UINT_PTR a2, UINT_PTR a3, UINT_PTR a4)
{
    ConLog("KernelCallback(0x%016zX, 0x%016zX, 0x%016zX, 0x%016zX)\n", a1, a2, a3, a4);
    return NtCallbackReturn(nullptr, 0, STATUS_SUCCESS);
}

void InitDummyKernelCallbacks()
{
    for (int i = 0; i < sizeof(KERNEL_CALLBACK_TABLE) / sizeof(UINT_PTR); i++)
    {
        ((PUINT_PTR)&g_userDispatch)[i] = (UINT_PTR)DummyKernelCallback;
    }
    NtCurrentPeb()->KernelCallbackTable = &g_userDispatch;
}

EXTERN_C NTSYSCALLAPI NTSTATUS NTAPI NtUserInitialize(HANDLE ProcessHandle, UINT_PTR Unknown1);
EXTERN_C NTSYSCALLAPI NTSTATUS NTAPI NtGdiInit();

NTSTATUS InitNtUser()
{
    InitDummyKernelCallbacks();

    //auto status = NtUserInitialize(NtCurrentProcess(), 0);
    //if (!NT_SUCCESS(status))
    //{
    //    ConLog("NtUserInitialize failed: 0x%08lX\n", status);
    //    return status;
    //}

    auto status = NtGdiInit();
    if (!NT_SUCCESS(status))
    {
        ConLog("NtGdiInit failed: 0x%08lX\n", status);
        return status;
    }

    return STATUS_SUCCESS;
}

void CheckForDebugEvent()
{
    // ntlaunch.exe signals this so this process can wait for it, and let a debugger attach
    HANDLE dbgEvent = nullptr;
    UNICODE_STRING eventName = RTL_CONSTANT_STRING(L"\\??\\NtLaunchDebugEvent");
    OBJECT_ATTRIBUTES attrs = RTL_CONSTANT_OBJECT_ATTRIBUTES(&eventName, 0);
    auto status = NtOpenEvent(&dbgEvent, EVENT_ALL_ACCESS, &attrs);
    if (NT_SUCCESS(status))
    {
        ConLog("waiting for debug event...\n");
        NtWaitForSingleObject(dbgEvent, FALSE, nullptr);
        NtClose(dbgEvent);
    }
}

int main(int argc, char* argv[], char* envp[], int dbgFlag)
{
    CheckForDebugEvent();

    auto status = InitNtUser();
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    DxCon state;

    status = state.CreateDeviceResources();
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    while (true)
    {
        state.Present();
    }

    return STATUS_SUCCESS;
}
