#pragma once

#include "environment.predef.h"
#include "../core/defs.h"

namespace laplace::vm
{
    class agent : public std::enable_shared_from_this<agent>
    {
    public:
        static constexpr size_t id_undefined = -1;

        agent();
        virtual ~agent();

        void set_environment(ptr_environment env);

        virtual auto perform(size_t id, cref_vbyte args) -> vbyte;

        auto get_env() const -> ptr_environment;

    private:
        std::weak_ptr<environment> m_env;
    };
}
