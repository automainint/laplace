#!/usr/bin/python3

import os, sys
from PIL import Image

folder = os.path.join('..', 'data')

file = os.path.join(folder, 'lcd.png')
img = Image.open(file)

width, height = img.size

s = ''
b = 0
n = 0

for y in range(0, height):
  for x in range(0, width):
    if n >= 64:
      if len(s) > 0:
        s += ', '
      s += format(b, '#018x')
      b = 0
      n = 0
    pix = img.getpixel((x, y))[0]
    b >>= 1
    if pix > 127:
      b |= 0x8000000000000000
    n += 1

if n > 0:
  b >>= (64 - n)
  if len(s) > 0:
    s += ', '
  s += format(b, '#0x')

previous_stdout = sys.stdout
sys.stdout = open(os.path.join(folder, 'lcd.h'), 'w')

print('size_t width = ' + str(width) + ';')
print('size_t height = ' + str(height) + ';')
print('uint64_t bits[ ] = { ' + s + ' };')

sys.stdout = previous_stdout
