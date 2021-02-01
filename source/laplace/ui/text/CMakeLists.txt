target_sources(
  ${EXE_NAME}
    PRIVATE
       uit_buffer.cpp uit_font.cpp uit_lcd.cpp uit_lcd_data.cpp
      uit_painter.cpp uit_renderer.cpp uit_ttf.cpp uit_wrap.cpp
    PUBLIC
       buffer.h font.h lcd.h painter.h renderer.h ttf.h wrap.h
)
