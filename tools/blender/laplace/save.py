import bpy

class save(bpy.types.Operator):
    """Laplace export operator."""

    bl_idname = "export.laplace"
    bl_label = "Export Laplace VEC"

    export_format: bpy.props.EnumProperty(
        name = 'Format',
        items = ((
                'BINARY',
                'Binary',
                'Exports a single file, with all data packed in binary form.'
            ), (
                'TEXT',
                'Text',
                'Exports a single file, with all data packed in text form.'
            )
        ),
        description = 'Output format. Binary is most efficient.',
        default = 'BINARY'
    )

    filter_glob: bpy.props.StringProperty(default = '*.vec', options = {'HIDDEN'})

    filepath: bpy.props.StringProperty(subtype = "FILE_PATH")
    
    def execute(self, context):
        data = model.from_context(context)
        package = data.to_family()
        file_name = bpy.path.ensure_ext(self.filepath, ".vec")

        if self.export_format == 'BINARY':
            package.pack()
            package.write_binary(file_name)
        else:
            package.write_text(file_name)

        return {'FINISHED'}

    def invoke(self, context, event):
        context.window_manager.fileselect_add(self)
        return {'RUNNING_MODAL'}
