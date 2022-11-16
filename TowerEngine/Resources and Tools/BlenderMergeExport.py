import bpy

from bpy.props import (BoolProperty, FloatProperty, StringProperty)
from bpy.types import (Operator)
from bpy_extras.io_utils import ExportHelper

# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
class EXPORT_OT_customFBX(Operator, ExportHelper):
    """Export the scene to FBX"""
    bl_idname = "export_scene.custom_fbx"
    bl_label = "Export DAE"

    # ExportHelper mixin class uses this
    filename_ext = ".dae"

    filter_glob: StringProperty(
        default="*.dae",
        options={'HIDDEN'},
        maxlen=255,  # Max internal buffer length, longer would be clamped.
    )

    # List of operator properties, the attributes will be assigned
    # to the class instance from the operator settings before calling.
    global_scale: FloatProperty(
        name="Scale",
        description="Scale",
        default=1.0,
    )

    use_subsurf: BoolProperty(
        name="Use Subsurf",
        description="Use Subsurf",
        default=False,
    )

    apply_unit_scale: BoolProperty(
        name="Apply Unit Scale",
        description="Use Subsurf",
        default=True,
    )

    def execute(self, context):

        viewport_selection = [o for o in context.selected_objects if o.type == 'MESH']
        bpy.ops.object.join()

        # Export
        bpy.ops.wm.collada_export(filepath=self.filepath)

        # Undo!
        bpy.ops.ed.undo()
        return {'FINISHED'} 

# Only needed if you want to add into a dynamic menu
def draw_export_fbx(self, context):
    self.layout.operator(EXPORT_OT_customFBX.bl_idname, text="Custom FBX (.fbx)", icon="MESH_MONKEY")


# Registration
classes = (
    EXPORT_OT_customFBX,
)

def register():
    from bpy.utils import register_class
    for cls in classes:
        register_class(cls)

    bpy.types.TOPBAR_MT_file_export.prepend(draw_export_fbx)


def unregister():
    from bpy.utils import unregister_class
    for cls in reversed(classes):
        unregister_class(cls)

    bpy.types.TOPBAR_MT_file_export.remove(draw_export_fbx)


if __name__ == "__main__":
    register()

    # test call
    bpy.ops.export_scene.custom_fbx('INVOKE_DEFAULT')