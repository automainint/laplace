#pragma once

#include "agent.h"
#include "execute.h"
#include "../platform/thread.h"
#include <map>

namespace laplace::vm
{
    /*  Laplace Virtual Machine implementation.
     */
    class instance final : public agent
    {
    public:
        static constexpr size_t default_thread_count = 0;
        static constexpr int default_thread_priority = platform::priority::highest;

        instance();
        ~instance();

        void new_thread(size_t id, span_cbyte args);
        auto perform(size_t id, span_cbyte args) -> vbyte final;

        void tick();

    private:
        void execute(execute::ref_state s);

        vbyte   m_bitcode;
        heap    m_heap;
        std::map<size_t, size_t>    m_exports;
        std::vector<execute::state> m_threads;
    };
}
