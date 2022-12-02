#ifndef LAPLACE_LAYOUT_H
#define LAPLACE_LAYOUT_H

#include <kit/dynamic_array.h>
#include <kit/string_ref.h>

#ifdef __cplusplus
extern "C" {
#endif

enum { LAPLACE_LAYOUT_FIELD_NAME_SIZE = 40 };

typedef struct {
  ptrdiff_t id;
  struct {
    ptrdiff_t size;
    char      values[LAPLACE_LAYOUT_FIELD_NAME_SIZE];
  } name;
} laplace_layout_field_t;

typedef KIT_DA(char) laplace_layout_code_t;

typedef KIT_DA(laplace_layout_field_t) laplace_layout_t;

void laplace_layout_init(laplace_layout_t *layout,
                         kit_allocator_t   alloc);

void laplace_layout_destroy(laplace_layout_t *layout);

void laplace_layout_add_field(laplace_layout_t *layout, ptrdiff_t id,
                              kit_str_t name);

ptrdiff_t laplace_layout_index_of(laplace_layout_t const *layout,
                                  ptrdiff_t               id);

laplace_layout_code_t laplace_layout_codegen(
    laplace_layout_t const *layout, ptrdiff_t indent,
    kit_str_t prefix, kit_str_t delim, kit_allocator_t alloc);

#define LAPLACE_LAYOUT_ADD_FIELD_S(layout_, id_, str_)  \
  do {                                                  \
    KIT_SZ(name_, str_);                                \
    laplace_layout_add_field(&(layout_), (id_), name_); \
  } while (0)

#ifndef LAPLACE_DISABLE_SHORT_NAMES
#  define LAYOUT_FIELD_NAME_SIZE LAPLACE_LAYOUT_FIELD_NAME_SIZE
#  define LAYOUT_ADD_FIELD_S LAPLACE_LAYOUT_ADD_FIELD_S

#  define layout_field_t laplace_layout_field_t
#  define layout_code_t laplace_layout_code_t
#  define layout_t laplace_layout_t

#  define layout_init laplace_layout_init
#  define layout_destroy laplace_layout_destroy
#  define layout_add_field laplace_layout_add_field
#  define layout_index_of laplace_layout_index_of
#  define layout_codegen laplace_layout_codegen
#endif

#ifdef __cplusplus
}
#endif

#endif
