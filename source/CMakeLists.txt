add_subdirectory(laplace)
add_subdirectory(codegen)

if(LAPLACE_ENABLE_TESTING)
  add_subdirectory(test)
endif()
