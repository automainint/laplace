#!  /bin/sh
#   tools/buildall.py
#
#       Rebuild the entire project with
#       all the dependencies.
#
#   Copyright (c) 2021 Mitya Selivanov
#
#   This file is part of the Laplace project.
#
#   Laplace is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty
#   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
#   the MIT License for more details.

generator=''
config='Release'

if [ $# -eq 1 ]; then
  config=$1
elif [ $# -eq 2 ]; then
  generator="$1"
  config=$2
fi

./cleanup.sh

if [ ! -f './update-deps.sh' ] || [ ! -f './rebuild-deps.sh' ]
  then
    echo "[ Generate build scripts ]"
    python ./gen-deps.py

    chmod a+x ./update-deps.sh
    chmod a+x ./rebuild-deps.sh
  fi

./update-deps.sh
./update-gl.sh
./rebuild-deps.sh "$generator" $config

echo "[ Generate OpenGL interface ]"
python ./gen-gl.py

echo "[ Generate embedded data code ]"
python ./embed.py

echo '[ Build Laplace ]'

if [ ! -f '../build' ]; then
  mkdir ../build
fi

cd ../build

if [ -n "$generator" ]; then
  cmake \
    -G "$generator" \
    -D CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded \
    -D CMAKE_BUILD_TYPE=$config \
    ..
else
  cmake \
    -D CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded \
    -D CMAKE_BUILD_TYPE=$config \
    ..
fi

cmake \
  --build . --config $config

cd ../tools/
