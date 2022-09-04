#ifndef LAPLACE_IMPACT_H
#define LAPLACE_IMPACT_H

#include "action.h"
#include <kit/dynamic_array.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  LAPLACE_IMPACT_ASYNC,
  LAPLACE_IMPACT_SYNC,
  LAPLACE_IMPACT_CONTROL
} laplace_impact_mode_t;

typedef enum {
  /*  No-op.
   */
  LAPLACE_IMPACT_NOOP,

  /*  Integer async operations.
   */
  LAPLACE_IMPACT_INTEGER_SET,
  LAPLACE_IMPACT_INTEGER_ADD,
  LAPLACE_IMPACT_INTEGER_WRITE_VALUES,
  LAPLACE_IMPACT_INTEGER_WRITE_DELTAS,

  /*  Byte async operations.
   */
  LAPLACE_IMPACT_BYTE_SET,
  LAPLACE_IMPACT_BYTE_ADD,
  LAPLACE_IMPACT_BYTE_WRITE_VALUES,
  LAPLACE_IMPACT_BYTE_WRITE_DELTAS,

  /*  Integer sync operations.
   */
  LAPLACE_IMPACT_INTEGER_RESERVE,
  LAPLACE_IMPACT_INTEGER_ALLOCATE_INTO,
  LAPLACE_IMPACT_INTEGER_ALLOCATE,
  LAPLACE_IMPACT_INTEGER_REALLOCATE,
  LAPLACE_IMPACT_INTEGER_DEALLOCATE,

  /*  Byte sync operations.
   */
  LAPLACE_IMPACT_BYTE_RESERVE,
  LAPLACE_IMPACT_BYTE_ALLOCATE_INTO,
  LAPLACE_IMPACT_BYTE_ALLOCATE,
  LAPLACE_IMPACT_BYTE_REALLOCATE,
  LAPLACE_IMPACT_BYTE_DEALLOCATE,

  /*  Random sync operations.
   */
  LAPLACE_IMPACT_INTEGER_SEED,
  LAPLACE_IMPACT_INTEGER_RANDOM,
  LAPLACE_IMPACT_BYTE_RANDOM,

  /*  Queue new action.
   */
  LAPLACE_IMPACT_QUEUE_ACTION,

  /*  Control flow.
   */
  LAPLACE_IMPACT_TICK_CONTINUE,

  LAPLACE_IMPACT_FIRST_SYNC_    = LAPLACE_IMPACT_INTEGER_RESERVE,
  LAPLACE_IMPACT_FIRST_CONTROL_ = LAPLACE_IMPACT_TICK_CONTINUE
} laplace_impact_type_t;

typedef struct {
  int _;
} laplace_noop_t;

typedef struct {
  int _;
} laplace_tick_continue_t;

typedef struct {
  ptrdiff_t count;
} laplace_integer_reserve_t;

typedef struct {
  laplace_handle_t handle;
  ptrdiff_t        size;
} laplace_integer_allocate_into_t;

typedef struct {
  ptrdiff_t        size;
  laplace_handle_t return_handle;
  ptrdiff_t        return_index;
} laplace_integer_allocate_t;

typedef struct {
  laplace_handle_t handle;
  ptrdiff_t        size;
} laplace_integer_reallocate_t;

typedef struct {
  laplace_handle_t handle;
} laplace_integer_deallocate_t;

typedef struct {
  laplace_handle_t  handle;
  ptrdiff_t         index;
  laplace_integer_t value;
} laplace_integer_set_t;

typedef struct {
  laplace_handle_t  handle;
  ptrdiff_t         index;
  laplace_integer_t delta;
} laplace_integer_add_t;

typedef struct {
  laplace_handle_t handle;
  ptrdiff_t        index;
  KIT_DA(values, laplace_integer_t);
} laplace_integer_write_values_t;

typedef struct {
  laplace_handle_t handle;
  ptrdiff_t        index;
  KIT_DA(deltas, laplace_integer_t);
} laplace_integer_write_deltas_t;

typedef struct {
  ptrdiff_t count;
} laplace_byte_reserve_t;

typedef struct {
  laplace_handle_t handle;
  ptrdiff_t        size;
} laplace_byte_allocate_into_t;

