#!  /bin/sh
#   update.sh
#
#       Update required third-party repos.

deps='deps.txt'

inc_folder='../include'

update_repo() {
  if [ -n "$url" ]; then
    echo "[ Update repo: $folder ]"

    if [ -d "$folder" ]
      then
        cd "$folder"
        git pull &> /dev/null
        cd ..
      else
        git clone $url "./$folder" &> /dev/null
      fi

    if [ -d "$folder" ]
      then
        cd "$folder"

        if [ -d "../$inc_folder" ]; then
          echo "[ Copy headers from $folder ]"
          cp -r `echo $headers` "../$inc_folder"
        fi

        cd ..
      fi
  fi
}

if [ -d "$inc_folder" ]; then
  rm -rf "$inc_folder/*"
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

next_repo

while read line; do
  if [ -n "$line" ]; then
    for arg in $line; do
      if [ "`echo $arg`" = '<repo>' ]
        then
          update_repo
          next_repo
        else
          next_arg $i `echo $arg`
          i=`expr $i + 1`
        fi
    done
  fi
done < $deps

update_repo
