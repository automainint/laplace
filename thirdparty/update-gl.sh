#!  /bin/sh
#   update-gl.sh
#
#       Getting OpenGL specification XML.

url='https://github.com/KhronosGroup/OpenGL-Registry.git'
folder='OpenGL-Registry'

if [ ! -f './gl.xml' ]
  then
    echo '[ Update OpenGL specification ]'

    if [ -d $folder ]
      then
        cd $folder
        git pull &> /dev/null
        cd ..
      else
        git clone $url $folder &> /dev/null
      fi

    cp "$folder/xml/gl.xml" './gl.xml'
  fi