typedef struct {
  ptrdiff_t        size;
  laplace_handle_t return_handle;
  ptrdiff_t        return_index;
} laplace_byte_allocate_t;

typedef struct {
  laplace_handle_t handle;
  ptrdiff_t        size;
} laplace_byte_reallocate_t;

typedef struct {
  laplace_handle_t handle;
} laplace_byte_deallocate_t;

typedef struct {
  laplace_handle_t handle;
  ptrdiff_t        index;
  laplace_byte_t   value;
} laplace_byte_set_t;

typedef struct {
  laplace_handle_t handle;
  ptrdiff_t        index;
  laplace_byte_t   delta;
} laplace_byte_add_t;

typedef struct {
  laplace_handle_t handle;
  ptrdiff_t        index;
  KIT_DA(values, laplace_byte_t);
} laplace_byte_write_values_t;

typedef struct {
  laplace_handle_t handle;
  ptrdiff_t        index;
  KIT_DA(deltas, laplace_byte_t);
} laplace_byte_write_deltas_t;

typedef struct {
  laplace_integer_t seed;
} laplace_integer_seed_t;

typedef struct {
  laplace_integer_t min;
  laplace_integer_t max;
  laplace_handle_t  return_handle;
  ptrdiff_t         return_index;
  ptrdiff_t         return_size;
} laplace_integer_random_t;

typedef struct {
  laplace_byte_t   min;
  laplace_byte_t   max;
  laplace_handle_t return_handle;
  ptrdiff_t        return_index;
  ptrdiff_t        return_size;
} laplace_byte_random_t;

typedef struct {
  laplace_action_t action;
} laplace_queue_action_t;

struct laplace_impact {
  laplace_impact_type_t type;
  union {
    laplace_noop_t                  noop;
    laplace_tick_continue_t         tick_continue;
    laplace_integer_reserve_t       integer_reserve;
    laplace_integer_allocate_into_t integer_allocate_into;
    laplace_integer_allocate_t      integer_allocate;
    laplace_integer_reallocate_t    integer_reallocate;
    laplace_integer_deallocate_t    integer_deallocate;
    laplace_integer_set_t           integer_set;
    laplace_integer_add_t           integer_add;
    laplace_integer_write_values_t  integer_write_values;
    laplace_integer_write_deltas_t  integer_write_deltas;
    laplace_byte_reserve_t          byte_reserve;
    laplace_byte_allocate_into_t    byte_allocate_into;
    laplace_byte_allocate_t         byte_allocate;
    laplace_byte_reallocate_t       byte_reallocate;
    laplace_byte_deallocate_t       byte_deallocate;
    laplace_byte_set_t              byte_set;
    laplace_byte_add_t              byte_add;
    laplace_byte_write_values_t     byte_write_values;
    laplace_byte_write_deltas_t     byte_write_deltas;
    laplace_integer_seed_t          integer_seed;
    laplace_integer_random_t        integer_random;
    laplace_byte_random_t           byte_random;
    laplace_queue_action_t          queue_action;
  };
  ptrdiff_t order;
};

KIT_DA_TYPE(laplace_impact_list_t, laplace_impact_t);

void laplace_impact_destroy(laplace_impact_t *impact);

void laplace_impact_list_destroy(laplace_impact_list_t *list);

laplace_impact_mode_t laplace_impact_mode_of(
    laplace_impact_t const *impact);

#define LAPLACE_NOOP() \
  { .type = LAPLACE_IMPACT_NOOP, .order = 0 }

#define LAPLACE_TICK_CONTINUE() \
  { .type = LAPLACE_IMPACT_TICK_CONTINUE, .order = 0 }

#define LAPLACE_INTEGER_RESERVE(count_)                  \
  {                                                      \
    .type            = LAPLACE_IMPACT_INTEGER_RESERVE,   \
    .integer_reserve = { .count = (count_) }, .order = 0 \
  }

#define LAPLACE_INTEGER_ALLOCATE_INTO(handle_, size_)              \
  {                                                                \
    .type                  = LAPLACE_IMPACT_INTEGER_ALLOCATE_INTO, \
    .integer_allocate_into = { .handle = (handle_),                \
                               .size   = (size_) },                  \
    .order                 = 0                                     \
  }

