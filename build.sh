#!  /bin/sh
#   build.sh
#
#       Build the project.
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

if [ ! -d ./lib ]; then
  mkdir lib
fi

if [ ! -d ./include ]; then
  mkdir include
fi

cd ./tools

if [ `ls ../lib | wc -l` -eq 0 ] || [ `ls ../include | wc -l` -eq 0 ]; then
  ./buildall.sh "$generator" $config
else
  gen_folder='../source/generated'
  embed_source='laplace_embedded.cpp'

  if [ `ls $gen_folder/gl | wc -l` -le 1 ]
    then
      ./update-gl.sh

      echo "[ Generate OpenGL interface ]"
      python ./gen-gl.py
    fi

  if [ ! -f "$gen_folder/$embed_source" ]
    then
      echo "[ Generate embedded data code ]"
      python ./embed.py
    fi

  echo '[ Build Laplace ]'

  if [ -f '../build' ]; then
    rm -rf ../build/*
  else
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
fi

cd ..
