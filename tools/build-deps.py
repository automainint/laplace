#!/usr/bin/python3

import os, shutil

def build_deps(folder: str, config: str, msvc_runtime: str, install_headers: bool):
  build_folder = '_build-deps'

  if install_headers:
    os.system(
        'cmake' +
        ' -D CMAKE_INSTALL_PREFIX=_tmp' +
        ' -D CMAKE_INSTALL_LIBDIR=../lib/' + config +
        ' -D CMAKE_INSTALL_INCLUDEDIR=../include' +
        ' -D CMAKE_BUILD_TYPE=' + config +
        ' -D CMAKE_MSVC_RUNTIME_LIBRARY=' + msvc_runtime +
        ' -D LAPLACE_ENABLE_EXE=OFF' +
        ' -B ' + folder + 
        ' -S .')
  else:
    os.system(
        'cmake' +
        ' -D CMAKE_INSTALL_PREFIX=_tmp' +
        ' -D CMAKE_INSTALL_LIBDIR=../lib/' + config +
        ' -D CMAKE_BUILD_TYPE=' + config +
        ' -D CMAKE_MSVC_RUNTIME_LIBRARY=' + msvc_runtime +
        ' -D LAPLACE_ENABLE_EXE=OFF' +
        ' -B ' + folder + 
        ' -S .')

  os.system(
      'cmake '
      ' --build ' + folder +
      ' --config ' + config)

  os.system(
      'cmake' +
      ' --install ' + folder +
      ' --config ' + config)

  if os.path.exists('_tmp'):
      shutil.rmtree('_tmp')

os.chdir('..')

build_deps('build-deps-debug', 'Debug', 'MultiThreadedDebug', False)
build_deps('build-deps-release', 'Release', 'MultiThreaded', True)

os.chdir('tools')
