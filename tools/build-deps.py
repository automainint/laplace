#!/usr/bin/python3

import os, shutil

def build_deps(
  folder: str,
  config: str,
  libs_suffix: str,
  architecture: str,
  generator: str,
  msvc_runtime: str,
  install_headers: bool
):
  libdir = '../lib/' + config + '-' + libs_suffix
  includedir = 'include'

  if install_headers:
    includedir = '../include'

  os.system(
      'cmake' +
      ' -D CMAKE_INSTALL_PREFIX=_tmp' +
      ' -D CMAKE_INSTALL_LIBDIR=' + libdir +
      ' -D CMAKE_INSTALL_INCLUDEDIR=' + includedir +
      ' -D CMAKE_BUILD_TYPE=' + config +
      ' -D CMAKE_MSVC_RUNTIME_LIBRARY=' + msvc_runtime +
      ' -D LAPLACE_ENABLE_EXE=OFF' +
      ' -G "' + generator + '"' +
      ' -A ' + architecture +
      ' -B ' + folder + 
      ' -S .')

  os.system(
      'cmake ' +
      ' --build ' + folder +
      ' --config ' + config)

  os.system(
      'cmake' +
      ' --install ' + folder +
      ' --config ' + config)

  if os.path.exists('_tmp'):
      shutil.rmtree('_tmp')

def main():
  os.chdir('..')

  build_deps('build-deps-debug-x86', 'Debug', 'x86', 'Win32', 'Visual Studio 16 2019', 'MultiThreadedDebug', False)
  build_deps('build-deps-debug-x64', 'Debug', 'x64', 'x64', 'Visual Studio 16 2019', 'MultiThreadedDebug', False)
  build_deps('build-deps-release-x86', 'Release', 'x86', 'Win32', 'Visual Studio 16 2019', 'MultiThreaded', False)
  build_deps('build-deps-release-x64', 'Release', 'x64', 'x64', 'Visual Studio 16 2019', 'MultiThreaded', True)

  os.chdir('tools')

if __name__ == '__main__':
  main()
