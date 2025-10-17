#include <stdio.h>

#include "phnt_windows.h"
#include "phnt.h"

#include <d3dkmthk.h>

#define ConLog(...) printf("NTCONU: " __VA_ARGS__)

NTSTATUS QueryAdapterInfo(D3DKMT_HANDLE adapter, KMTQUERYADAPTERINFOTYPE type, PVOID data, UINT size)
{
    D3DKMT_QUERYADAPTERINFO queryInfo = {};
    queryInfo.hAdapter = adapter;
    queryInfo.Type = type;
    queryInfo.pPrivateDriverData = data;
    queryInfo.PrivateDriverDataSize = size;

    return D3DKMTQueryAdapterInfo(&queryInfo);
}

int main(int argc, char* argv[])
{
    D3DKMT_ENUMADAPTERS adapters = {};
    auto status = D3DKMTEnumAdapters(&adapters);
    if (!NT_SUCCESS(status))
    {
        ConLog("D3DKMTEnumAdapters failed: 0x%08lX\n", status);
        return status;
    }

    if (adapters.NumAdapters < 1)
    {
        ConLog("Didn't get any adapters\n");
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    auto adapterIdx = 0;
    auto adapter = adapters.Adapters[adapterIdx].hAdapter;

    ConLog("Getting info for adapter %d\n", adapterIdx);
    D3DKMT_ADAPTERREGISTRYINFO regInfo = {};
    status = QueryAdapterInfo(adapter, KMTQAITYPE_ADAPTERREGISTRYINFO, &regInfo, sizeof(D3DKMT_ADAPTERREGISTRYINFO));
    if (!NT_SUCCESS(status))
    {
        ConLog("Failed to get adapter info: 0x%08lX\n", status);
    }

    ConLog("Adapter info:\n");
    ConLog("\tAdapter: %ws\n", regInfo.AdapterString);
    ConLog("\tBIOS: %ws\n", regInfo.BiosString);
    ConLog("\tChip type: %ws\n", regInfo.ChipType);
    ConLog("\tDAC type: %ws\n", regInfo.DacType);

    D3DKMT_CREATEDEVICE createDev = {};
    createDev.hAdapter = adapter;

    ConLog("Creating device\n");
    status = D3DKMTCreateDevice(&createDev);
    if (!NT_SUCCESS(status))
    {
        ConLog("D3DKMTCreateDevice failed: 0x%08lX\n", status);
        return status;
    }

    return STATUS_SUCCESS;
}
