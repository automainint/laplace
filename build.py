#!/usr/bin/python3

import os, sys

def main():
  build_to      = 'build'
  config        = 'Release'
  msvc_runtime  = 'MultiThreaded'

  arg_msvc = ''
  if sys.platform == 'win32':
    arg_msvc = ' -D CMAKE_MSVC_RUNTIME_LIBRARY=' + msvc_runtime

  os.system(
    'cmake' +
    ' -D CMAKE_BUILD_TYPE=' + config +
    arg_msvc +
    ' -D LAPLACE_ENABLE_TESTING=ON' +
    ' -B ' + build_to + 
    ' -S .')

  os.system(
    'cmake '
    ' --build ' + build_to +
    ' --config ' + config)

if __name__ == '__main__':
  main()
