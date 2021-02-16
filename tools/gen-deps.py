#!  /usr/bin/python
#   tools/gen-deps.py
#
#       Generate dependencies build scripts.
#
#   Copyright (c) 2021 Mitya Selivanov
#
#   This file is part of the Laplace project.
#
#   Laplace is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty
#   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
#   the MIT License for more details.

def do_escapes(s):
  buf=''
  esc=False
  for c in s:
    if not esc and c == '\\':
      esc=True
    else:
      buf += c
      esc=False
  return buf

def tokenize(lines):
  tokens = list()
  buf=''

  for line in lines:
    for word in line.split():
      if len(buf) > 0 and buf[len(buf)-1] == '\\':
        buf += ' '
        buf += word
      else:
        tokens.append(do_escapes(buf))
        buf = word

  if len(buf) > 0:
    tokens.append(do_escapes(buf))

  return tokens

def parse_deps(file):
  in_repo=False
  in_flags=False

  default = dict()
  default['url'] = ''
  default['folder'] = ''
  default['build-folder'] = ''
  default['include'] = ''
  default['flags'] = ''
  
  deps = list()

  toks = tokenize(open(file, 'r'))

  i = 0
  while i < len(toks):
    if toks[i] == 'repo' and toks[i+1] == '{':
      current = dict(default)
      in_repo = True
      i = i+2
    elif toks[i] == 'flags' and toks[i+1] == '{':
      in_flags = True
      i = i+2
    elif toks[i] == '}':
      if in_flags:
        in_flags = False
      elif in_repo:
        deps.append(current)
        in_repo = False
      i = i+1
    elif in_flags:
      if in_repo:
        current['flags'] += ' '
        current['flags'] += toks[i]
      else:
        default['flags'] += ' '
        default['flags'] += toks[i]
      i = i+1
    elif toks[i] == 'build-folder:':
      if in_repo:
        current['build-folder'] = toks[i+1]
      else:
        default['build-folder'] = toks[i+1]
      i = i+2
    elif toks[i] == 'include:':
      if in_repo:
        current['include'] = toks[i+1]
      else:
        default['include'] = toks[i+1]
      i = i+2
    elif toks[i] == 'url:':
      if in_repo:
        current['url'] = toks[i+1]
      else:
        default['url'] = toks[i+1]
      i = i+2
    elif toks[i] == 'folder:':
      if in_repo:
        current['folder'] = toks[i+1]
      else:
        default['folder'] = toks[i+1]
      i = i+2
    else:
      i = i+1

  return deps

def write_update(deps, file):
  out = open(file, 'w')

  out.write(
'''#  !/bin/sh
#  Generated with Laplace build script generation tool.

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

        if [ ! -f "../$inc_folder" ]; then
          mkdir "../$inc_folder"
        fi

        if [ -d "../$inc_folder" ]; then
          echo "[ Copy headers from $folder ]"
          cp -r `echo $headers` "../$inc_folder"
        fi

        cd ..
      fi
  fi
}

if [ ! -d '../thirdparty' ]; then
  mkdir ../thirdparty
fi

cd ../thirdparty

''')

  for dep in deps:
    out.write('url=\'' + dep['url'] + '\'\n')
    out.write('folder=\'' + dep['folder'] + '\'\n')
    out.write('headers=\'' + dep['include'] + '\'\n')
    out.write('update_repo\n\n')

  out.write('cd ../tools\n\n')

def write_rebuild(deps, file):
  out = open(file, 'w')

  out.write(
'''#  !/bin/sh
#  Generated with Laplace build script generation tool.

generator=''
config='Release'

lib_folder='../../lib/'

set_cmp0091() {
  python ../../tools/set-cmp0091.py
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
        cmake \\
          -G "$generator" \\
          $flags \\
          -D CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded \\
          -D CMAKE_BUILD_TYPE=$config \\
          -B"$build_to" -H. \\
          &> /dev/null
      else
        cmake \\
          $flags \\
          -D CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded \\
          -D CMAKE_BUILD_TYPE=$config \\
          -B"$build_to" -H. \\
          &> /dev/null
      fi

      cmake \\
        --build "$build_to" --config "$config" \\
        &> /dev/null

      if [ ! -f "$lib_folder" ]; then
        mkdir "$lib_folder"
      fi

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

if [ ! -d '../thirdparty' ]; then
  mkdir ../thirdparty
fi

cd ../thirdparty

ft_license='freetype/docs/LICENSE.TXT'
ft_license_copy='freetype/LICENSE.TXT'

if [ -d './freetype' ] && [ ! -f $ft_license ] && [ -f $ft_license_copy ]; then
  echo "[ Fix FreeType license file ]"
  cp $ft_license_copy $ft_license
else
  ft_license=''
fi

''')

  for dep in deps:
    if len(dep['build-folder']) > 0:
      out.write('url=\'' + dep['url'] + '\'\n')
      out.write('folder=\'' + dep['folder'] + '\'\n')
      out.write('build_to=\'' + dep['build-folder'] + '\'\n')
      out.write('flags=\'' + dep['flags'] + '\'\n')
      out.write('rebuild_lib\n\n')

  out.write(
'''
if [ -n "$ft_license" ]; then
  rm $ft_license
fi

cd ../tools

''')

deps = parse_deps('deps.txt')
write_update(deps, 'update.sh')
write_rebuild(deps, 'rebuild.sh')
