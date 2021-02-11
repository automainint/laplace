/*  laplace/platform/win32/win32_audio.cpp
 *
 *      Win32 audio implementation.
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

#define __laplace__windows_header__
#include <audioclient.h>
#include <mmdeviceapi.h>

#include "audio.h"
#include "thread.h"
#include <algorithm>

namespace laplace::win32 {
  using std::make_unique, std::unique_lock, std::shared_lock,
      std::vector, std::string, std::memory_order_release,
      std::memory_order_acquire, std::thread,
      std::chrono::duration;

  namespace this_thread = std::this_thread;

  const CLSID CLSID_MMDeviceEnumerator = __uuidof(
      MMDeviceEnumerator);
  const IID IID_IMMDeviceEnumerator = __uuidof(
      IMMDeviceEnumerator);
  const IID IID_IAudioClient       = __uuidof(IAudioClient);
  const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);

  audio::audio(audio::format request) {
    m_channel_count  = request.channel_count;
    m_sample_rate_hz = request.sample_rate_hz;
    m_sample_bits    = request.sample_bits;

    m_sample_bytes_total = (request.channel_count *
                            request.sample_bits) /
                           8;

    m_done.store(false, memory_order_release);
    m_active.store(false, memory_order_release);
    m_sync.store(false, memory_order_release);

    m_read  = 0;
    m_write = 0;

    m_thread = make_unique<thread>([=]() {
      render_thread(*this);
    });

    win32::set_thread_priority(
        *m_thread, default_thread_priority);
  }

  audio::~audio() {
    m_done.store(true, memory_order_release);
    m_thread->join();
  }

  void audio::write(cref_vbyte samples) {
    size_t offset = adjust_offset();
    size_t size   = adjust_bytes(samples.size());

    buffer_adjust(offset + size);
    buffer_write(samples, size);
  }

  void audio::set_active(bool is_active) {
    m_active.store(is_active, memory_order_release);
  }

  void audio::set_thread_priority(int priority) {
    auto _ul = unique_lock(m_lock);
    win32::set_thread_priority(*m_thread, priority);
  }

  void audio::sync() {
    while (!m_sync.load(memory_order_acquire)) {
      /*  TODO:
       *  Replace with std::condition_variable.
       */
      this_thread::yield();
    }
  }

  auto audio::get_channel_count() -> size_t {
    auto _sl = shared_lock(m_lock);
    return m_channel_count;
  }

  auto audio::get_sample_rate() -> size_t {
    auto _sl = shared_lock(m_lock);
    return m_sample_rate_hz;
  }

  auto audio::get_sample_bits() -> size_t {
    auto _sl = shared_lock(m_lock);
    return m_sample_bits;
  }

  auto audio::get_errors() -> vector<string> {
    auto _ul    = unique_lock(m_lock);
    auto result = vector<string>(m_errors);
    m_errors.clear();
    return result;
  }

  auto audio::adjust_offset() -> size_t {
    auto _sl = shared_lock(m_lock);

    return m_read <= m_write
               ? m_write - m_read
               : m_buffer.size() - m_read + m_write;
  }

  auto audio::adjust_bytes(size_t size) -> size_t {
    auto _sl = shared_lock(m_lock);
    return (size / m_sample_bytes_total) * m_sample_bytes_total;
  }

  auto audio::buffer_size() -> size_t {
    auto _sl = shared_lock(m_lock);
    return m_buffer.size();
  }

  void audio::buffer_adjust(size_t size_required) {
    if (buffer_size() < size_required) {
      auto _ul = unique_lock(m_lock);

      if (m_buffer.size() < size_required) {
        if (m_read <= m_write) {
          m_write -= m_read;
        } else {
          m_write += m_buffer.size() - m_read;
        }

        if (m_read > 0) {
          vbyte temp(m_read);
          move(m_buffer.begin(), m_buffer.begin() + m_read,
               temp.begin());
          move(m_buffer.begin() + m_read, m_buffer.end(),
               m_buffer.begin());
          move(temp.begin(), temp.end(), m_buffer.end() - m_read);
          m_read = 0;
        }

        m_buffer.resize(size_required);
      }
    }
  }

  void audio::buffer_write(cref_vbyte samples, size_t size) {
    auto _ul = unique_lock(m_lock);

    size_t i0 = m_write;
    size_t i1 = i0 + size;

    if (i1 >= m_buffer.size()) {
      m_write = i1 - m_buffer.size();
      i1      = m_buffer.size();

      move(samples.begin(), samples.begin() + (i1 - i0),
           m_buffer.begin() + i0);
      move(samples.begin() + (i1 - i0), samples.end(),
           m_buffer.begin());
    } else {
      m_write = i1;

      move(samples.begin(), samples.end(), m_buffer.begin() + i0);
    }
  }

  void audio::set_format(size_t channel_count,
                         size_t sample_rate_hz,
                         size_t sample_bits) {
    auto _ul = unique_lock(m_lock);

    m_channel_count  = channel_count;
    m_sample_rate_hz = sample_rate_hz;
    m_sample_bits    = sample_bits;

    m_sample_bytes_total = (channel_count * sample_bits) / 8;
  }

  void audio::render_buffer(uint8_t *data, size_t size) {
    buffer_adjust(size);

    bool reset = false;

    auto _sl = shared_lock(m_lock);

    if (m_read + size <= m_buffer.size()) {
      copy(m_buffer.begin() + m_read,
           m_buffer.begin() + m_read + size, data);

      reset = m_write >= m_read && m_write < m_read + size;

      m_read += size;

      if (m_read == m_buffer.size()) {
        m_read = 0;
      }
    } else {
      size_t m = m_buffer.size() - m_read;
      size_t n = size - m;

      copy(m_buffer.begin() + m_read, m_buffer.end(), data);
      copy(m_buffer.begin(), m_buffer.begin() + n, data + m);

      reset = m_write >= m_read || m_write < n;

      m_read = n;
    }

    if (reset) {
      m_write = m_read;
    }
  }

  void audio::render_thread(audio::ref a) {
    using clock = std::chrono::steady_clock;

    auto to_hns = [](double time_sec) -> REFERENCE_TIME {
      /*  Convert seconds to 100-nanoseconds.
       */
      return static_cast<REFERENCE_TIME>(time_sec * 10000000 +
                                         0.5);
    };

    HRESULT status;

    IMMDeviceEnumerator *enumerator    = nullptr;
    IMMDevice *          device        = nullptr;
    IAudioClient *       audio_client  = nullptr;
    IAudioRenderClient * render_client = nullptr;
    WAVEFORMATEX *       format        = nullptr;

    uint32_t buffer_size;
    uint32_t num_frames_available;
    uint32_t num_frames_padding;

    uint8_t *buffer_data = nullptr;

    status = CoInitializeEx(
        nullptr,
        COINIT_APARTMENTTHREADED | COINIT_SPEED_OVER_MEMORY);

    if (FAILED(status)) {
      auto _sl = shared_lock(a.m_lock);
      a.m_errors.emplace_back(__FUNCTION__);
      a.m_errors.back().append(": CoInitializeEx failed.");
    }

    status = CoCreateInstance(
        CLSID_MMDeviceEnumerator, nullptr, CLSCTX_ALL,
        IID_IMMDeviceEnumerator,
        reinterpret_cast<void **>(&enumerator));

    if (FAILED(status)) {
      auto _sl = shared_lock(a.m_lock);
      a.m_errors.emplace_back(__FUNCTION__);
      a.m_errors.back().append(": CoCreateInstance failed.");
      a.m_done.store(true, memory_order_release);
    }

    status = enumerator->GetDefaultAudioEndpoint(
        eRender, eMultimedia, &device);

    if (FAILED(status)) {
      auto _sl = shared_lock(a.m_lock);
      a.m_errors.emplace_back(__FUNCTION__);
      a.m_errors.back().append(
          ": GetDefaultAudioEndpoint failed.");
      a.m_done.store(true, memory_order_release);
    }

    status = device->Activate(
        IID_IAudioClient, CLSCTX_ALL, nullptr,
        reinterpret_cast<void **>(&audio_client));

    if (FAILED(status)) {
      auto _sl = shared_lock(a.m_lock);
      a.m_errors.emplace_back(__FUNCTION__);
      a.m_errors.back().append(": Activate failed.");
      a.m_done.store(true, memory_order_release);
    }

    format = reinterpret_cast<WAVEFORMATEX *>(
        CoTaskMemAlloc(sizeof format));

    format->cbSize     = 0;
    format->wFormatTag = WAVE_FORMAT_PCM;

    format->nChannels      = a.get_channel_count();
    format->nSamplesPerSec = a.get_sample_rate();
    format->wBitsPerSample = a.get_sample_bits();

    format->nBlockAlign = (format->nChannels *
                           format->wBitsPerSample) /
                          8;
    format->nAvgBytesPerSec = format->nSamplesPerSec *
                              format->nBlockAlign;

    status = audio_client->Initialize(AUDCLNT_SHAREMODE_SHARED,
                                      0, to_hns(0.1), 0, format,
                                      nullptr);

    if (FAILED(status)) {
      auto _sl = shared_lock(a.m_lock);
      a.m_errors.emplace_back(__FUNCTION__);
      a.m_errors.back().append(": Initialize failed.");
      a.m_done.store(true, memory_order_release);
    }

    CoTaskMemFree(format);

    status = audio_client->GetMixFormat(&format);

    if (FAILED(status)) {
      auto _sl = shared_lock(a.m_lock);
      a.m_errors.emplace_back(__FUNCTION__);
      a.m_errors.back().append(": GetMixFormat failed.");
      a.m_done.store(true, memory_order_release);
    }

    a.set_format(format->nChannels, format->nSamplesPerSec,
                 format->wBitsPerSample);

    status = audio_client->GetBufferSize(&buffer_size);

    if (FAILED(status)) {
      auto _sl = shared_lock(a.m_lock);
      a.m_errors.emplace_back(__FUNCTION__);
      a.m_errors.back().append(": GetBufferSize failed.");
      a.m_done.store(true, memory_order_release);
    }

    status = audio_client->GetService(
        IID_IAudioRenderClient,
        reinterpret_cast<void **>(&render_client));

    if (FAILED(status)) {
      auto _sl = shared_lock(a.m_lock);
      a.m_errors.emplace_back(__FUNCTION__);
      a.m_errors.back().append(": GetService failed.");
      a.m_done.store(true, memory_order_release);
    }

    auto chunk_duration_sec = static_cast<double>(buffer_size) /
                              format->nSamplesPerSec;
    auto half_duration_sec = chunk_duration_sec * .5;

    clock::time_point time;

    bool is_started = false;

    while (!a.m_done.load(memory_order_acquire)) {
      if (a.m_active.load(memory_order_acquire)) {
        if (!is_started) {
          time = clock::now();

          status = render_client->GetBuffer(
              buffer_size, &buffer_data);

          if (FAILED(status)) {
            auto _sl = shared_lock(a.m_lock);
            a.m_errors.emplace_back(__FUNCTION__);
            a.m_errors.back().append(": GetBuffer failed.");
            a.m_done.store(true, memory_order_release);
          }

          a.render_buffer(buffer_data, buffer_size);

          status = render_client->ReleaseBuffer(buffer_size, 0);

          if (FAILED(status)) {
            auto _sl = shared_lock(a.m_lock);
            a.m_errors.emplace_back(__FUNCTION__);
            a.m_errors.back().append(": ReleaseBuffer failed.");
            a.m_done.store(true, memory_order_release);
          }

          status = audio_client->Start();

          if (FAILED(status)) {
            auto _sl = shared_lock(a.m_lock);
            a.m_errors.emplace_back(__FUNCTION__);
            a.m_errors.back().append(": Start failed.");
            a.m_done.store(true, memory_order_release);
          }

          a.m_sync.store(true, memory_order_release);

          is_started = true;
        } else {
          auto new_time = clock::now();
          auto elapsed_sec =
              duration<double>(new_time - time).count();
          auto remaining_sec = half_duration_sec - elapsed_sec;

          if (remaining_sec > 0) {
            /*  TODO:
             *  Replace with std::condition_variable.
             */
            this_thread::sleep_for(
                duration<double>(remaining_sec));
          }

          time = new_time;
        }

        status = audio_client->GetCurrentPadding(
            &num_frames_padding);

        if (FAILED(status)) {
          auto _sl = shared_lock(a.m_lock);
          a.m_errors.emplace_back(__FUNCTION__);
          a.m_errors.back().append(
              ": GetCurrentPadding failed.");
          a.m_done.store(true, memory_order_release);
        }

        num_frames_available = buffer_size - num_frames_padding;

        if (num_frames_available > 0) {
          status = render_client->GetBuffer(
              num_frames_available, &buffer_data);

          if (FAILED(status)) {
            auto _sl = shared_lock(a.m_lock);
            a.m_errors.emplace_back(__FUNCTION__);
            a.m_errors.back().append(": GetBuffer failed.");
            a.m_done.store(true, memory_order_release);
          }

          a.render_buffer(buffer_data, num_frames_available);

          status = render_client->ReleaseBuffer(
              num_frames_available, 0);

          if (FAILED(status)) {
            auto _sl = shared_lock(a.m_lock);
            a.m_errors.emplace_back(__FUNCTION__);
            a.m_errors.back().append(": ReleaseBuffer failed.");
            a.m_done.store(true, memory_order_release);
          }
        }
      } else {
        if (is_started) {
          status = audio_client->Stop();

          if (FAILED(status)) {
            auto _sl = shared_lock(a.m_lock);
            a.m_errors.emplace_back(__FUNCTION__);
            a.m_errors.back().append(": Stop failed.");
            a.m_done.store(true, memory_order_release);
          }

          a.m_sync.store(false, memory_order_release);

          is_started = false;
        }

        /*  TODO:
         *  Replace with std::condition_variable.
         */
        this_thread::yield();
      }
    }

    if (is_started) {
      status = audio_client->Stop();

      if (FAILED(status)) {
        auto _sl = shared_lock(a.m_lock);
        a.m_errors.emplace_back(__FUNCTION__);
        a.m_errors.back().append(": Stop failed.");
        a.m_done.store(true, memory_order_release);
      }
    }

    if (format) {
      CoTaskMemFree(format);
      format = nullptr;
    }

    if (enumerator) {
      enumerator->Release();
      enumerator = nullptr;
    }

    if (device) {
      device->Release();
      device = nullptr;
    }

    if (audio_client) {
      audio_client->Release();
      audio_client = nullptr;
    }

    if (render_client) {
      render_client->Release();
      render_client = nullptr;
    }

    CoUninitialize();
  }
}
