target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      p_opengl.cpp p_socket.cpp
    PUBLIC
      dummy.h events.h gldef.h keys.h opengl.h socket.h
      thread.h wrap.h
)
if(WIN32)
  add_subdirectory(win32)
endif()
