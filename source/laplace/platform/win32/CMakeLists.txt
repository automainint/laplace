target_sources(
  ${LAPLACE_EXE}
    PRIVATE
      win32_audio.cpp win32_glcontext.cpp win32_input.cpp
      win32_thread.cpp win32_wgl.cpp win32_window.cpp
    PUBLIC
      audio.h clipboard.h glcontext.h input.h thread.h wgl.h
      win.predef.h window.h
)
