include(GNUInstallDirs)
include(cmake/hotfix-ft2.cmake)

string(TOLOWER ${CMAKE_SYSTEM_NAME} system_lower_)
string(TOLOWER ${PROJECT_NAME}      project_lower_)

if(LAPLACE_ENABLE_OBJ)
  target_include_directories(
    ${LAPLACE_OBJ}
      INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/source)
endif()

if(LAPLACE_ENABLE_OBJ OR LAPLACE_ENABLE_LIB)
  macro(laplace_add_headers folder)
    file(
      GLOB_RECURSE headers_
      RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/source/${folder}
      ${CMAKE_CURRENT_SOURCE_DIR}/source/${folder}/*.h)

    foreach(path_ ${headers_})
      get_filename_component(dir_ "${path_}" DIRECTORY)

      install(
        FILES ${CMAKE_CURRENT_SOURCE_DIR}/source/${folder}/${path_}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${folder}/${dir_}
        COMPONENT ${LAPLACE_COMPONENT_DEVELOPMENT})
    endforeach()

    unset(headers_)
    unset(path_)
    unset(dir_)
  endmacro()

  laplace_add_headers(laplace)
  laplace_add_headers(generated)

  install(
    TARGETS ${LAPLACE_OBJ}
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
      COMPONENT ${LAPLACE_COMPONENT_DEVELOPMENT})
endif()

if(LAPLACE_ENABLE_EXE)
  install(
    TARGETS ${QUADWAR_EXE}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
      COMPONENT ${QUADWAR_COMPONENT_RUNTIME})
endif()

if(LAPLACE_ENABLE_LIB)
  set_property(TARGET ${LAPLACE_LIB} PROPERTY VERSION ${PROJECT_VERSION})
  set_property(TARGET ${LAPLACE_LIB} PROPERTY SOVERSION ${PROJECT_VERSION_MAJOR})
  set_property(TARGET ${LAPLACE_LIB} PROPERTY INTERFACE_${LAPLACE_LIB}_MAJOR_VERSION ${PROJECT_VERSION_MAJOR})
  set_property(TARGET ${LAPLACE_LIB} APPEND PROPERTY COMPATIBLE_INTERFACE_STRING ${LAPLACE_LIB}_MAJOR_VERSION)

  export(
    EXPORT    ${LAPLACE_LIB}
    FILE      "${CMAKE_CURRENT_BINARY_DIR}/cmake/${LAPLACE_LIB_TARGETS}.cmake"
    NAMESPACE ${PROJECT_NAME}::)

  install(
    TARGETS ${LAPLACE_LIB}
    EXPORT  ${LAPLACE_LIB_TARGETS}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
      COMPONENT           ${QUADWAR_COMPONENT_RUNTIME}
      NAMELINK_COMPONENT  ${LAPLACE_COMPONENT_DEVELOPMENT}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
      COMPONENT           ${LAPLACE_COMPONENT_DEVELOPMENT})

  install(
    EXPORT      ${LAPLACE_LIB_TARGETS}
    FILE        ${LAPLACE_LIB_TARGETS}.cmake
    NAMESPACE   ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

  include(CMakePackageConfigHelpers)

  configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/${project_lower_}-config.cmake"
      INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${project_lower_}-config-version.cmake"
      VERSION "${PROJECT_VERSION}"
      COMPATIBILITY AnyNewerVersion)
  
  install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${project_lower_}-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${project_lower_}-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
endif()

set(
  CPACK_INSTALL_CMAKE_PROJECTS
  "${CMAKE_CURRENT_BINARY_DIR};${PROJECT_NAME};${QUADWAR_COMPONENT_RUNTIME};/")

set(CPACK_PACKAGE_NAME              ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR            ${LAPLACE_VENDOR})
set(CPACK_PACKAGE_VERSION_MAJOR     ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR     ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH     ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE     "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_FILE_NAME         "${project_lower_}-${system_lower_}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME  "${project_lower_}-${PROJECT_VERSION}")

include(CPack)

unset(project_lower_)
unset(system_lower_)
