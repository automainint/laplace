#!  /usr/bin/python
#   cmp0091.py
#
#       Add the CMP0091 policy line
#       to CMakeLists.txt file.
#
#       This is required for projects using
#       old CMake version to set MSVC runtime
#       library from command line.

import os, sys

os.rename('CMakeLists.txt', 'CMakeLists.txt.back')

prev = open('CMakeLists.txt.back', 'r')
curr = open('CMakeLists.txt', 'w')

for line in prev.readlines():
  curr.write(line)
  if 'cmake_minimum_required' in line:
    curr.write('cmake_policy(SET CMP0091 NEW)\n')
