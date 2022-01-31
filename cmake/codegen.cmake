if(LAPLACE_CODEGEN)
  find_package(Python3 REQUIRED)

  message(STATUS "Laplace code generation")
  execute_process(
    COMMAND ${Python3_EXECUTABLE} embed.py
    COMMAND ${Python3_EXECUTABLE} gen-gl.py
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tools
  )
  message(STATUS "Laplace code generation - done")
endif()
