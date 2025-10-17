#pragma once

#include "phnt_windows.h"
#include "phnt.h"

#include <d3dkmthk.h>
#include <d3dumddi.h>

#define ConLog(...) DbgPrint("DXCON: " __VA_ARGS__)

struct DxCon
{
    UINT adapterIdx = 0;
    D3DKMT_HANDLE adapter = 0;

    PVOID umdBase = nullptr;

    D3DKMT_HANDLE device = 0;
    D3DKMT_HANDLE context = 0;

    D3DKMT_HANDLE framebufferAlloc = 0;
    D3DKMT_HANDLE framebufferResource = 0;
    PVOID framebufferMem = nullptr;
    SIZE_T framebufferSize = 0;
    
    UINT32 width = 800;
    UINT32 height = 600;

    D3DDDI_ADAPTERFUNCS adapterFuncs = {};

    DxCon() = default;
    DxCon(UINT adapter, UINT width, UINT height)
        : adapterIdx(adapter), width(width), height(height)
    {
    }

    NTSTATUS CreateDeviceResources();
    NTSTATUS Present();

  private:
    NTSTATUS QueryAdapterInfo(KMTQUERYADAPTERINFOTYPE type, PVOID data, UINT size);
    NTSTATUS GetAdapter();
    NTSTATUS LoadUMDriver();
    NTSTATUS CreateDevice();
    NTSTATUS CreateContext();
    NTSTATUS AllocFramebuffer();
    NTSTATUS SetDisplayMode();
};
