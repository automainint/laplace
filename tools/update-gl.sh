#!  /bin/sh
#   tools/update-gl.sh
#
#       Getting OpenGL specification XML.
#
#   Copyright (c) 2021 Mitya Selivanov
#
#   This file is part of the Laplace project.
#
#   Laplace is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty
#   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
#   the MIT License for more details.

url='https://github.com/KhronosGroup/OpenGL-Registry.git'
glxml='../thirdparty/gl.xml'
folder='../thirdparty/OpenGL-Registry'

if [ ! -d '../thirdparty' ]; then
  mkdir ../thirdparty
fi

if [ ! -f "$glxml" ]
  then
    echo '[ Update OpenGL specification ]'

    if [ -d "$folder" ]
      then
        cd $folder
        git pull &> /dev/null
        cd ../../tools
      else
        git clone $url $folder &> /dev/null
      fi

    cp "$folder/xml/gl.xml" "$glxml"
  fi
