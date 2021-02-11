#!  /bin/sh
#   gen-source.py
#
#       Generate source files for OpenGL
#       interface and embedded data.

./update-gl.sh

echo "[ Generate OpenGL interface ]"
python ./gen-gl.py

echo "[ Generate embedded data code ]"
python ./embed.py
