#!/usr/bin/python3

import os, glob

target_name = "${LAPLACE_EXE}"

def get_subdirs(folder):
  dirs = list()
  for f in glob.glob(os.path.join(folder, '*', '')):
    dirs.append(os.path.basename(os.path.normpath(f)))
  return dirs

def get_files(folder, ext):
  files = list()
  for f in glob.glob(os.path.join(folder, ext)):
    files.append(os.path.basename(f))
  return files

def check_subdirs(folder):
  for r, d, f in os.walk(folder):
    for file in f:
      return True
  return False

def print_list(s, offset):
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

def print_sources(folder):
  buf = ''
  srcs = get_files(folder, '*.cpp')
  hdrs = get_files(folder, '*.h')
  if len(srcs) > 0 or len(hdrs) > 0:
    buf += 'target_sources(\n  ' + target_name
    if len(srcs) > 0:
      buf += '\n    PRIVATE\n' + print_list(srcs, 6)
    if len(hdrs) > 0:
      buf += '\n    PUBLIC\n' + print_list(hdrs, 6)
    buf += '\n)\n'
  return buf

def print_subdirs(folder):
  buf = ''
  dirs = get_subdirs(folder)
  for f in dirs:
    if check_subdirs(os.path.join(folder, f)):
      if f == 'win32':
        buf += 'if(WIN32)\n'
        buf += '  add_subdirectory(' + f + ')\n'
        buf += 'endif()\n'
      else:
        buf += 'add_subdirectory(' + f + ')\n'
  return buf

def write_subdirs(folder):
  if check_subdirs(folder):
    out = open(os.path.join(folder, 'CMakeLists.txt'), 'w')
    out.write(print_sources(folder))
    out.write(print_subdirs(folder))
    out.close()

    for dir in get_subdirs(folder):
      write_subdirs(os.path.join(folder, dir))

def clean_subdirs(folder):
  for r, d, f in os.walk(folder):
    for file in f:
      if file == 'CMakeLists.txt':
        os.remove(os.path.join(r, file))

clean_subdirs(os.path.join('..', 'source'))
write_subdirs(os.path.join('..', 'source'))
