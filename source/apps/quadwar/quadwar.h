/*  apps/quadwar/quadwar.h
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

#ifndef __quadwar__quadwar__
#define __quadwar__quadwar__

#include "../../laplace/stem/app_flat.h"
#include "mainmenu.h"
#include "session.h"

namespace quadwar_app {
  using namespace laplace;

  static constexpr auto log_file_name = "quadwar.log";

  class quadwar : public stem::app_flat {
  public:
    static constexpr auto caption = u8"Quadwar";

    quadwar(int argc, char **argv);
    ~quadwar() override = default;

  protected:
    void init() override;
    void cleanup() override;
    void update(size_t delta_msec) override;
    void render() override;
    void set_frame_size(size_t width, size_t height) override;

  private:
    static auto get_config() -> core::family;

    ptr_mainmenu m_mainmenu;
    ptr_session  m_session;
  };
}

#endif
