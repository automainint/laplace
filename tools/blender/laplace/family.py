from __future__ import annotations
from typing import List, Dict;
from enum import Enum;

class ids(Enum):
    empty       = 0
    boolean     = 1
    integer     = 2
    real        = 3
    string      = 4
    uint        = 5
    vbyte       = 6
    vector      = 7
    composite   = 8

keys = [
    "_undefined",
    "_function",
    "_arguments",
    "_commands",
    "load",
    "nodes",
    "nunches",
    "lights",
    "binds",
    "position",
    "rotation",
    "texcoord",
    "is_opaque",
    "is_smooth",
    "has_shadow",
    "material",
    "verticles",
    "indices",
    "color",
    "diffuse",
    "specular",
    "focus",
    "reflection",
    "refraction_factor",
    "refraction_ratio",
    "colormap"
]

table = {}

class invalid_format: ...

class family:
    """PlainTeq family data format."""

    root = {
        id: ids.empty
    }

    def set_empty(self):
        self.root = {
            id: ids.empty
        }

    def boolean(value: bool) -> family:
        result = family()
        result.root['id'] = ids.boolean
        result.root['value'] = value
        return result

    def integer(value: int) -> family:
        result = family()
        result.root['id'] = ids.integer
        result.root['value'] = value
        return result

    def real(value: float) -> family:
        result = family()
        result.root['id'] = ids.real
        result.root['value'] = value
        return result

    def string(value: str) -> family:
        result = family()
        result.root['id'] = ids.string
        result.root['value'] = value
        return result

    def uint(value: int) -> family:
        result = family()
        result.root['id'] = ids.uint
        result.root['value'] = value
        return result

    def vbyte(value: bytes) -> family:
        result = family()
        result.root['id'] = ids.vbyte
        result.root['value'] = value
        return result

    def vector(elements: List[family]) -> family:
        result = family()
        result.root['id'] = ids.vector
        result.root['elements'] = elements
        return result

    def composite(childs: Dict[family, family]) -> family:
        result = family()
        result.root['id'] = ids.composite
        result.root['childs'] = childs
        return result

    def init_table():
        if len(table) == 0:
            table = {keys[x]: x for x in range(len(keys))}

    def pack(self):
        """
        Replace each string key in composites with
        uint equivalent from table.
        """

        init_table()

        if self.root['id'] == ids.composite:
            for key in self.root['childs']:
                if key.root['id'] == ids.string:
                    try:
                        index = table[key.root['value']]
                        value = self.root['childs'][key]
                        self.root['childs'].pop(key)
                        self.root['childs'][family.uint(index)] = value
                    except Exception:
                        pass
                self.root['childs'][key].pack()

    def unpack(self):
        """
        Replace each uint key in composites with
        string equivalent from keys.
        """

        if self.root['id'] == ids.composite:
            for key in self.root['childs']:
                if key.root['id'] == ids.uint:
                    try:
                        name = keys[key.root['value']]
                        value = self.root['childs'][key]
                        self.root['childs'].pop(key)
                        self.root['childs'][family.string(name)] = value
                    except Exception:
                        pass
                self.root['childs'][key].unpack()

    def write_text_to(self, stream):
        ...

    def write_binary_to(self, stream):
        ...

    def read_text_from(self, stream):
        ...

    def read_binary_from(self, stream):
        ...

    def write_text(self, file_name: str):
        stream = open(file_name, "w")
        self.write_text_to(stream)
    
    def write_binary(self, file_name: str):
        stream = open(file_name, "w")
        self.write_binary_to(stream)

    def read_text(file_name: str) -> family:
        stream = open(file_name, "r")
        data = family()
        data.read_text_from(stream)
        return data

    def read_binary(file_name: str) -> family:
        stream = open(file_name, "r")
        data = family()
        data.read_binary_from(stream)
        return data
    
    def read_any(file_name: str) -> family:
        try:
            return read_binary(str)
        except invalid_format:
            return read_text(str)
