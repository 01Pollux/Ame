#include <Asset/Types/Common/TextFile.hpp>
#include <rttr/registration.h>

RTTR_REGISTRATION
{
    using namespace rttr;
    using namespace Ame::Asset::Common;

    registration::class_<TextFileAsset>("TextFileAsset")
        .constructor(&TextFileAsset::Create)
        .method("GetRef", &TextFileAsset::GetRef)
        .property("Text", &TextFileAsset::Get, &TextFileAsset::Set, rttr::access_levels::public_access);
}