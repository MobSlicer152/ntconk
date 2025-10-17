#include "ntconk.h"

NTSTATUS QueryAdapterInfo(D3DKMT_HANDLE adapter, KMTQUERYADAPTERINFOTYPE type, PVOID data, UINT size)
{
    D3DKMT_QUERYADAPTERINFO queryInfo = {};
    queryInfo.hAdapter = adapter;
    queryInfo.Type = type;
    queryInfo.pPrivateDriverData = data;
    queryInfo.PrivateDriverDataSize = size;

    return D3DKMTQueryAdapterInfo(&queryInfo);
}
