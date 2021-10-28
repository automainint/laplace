target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      st_renderer.cpp st_ttf.cpp st_lcd_data.cpp st_wrap.cpp
      st_lcd.cpp st_painter.cpp st_font.cpp
    PUBLIC
      painter.h font.h wrap.h renderer.h ttf.h lcd.h
)