#define LAPLACE_INTEGER_ALLOCATE(size_, return_handle_,      \
                                 return_index_)              \
  {                                                          \
    .type             = LAPLACE_IMPACT_INTEGER_ALLOCATE,     \
    .integer_allocate = { .size          = (size_),          \
                          .return_handle = (return_handle_), \
                          .return_index  = (return_index_) }, \
    .order            = 0                                    \
  }

#define LAPLACE_INTEGER_DEALLOCATE(handle_)                   \
  {                                                           \
    .type               = LAPLACE_IMPACT_INTEGER_DEALLOCATE,  \
    .integer_deallocate = { .handle = (handle_) }, .order = 0 \
  }

#define LAPLACE_INTEGER_SET(handle_, index_, value_) \
  {                                                  \
    .type        = LAPLACE_IMPACT_INTEGER_SET,       \
    .integer_set = { .handle = (handle_),            \
                     .index  = (index_),             \
                     .value  = (value_) },            \
    .order       = 0                                 \
  }

#define LAPLACE_INTEGER_ADD(handle_, index_, delta_) \
  {                                                  \
    .type        = LAPLACE_IMPACT_INTEGER_ADD,       \
    .integer_add = { .handle = (handle_),            \
                     .index  = (index_),             \
                     .delta  = (delta_) },            \
    .order       = 0                                 \
  }

#define LAPLACE_INTEGER_WRITE_VALUES(handle_, index_, values_)     \
  {                                                                \
    .type                 = LAPLACE_IMPACT_INTEGER_WRITE_VALUES,   \
    .integer_write_values = { .handle = (handle_),                 \
                              .index  = (index_),                  \
                              .values = { .capacity =              \
                                              (values_).capacity,  \
                                          .size = (values_).size,  \
                                          .values =                \
                                              (values_).values,    \
                                          .alloc =                 \
                                              (values_).alloc } }, \
    .order                = 0                                      \
  }

#define LAPLACE_INTEGER_WRITE_DELTAS(handle_, index_, deltas_)     \
  {                                                                \
    .type                 = LAPLACE_IMPACT_INTEGER_WRITE_DELTAS,   \
    .integer_write_deltas = { .handle = (handle_),                 \
                              .index  = (index_),                  \
                              .deltas = { .capacity =              \
                                              (deltas_).capacity,  \
                                          .size = (deltas_).size,  \
                                          .values =                \
                                              (deltas_).values,    \
                                          .alloc =                 \
                                              (deltas_).alloc } }, \
    .order                = 0                                      \
  }

#define LAPLACE_BYTE_RESERVE(count_)                  \
  {                                                   \
    .type         = LAPLACE_IMPACT_BYTE_RESERVE,      \
    .byte_reserve = { .count = (count_) }, .order = 0 \
  }

#define LAPLACE_BYTE_ALLOCATE_INTO(handle_, size_)                  \
  {                                                                 \
    .type               = LAPLACE_IMPACT_BYTE_ALLOCATE_INTO,        \
    .byte_allocate_into = { .handle = (handle_), .size = (size_) }, \
    .order              = 0                                         \
  }

#define LAPLACE_BYTE_ALLOCATE(size_, return_handle_, return_index_) \
  {                                                                 \
    .type          = LAPLACE_IMPACT_BYTE_ALLOCATE,                  \
    .byte_allocate = { .size          = (size_),                    \
                       .return_handle = (return_handle_),           \
                       .return_index  = (return_index_) },           \
    .order         = 0                                              \
  }

#define LAPLACE_BYTE_DEALLOCATE(handle_)                   \
  {                                                        \
    .type            = LAPLACE_IMPACT_BYTE_DEALLOCATE,     \
    .byte_deallocate = { .handle = (handle_) }, .order = 0 \
  }

#define LAPLACE_BYTE_SET(handle_, index_, value_) \
  {                                               \
    .type     = LAPLACE_IMPACT_BYTE_SET,          \
    .byte_set = { .handle = (handle_),            \
                  .index  = (index_),             \
                  .value  = (value_) },            \
    .order    = 0                                 \
  }

