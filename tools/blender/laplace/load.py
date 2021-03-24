import bpy
from .model import model
from .family import family

class load(bpy.types.Operator):
    """Laplace import operator."""

    bl_idname = "import.laplace"
    bl_label = "Import Laplace VEC"

    filepath: bpy.props.StringProperty(subtype = "FILE_PATH")

    def execute(self, context):
        package = family.read_any(filepath)
        data = model.from_family(package)

        data.setup(context)

        return {'FINISHED'}

    def invoke(self, context, event):
        context.window_manager.fileselect_add(self)
        return {'RUNNING_MODAL'}
