# FreeType2 include folder hotfix.
# Move "include/freetype2/*" to "include/".

install(CODE "set(FORWARD_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})")
install(CODE "set(FORWARD_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR})")

install(CODE [[
  file(
    GLOB freetype2_headers
    LIST_DIRECTORIES true
    ${FORWARD_INSTALL_PREFIX}/${FORWARD_INSTALL_INCLUDEDIR}/freetype2/*
  )

  foreach(fullpath ${freetype2_headers})
    get_filename_component(filename ${fullpath} NAME)

    file(
      RENAME
      ${FORWARD_INSTALL_PREFIX}/${FORWARD_INSTALL_INCLUDEDIR}/freetype2/${filename}
      ${FORWARD_INSTALL_PREFIX}/${FORWARD_INSTALL_INCLUDEDIR}/${filename}
    )
  endforeach()

  file(REMOVE_RECURSE ${FORWARD_INSTALL_PREFIX}/${FORWARD_INSTALL_INCLUDEDIR}/freetype2)

  unset(freetype2_headers)
  unset(fullpath)
  unset(filename)
  
  unset(FORWARD_INSTALL_PREFIX)
  unset(FORWARD_INSTALL_INCLUDEDIR)
]])
