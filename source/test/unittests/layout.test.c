#include "../../laplace/layout.h"
#include <string.h>

#define KIT_TEST_FILE layout
#include <kit_test/test.h>

TEST("layout add 3 fields") {
  layout_t layout;
  layout_init(&layout, kit_alloc_default());

  SZ(empty, "");

  layout_add_field(&layout, 3, empty);
  layout_add_field(&layout, 1, empty);
  layout_add_field(&layout, 2, empty);

  REQUIRE(layout.size == 3);
  REQUIRE(layout_index_of(&layout, 1) == 0);
  REQUIRE(layout_index_of(&layout, 2) == 1);
  REQUIRE(layout_index_of(&layout, 3) == 2);

  layout_destroy(&layout);
}

TEST("layout codegen two fields") {
  layout_t layout;
  layout_init(&layout, kit_alloc_default());

  SZ(prefix, "n_");
  SZ(delim, ",\n");

  LAYOUT_ADD_FIELD_S(layout, 1, "foo");
  LAYOUT_ADD_FIELD_S(layout, 2, "bar");

  layout_code_t code = layout_codegen(&layout, 2, prefix, delim,
                                      kit_alloc_default());
  layout_destroy(&layout);

  DA_RESIZE(code, code.size + 1);
  code.values[code.size - 1] = '\0';
  DA_RESIZE(code, code.size - 1);

  REQUIRE(code.size > 0 &&
          strcmp(code.values, "  n_foo = 0x0000,\n"
                              "  n_bar = 0x0001,\n") == 0);
  DA_DESTROY(code);
}
