/*  laplace/platform/win32/audio.h
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

#ifndef laplace_platform_win32_audio_h
#define laplace_platform_win32_audio_h

#include "../../core/defs.h"
#include "../thread.h"
#include <atomic>
#include <functional>
#include <shared_mutex>
#include <thread>

namespace laplace::win32 {
  class audio : public std::enable_shared_from_this<audio> {
  public:
    using fn_read = std::function<void(span_byte buffer)>;

    struct format {
      sl::whole channel_count;
      sl::whole sample_rate_hz;
      sl::whole sample_bits;
    };

    static constexpr format default_format_request = { 2, 44100, 32 };
    static constexpr int    default_thread_priority =
        platform::priority::critical;

    audio(const audio &) = delete;
    auto operator=(const audio &) = delete;

    audio(format request = default_format_request);
    ~audio();

    void write(span_cbyte samples);

    void set_active(bool is_active);
    void set_thread_priority(int priority);

    /*  Wait for render thread to start.
     */
    void sync();

    /*  1 - mono, 2 - stereo.
     */
    auto get_channel_count() -> sl::index;

    /*  Samples per second for
     *  one channel.
     */
    auto get_sample_rate() -> sl::index;

    /*  Sample bits:
     *  8-bit, 16-bit, 24-bit - integers,
     *  32-bit - floats.
     */
    auto get_sample_bits() -> sl::index;

    auto get_errors() -> std::vector<std::string>;

  private:
    auto adjust_offset() -> sl::index;
    auto adjust_bytes(sl::whole size) -> sl::whole;
    auto buffer_size() -> sl::whole;
    void buffer_adjust(sl::whole size_required);
    void buffer_write(span_cbyte samples, sl::whole size);

    void set_format(sl::whole channel_count, sl::whole sample_rate_hz,
                    sl::whole sample_bits);
    void render_buffer(uint8_t *data, sl::whole size);

    static void render_thread(audio &a);

    std::atomic_bool m_done;
    std::atomic_bool m_active;
    std::atomic_bool m_sync;

    std::shared_mutex            m_lock;
    std::unique_ptr<std::thread> m_thread;

    std::vector<std::string> m_errors;

    vbyte     m_buffer;
    sl::index m_read;
    sl::index m_write;

    sl::whole m_channel_count;
    sl::whole m_sample_rate_hz;
    sl::whole m_sample_bits;
    sl::whole m_sample_bytes_total;
  };
}

#endif
