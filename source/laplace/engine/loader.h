/*  laplace/engine/loader.h
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

#ifndef laplace_engine_loader_h
#define laplace_engine_loader_h

#include "basic_impact.h"

namespace laplace::engine {
  /*  Queued background loading.
   */
  class loader {
  public:
    loader();
    ~loader();

    void set_world(ptr_world w);

    void add_task(ptr_impact task);

    /*  Check if all tasks done.
     */
    [[nodiscard]] auto is_ready() -> bool;

    /*  Get done task count.
     */
    [[nodiscard]] auto get_progress() -> size_t;

  private:
    void set_ready(bool is_ready);
    void done();

    std::mutex              m_lock;
    std::condition_variable m_sync;
    std::jthread            m_thread;

    bool        m_is_ready = true;
    bool        m_is_done  = false;
    size_t      m_progress = 0;
    ptr_world   m_world;
    vptr_impact m_queue;
  };
}

#endif
