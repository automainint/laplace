target_sources(
  ${EXE_NAME}
    PRIVATE
      p_opengl.cpp p_socket.cpp
    PUBLIC
      dummy.h events.h gldef.h keys.h opengl.h socket.h
      thread.h wrap.h
)
add_subdirectory(win32)
