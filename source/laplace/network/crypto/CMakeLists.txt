target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      nc_basic_cipher.cpp nc_ecc.cpp nc_ecc_rabbit.cpp
      nc_rabbit.cpp nc_stream_cipher.cpp
    PUBLIC
      basic_cipher.h ecc.h ecc_rabbit.h prime.h rabbit.h
      stream_cipher.h
)
