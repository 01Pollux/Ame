#include <Core/BufferBlob.hpp>
#include <Core/BufferBlob.Export.h>

void Ame_IDataBlob_Resize(Ame_DataBlob_t* blobHandle, size_t newSize)
{
    auto blob = std::bit_cast<Ame::IDataBlob*>(blobHandle);
    blob->Resize(newSize);
}

size_t Ame_IDataBlob_GetSize(Ame_DataBlob_t* blobHandle)
{
    auto blob = std::bit_cast<Ame::IDataBlob*>(blobHandle);
    return blob->GetSize();
}

void* Ame_IDataBlob_GetData(Ame_DataBlob_t* blobHandle)
{
    auto blob = std::bit_cast<Ame::IDataBlob*>(blobHandle);
    return blob->GetDataPtr();
}

const void* Ame_IDataBlob_GetConstData(Ame_DataBlob_t* blobHandle)
{
    auto blob = std::bit_cast<Ame::IDataBlob*>(blobHandle);
    return blob->GetConstDataPtr();
}

//

Ame_DataBlob_t* Ame_RawDataBlob_CreateDataBlob(const void* data, size_t initialSize)
{
    return std::bit_cast<Ame_DataBlob_t*>(
        AmeCreateRaw(Ame::RawDataBlob, std::bit_cast<const std::byte*>(data), initialSize));
}

Ame_DataBlob_t* Ame_RawDataBlob_CreateDataBlobFromBlob(const Ame_DataBlob_t* otherBlobHandle)
{
    auto otherBlob = std::bit_cast<const Ame::IDataBlob*>(otherBlobHandle);
    return std::bit_cast<Ame_DataBlob_t*>(AmeCreateRaw(Ame::RawDataBlob, otherBlob));
}
