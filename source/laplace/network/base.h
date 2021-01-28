#pragma once

#include "defs.h"
#include "../core/defs.h"
#include "../platform/socket.h"
#include <cstdint>
#include <string>
#include <functional>
#include <vector>
#include <memory>

namespace laplace::network
{
    class base
    {
    public:
        static constexpr size_t address_size = 64;

        base();
        virtual ~base();

        auto receive_to(uint8_t *p, size_t count, io_mode mode) -> size_t;
        auto receive(size_t count, io_mode mode) -> vbyte;
        auto send(cref_vbyte seq) -> size_t;

        auto is_done() const -> bool;

        void close();

    protected:
        void done();

        bool m_is_done;
        SOCKET m_socket;
    };
}
