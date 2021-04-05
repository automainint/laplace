target_sources(
  ${LAPLACE_EXE}
    PRIVATE
      uie_button.cpp uie_canvas.cpp uie_console.cpp
      uie_panel.cpp uie_slider.cpp uie_sprite.cpp uie_textarea.cpp
      uie_textbutton.cpp uie_textedit.cpp uie_toggle.cpp
    PUBLIC
      button.h canvas.h console.h iconbutton.h panel.h
      slider.h sprite.h textarea.h textbutton.h textedit.h toggle.h
)
