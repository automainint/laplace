/*  laplace/platform/win32/input.h
 *
 *      Win32 audio declarations.
 *
 *  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#pragma once

#include "../../core/defs.h"
#include "../thread.h"
#include <functional>
#include <shared_mutex>
#include <atomic>
#include <thread>

#undef min
#undef max
#undef near
#undef far

namespace laplace::win32
{
    class audio : public std::enable_shared_from_this<audio>
    {
    public:
        using ref   = audio &;
        using cref  = const audio &;
        using ptr   = std::shared_ptr<audio>;

        using fn_read = std::function<void (ref_vbyte buffer)>;

        struct format
        {
            size_t channel_count;
            size_t sample_rate_hz;
            size_t sample_bits;
        };

        static constexpr format default_format_request  = { 2, 44100, 32 };
        static constexpr int    default_thread_priority = platform::priority::critical;

        audio(format request = default_format_request);
        ~audio();

        void write(cref_vbyte samples);

        void set_active(bool is_active);
        void set_thread_priority(int priority);

        /*  Wait for render thread to start.
         */
        void sync();

        /*  1 - mono, 2 - stereo.
         */
        auto get_channel_count() -> size_t;

        /*  Samples per second for
         *  one channel.
         */
        auto get_sample_rate() -> size_t;

        /*  Sample bits:
         *  8-bit, 16-bit, 24-bit - integers,
         *  32-bit - floats.
         */
        auto get_sample_bits() -> size_t;

        auto get_errors() -> std::vector<std::string>;

    private:
        auto adjust_offset() -> size_t;
        auto adjust_bytes(size_t size) -> size_t;
        auto buffer_size() -> size_t;
        void buffer_adjust(size_t size_required);
        void buffer_write(cref_vbyte samples, size_t size);

        void set_format(size_t channel_count, size_t sample_rate_hz, size_t sample_bits);
        void render_buffer(uint8_t *data, size_t size);

        static void render_thread(ref a);

        std::shared_mutex               m_lock;
        std::atomic_bool                m_done;
        std::atomic_bool                m_active;
        std::atomic_bool                m_sync;
        std::unique_ptr<std::thread>    m_thread;

        std::vector<std::string> m_errors;

        vbyte   m_buffer;
        size_t  m_read;
        size_t  m_write;

        size_t  m_channel_count;
        size_t  m_sample_rate_hz;
        size_t  m_sample_bits;
        size_t  m_sample_bytes_total;
    };

    using ref_audio = audio::ref;
    using cref_audio = audio::cref;
    using ptr_audio = audio::ptr;
}
