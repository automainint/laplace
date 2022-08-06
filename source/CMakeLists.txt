add_subdirectory(laplace)

if(LAPLACE_ENABLE_TESTING)
  add_subdirectory(test)
endif()
