#pragma once

#include "phnt_windows.h"
#include "phnt.h"

#include <d3dkmthk.h>

#define ConLog(...) DbgPrint("DXCON: " __VA_ARGS__)

struct DxCon
{
    UINT adapterIdx;
    D3DKMT_HANDLE adapter;

    D3DKMT_HANDLE device;
    D3DKMT_HANDLE context;
    
    D3DKMT_HANDLE framebufferResource;
    PVOID framebufferMem;
    SIZE_T framebufferSize;
    
    UINT32 width;
    UINT32 height;

    NTSTATUS CreateDeviceResources();
    NTSTATUS Present();

  private:

    NTSTATUS QueryAdapterInfo(KMTQUERYADAPTERINFOTYPE type, PVOID data, UINT size);
    NTSTATUS GetAdapter();
    NTSTATUS CreateDevice();
    NTSTATUS CreateContext();
    NTSTATUS AllocFramebuffer();
};
