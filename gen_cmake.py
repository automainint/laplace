#!/usr/bin/python3

import os, glob

def get_subdirs(folder: str):
  dirs = list()
  for f in glob.glob(os.path.join(folder, '*', '')):
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
  srcs = get_files(folder, '*.c') + get_files(folder, '*.cpp')
  hdrs = get_files(folder, '*.h')
  if len(srcs) > 0 or len(hdrs) > 0:
    buf += 'target_sources(\n  ' + target_name
    if len(srcs) > 0:
      buf += '\n    PRIVATE\n' + print_list(srcs, 6)
    if len(hdrs) > 0:
      buf += '\n    PUBLIC'
    for f in hdrs:
      buf += '\n      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/' + f + '>'
    buf += ')\n'
  return buf

def print_subdirs(folder: str):
  buf = ''
  dirs = get_subdirs(folder)
  for f in dirs:
    if check_subdirs(os.path.join(folder, f)):
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
  gen_cmake(os.path.join('source', 'laplace'),            '${LAPLACE_LIBRARY}')
  gen_cmake(os.path.join('source', 'test', 'unittests'),  '${LAPLACE_TEST}')

if __name__ == '__main__':
  main()
