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

if [ `ls ./lib | wc -l` -eq 0 ] || [ `ls ./include | wc -l` -eq 0 ]; then
  ./buildall.sh "$generator" $config
else
  gen_folder='./source/generated'
  embed_source='laplace_embedded.cpp'

  if [ `ls $gen_folder/gl | wc -l` -le 1 ]
    then
      cd ./thirdparty
      ./update-gl.sh
      echo "[ Generate OpenGL interface ]"
      python ./gen-gl.py
      cd ..
    fi

  if [ ! -f "$gen_folder/$embed_source" ]
    then
      cd ./tools
      echo "[ Generate embedded data code ]"
      python ./embed.py
      cd ..
    fi

  echo '[ Build Laplace ]'

  rm -rf ./build/*
  touch ./bin/.placeholder

  cd ./build

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

  cd ..
fi
