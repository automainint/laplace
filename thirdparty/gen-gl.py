#!  /usr/bin/python
#   gen-gl.py
#
#       Generate OpenGL interface files
#       from XML registry.

import xml.etree.ElementTree
import os, sys

def adjust_typedef(text):
  if text != None:
    s = text
    if s[0:8] == 'typedef ':
      s = s[8:]
    if s[len(s)-1] == ' ':
      s = s[0:len(s)-1];
    return s
  return ''
  
def adjust_khronos(text):
  if text != None:
    if text == 'khronos_int8_t':
      return 'int8_t'
    if text == 'khronos_int16_t':
      return 'int16_t'
    if text == 'khronos_int32_t':
      return 'int32_t'
    if text == 'khronos_int64_t':
      return 'int64_t'
    if text == 'khronos_uint8_t':
      return 'uint8_t'
    if text == 'khronos_uint16_t':
      return 'uint16_t'
    if text == 'khronos_uint32_t':
      return 'uint32_t'
    if text == 'khronos_uint64_t':
      return 'uint64_t'
    if text == 'khronos_intptr_t':
      return 'ptrdiff_t'
    if text == 'khronos_uintptr_t':
      return 'size_t'
    if text == 'khronos_ssize_t':
      return 'ptrdiff_t'
    if text == 'khronos_usize_t':
      return 'size_t'
    if text == 'khronos_float_t':
      return 'float'
    return text
  return ''

def adjust_type(text):
  if text:
    if text == 'unsigned int':
      return 'uint32_t'
    if text == 'unsigned short':
      return 'uint16_t'
    if text == 'unsigned char':
      return 'uint8_t'
    if text == 'int':
      return 'int32_t'
    return text
  return ''
 
def adjust_arg(arg):
  c = arg[len(arg)-1]
  if c != ' ' and c != '*':
    return arg + ' '
  return arg

def args_str(args):
  s = ''
  for arg in args:
    if len(s) > 0:
      s += ', '
    s += adjust_arg(arg[0]) + arg[1]
  return '(' + s + ')'

gl_xml = xml.etree.ElementTree.parse('gl.xml')
root = gl_xml.getroot()

types = list()
enums = list()
funcs = list()
extensions = list()

types.append('using GLhandleARB = uint32_t;')

for child in root:
  if child.tag == 'types':
    for type in child:
      for name in type:
        decl = adjust_typedef(type.text)
        if len(decl) > 0 and name.text != None:
          decl = adjust_khronos(decl)
          decl = adjust_type(decl)
          if type.find('apientry') != None:
            decl += 'LAPLACE_GL_API *'
          types.append('using ' + name.text + ' = ' + decl + name.tail)
  elif child.tag == 'enums':
    for en in child:
      at = en.attrib
      if 'value' in at and 'name' in at:
        name = at['name']
        enums.append([ name, at['value'].lower() ])
  elif child.tag == 'commands':
    for cmd in child:
      proto = cmd.find('proto')
      proto_full = ''      
      if proto.text != None:
        proto_full = proto.text
      ptype = proto.find('ptype')
      if ptype != None:
        proto_full = proto_full + ptype.text + ptype.tail
      name = proto.find('name').text
      args = list()
      for par in cmd:
        if par.tag == 'param':
          atype = par.find('ptype')
          aname = par.find('name')
          type_full = ''
          if par.text != None:
            type_full += par.text
          if atype != None:
            type_full += atype.text;
            if atype.tail != None:
              type_full += atype.tail
          if aname != None:
            args.append([ type_full, aname.text ])
          else:
            args.append([ type_full, '' ])
      funcs.append([ name, proto_full, args, 0 ])

for child in root:
  if child.tag == 'extensions':
    for ex in child.findall('extension'):
      name = ex.get('name')
      cmds = list()
      for req in ex.findall('require'):
        for cmd in req.findall('command'):
          fname = cmd.get('name')
          for i, f in enumerate(funcs):
            if f[0] == fname:
              funcs[i][3] += 1
              cmds.append(i)
              break
      extensions.append([ name, cmds ])

# glNamedBufferPageCommitmentARB
# glVertexWeighthNV
# glVertexWeighthvNV

enums_buf = enums
enums = list()
enums64 = list()
enumsSigned = list()

def is_64(x):
  value = int(x, 0)
  return value > 0xffffffff
  
def is_signed(x):
  value = int(x, 0)
  return value < 0

for i, en in enumerate(enums_buf):
  if en != None:
    last_en = en
    for j in range(i + 1, len(enums_buf)):
      if enums_buf[j] != None and enums_buf[j][0] == en[0]:
        last_en = enums_buf[j]
        enums_buf[j] = None
    if is_signed(last_en[1]):
      enumsSigned.append(last_en)
    elif is_64(last_en[1]):
      enums64.append(last_en)
    else:
      enums.append(last_en)

folder = '../source/generated/gl/'
os.makedirs(folder, exist_ok=True)

previous_stdout = sys.stdout

sys.stdout = open(folder + 'types.h', 'w')

print('/*  Generated with the Laplace OpenGL interface tool.\n */\n')
print('#include <cstdint>\n')
print('#define LAPLACE_GL_API __stdcall\n')
print('namespace laplace::gl {')
for type in types:
  print('  ' + type)
