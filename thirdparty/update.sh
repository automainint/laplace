#!  /bin/sh
#   update.sh
#
#       Update required third-party repos.
#
#   Copyright (c) 2021 Mitya Selivanov
#
#   This file is part of the Laplace project.
#
#   Laplace is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty
#   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
#   the MIT License for more details.

update_repo() {
  if [ -d $1 ]; then
    cd $1
    git pull
    cd ..
  else
    git clone $2
  fi
}

update_repo "freetype" "https://gitlab.freedesktop.org/freetype/freetype.git"
update_repo "bzip2" "https://gitlab.com/federicomenaquintero/bzip2.git"
update_repo "wolfssl" "https://github.com/wolfSSL/wolfssl.git"
update_repo "googletest" "https://github.com/google/googletest.git"
update_repo "benchmark" "https://github.com/google/benchmark.git"

if [ ! -d "gl.xml" ]; then
  update_repo "OpenGL-Registry" "https://github.com/KhronosGroup/OpenGL-Registry.git"

  mv "OpenGL-Registry/xml/gl.xml" "gl.xml"
  rm -rf "OpenGL-Registry"
fi
