target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      nc_ecc_rabbit.cpp nc_ecc.cpp nc_basic_cipher.cpp
      nc_stream_cipher.cpp nc_rabbit.cpp
    PUBLIC
      ecc.h rabbit.h stream_cipher.h basic_cipher.h
      ecc_rabbit.h)
