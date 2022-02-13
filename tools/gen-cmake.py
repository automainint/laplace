#!/usr/bin/python3

import os, glob

def get_subdirs(folder: str):
  dirs = list()
  for f in glob.glob(os.path.join(folder, '*', '')):
    dirs.append(os.path.basename(os.path.normpath(f)))
  for f in glob.glob(os.path.join(folder, '.*', '')):
    dirs.append(os.path.basename(os.path.normpath(f)))
  return dirs

def get_files(folder: str, ext: str):
  files = list()
  for f in glob.glob(os.path.join(folder, ext)):
    files.append(os.path.basename(f))
  return files

def check_subdirs(folder):
  for r, d, f in os.walk(folder):
    for file in f:
      return True
  return False

def print_list(s: list, offset: int):
  buf = ''
  char_count = offset
  for i in range(char_count - 1):
    buf += ' '
  for f in s:
    char_count += len(f) + 1
    if char_count >= 64:
      char_count = 0
    if char_count == 0:
      buf += '\n'
      for i in range(offset - 1):
        buf += ' '
      char_count = offset - 1
    buf += ' ' + f    
  return buf

def print_sources(folder: str, target_name: str):
  buf = ''
  srcs = get_files(folder, '*.cpp')
  hdrs = get_files(folder, '*.h')
  embedded = 'laplace_embedded.cpp'
  if embedded in srcs:
    buf += 'if(LAPLACE_ENABLE_EMBEDDED)\n'
    buf += '  target_sources(' + target_name
    buf += ' PRIVATE ' + embedded + ')\n'
    buf += 'endif()\n'
    srcs.remove(embedded)
  if len(srcs) > 0 or len(hdrs) > 0:
    buf += 'target_sources(\n  ' + target_name
    if len(srcs) > 0:
      buf += '\n    PRIVATE\n' + print_list(srcs, 6)
    if len(hdrs) > 0:
      buf += '\n    PUBLIC\n' + print_list(hdrs, 6)
    buf += ')\n'
  return buf

def print_subdirs(folder: str):
  buf = ''
  dir = os.path.basename(folder)
  dirs = get_subdirs(folder)
  for f in dirs:
    if check_subdirs(os.path.join(folder, f)):
      if dir == 'platform' and f == 'linux':
        buf += 'if(UNIX AND (NOT APPLE))\n'
        buf += '  add_subdirectory(' + f + ')\n'
        buf += 'endif()\n'
      elif dir == 'platform' and f == 'win32':
        buf += 'if(WIN32)\n'
        buf += '  add_subdirectory(' + f + ')\n'
        buf += 'endif()\n'
      elif dir == 'platform' and f == 'macos':
        buf += 'if(APPLE)\n'
        buf += '  add_subdirectory(' + f + ')\n'
        buf += 'endif()\n'
      elif dir == 'laplace' and f == 'core':
        buf += 'if(LAPLACE_ENABLE_CORE)\n'
        buf += '  add_subdirectory(' + f + ')\n'
        buf += 'endif()\n'
      elif dir == 'laplace' and f == 'coroutine':
        buf += 'if(LAPLACE_ENABLE_COROUTINE)\n'
        buf += '  add_subdirectory(' + f + ')\n'
        buf += 'endif()\n'
      elif dir == 'laplace' and f == 'math':
        buf += 'if(LAPLACE_ENABLE_MATH)\n'
        buf += '  add_subdirectory(' + f + ')\n'
        buf += 'endif()\n'
      elif dir == 'laplace' and f == 'engine':
        buf += 'if(LAPLACE_ENABLE_ENGINE)\n'
        buf += '  add_subdirectory(' + f + ')\n'
        buf += 'endif()\n'
      elif dir == 'laplace' and f == 'network':
        buf += 'if(LAPLACE_ENABLE_NETWORK)\n'
        buf += '  add_subdirectory(' + f + ')\n'
        buf += 'endif()\n'
      elif dir == 'laplace' and f == 'format':
        buf += 'if(LAPLACE_ENABLE_FORMAT)\n'
        buf += '  add_subdirectory(' + f + ')\n'
        buf += 'endif()\n'
      elif dir == 'laplace' and f == 'ui':
        buf += 'if(LAPLACE_ENABLE_UI)\n'
        buf += '  add_subdirectory(' + f + ')\n'
        buf += 'endif()\n'
      elif dir == 'laplace' and f == 'platform':
        buf += 'if(LAPLACE_ENABLE_PLATFORM)\n'
        buf += '  add_subdirectory(' + f + ')\n'
        buf += 'endif()\n'
      elif dir == 'laplace' and f == 'graphics':
        buf += 'if(LAPLACE_ENABLE_GRAPHICS)\n'
        buf += '  add_subdirectory(' + f + ')\n'
        buf += 'endif()\n'
      elif dir == 'laplace' and f == 'render':
        buf += 'if(LAPLACE_ENABLE_RENDER)\n'
        buf += '  add_subdirectory(' + f + ')\n'
        buf += 'endif()\n'
      elif dir == 'laplace' and f == 'stem':
        buf += 'if(LAPLACE_ENABLE_STEM)\n'
        buf += '  add_subdirectory(' + f + ')\n'
        buf += 'endif()\n'
      elif dir == '.generated' and f == 'gl':
        buf += 'if(LAPLACE_ENABLE_OPENGL)\n'
        buf += '  add_subdirectory(' + f + ')\n'
        buf += 'endif()\n'
      else:
        buf += 'add_subdirectory(' + f + ')\n'
  return buf

def write_subdirs(folder: str, target_name: str):
  if check_subdirs(folder):
    out = open(os.path.join(folder, 'CMakeLists.txt'), 'w')
    out.write(print_sources(folder, target_name))
    out.write(print_subdirs(folder))
    out.close()

    for dir in get_subdirs(folder):
      write_subdirs(os.path.join(folder, dir), target_name)

def clean_subdirs(folder: str):
  for r, d, f in os.walk(folder):
    for file in f:
      if file == 'CMakeLists.txt':
        os.remove(os.path.join(r, file))

def gen_cmake(folder: str, target_name: str):
  clean_subdirs(folder)
  write_subdirs(folder, target_name)

def main():
  src_dir = os.path.join('..', 'source')

  gen_cmake(os.path.join(src_dir, 'laplace'),         '${LAPLACE_OBJ}')
  gen_cmake(os.path.join(src_dir, 'test'),            '${LAPLACE_OBJ}')
  gen_cmake(os.path.join(src_dir, 'apps', 'quadwar'), '${QUADWAR_OBJ}')

if __name__ == '__main__':
  main()
