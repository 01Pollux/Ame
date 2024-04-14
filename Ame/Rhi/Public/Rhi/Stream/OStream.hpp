#pragma once

#include <Rhi/Stream/Device.hpp>
#include <Rhi/Stream/Sink.hpp>
#include <Rhi/Stream/Source.hpp>
#include <boost/iostreams/stream.hpp>

namespace Ame::Rhi::Streaming
{
    using BufferStream  = boost::iostreams::stream<Streaming::BufferDevice>;
    using BufferOStream = boost::iostreams::stream<Streaming::BufferSink>;
    using BufferIStream = boost::iostreams::stream<Streaming::BufferSource>;
} // namespace Ame::Rhi::Streaming
