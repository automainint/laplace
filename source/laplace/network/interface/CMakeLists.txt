target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      ni_log.cpp ni_protocol.cpp ni_execution.cpp
    PUBLIC
      execution.h socket.h log.h protocol.h)
