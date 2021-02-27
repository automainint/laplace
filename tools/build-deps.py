#!/usr/bin/python3

import os, sys, shutil, glob

def parse_argv():
  args = dict()
  args['generator'] = ''
  args['config'] = 'Release'

  argv = sys.argv

  for i, arg in enumerate(argv):
    if arg == '--generator':
      args['generator'] = argv[i+1]
    elif arg == '--config':
      args['config'] = argv[i+1]

  return args

def do_escapes(s):
  buf=''
  esc=False
  for c in s:
    if not esc and c == '\\':
      esc=True
    else:
      buf += c
      esc=False
  return buf

def tokenize(lines):
  tokens = list()
  buf=''

  for line in lines:
    for word in line.split():
      if len(buf) > 0 and buf[len(buf)-1] == '\\':
        buf += ' '
        buf += word
      else:
        tokens.append(do_escapes(buf))
        buf = word

  if len(buf) > 0:
    tokens.append(do_escapes(buf))

  return tokens

def parse_deps(file):
  in_repo=False
  in_flags=False

  default = dict()
  default['url'] = ''
  default['folder'] = ''
  default['build-folder'] = ''
  default['flags'] = ''
  
  deps = list()

  toks = tokenize(open(file, 'r'))

  i = 0
  while i < len(toks):
    if toks[i] == 'repo' and toks[i+1] == '{':
      current = dict(default)
      in_repo = True
      i = i+2
    elif toks[i] == 'flags' and toks[i+1] == '{':
      in_flags = True
      i = i+2
    elif toks[i] == '}':
      if in_flags:
        in_flags = False
      elif in_repo:
        deps.append(current)
        in_repo = False
      i = i+1
    elif in_flags:
      if in_repo:
        current['flags'] += ' '
        current['flags'] += toks[i]
      else:
        default['flags'] += ' '
        default['flags'] += toks[i]
      i = i+1
    elif toks[i] == 'build-folder:':
      if in_repo:
        current['build-folder'] = toks[i+1]
      else:
        default['build-folder'] = toks[i+1]
      i = i+2
    elif toks[i] == 'url:':
      if in_repo:
        current['url'] = toks[i+1]
      else:
        default['url'] = toks[i+1]
      i = i+2
    elif toks[i] == 'folder:':
      if in_repo:
        current['folder'] = toks[i+1]
      else:
        default['folder'] = toks[i+1]
      i = i+2
    else:
      i = i+1

  return deps

def set_cmp0091():
  os.rename('CMakeLists.txt', 'CMakeLists.txt.back')

  prev = open('CMakeLists.txt.back', 'r')
  curr = open('CMakeLists.txt', 'w')

  for line in prev.readlines():
    curr.write(line)
    if 'cmake_minimum_required' in line:
      curr.write('\nif(POLICY CMP0091)\n')
      curr.write('  cmake_policy(SET CMP0091 NEW)\n')
      curr.write('endif()\n')

def freetype_license_fix():
  src = 'LICENSE.TXT'
  dst = os.path.join('docs', 'LICENSE.TXT')
  if not os.path.exists(dst):
    print('[ Fix FreeType license file ]', flush=True)
    shutil.copy(src, dst)

def freetype_include_fix():
  inc_folder = os.path.join('..', 'include')
  ft2_folder = os.path.join(inc_folder, 'freetype2')
  if os.path.exists(ft2_folder):
    for f in glob.glob(os.path.join(inc_folder, 'freetype2', '*')):
      dst = os.path.join(inc_folder, os.path.basename(f))
      print(f + ' -> ' + dst, flush=True)
      shutil.move(f, dst)
    shutil.rmtree(ft2_folder)

def build_repo(build_to, generator, config, flags):
  if not os.path.exists(build_to):
    os.mkdir(build_to)
  gen = ''
  if len(generator) > 0:
    gen = ' -G "' + generator + '"'

  os.system(
    'cmake' + gen + flags +
    ' -D CMAKE_INSTALL_PREFIX=../../tmp' +
    ' -D CMAKE_INSTALL_LIBDIR=../lib' +
    ' -D CMAKE_INSTALL_INCLUDEDIR=../include' +
    ' -D CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded' +
    ' -D CMAKE_BUILD_TYPE=' + config +
    ' -B ' + build_to + 
    ' -S .')

  os.system(
    'cmake '
    ' --build ' + build_to +
    ' --config ' + config)

  os.system(
    'cmake' +
    ' --install ' + build_to +
    ' --config ' + config)

  shutil.rmtree('../../tmp')

args = parse_argv()
deps = parse_deps('deps.txt')

thirdparty = os.path.join('..', 'thirdparty')

if not os.path.exists(thirdparty):
  os.mkdir(thirdparty)

os.chdir(thirdparty)

for dep in deps:
  folder = dep['folder']
  url = dep["url"]

  if os.path.exists(folder):
    print('[ Pull ' + folder + ' ]')
    print('', flush=True)

    os.chdir(folder)
    os.system('git pull')

    print('')
  else:
    print('[ Clone ' + url + ' ]');
    print('', flush=True)

    os.system('git clone ' + url + ' ' + folder)
    os.chdir(folder)

    set_cmp0091()
    if folder == 'freetype':
      freetype_license_fix()

    print('')

  print('[ Build ' + folder + ' ]')
  if len(dep['flags']) > 0:
    print('  flags: ' + dep['flags'])
  print('', flush=True)
  
  build_repo(
    dep['build-folder'],
    args['generator'],
    args['config'],
    dep['flags'])
  print('')

  os.chdir('..')

  if folder == 'freetype':
    freetype_include_fix()
