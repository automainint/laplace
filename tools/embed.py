#!  /usr/bin/python3
#   embed.py
#
#       The Laplace embed files tool.
#
#   Copyright (c) 2021 Mitya Selivanov
#
#   This file is part of the Laplace project.
#
#   Laplace is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty
#   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
#   the MIT License for more details.

import os, sys

data_folder = '../data/'
folder = '../source/generated/'
os.makedirs(folder, exist_ok=True)

list_file = open(data_folder + 'embed.txt', 'r')

previous_stdout = sys.stdout
sys.stdout = open(folder + 'laplace_embedded.cpp', 'w')

print('/*  Generated with the Laplace embed files tool.\n */\n')
print('#include <string>')
print('#include <vector>')
print('#include <cstdint>\n')
print('using namespace std;\n')
print('namespace laplace::embedded')
print('{');

lines = list_file.readlines()
aliases = list()

for line in lines:
  s = line.lower().replace('\\', '/').replace('\n', '')
  if len(s) > 0:
    aliases.append(s)

print('    vector<wstring> aliases = {')

k = 0
for alias in aliases:
  s = '        L":/' + alias + '"'
  k += 1
  if k < len(aliases):
    s += ','
  print(s)

print('    };\n')
print('    vector<vector<uint8_t>> bytes = {')

k = 0
for alias in aliases:
  s = ''
  with open(data_folder + alias, 'r') as f:
    buf = bytes(f.read(), 'ascii')
    n = 0
    for x in buf:
      s += format(x, '#04x')
      n += 1
      if n < len(buf):
        s += ','
        if (n % 16) == 0:
          s += '\n            '
        else:
          s += ' '
  k += 1
  s = '        {   ' + s + ' }'
  if k < len(aliases):
    s += ','
  print(s)

print('    };')
print('}')

sys.stdout = previous_stdout
