target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      ni_protocol.cpp ni_execution.cpp
    PUBLIC
      execution.h socket.h protocol.h)
