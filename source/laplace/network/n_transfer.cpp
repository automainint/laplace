/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "transfer.h"

#include "../core/serial.h"
#include "../core/utils.h"

namespace laplace::network {
  using std::min, std::unique_ptr, crypto::basic_cipher, std::span,
      serial::rd, serial::wr;

  void transfer::set_verbose(bool is_verbose) noexcept {
    m_verbose = is_verbose;
  }

  void transfer::set_cipher(
      unique_ptr<basic_cipher> cipher) noexcept {
    m_cipher = std::move(cipher);
  }

  void transfer::set_remote_key(span_cbyte key) noexcept {
    if (m_cipher)
      m_cipher->set_remote_key(key);
    else
      error_("No cipher.", __FUNCTION__);
  }

  void transfer::enable_encryption(bool is_enabled) noexcept {
    m_encryption = is_enabled;
  }

  auto transfer::pack(span<const span_cbyte> data) noexcept -> vbyte {
    return pack_internal(data, mark_plain);
  }

  auto transfer::encode(span<const span_cbyte> data) noexcept
      -> vbyte {
    if (is_encryption_enabled())
      return m_cipher->encrypt(pack_internal(data, mark_encrypted));
    return pack_internal(data, mark_plain);
  }

  auto transfer::decode(span_cbyte data) noexcept
      -> sl::vector<vbyte> {
    if (is_encryption_enabled()) {
      const auto plain = m_cipher->decrypt(data);
      m_loss_count     = m_cipher->get_loss_count();
      return unpack_internal(plain, mark_encrypted);
    }

    m_loss_count = 0;
    return unpack_internal(data, mark_plain);
  }

  auto transfer::get_public_key() const noexcept -> span_cbyte {
    if (!m_cipher)
      return {};
    return m_cipher->get_public_key();
  }

  auto transfer::is_cipher_set() const noexcept -> bool {
    return m_cipher ? true : false;
  }

  auto transfer::is_encryption_enabled() const noexcept -> bool {
    return is_cipher_set() && m_encryption;
  }

  auto transfer::get_loss_count() const noexcept -> sl::whole {
    return m_loss_count;
  }

  auto transfer::get_data_overhead() noexcept -> sl::whole {
    return n_data;
  }

  auto transfer::check_sum(span_cbyte data) noexcept -> uint64_t {
    auto sum = uint64_t {};

    for (sl::whole i = 0; i < data.size(); i += sizeof sum) {
      auto part = uint64_t {};

      memcpy(&part, data.data() + i,
             min(sizeof part, data.size() - i));

      sum ^= part;
    }

    return sum;
  }

  auto transfer::pack_internal(span<const span_cbyte> data,
                               uint16_t const         mark) noexcept
      -> vbyte {
    auto buf = vbyte {};

    buf.reserve([&]() {
      auto size = sl::whole {};
      for (auto message : data) size += n_data + message.size();
      return size;
    }());

    for (auto message : data) {
      const sl::whole offset = buf.size();
      buf.resize(offset + n_data + message.size());

      auto const sum  = check_sum(message);
      auto const size = uint64_t { message.size() };

      wr(buf, offset + n_mark, mark);
      wr(buf, offset + n_sum, sum);
      wr(buf, offset + n_size, size);

      memcpy(buf.data() + offset + n_data, message.data(),
             message.size());
    }

    return buf;
  }

  auto transfer::unpack_internal(span_cbyte     data,
                                 uint16_t const mark) noexcept
      -> sl::vector<vbyte> {

    auto buf    = sl::vector<vbyte> {};
    auto offset = sl::whole {};

    while (offset < data.size()) {
      const auto size = scan({ data.begin() + offset, data.end() },
                             mark);

      if (size > 0) {
        offset += n_data;

        buf.emplace_back(vbyte { data.begin() + offset,
                                 data.begin() + offset + size });

        offset += size;

      } else {
        m_loss_count++;
        offset++;
      }
    }

    return buf;
  }

  auto transfer::scan(span_cbyte data, uint16_t mark) const noexcept
      -> sl::whole {

    if (rd<uint16_t>(data, n_mark) != mark)
      return 0;

    auto const sum  = rd<uint64_t>(data, n_sum);
    auto const size = as_index(rd<uint64_t>(data, n_size));

    if (size < 0 || size + n_data > data.size())
      return 0;

    if (sum !=
        check_sum({ data.data() + n_data,
                    static_cast<span_cbyte::size_type>(size) })) {
      if (m_verbose)
        verb("Transfer: Wrong check sum.");
      return 0;
    }

    return size;
  }
}
