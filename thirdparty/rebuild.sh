#!  /bin/sh
#   rebuild.sh
#
#       Rebuild required third-party repos.
#
#   Copyright (c) 2021 Mitya Selivanov
#
#   This file is part of the Laplace project.
#
#   Laplace is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty
#   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
#   the MIT License for more details.

rebuild_lib() {
  cd $1
  if [ -d $2 ]; then
    rm -rf $2
  fi
  cmake $4 -B$2 -H.
  cmake --build $2 --config $3
  cd ..
}

gen=""
cfg="Release"

if [ $# -ge 1 ]; then
  gen=$1
  if [ $# -eq 2 ]; then
    cfg=$2
  fi
fi

ft_license="freetype/docs/LICENSE.TXT"

if [ ! -a $ft_license ]; then
  cp freetype/LICENSE.TXT $ft_license
else
  ft_license=""
fi

rebuild_lib "freetype" "build-cmake" "$cfg" "$gen \
  -D SKIP_INSTALL_ALL=ON
  -D CMAKE_DISABLE_FIND_PACKAGE_ZLIB=ON \
  -D CMAKE_DISABLE_FIND_PACKAGE_BZip2=ON \
  -D CMAKE_DISABLE_FIND_PACKAGE_PNG=ON \
  -D CMAKE_DISABLE_FIND_PACKAGE_HarfBuzz=ON \
  -D CMAKE_DISABLE_FIND_PACKAGE_BrotliDec=ON"

if [ -n $ft_license ]; then
  rm $ft_license
fi

rebuild_lib "bzip2" "build-cmake" "$cfg" "$gen"

rebuild_lib "wolfssl" "build-cmake" "$cfg" "$gen \
  -D WOLFSSL_RABBIT=ON \
  -D WOLFSSL_ECC=ON"

rebuild_lib "googletest" "build-cmake" "$cfg" "$gen"

rebuild_lib "benchmark" "build-cmake" "$cfg" "$gen \
  -D BENCHMARK_ENABLE_TESTING=OFF"
