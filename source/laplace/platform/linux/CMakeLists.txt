target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      linux_input.cpp linux_glx.cpp linux_clipboard.cpp
      linux_audio.cpp linux_thread.cpp linux_glcontext.cpp linux_window.cpp
    PUBLIC
      audio.h glx.h thread.h input.h xlib.h clipboard.h
      glcontext.h window.h
)
