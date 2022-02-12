add_subdirectory(apps)
add_subdirectory(laplace)

if(LAPLACE_ENABLE_TESTING OR LAPLACE_ENABLE_COVERAGE)
  add_subdirectory(test)
endif()
