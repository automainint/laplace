# FreeType2 include folder hotfix.
# Move "include/freetype2/*" to "include/".

install(CODE "set(forward_prefix_     ${CMAKE_INSTALL_PREFIX})")
install(CODE "set(forward_includedir_ ${CMAKE_INSTALL_INCLUDEDIR})")

install(CODE [[
  file(
    GLOB headers_
    LIST_DIRECTORIES true
    ${forward_prefix_}/${forward_includedir_}/freetype2/*)

  foreach(fullpath_ ${headers_})
    get_filename_component(filename_ ${fullpath_} NAME)

    file(
      RENAME
      ${forward_prefix_}/${forward_includedir_}/freetype2/${filename_}
      ${forward_prefix_}/${forward_includedir_}/${filename_})
  endforeach()

  file(REMOVE_RECURSE ${forward_prefix_}/${forward_includedir_}/freetype2)

  unset(headers_)
  unset(fullpath_)
  unset(filename_)
  
  unset(forward_prefix_)
  unset(forward_includedir_)
]])
