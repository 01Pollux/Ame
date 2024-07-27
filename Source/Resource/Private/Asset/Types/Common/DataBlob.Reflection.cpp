#include <Asset/Types/Common/DataBlob.hpp>
#include <rttr/registration.h>

RTTR_REGISTRATION
{
    using namespace rttr;
    using namespace Ame::Asset::Common;

    registration::class_<DataBlobAsset>("DataBlobAsset")
        .constructor(&DataBlobAsset::Create);
}