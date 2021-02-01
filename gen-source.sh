#!  /bin/sh
#   gen-source.py
#
#       Generate source files for OpenGL
#       interface and embedded data.

cd ./thirdparty
./update-gl.sh
echo "[ Generate OpenGL interface ]"
python ./gen-gl.py
cd ..

cd ./tools
echo "[ Generate embedded data code ]"
python ./embed.py
cd ..
