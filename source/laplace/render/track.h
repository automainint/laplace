/*  laplace/render/track.h
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

#ifndef laplace_render_track_h
#define laplace_render_track_h

#include "../math/basic.h"
#include "../math/spline.h"
#include "defs.h"
#include <algorithm>
#include <vector>

namespace laplace::render {
  /*  Animation track.
   */
  template <typename type_>
  class track {
  public:
    using point       = math::vector<2, double>;
    using splineval   = math::spline<double, point>;
    using cref_spline = const splineval &;

    struct piece {
      splineval spline = {};
      timeval   time   = 0;
      type_     begin  = 0;
      type_     end    = 0;
    };

    using vpiece     = std::vector<piece>;
    using ref_piece  = piece &;
    using cref_piece = const piece &;

    static constexpr size_t solving_depth = 20;

    track();
    ~track();

    void clear();
    void add(cref_spline spline, timeval time, type_ begin, type_ end);
    void adjust();

    auto solve(timeval t) const -> type_;

    auto approximate(timeval delta) const -> track<type_>;

    void set_count(size_t count);
    auto get_count() const -> size_t;
    void set_piece(size_t index, cref_piece p);
    auto get_piece(size_t index) -> ref_piece;
    auto get_piece(size_t index) const -> cref_piece;

    auto operator()(timeval t) const -> type_;

    auto operator[](size_t index) -> ref_piece;
    auto operator[](size_t index) const -> cref_piece;

  private:
    auto op(cref_piece p, timeval t) -> bool;
    auto solve(size_t i, timeval time) const -> type_;

    bool    m_is_uniform;
    timeval m_delta;
    vpiece  m_pieces;
  };

  using real_track = track<real>;
  using vec2_track = track<vec2>;
  using vec3_track = track<vec3>;
  using vec4_track = track<vec4>;
  using quat_track = track<quat>;

  using ref_real_track = real_track &;
  using ref_vec2_track = vec2_track &;
  using ref_vec3_track = vec3_track &;
  using ref_vec4_track = vec4_track &;
  using ref_quat_track = quat_track &;

  using cref_real_track = const real_track &;
  using cref_vec2_track = const vec2_track &;
  using cref_vec3_track = const vec3_track &;
  using cref_vec4_track = const vec4_track &;
  using cref_quat_track = const quat_track &;
}

#include "track.impl.h"

#endif
