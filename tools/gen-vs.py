#!/usr/bin/python3

import shutil, uuid

guid_sln_project = '{' + str(uuid.uuid1()).upper() + '}'
guid_project     = '{' + str(uuid.uuid1()).upper() + '}'
guid_solution    = '{' + str(uuid.uuid1()).upper() + '}'

def process(path: str):
  content = open(path).read()

  return content
    .replace('{{GUID-SLN-PROJECT}}', guid_sln_project)
    .replace('{{GUID-PROJECT}}', guid_project)
    .replace('{{GUID-SOLUTION}}', guid_solution)

def write_to(content: str, path: str):
  open(path, 'w').write(content)

write_to('../laplace.sln', process('vs/laplace.sln.in'))
write_to('../laplace.vcxproj', process('vs/laplace.vcxproj.in'))
