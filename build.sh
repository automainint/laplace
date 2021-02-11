#!  /bin/sh
#   cleanup.py
#
#       Build the project.

generator=''
config='Release'

if [ $# -eq 1 ]; then
  config=$1
elif [ $# -eq 2 ]; then
  generator="$1"
  config=$2
fi

cd ./tools

if [ `ls ./lib | wc -l` -eq 0 ] || [ `ls ./include | wc -l` -eq 0 ]; then
  ./buildall.sh "$generator" $config
else
  gen_folder='./source/generated'
  embed_source='laplace_embedded.cpp'

  if [ ! -f './tools/update.sh' ] || [ ! -f './tools/rebuild.sh' ]
    then
      cd ./tools

      echo "[ Generate build scripts ]"
      python ./gen-deps.py

      cd ..
    fi

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
