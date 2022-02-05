#!/usr/bin/python3

import os

def rejoin_alias(alias):
  result = ''
  for s in alias.split('/'):
    if len(result) > 0:
      result = os.path.join(result, s)
    else:
      result = s
  return result

def is_code(file_name):
  s = os.path.basename(file_name).split('.')
  n = len(s)
  ext = ''
  if n > 0:
    ext = s[n-1]
  if ext == 'geom':
    return True
  if ext == 'vert':
    return True
  if ext == 'frag':
    return True
  if ext == 'comp':
    return True
  if ext == 'glsl':
    return True
  return False

def get_include_file(line):
  n = 0
  while n < len(line) and line[n] == ' ':
    n += 1
  if line[n] != '#':
    return ''
  n += 1
  while n < len(line) and line[n] == ' ':
    n += 1
  if line[n:n+7] != 'include':
    return ''
  n += 7
  while n < len(line) and line[n] == ' ':
    n += 1
  if line[n] != '"':
    return ''
  n += 1
  n0 = n
  while n < len(line) and line[n] != '"':
    n += 1
  if line[n] != '"':
    return ''
  n1 = n
  n += 1
  while n < len(line) and line[n] == ' ':
    n += 1
  if n != len(line) and line[n] != '\n':
    print(line + ' // 5 n=' + str(n) + ' s=' + line[n:])
    return ''
  return line[n0:n1]

def process_file(file_name, incs = list()):
  if file_name in incs:
    print('Error: Recursive include in \'' + incs[len(incs)-1] + '\'.')
    return ''
  if is_code(file_name):
    f = open(file_name, 'rt', encoding='utf-8')
    chars = ''
    for line in f:
      inc_file = get_include_file(line)
      if len(inc_file) > 0:
        inc_path = os.path.normpath(
          os.path.join(os.path.dirname(file_name),
          inc_file))
        incs.append(inc_path)
        chars += process_file(inc_path, incs)
        chars += '\n'
      else:
        chars += line
        chars += '\n'
    return bytes(chars, 'ascii')
  else:
    f = open(file_name, 'rb')
    return f.read()

def main():
  data_folder = os.path.join('..', 'data')
  folder = os.path.join('..', 'source', 'laplace', 'generated')

  os.makedirs(folder, exist_ok=True)

  list_file = open(os.path.join(data_folder, 'embed.txt'), 'r')

  out = open(os.path.join(folder, 'laplace_embedded.cpp'), 'w')

  out.write('/*  Generated with the Laplace embed files tool.\n */\n\n')
  out.write('#include <cstdint>\n')
  out.write('#include <string>\n')
  out.write('#include <vector>\n\n')
  out.write('namespace laplace::embedded {\n')

  lines = list_file.readlines()
  aliases = list()

  for line in lines:
    s = line.lower().replace('\\', '/').replace('\n', '')
    if len(s) > 0:
      aliases.append(s)

  out.write('  std::vector<std::wstring> aliases = {\n')

  k = 0
  for alias in aliases:
    s = '    L":/' + alias + '"'
    k += 1
    if k < len(aliases):
      s += ','
    out.write(s + '\n')

  out.write('  };\n\n')
  out.write('  std::vector<std::vector<uint8_t>> bytes = {\n')

  k = 0
  for alias in aliases:
    s = ''
    file_name = os.path.join(data_folder, rejoin_alias(alias))
    buf = process_file(file_name)
    n = 0
    for x in buf:
      s += format(x, '#04x')
      n += 1
      if n < len(buf):
        s += ','
        if (n % 16) == 0:
          s += '\n        '
        else:
          s += ' '
    k += 1
    s = '    {   ' + s + ' }'
    if k < len(aliases):
      s += ','
    out.write(s + '\n')

  out.write('  };\n')
  out.write('}\n')

if __name__ == '__main__':
  main()
