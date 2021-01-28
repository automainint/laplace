#include "model.h"

using namespace laplace;
using namespace render;
using namespace core;
using namespace std;

model::model()
{
    static element::type type_model;
    set_type_id(type_model.get_id());
}

model::~model() { }

void model::clear()
{
    nodes.clear();
    bunches.clear();
    lights.clear();
}
