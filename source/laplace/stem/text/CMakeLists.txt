target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      st_font.cpp st_lcd.cpp st_lcd_data.cpp st_painter.cpp
      st_renderer.cpp st_ttf.cpp st_wrap.cpp
    PUBLIC
      font.h lcd.h painter.h renderer.h ttf.h wrap.h
)
