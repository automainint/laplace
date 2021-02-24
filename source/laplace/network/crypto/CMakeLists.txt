target_sources(
  ${EXE_NAME}
    PRIVATE
      nc_basic_cipher.cpp nc_deffiehellman.cpp
      nc_dh_rabbit.cpp nc_rabbit.cpp nc_stream_cipher.cpp
    PUBLIC
      basic_cipher.h deffiehellman.h dh_rabbit.h prime.h
      rabbit.h stream_cipher.h
)
