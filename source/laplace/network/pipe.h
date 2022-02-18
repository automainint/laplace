/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_NETWORK_PIPE_H
#define LAPLACE_NETWORK_PIPE_H

#include "interface/io.h"
#include <mutex>

namespace laplace::network {
  class pipe_node : public io_interface::node {
  public:
    struct state {
      bool     available   = true;
      uint16_t remote_port = any_port;
      vbyte    buffer;
    };

    log_handler log = get_global_log();

    pipe_node(pipe_node const &)     = delete;
    pipe_node(pipe_node &&) noexcept = default;
    pipe_node &operator=(pipe_node const &) = delete;
    pipe_node &operator=(pipe_node &&) noexcept = default;

    explicit pipe_node(uint16_t port) noexcept;
    ~pipe_node() noexcept override;

    [[nodiscard]] auto receive(span_byte buf) noexcept
        -> sl::whole override;

    [[nodiscard]] auto send(std::string_view, uint16_t port,
                            span_cbyte buf) noexcept
        -> sl::whole override;

    [[nodiscard]] auto get_port() const noexcept -> uint16_t override;

    [[nodiscard]] auto get_remote_address() const noexcept
        -> std::string override;

    [[nodiscard]] auto get_remote_port() const noexcept
        -> uint16_t override;

    [[nodiscard]] auto is_size_error() const noexcept
        -> bool override;

    [[nodiscard]] auto is_connection_reset() const noexcept
        -> bool override;

  private:
    uint16_t m_port;
  };

  class pipe : public io_interface {
  public:
    friend class pipe_node;

    pipe(pipe const &) noexcept = default;
    pipe(pipe &&) noexcept      = default;
    pipe &operator=(pipe const &) noexcept = default;
    pipe &operator=(pipe &&) noexcept = default;

    pipe() noexcept           = default;
    ~pipe() noexcept override = default;

    [[nodiscard]] auto open(uint16_t port) noexcept
        -> std::unique_ptr<node> override;

    [[nodiscard]] static auto remote_port_of(
        uint16_t port, log_handler log = get_global_log()) noexcept
        -> uint16_t &;
    [[nodiscard]] static auto buffer_of(
        uint16_t port, log_handler log = get_global_log()) noexcept
        -> vbyte &;

  private:
    [[nodiscard]] static auto add(uint16_t port) noexcept -> uint16_t;
    static void               free(uint16_t port) noexcept;
    [[nodiscard]] static auto receive(uint16_t  port,
                                      span_byte buf) noexcept
        -> sl::whole;
    [[nodiscard]] static auto send(uint16_t   from_port,
                                   uint16_t   to_port,
                                   span_cbyte buf) noexcept
        -> sl::whole;

    static std::mutex                   m_sync;
    static sl::vector<pipe_node::state> m_nodes;
  };
}

#endif
