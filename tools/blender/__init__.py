bl_info = {
    "name" : "PlainTeq Model",
    "author" : "Mitya Selivanov",
    "description" : "PlainTeq model file format import and export features.",
    "blender" : (2, 80, 0),
    "version" : (0, 0, 1),
    "location" : "File > Import-Export",
    "warning" : "",
    "category" : "Import-Export"
}

import bpy
from .plainteq.load import load
from .plainteq.save import save

def menu_func_import(self, context):
    self.layout.operator_context = 'INVOKE_DEFAULT'
    self.layout.operator(load.bl_idname, text="PlainTeq Model (.teq)")

def menu_func_export(self, context):
    self.layout.operator_context = 'INVOKE_DEFAULT'
    self.layout.operator(save.bl_idname, text="PlainTeq Model (.teq)")

def register():
    bpy.utils.register_class(load)
    bpy.utils.register_class(save)

    bpy.types.TOPBAR_MT_file_import.append(menu_func_import)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)

def unregister():
    bpy.utils.unregister_class(load)
    bpy.utils.unregister_class(save)

    bpy.types.TOPBAR_MT_file_import.remove(menu_func_import)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)

if __name__ == "__main__":
    register()
