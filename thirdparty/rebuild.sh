#!  /bin/sh
#   rebuild.sh
#
#       Rebuild required third-party repos.

deps='deps.txt'

generator=''
config='Release'

lib_folder='../../lib/'

set_cmp0091() {
  python ../set-cmp0091.py
}

set_cmp0091_back() {
  mv -f ./CMakeLists.txt.back ./CMakeLists.txt
}

rebuild_lib() {
  if [ -d "$folder" ]
    then
      echo "[ Build repo: $folder ]"

      cd "$folder"

      if [ -d $build_to ]; then
        rm -rf $build_to
      fi
      
      set_cmp0091

      if [ -n "$generator" ]; then
        cmake \
          -G "$generator" \
          $flags \
          -D CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded \
          -D CMAKE_BUILD_TYPE=$config \
          -B"$build_to" -H. \
          &> /dev/null
      else
        cmake \
          $flags \
          -D CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded \
          -D CMAKE_BUILD_TYPE=$config \
          -B"$build_to" -H. \
          &> /dev/null
      fi

      cmake \
        --build "$build_to" --config "$config" \
        &> /dev/null

      if [ -d "$lib_folder" ]; then
        echo "[ Copy libs from $folder ]"

        if [ -d "$build_to/$config" ]; then
          cp -f "$build_to/$config"/*.a "$lib_folder" 2> /dev/null
          cp -f "$build_to/$config"/*.lib "$lib_folder" 2> /dev/null
        elif [ -d "$build_to/lib/$config" ]; then
          cp -f "$build_to/lib/$config"/*.a "$lib_folder" 2> /dev/null
          cp -f "$build_to/lib/$config"/*.lib "$lib_folder" 2> /dev/null
        elif [ -d "$build_to/lib" ]; then
          cp -f "$build_to/lib"/*.a "$lib_folder" 2> /dev/null
          cp -f "$build_to/lib"/*.lib "$lib_folder" 2> /dev/null
        elif [ -d "$build_to/src/$config" ]; then
          cp -f "$build_to/src/$config"/*.a "$lib_folder" 2> /dev/null
          cp -f "$build_to/src/$config"/*.lib "$lib_folder" 2> /dev/null
        fi
      fi
      
      set_cmp0091_back
      
      cd ..
    fi
}

if [ $# -eq 1 ]; then
  config=$1
elif [ $# -eq 2 ]; then
  generator=$1
  config=$2
fi

next_repo() {
  url=''
  folder=''
  build_to=''
  headers=''
  flags=''
  i=0
}

next_arg() {
  case $1 in
    0) url=$2 ;;
    1) folder=$2 ;;
    2) build_to=$2 ;;
    3) headers=$2 ;;
    *) flags="$flags $2" ;;
  esac
}

ft_license='freetype/docs/LICENSE.TXT'
ft_license_copy='freetype/LICENSE.TXT'

if [ ! -f $ft_license ] && [ -f $ft_license_copy ]; then
  echo "[ Fix FreeType license file ]"
  cp $ft_license_copy $ft_license
else
  ft_license=''
fi

next_repo

while read line; do
  if [ -n "$line" ]; then
    for arg in $line; do
      if [ "`echo $arg`" = '<repo>' ]
        then
          rebuild_lib
          next_repo
        else
          next_arg $i `echo $arg`
          i=`expr $i + 1`
        fi
    done
  fi
done < $deps

rebuild_lib

if [ -n "$ft_license" ]; then
  rm $ft_license
fi
