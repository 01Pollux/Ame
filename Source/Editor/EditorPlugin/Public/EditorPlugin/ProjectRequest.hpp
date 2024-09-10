#pragma once

#include <Core/Interface.hpp>
#include <Core/Coroutine.hpp>

namespace Ame::Editor
{
    class IProjectRequest : public IObject
    {
    public:
        [[nodiscard]] Co::shared_result<void> Process()
        {
            return m_Result;
        }

        void SetResult()
        {
            m_Promise.set_result();
        }

    private:
        Co::result_promise<void> m_Promise;
        Co::shared_result<void>  m_Result = m_Promise.get_result();
    };
} // namespace Ame::Editor