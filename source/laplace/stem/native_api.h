#pragma once

#include "../vm/agent.h"

namespace laplace::stem
{
    class native_api final : public vm::agent
    {
    public:
        native_api();
        ~native_api();

        auto perform(size_t id, cref_vbyte args) -> vbyte final;

    private:
    };
}
