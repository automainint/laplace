/*  laplace/stem/app_flat.h
 *
 *      Base class for UI and 2D graphics application.
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

#ifndef laplace_stem_app_flat_h
#define laplace_stem_app_flat_h

#include "application.h"
#include "ui/frame.h"

namespace laplace::stem {
  class app_flat : public application {
  public:
    static constexpr graphics::vec4 clear_color = { .1f, .09f, .12f,
                                                    1.f };

    app_flat(int               argc,
             char **           argv,
             core::cref_family def_cfg) noexcept;

    ~app_flat() noexcept override = default;

  protected:
    void init() noexcept override;
    void cleanup() noexcept override;
    void update(sl::time delta_msec) noexcept override;
    void render() noexcept override;
    void adjust_layout(sl::whole width,
                       sl::whole height) noexcept override;

    ui::ptr_frame m_ui;
  };
}

#endif
