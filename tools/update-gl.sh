#!  /bin/sh
#   update-gl.sh
#
#       Getting OpenGL specification XML.

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