#define LAPLACE_BYTE_ADD(handle_, index_, delta_) \
  {                                               \
    .type     = LAPLACE_IMPACT_BYTE_ADD,          \
    .byte_add = { .handle = (handle_),            \
                  .index  = (index_),             \
                  .delta  = (delta_) },            \
    .order    = 0                                 \
  }

#define LAPLACE_BYTE_WRITE_VALUES(handle_, index_, values_)          \
  {                                                                  \
    .type              = LAPLACE_IMPACT_BYTE_WRITE_VALUES,           \
    .byte_write_values = { .handle = (handle_),                      \
                           .index  = (index_),                       \
                           .values = { .capacity =                   \
                                           (values_).capacity,       \
                                       .size   = (values_).size,     \
                                       .values = (values_).values,   \
                                       .alloc  = (values_).alloc } }, \
    .order             = 0                                           \
  }

#define LAPLACE_BYTE_WRITE_DELTAS(handle_, index_, deltas_)          \
  {                                                                  \
    .type              = LAPLACE_IMPACT_BYTE_WRITE_DELTAS,           \
    .byte_write_deltas = { .handle = (handle_),                      \
                           .index  = (index_),                       \
                           .deltas = { .capacity =                   \
                                           (deltas_).capacity,       \
                                       .size   = (deltas_).size,     \
                                       .values = (deltas_).values,   \
                                       .alloc  = (deltas_).alloc } }, \
    .order             = 0                                           \
  }

#define LAPLACE_INTEGER_SEED(seed_)                 \
  {                                                 \
    .type         = LAPLACE_IMPACT_INTEGER_SEED,    \
    .integer_seed = { .seed = (seed_) }, .order = 0 \
  }

#define LAPLACE_INTEGER_RANDOM(min_, max_, return_handle_,  \
                               return_index_, return_size_) \
  {                                                         \
    .type           = LAPLACE_IMPACT_INTEGER_RANDOM,        \
    .integer_random = { .min           = (min_),            \
                        .max           = (max_),            \
                        .return_handle = (return_handle_),  \
                        .return_index  = (return_index_),   \
                        .return_size   = (return_size_) },    \
    .order          = 0                                     \
  }

#define LAPLACE_BYTE_RANDOM(min_, max_, return_handle_,  \
                            return_index_, return_size_) \
  {                                                      \
    .type        = LAPLACE_IMPACT_BYTE_RANDOM,           \
    .byte_random = { .min           = (min_),            \
                     .max           = (max_),            \
                     .return_handle = (return_handle_),  \
                     .return_index  = (return_index_),   \
                     .return_size   = (return_size_) },    \
    .order       = 0                                     \
  }

#define LAPLACE_QUEUE_ACTION(...)                         \
  {                                                       \
    .type         = LAPLACE_IMPACT_QUEUE_ACTION,          \
    .queue_action = { .action = __VA_ARGS__ }, .order = 0 \
  }

#define LAPLACE_IMPACT_MODE_OF(type_)                          \
  ((type_) < LAPLACE_IMPACT_FIRST_SYNC_ ? LAPLACE_IMPACT_ASYNC \
   : (type_) < LAPLACE_IMPACT_FIRST_CONTROL_                   \
       ? LAPLACE_IMPACT_SYNC                                   \
       : LAPLACE_IMPACT_CONTROL)

#ifndef LAPLACE_DISABLE_SHORT_NAMES
#  define IMPACT_ASYNC LAPLACE_IMPACT_ASYNC
#  define IMPACT_SYNC LAPLACE_IMPACT_SYNC
#  define IMPACT_CONTROL LAPLACE_IMPACT_CONTROL
#  define IMPACT_NOOP LAPLACE_IMPACT_NOOP
#  define IMPACT_TICK_CONTINUE LAPLACE_IMPACT_TICK_CONTINUE
#  define IMPACT_INTEGER_RESERVE LAPLACE_IMPACT_INTEGER_RESERVE
#  define IMPACT_INTEGER_ALLOCATE_INTO \
    LAPLACE_IMPACT_INTEGER_ALLOCATE_INTO