print('}')

sys.stdout = open(folder + 'enums.h', 'w')

print('/*  Generated with the Laplace OpenGL interface tool.\n */\n')
print('#include "types.h"\n')
print('namespace laplace::gl {')

comma = ','
print('  enum __glenum : GLenum {')
for i, en in enumerate(enums):
  if i + 1 == len(enums):
    comma = ''
  print('    ' + en[0] + ' = ' + en[1] + comma)
print('  };')

comma = ','
print('\n  enum __glenum64 : uint64_t {')
for i, en in enumerate(enums64):
  if i + 1 == len(enums64):
    comma = ''
  print('    ' + en[0] + ' = ' + en[1] + comma)
print('  };')

comma = ','
print('\n  enum __glenum_int32 : int32_t {')
for i, en in enumerate(enumsSigned):
  if i + 1 == len(enumsSigned):
    comma = ''
  print('    ' + en[0] + ' = ' + en[1] + comma)
print('  };')

print('}')

sys.stdout = open(folder + 'decls.h', 'w')

print('/*  Generated with the Laplace OpenGL interface tool.\n */\n')
print('#include "types.h"\n')
print('namespace laplace::gl {')

for f in funcs:
  print('  using pfn_' + f[0] + ' = ' + adjust_arg(f[1]) + '(LAPLACE_GL_API *)' + args_str(f[2]) + ';')

print('')

for f in funcs:
  print('  extern pfn_' + f[0] + ' ' + f[0] + ';')

print('}')

sys.stdout = open(folder + 'funcs.impl.h', 'w')

print('/*  Generated with the Laplace OpenGL interface tool.\n */\n')
print('#include "decls.h"\n')
print('namespace laplace::gl {')

for f in funcs:
  print('  pfn_' + f[0] + ' ' + f[0] + ' = nullptr;')

print('}')

sys.stdout = open(folder + 'loads.impl.h', 'w')

print('/*  Generated with the Laplace OpenGL interface tool.\n */\n')
print('#ifndef LAPLACE_GL_HAS\n#  define LAPLACE_GL_HAS(x) false\n#endif\n')
print('#ifndef LAPLACE_GL_LOAD\n#  define LAPLACE_GL_LOAD(x) { }\n#endif\n')
print('#ifndef LAPLACE_GL_LOAD_EX\n#  define LAPLACE_GL_LOAD(x) { }\n#endif\n')
print('#ifndef LAPLACE_GL_BEGIN_EX\n#  define LAPLACE_GL_BEGIN_EX() { }\n#endif\n')
print('#ifndef LAPLACE_GL_END_EX\n#  define LAPLACE_GL_END_EX(x) { }\n#endif\n')

for f in funcs:
  if f[3] == 0:
    print('LAPLACE_GL_LOAD(' + f[0] + ');')

for ex in extensions:
  ex_fns = list()
  for n in ex[1]:
    if funcs[n][3] == 1:
      ex_fns.append(funcs[n][0])
  if len(ex_fns) > 0:
    print('\nif (LAPLACE_GL_HAS(' + ex[0] + ')) {')
    print('  LAPLACE_GL_BEGIN_EX();')
    for f in ex_fns:
      print('  LAPLACE_GL_LOAD_EX(' + f + ');')
    print('  LAPLACE_GL_END_EX(' + ex[0] + ')')
    print('}')

# Custom dictionary utility
# with list-keys.

def is_equals(a, b):
  """
    a, b - lists
    Check if two lists are aqual.
  """
  if len(a) != len(b):
    return False
  for i, x in enumerate(a):
    if x != b[i]:
      return False
  return True

def check_key(keys, key):
  """
    keys  - list or (list -> list) pairs
    key   - list

    Add new list key to dictionary if needed.
  """
  for i, k in enumerate(keys):
    if is_equals(k[0], key):
      return keys
  res = keys
  res.append([ key, list() ])
  return res
 
def append_to(keys, key, value):
  """
    keys  - list or (list -> list) pairs
    key   - list
    value - any data
    
    Append new value to dictionary element.
  """
  for i, k in enumerate(keys):
    if is_equals(k[0], key):
      res = keys
      res[i][1].append(value)
      return res
  return keys

ex2_funcs = list()

for i, f in enumerate(funcs):
  if f[3] > 1:
    ex_list = list()
    for ex in extensions:
      for n in ex[1]:
        if n == i:
          ex_list.append(ex[0])
    ex2_funcs = check_key(ex2_funcs, ex_list)
    ex2_funcs = append_to(ex2_funcs, ex_list, f[0])

for ex in ex2_funcs:
  reqs = ''
  for name in ex[0]:
    if len(reqs) > 0:
      reqs += ' && '
    reqs += 'LAPLACE_GL_HAS(' + name + ')'
  print('\nif (' + reqs + ') {')
  print('  LAPLACE_GL_BEGIN_EX();')
  for f in ex[1]:
    print('  LAPLACE_GL_LOAD_EX(' + f + ');')
  for name in ex[0]:
    print('  LAPLACE_GL_END_EX(' + name + ')')
  print('}')

sys.stdout = previous_stdout
