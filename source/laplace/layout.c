#include "layout.h"

#include "options.h"

#include <kit/lower_bound.h>
#include <stdio.h>
#include <string.h>

static int field_less(laplace_layout_field_t const left,
                      ptrdiff_t const              right) {
  return left.id < right;
}

void laplace_layout_init(laplace_layout_t *layout,
                         kit_allocator_t   alloc) {
  DA_INIT(*layout, 0, alloc);
}

void laplace_layout_destroy(laplace_layout_t *const layout) {
  DA_DESTROY(*layout);
}

void laplace_layout_add_field(laplace_layout_t *const layout,
                              ptrdiff_t const id, str_t const name) {
  ptrdiff_t index;
  LOWER_BOUND(index, *layout, id, field_less);

  if (index < layout->size && layout->values[index].id == id)
    return;

  laplace_layout_field_t field = { .id   = id,
                                   .name = { .size = name.size } };
  if (field.name.size > LAYOUT_FIELD_NAME_SIZE)
    field.name.size = LAYOUT_FIELD_NAME_SIZE;
  memcpy(field.name.values, name.values, field.name.size);
  DA_INSERT(*layout, index, field);
}

ptrdiff_t laplace_layout_index_of(
    laplace_layout_t const *const layout, ptrdiff_t const id) {
  ptrdiff_t index;
  LOWER_BOUND(index, *layout, id, field_less);

  if (index >= layout->size || layout->values[index].id != id)
    return LAPLACE_ID_UNDEFINED;

  return index;
}

static int append_str(laplace_layout_code_t const *const code,
                      str_t const s, ptrdiff_t repeat) {
  ptrdiff_t offset = code->size;
  DA_RESIZE(*code, offset + s.size * repeat);
  if (code->size != offset + s.size * repeat)
    return 0;
  for (ptrdiff_t i = 0; i < repeat; i++)
    memcpy(code->values + offset + i * s.size, s.values, s.size);
  return 1;
}
static int append_sz(laplace_layout_code_t const *const code,
                     char const *s, ptrdiff_t repeat) {
  str_t str = { .size = strlen(s), .values = s };
  return append_str(code, str, repeat);
}

laplace_layout_code_t laplace_layout_codegen(
    laplace_layout_t const *const layout, ptrdiff_t const indent,
    kit_str_t const prefix, kit_str_t const delim,
    kit_allocator_t const alloc) {
  laplace_layout_code_t code;
  DA_INIT(code, 0, alloc);

  int ok = 1;
  for (ptrdiff_t i = 0; ok && i < layout->size; i++) {
    str_t const name = { .size   = layout->values[i].name.size,
                         .values = layout->values[i].name.values };
    char        buf[200];
    str_t const num = { .size   = snprintf(buf, sizeof buf, "%tx", i),
                        .values = buf };

    ok = append_sz(&code, " ", indent);
    ok = ok && append_str(&code, prefix, 1);
    ok = ok && append_str(&code, name, 1);
    ok = ok && append_sz(&code, " = ", 1);
    ok = ok && append_str(&code, num, 1);
    ok = ok && append_str(&code, delim, 1);
  }
  if (!ok)
    DA_RESIZE(code, 0);
  return code;
}
