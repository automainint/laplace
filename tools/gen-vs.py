#!/usr/bin/python3

import shutil, uuid, os, glob

def get_sources(folder: str, prefix: str, ext: str):
  files = list()
  for f in glob.glob(os.path.join(folder, ext)):
    files.append(
      os.path.join(prefix, os.path.basename(f)))
  for d in glob.glob(os.path.join(folder, '*', '')):
    files += get_sources(
      d, os.path.join(
        prefix,
        os.path.basename(os.path.dirname(d))
      ), ext)
  return files

def gen_group(tag: str, files: list):
  s = ''
  for f in files:
    s += '\n    <' + tag + ' Include="' + f + '" />'
  if s:
    s += '\n  ';
  return s;

def main():
  headers_list = get_sources(os.path.join('..', 'source'), 'source', '*.h')
  sources_list = get_sources(os.path.join('..', 'source'), 'source', '*.cpp')

  includes = gen_group('ClInclude', headers_list)
  compiles = gen_group('ClCompile', sources_list)

  guid_sln_project = '{' + str(uuid.uuid1()).upper() + '}'
  guid_project     = '{' + str(uuid.uuid1()).upper() + '}'
  guid_solution    = '{' + str(uuid.uuid1()).upper() + '}'

  def process(path: str):
    content = open(path).read()

    return content.replace(
      '{{GUID-SLN-PROJECT}}', guid_sln_project).replace(
      '{{GUID-PROJECT}}', guid_project).replace(
      '{{GUID-SOLUTION}}', guid_solution).replace(
      '{{INCLUDE}}', includes).replace(
      '{{COMPILE}}', compiles)

  def write_to(path: str, content: str):
    dir = os.path.dirname(path)

    if not os.path.exists(dir):
      os.makedirs(dir)

  open(path, 'w').write(content)

  write_to(
    os.path.join('..', 'laplace.sln'),
    process(os.path.join('vs', 'laplace.sln.in')))

  write_to(
    os.path.join('..', 'laplace.vcxproj'),
    process(os.path.join('vs', 'laplace.vcxproj.in')))

if __name__ == '__main__':
  main()
