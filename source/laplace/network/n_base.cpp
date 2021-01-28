#include "base.h"
#include "utils.h"
#include <algorithm>
#include <thread>
#include <chrono>

using namespace laplace;
using namespace network;
using namespace std;
using namespace chrono;

base::base()
{
    m_is_done = false;
    m_socket = INVALID_SOCKET;
}

base::~base()
{
    close();
}

auto base::receive_to(uint8_t *p, size_t count, io_mode mode) -> size_t
{
    size_t size = 0;

    if (m_socket != INVALID_SOCKET && (p != nullptr || count == 0))
    {
        auto buf = reinterpret_cast<char *>(p);
        bool is_sync = false;

        for (; size < count;)
        {
            auto part = clamp_chunk(count - size);

            auto n = ::recv(m_socket, buf + size, part, 0);

            if (n != SOCKET_ERROR)
            {
                size += n;
            }
            else if (socket_error() != socket_wouldblock() || is_done())
            {
                cerr << "TCP: " << __FUNCTION__ << " - " << socket_error() << endl;
                close();
                break;
            }
            else if (mode == sync)
            {
                is_sync = set_mode(m_socket, sync);

                if (!is_sync)
                {
                    cerr << "TCP: " << __FUNCTION__ << " - ioctlsocket failed." << endl;
                    close();
                    break;
                }
            }
            else break;
        }
        
        if (is_sync)
        {
            if (!set_mode(m_socket, async))
            {
                cerr << "TCP: " << __FUNCTION__ << " - ioctlsocket failed." << endl;
                close();
            }
        }
    }

    return size;
}

auto base::receive(size_t count, io_mode mode) -> vbyte
{
    vbyte seq;

    if (m_socket != INVALID_SOCKET)
    {
        seq.resize(count);
        seq.resize(receive_to(seq.data(), count, mode));
    }

    return seq;
}

auto base::send(cref_vbyte seq) -> size_t
{
    size_t count = 0;

    if (m_socket != INVALID_SOCKET)
    {
        auto buf = reinterpret_cast<const char *>(seq.data());
        bool is_sync = false;

        for (; count < seq.size();)
        {
            auto part = clamp_chunk(seq.size() - count);

            auto n = ::send(m_socket, buf + count, part, 0);

            if (n != SOCKET_ERROR)
            {
                count += n;
            }
            else if (socket_error() != socket_wouldblock() || is_done())
            {
                cerr << "TCP: " << __FUNCTION__ << " - " << socket_error() << endl;
                close();
                break;
            }
            else
            {
                is_sync = set_mode(m_socket, sync);

                if (!is_sync)
                {
                    cerr << "TCP: " << __FUNCTION__ << " - ioctlsocket failed." << endl;
                    close();
                    break;
                }
            }
        }

        if (is_sync)
        {
            if (!set_mode(m_socket, async))
            {
                cerr << "TCP: " << __FUNCTION__ << " - ioctlsocket failed." << endl;
                close();
            }
        }
    }

    return count;
}

auto base::is_done() const -> bool
{
    return m_is_done;
}

void base::close()
{
    if (m_socket != INVALID_SOCKET)
    {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
    }
}

void base::done()
{
    m_is_done = true;
}
