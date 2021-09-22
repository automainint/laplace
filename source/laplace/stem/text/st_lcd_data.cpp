/*  laplace/stem/text/st_lcd_data.cpp
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

#include "painter.h"

namespace laplace::stem::text {
  sl::index const painter::default_lcd_char_top    = 6;
  sl::index const painter::default_lcd_char_width  = 5;
  sl::index const painter::default_lcd_char_height = 7;

  uint64_t const painter::default_lcd_bits[] = {
    0xffe77bfe7fbfff3f, 0xfddd991ad73fdfff, 0xa9d0d73fdfffffc2,
    0xff3feffffdc2fddc, 0xefe3f8e6fdff77ba, 0x9dfefdfa8b787f3f,
    0xfdfd8d1affffee7f, 0x7fbfff3ff67f9ffe, 0xe739f7ffbfff7bfa,
    0xcfffffff398661bc, 0x7ce6d6bdbd9bdf76, 0xd9dfb1addf76b77f,
    0x2fabef76bee3bce6, 0xf776ddffdffe36ec, 0xcee3bce6f6edad0b,
    0x7ce739ee73bcc639, 0xffffffffffffff7f, 0x0108c631cffffdff,
    0x318e8b9d0a060c88, 0x63099a4f2ce738c6, 0x9c4f20e738ce3182,
    0x202420ce40026201, 0x38c6319e6095984f, 0x8191619c9a4b2c67,
    0x8b9c0a660c8f0108, 0xffffffffffffffff, 0xce738808c230ffff,
    0x89b6318cfef3dcc1, 0xb18cfd77deddd573, 0xfbb7beeebb5549bc,
    0xbef77b5549bbc190, 0x7506c9b6319cfff7, 0xe3b8b23cfff7befb,
    0xfcfffff77ec36eae, 0x07f37cffffffffff, 0xbeef7ffffffffff3,
    0xfdccf37cfb37ffff, 0x6d7b7aefce69beff, 0x621fb555baef7db7,
    0xb555bcef79b4611f, 0xbaef75cf7d6b5adf, 0x75f7730cc33fb555,
    0xffffffffcd559aef, 0xfffffffffff7ffcf, 0x07f3b9ffffffffff,
    0xbdc2d673abbcd738, 0xd673ab1f66d667f7, 0x6bbcf6d66377bdde,
    0xf63862afbee73955, 0x65b7bdfb76046bbb, 0xbdc396aee77cf6fe,
    0xfffffffffefe67f7, 0x7f3b9ff
  };
}