#  define IMPACT_INTEGER_ALLOCATE LAPLACE_IMPACT_INTEGER_ALLOCATE
#  define IMPACT_INTEGER_DEALLOCATE LAPLACE_IMPACT_INTEGER_DEALLOCATE
#  define IMPACT_INTEGER_SET LAPLACE_IMPACT_INTEGER_SET
#  define IMPACT_INTEGER_ADD LAPLACE_IMPACT_INTEGER_ADD
#  define IMPACT_INTEGER_WRITE_VALUES \
    LAPLACE_IMPACT_INTEGER_WRITE_VALUES
#  define IMPACT_INTEGER_WRITE_DELTAS \
    LAPLACE_IMPACT_INTEGER_WRITE_DELTAS
#  define IMPACT_BYTE_RESERVE LAPLACE_IMPACT_BYTE_RESERVE
#  define IMPACT_BYTE_ALLOCATE_INTO LAPLACE_IMPACT_BYTE_ALLOCATE_INTO
#  define IMPACT_BYTE_ALLOCATE LAPLACE_IMPACT_BYTE_ALLOCATE
#  define IMPACT_BYTE_DEALLOCATE LAPLACE_IMPACT_BYTE_DEALLOCATE
#  define IMPACT_BYTE_SET LAPLACE_IMPACT_BYTE_SET
#  define IMPACT_BYTE_ADD LAPLACE_IMPACT_BYTE_ADD
#  define IMPACT_BYTE_WRITE_VALUES LAPLACE_IMPACT_BYTE_WRITE_VALUES
#  define IMPACT_BYTE_WRITE_DELTAS LAPLACE_IMPACT_BYTE_WRITE_DELTAS
#  define IMPACT_INTEGER_SEED LAPLACE_IMPACT_INTEGER_SEED
#  define IMPACT_INTEGER_RANDOM LAPLACE_IMPACT_INTEGER_RANDOM
#  define IMPACT_BYTE_RANDOM LAPLACE_IMPACT_BYTE_RANDOM
#  define IMPACT_QUEUE_ACTION LAPLACE_IMPACT_QUEUE_ACTION
#  define NOOP LAPLACE_NOOP
#  define TICK_CONTINUE LAPLACE_TICK_CONTINUE
#  define INTEGER_RESERVE LAPLACE_INTEGER_RESERVE
#  define INTEGER_ALLOCATE_INTO LAPLACE_INTEGER_ALLOCATE_INTO
#  define INTEGER_ALLOCATE LAPLACE_INTEGER_ALLOCATE
#  define INTEGER_DEALLOCATE LAPLACE_INTEGER_DEALLOCATE
#  define INTEGER_SET LAPLACE_INTEGER_SET
#  define INTEGER_ADD LAPLACE_INTEGER_ADD
#  define INTEGER_WRITE_VALUES LAPLACE_INTEGER_WRITE_VALUES
#  define INTEGER_WRITE_DELTAS LAPLACE_INTEGER_WRITE_DELTAS
#  define BYTE_RESERVE LAPLACE_BYTE_RESERVE
#  define BYTE_ALLOCATE_INTO LAPLACE_BYTE_ALLOCATE_INTO
#  define BYTE_ALLOCATE LAPLACE_BYTE_ALLOCATE
#  define BYTE_DEALLOCATE LAPLACE_BYTE_DEALLOCATE
#  define BYTE_SET LAPLACE_BYTE_SET
#  define BYTE_ADD LAPLACE_BYTE_ADD
#  define BYTE_WRITE_VALUES LAPLACE_BYTE_WRITE_VALUES
#  define BYTE_WRITE_DELTAS LAPLACE_BYTE_WRITE_DELTAS
#  define INTEGER_SEED LAPLACE_INTEGER_SEED
#  define INTEGER_RANDOM LAPLACE_INTEGER_RANDOM
#  define BYTE_RANDOM LAPLACE_BYTE_RANDOM
#  define QUEUE_ACTION LAPLACE_QUEUE_ACTION
#  define IMPACT_MODE_OF LAPLACE_IMPACT_MODE_OF

#  define impact_mode_t laplace_impact_mode_t
#  define impact_type_t laplace_impact_type_t
#  define impact_t laplace_impact_t
#  define impact_list_t laplace_impact_list_t
#  define impact_mode_of laplace_impact_mode_of
#endif

#ifdef __cplusplus
}
#endif

#endif
