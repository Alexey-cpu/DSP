#include "DZG_v6_SimGen_capi_host.h"
static DZG_v6_SimGen_host_DataMapInfo_T root;
static int initialized = 0;
__declspec( dllexport ) rtwCAPI_ModelMappingInfo *getRootMappingInfo()
{
    if (initialized == 0) {
        initialized = 1;
        DZG_v6_SimGen_host_InitializeDataMapInfo(&(root), "DZG_v6_SimGen");
    }
    return &root.mmi;
}

rtwCAPI_ModelMappingInfo *mexFunction() {return(getRootMappingInfo());}
