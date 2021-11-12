#!/usr/bin/python3

import os, sys, shutil

def build_deps(
  folder: str,
  config: str,
  libs_folder: str,
  extra_flags: str,
  install_headers: bool
):
  libdir = '../lib/' + libs_folder
  includedir = 'include'

  if install_headers:
    includedir = '../include'

  os.system(
      'cmake' +
      ' -D LAPLACE_ENABLE_EXE=OFF' +
      ' -D CMAKE_INSTALL_PREFIX=_tmp' +
      ' -D CMAKE_INSTALL_LIBDIR=' + libdir +
      ' -D CMAKE_INSTALL_INCLUDEDIR=' + includedir +
      ' -D CMAKE_BUILD_TYPE=' + config +
      extra_flags +
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

def build_deps_vs(
  folder: str,
  config: str,
  libs_suffix: str,
  architecture: str,
  msvc_runtime: str,
  install_headers: bool
):
  build_deps_vs(
    folder, config, config + '-' + libs_suffix,
    ' -D CMAKE_MSVC_RUNTIME_LIBRARY=' + msvc_runtime +
    ' -G "Visual Studio 16 2019"' +
    ' -A ' + architecture,
    install_headers)

def main():
  os.chdir('..')

  if sys.platform == 'win32':
    build_deps_vs('build-deps-debug-x86', 'Debug', 'x86', 'Win32', 'MultiThreadedDebug', False)
    build_deps_vs('build-deps-debug-x64', 'Debug', 'x64', 'x64', 'MultiThreadedDebug', False)
    build_deps_vs('build-deps-release-x86', 'Release', 'x86', 'Win32', 'MultiThreaded', False)
    build_deps_vs('build-deps-release-x64', 'Release', 'x64', 'x64', 'MultiThreaded', True)
  else:
    build_deps('build-deps-debug', 'Debug', 'debug', '', False)
    build_deps('build-deps-release', 'Release', 'release', '', True)

  os.chdir('tools')

if __name__ == '__main__':
  main()
