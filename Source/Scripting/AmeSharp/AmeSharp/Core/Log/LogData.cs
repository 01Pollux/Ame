using AmeSharp.Core.Base.Types;

namespace AmeSharp.Core.Log;

public record LogData(string Message, uint ThreadId, DateTimeOffset Time, LogLevel Level)
{
}
