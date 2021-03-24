from __future__ import annotations
from .family import family;
import bpy

class model:
    """PlainTeq model data."""

    def from_family() -> model:
        return model()
    
    def to_family(self) -> family:
        return family()
    
    def from_context(context) -> model:
        return model()
    
    def setup(self, context):
        ...
