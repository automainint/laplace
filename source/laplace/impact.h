#ifndef LAPLACE_IMPACT_H
#define LAPLACE_IMPACT_H

#include "action.h"
#include <kit/dynamic_array.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { LAPLACE_IMPACT_NOOP } impact_type_t;

typedef struct {
  int _;
} laplace_noop_t;

struct laplace_impact {
  impact_type_t type;
  union {
    laplace_noop_t noop;
  };
};

KIT_DA_TYPE(laplace_impact_list_t, laplace_impact_t);

#ifndef LAPLACE_DISABLE_SHORT_NAMES
#  define IMPACT_NOOP LAPLACE_IMPACT_NOOP

#  define impact_t laplace_impact_t
#  define impact_list_t laplace_impact_list_t
#endif

#ifdef __cplusplus
}
#endif

#endif
