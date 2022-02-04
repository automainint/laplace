target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      win32_wgl.cpp win32_audio.cpp win32_thread.cpp
      win32_glcontext.cpp win32_window.cpp win32_input.cpp
    PUBLIC
      audio.h thread.h input.h win.predef.h clipboard.h wgl.h
      glcontext.h window.h)
