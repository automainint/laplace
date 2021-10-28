#!/usr/bin/python3

import os

build_to = "build"
config = "Release"
msvc_runtime = "MultiThreaded"

os.system(
    'cmake' +
    ' -D CMAKE_BUILD_TYPE=' + config +
    ' -D CMAKE_MSVC_RUNTIME_LIBRARY=' + msvc_runtime +
    ' -D LAPLACE_ENABLE_TESTING=ON' +
    ' -B ' + build_to + 
    ' -S .')

os.system(
    'cmake '
    ' --build ' + build_to +
    ' --config ' + config)
