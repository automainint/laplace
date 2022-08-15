#include "impact.h"

void laplace_impact_destroy(laplace_impact_t *impact) {
  switch (impact->type) {
    case LAPLACE_IMPACT_INTEGER_WRITE_VALUES:
      DA_DESTROY(impact->integer_write_values.values);
      break;

    case LAPLACE_IMPACT_INTEGER_WRITE_DELTAS:
      DA_DESTROY(impact->integer_write_deltas.deltas);
      break;

    case LAPLACE_IMPACT_BYTE_WRITE_VALUES:
      DA_DESTROY(impact->byte_write_values.values);
      break;

    case LAPLACE_IMPACT_BYTE_WRITE_DELTAS:
      DA_DESTROY(impact->byte_write_deltas.deltas);
      break;

    default:;
  }
}

void laplace_impact_list_destroy(laplace_impact_list_t *list) {
  for (ptrdiff_t i = 0; i < list->size; i++)
    laplace_impact_destroy(list->values + i);
  DA_DESTROY(*list);
}

laplace_impact_mode_t laplace_impact_mode_of(
    laplace_impact_t const *const impact) {
  return LAPLACE_IMPACT_MODE_OF(impact->type);
}
