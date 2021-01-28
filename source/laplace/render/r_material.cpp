#include "material.h"

using namespace laplace::render;
using namespace laplace::core;
using namespace std;

material::material()
{
    static element::type type_material;
    set_type_id(type_material.get_id());
}

material::~material() { }
