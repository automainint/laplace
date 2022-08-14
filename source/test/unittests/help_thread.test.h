#ifndef LAPLACE_HELP_THREAD_TEST_H
#define LAPLACE_HELP_THREAD_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void *test_thread_t;
typedef void (*test_thread_routine)(void *user_data);

void test_thread_create(test_thread_t      *thread,
                        test_thread_routine routine, void *user_data);

void test_thread_join(test_thread_t thread);

#ifdef __cplusplus
}
#endif

#endif
