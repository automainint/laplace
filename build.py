#!/usr/bin/python3

import os, sys

def parse_argv():
  args = dict()
  args['py'] = 'python3'
  args['generator'] = ''
  args['config'] = 'Release'

  argv = sys.argv

  for i, arg in enumerate(argv):
    if arg == '--generator':
      args['generator'] = argv[i+1]
    elif arg == '--config':
      args['config'] = argv[i+1]
    elif arg == '--use-python-alias':
      args['py'] = argv[i+1]

  return args

args = parse_argv()
cmd = ''
gen = ''
cfg = 'Release'

if len(args['generator']) > 0:
  gen = ' -G "' + args['generator'] + '"'
  cmd = cmd + ' --generator ' + args['generator']
if len(args['config']) > 0:
  cfg = args['config']
  cmd = cmd + ' --config ' + args['config']

os.chdir('tools')

os.system(args['py'] + ' build-deps.py' + cmd)
os.system(args['py'] + ' gen-gl.py')
os.system(args['py'] + ' embed.py')

os.chdir('..')

print('[ Build Laplace ]')
print('', flush=True)

if not os.path.exists('build'):
  os.mkdir('build')

os.system('cmake -B build -S .')
os.system('cmake --build build --config ' + cfg)

print('')
