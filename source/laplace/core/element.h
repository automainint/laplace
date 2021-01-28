#pragma once

#include "defs.h"
#include <memory>
#include <atomic>

namespace laplace::core
{
    /*  Base class for any object, that
     *  can be loaded from a file.
     */
    class element
    {
    public:
        std::wstring file_name = L"";

        element();
        virtual ~element();

        auto get_type_id() const -> size_t;
        static auto type_id() -> size_t;

    protected:
        void set_type_id(size_t id);

        class type
        {
        public:
            type();
            ~type();

            auto get_id() const -> size_t;

        private:
            static std::atomic_size_t m_count;

            size_t m_id;
        };

    private:
        size_t m_type_id;
    };

    using ptr_element = std::shared_ptr<element>;
    using vptr_element = std::vector<ptr_element>;
}
