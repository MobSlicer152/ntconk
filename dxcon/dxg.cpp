#include "dxcon.h"

NTSTATUS DxCon::QueryAdapterInfo(KMTQUERYADAPTERINFOTYPE type, PVOID data, UINT size)
{
    D3DKMT_QUERYADAPTERINFO queryInfo = {};
    queryInfo.hAdapter = adapter;
    queryInfo.Type = type;
    queryInfo.pPrivateDriverData = data;
    queryInfo.PrivateDriverDataSize = size;

    return D3DKMTQueryAdapterInfo(&queryInfo);
}

NTSTATUS DxCon::GetAdapter()
{
    D3DKMT_ENUMADAPTERS adapters = {};
    auto status = D3DKMTEnumAdapters(&adapters);
    if (!NT_SUCCESS(status))
    {
        ConLog("D3DKMTEnumAdapters failed: 0x%08lX\n", status);
        return status;
    }

    if (adapters.NumAdapters < adapterIdx)
    {
        ConLog("Adapter %u was requested, only %u are present\n", adapterIdx, adapters.NumAdapters);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    adapter = adapters.Adapters[adapterIdx].hAdapter;

    ConLog("Getting info for adapter %d\n", adapterIdx);
    D3DKMT_ADAPTERREGISTRYINFO regInfo = {};
    status = QueryAdapterInfo(KMTQAITYPE_ADAPTERREGISTRYINFO, &regInfo, sizeof(D3DKMT_ADAPTERREGISTRYINFO));
    if (!NT_SUCCESS(status))
    {
        ConLog("Failed to get adapter info: 0x%08lX\n", status);
        return status;
    }

    ConLog("Adapter info:\n");
    ConLog("\tAdapter: %ws\n", regInfo.AdapterString);
    ConLog("\tBIOS: %ws\n", regInfo.BiosString);
    ConLog("\tChip type: %ws\n", regInfo.ChipType);
    ConLog("\tDAC type: %ws\n", regInfo.DacType);

    return STATUS_SUCCESS;
}

NTSTATUS DxCon::CreateDevice()
{
    D3DKMT_CREATEDEVICE createDev = {};
    createDev.hAdapter = adapter;

    ConLog("Creating device\n");
    auto status = D3DKMTCreateDevice(&createDev);
    if (!NT_SUCCESS(status))
    {
        ConLog("D3DKMTCreateDevice failed: 0x%08lX\n", status);
        return status;
    }

    device = createDev.hDevice;
    return STATUS_SUCCESS;
}

NTSTATUS DxCon::CreateContext()
{
    ConLog("Creating context\n");
    D3DKMT_CREATECONTEXT createCtx = {};
    createCtx.hDevice = device;
    auto status = D3DKMTCreateContext(&createCtx);
    if (!NT_SUCCESS(status))
    {
        ConLog("D3DKMTCreateContext failed: 0x%08lX\n", status);
        return status;
    }

    context = createCtx.hContext;
    return STATUS_SUCCESS;
}

NTSTATUS DxCon::AllocFramebuffer()
{
    D3DDDI_SURFACEINFO surfInfo = {};
    surfInfo.Width = width;
    surfInfo.Height = width;

    ConLog("Allocating framebuffer memory\n");

    framebufferSize = ALIGN_UP_BY(width * height * sizeof(UINT), PAGE_SIZE);
    auto status = NtAllocateVirtualMemory(NtCurrentProcess(), &framebufferMem, 0, &framebufferSize, MEM_COMMIT, PAGE_READWRITE);
    if (!NT_SUCCESS(status))
    {
        ConLog("NtAllocateVirtualMemory failed: 0x%08lX\n", status);
        return status;
    }

    ConLog("Creating framebuffer resource\n");
    D3DKMT_CREATESTANDARDALLOCATION stdAlloc = {};
    stdAlloc.Type = D3DKMT_STANDARDALLOCATIONTYPE_EXISTINGHEAP;
    stdAlloc.ExistingHeapData.Size = framebufferSize;

    D3DDDI_ALLOCATIONINFO allocInfo = {};
    allocInfo.pSystemMem = framebufferMem;

    D3DKMT_CREATEALLOCATION createAlloc = {};
    createAlloc.hDevice = device;
    createAlloc.NumAllocations = 1;
    createAlloc.pAllocationInfo = &allocInfo;
    createAlloc.pStandardAllocation = &stdAlloc;

    createAlloc.Flags.CreateResource = true;
    createAlloc.Flags.CreateShared = true;
    createAlloc.Flags.CrossAdapter = true;
    createAlloc.Flags.StandardAllocation = true;
    createAlloc.Flags.ExistingSysMem = true;

    status = D3DKMTCreateAllocation(&createAlloc);
    if (!NT_SUCCESS(status))
    {
        ConLog("D3DKMTCreateAllocation failed: 0x%08lX\n", status);
        return status;
    }

    framebufferResource = createAlloc.hResource;
    return STATUS_SUCCESS;
}

NTSTATUS DxCon::CreateDeviceResources()
{
    auto status = GetAdapter();
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    status = CreateDevice();
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    status = CreateContext();
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    status = AllocFramebuffer();
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    return STATUS_SUCCESS;
}

NTSTATUS DxCon::Present()
{
    D3DKMT_PRESENT present = {};
    present.hContext = context;
    present.hAdapter = adapter;
    present.hSource = framebufferResource;

    auto status = D3DKMTPresent(&present);
    if (!NT_SUCCESS(status))
    {
        ConLog("D3DKMTPresent failed: 0x%08lX\n", status);
        return status;
    }

    return STATUS_SUCCESS;
}