#include <stdio.h>

#include <phnt_windows.h>
#include <phnt.h>

int wmain(int argc, wchar_t* argv[])
{
    if (argc < 2)
    {
        printf("usage: %ls <exe name> \"<command line>\"\n", argv[0]);
        return STATUS_INVALID_PARAMETER;
    }

    UNICODE_STRING imagePath = {};
    RtlDosPathNameToNtPathName_U(argv[1], &imagePath, nullptr, nullptr);
    UNICODE_STRING cmdLine = {};
    RtlInitUnicodeString(&cmdLine, argv[2]);

    printf("creating process with image \"%lZ\" and commandline \"%lZ\"\n", imagePath, cmdLine);

    PRTL_USER_PROCESS_PARAMETERS params;
    auto status = RtlCreateProcessParametersEx(&params,
                                               &imagePath,
                                               nullptr,
                                               nullptr,
                                               &cmdLine,
                                               nullptr,
                                               nullptr,
                                               nullptr,
                                               nullptr,
                                               nullptr,
                                               RTL_USER_PROC_PARAMS_NORMALIZED);
    if (!NT_SUCCESS(status))
    {
        printf("RtlCreateProcessParametersEx failed: NTSTATUS 0x%08lX\n", status);
        return status;
    }

    BYTE attrsRaw[sizeof(PS_ATTRIBUTE_LIST) + sizeof(PS_ATTRIBUTE)] = {};
    PS_ATTRIBUTE_LIST& attrs = *(PPS_ATTRIBUTE_LIST)attrsRaw;
    attrs.TotalLength = sizeof(attrsRaw);
    attrs.Attributes[0].Attribute = PS_ATTRIBUTE_IMAGE_NAME;
    attrs.Attributes[0].Value = (ULONG_PTR)imagePath.Buffer;
    attrs.Attributes[0].Size = imagePath.Length;
    attrs.Attributes[1].Attribute = PS_ATTRIBUTE_PARENT_PROCESS;
    attrs.Attributes[1].Value = (ULONG_PTR)NtCurrentProcess();
    attrs.Attributes[1].Size = sizeof(HANDLE);

    PS_CREATE_INFO info = {};
    info.State = PsCreateInitialState;
    info.Size = sizeof(PS_CREATE_INFO);

    HANDLE processHandle = nullptr;
    HANDLE threadHandle = nullptr;
    status = NtCreateUserProcess(&processHandle,
                                 &threadHandle,
                                 PROCESS_ALL_ACCESS,
                                 THREAD_ALL_ACCESS,
                                 nullptr,
                                 nullptr,
                                 PROCESS_CREATE_FLAGS_CREATE_SUSPENDED,
                                 0,
                                 params,
                                 &info,
                                 &attrs);
    if (!NT_SUCCESS(status))
    {
        printf("NtCreateUserProcess failed: NTSTATUS 0x%08lX\n", status);
        return status;
    }

    PROCESS_BASIC_INFORMATION basicInfo = {};
    NtQueryInformationProcess(processHandle, ProcessBasicInformation, &basicInfo, sizeof(PROCESS_BASIC_INFORMATION), nullptr);
    printf("process %zu created (state = 0x%08lX)!\n", basicInfo.UniqueProcessId, info.State);

    // assume that if this process is being debugged, it's so orbss.exe can be debugged too
    HANDLE event = nullptr;
    if (NtCurrentPeb()->BeingDebugged)
    {
        UNICODE_STRING eventName = RTL_CONSTANT_STRING(L"\\??\\NtLaunchDebugEvent");
        OBJECT_ATTRIBUTES attrs = RTL_CONSTANT_OBJECT_ATTRIBUTES(&eventName, 0);
        status = NtCreateEvent(&event, EVENT_ALL_ACCESS, &attrs, NotificationEvent, FALSE);
        if (NT_SUCCESS(status))
        {
            printf("created debug event\n");
        }
    }

    NtResumeProcess(processHandle);

    if (event)
    {
        NtSetEvent(event, nullptr);
    }

    printf("process resumed, waiting...\n");
    NtWaitForSingleObject(processHandle, FALSE, nullptr);

    NtQueryInformationProcess(processHandle, ProcessBasicInformation, &basicInfo, sizeof(PROCESS_BASIC_INFORMATION), nullptr);
    printf("process returned %lu 0x%08lX\n", basicInfo.ExitStatus, basicInfo.ExitStatus);
    return status;
}
