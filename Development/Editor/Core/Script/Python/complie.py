# -*- coding: utf-8 -*-

import os
import subprocess
import util.msvc
import util.path
import util.filesystem as fs
path = util.path.path

def run_bat(bat_path, cmd=None):
    if cmd is None:
        cmd = str(bat_path)
    else:
        cmd = '"' + str(bat_path) + '" ' + cmd
        
    p = subprocess.Popen(cmd, cwd=str(bat_path.parent_path()), stdin=open(os.devnull), stderr=subprocess.PIPE, stdout=subprocess.PIPE, shell=True)
    for line in p.stdout.readlines():
        print(line)
        
def complie_opensource(configuration):
    util.msvc.rebuild(path['OpenSource']/ 'Lua' / 'Current' / 'makefiles' / 'luacore.sln', configuration)
    util.msvc.rebuild(path['OpenSource']/ 'LuaBind' / 'Current' / 'makefiles' / 'luabind.sln', configuration)
    run_bat(path['OpenSource']/ 'luaffi' / 'msvcbuild.bat', configuration.lower())

def complie(configuration):
    print('complie')
    print('msvc.setup_env')
    util.msvc.setup_env()
    print('rebuild opensource')
    complie_opensource(configuration)
    print('rebuild ydwe')
    util.msvc.rebuild(path['BuildRoot']/ 'Editor' / 'Core' / 'Solution' / 'YDWE.sln', configuration)    
    util.msvc.rebuild(path['BuildRoot']/ 'Editor' / 'Plugin' / 'YDColorizer' / 'YDColorizer.sln', configuration, 'Any CPU')
    util.msvc.rebuild(path['BuildRoot']/ 'Editor' / 'UI' / 'sln' / 'YDUI.sln', 'YDWE')
    util.msvc.rebuild(path['BuildRoot']/ 'Editor' / 'UI' / 'sln' / 'YDUI.sln', 'YDTrigger')

def Configuration():
    import sys
    if len(sys.argv) > 1:
        return sys.argv[1]
    return 'Debug'

if __name__ == '__main__':
    util.path.ResetPath(Configuration())
    complie(Configuration())
