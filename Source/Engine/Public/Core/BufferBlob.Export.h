#pragma once

typedef struct Ame_DataBlob_t Ame_DataBlob_t;

// clang-format off
extern "C"
{
    AME_ENGINE_C(void, Ame_IDataBlob_Resize(Ame_DataBlob_t* blobHandle, size_t newSize));
    AME_ENGINE_C(size_t, Ame_IDataBlob_GetSize(Ame_DataBlob_t* blobHandle));
    AME_ENGINE_C(void*, Ame_IDataBlob_GetData(Ame_DataBlob_t* blobHandle));
    AME_ENGINE_C(const void*, Ame_IDataBlob_GetConstData(Ame_DataBlob_t* blobHandle));

    //

    // Must be released with Ame_IBaseObject_Release
    AME_ENGINE_C(Ame_DataBlob_t*, Ame_RawDataBlob_CreateDataBlob(const void* data, size_t initialSize));
    AME_ENGINE_C(Ame_DataBlob_t*, Ame_RawDataBlob_CreateDataBlobFromBlob(const Ame_DataBlob_t* otherBlobHandle));
}
// clang-format on