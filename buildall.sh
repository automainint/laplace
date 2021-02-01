#!  /bin/sh
#   buildall.py
#
#       Rebuild the entire project with
#       all the dependencies.

generator=''
config='Release'

if [ $# -eq 1 ]; then
  config=$1
elif [ $# -eq 2 ]; then
  generator="$1"
  config=$2
fi

./cleanup.sh

cd ./thirdparty
./update.sh
./update-gl.sh
./rebuild.sh "$generator" $config
echo "[ Generate OpenGL interface ]"
python ./gen-gl.py
cd ..

cd ./tools
echo "[ Generate embedded data code ]"
python ./embed.py
cd ..

echo '[ Build Lapalce ]'

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
