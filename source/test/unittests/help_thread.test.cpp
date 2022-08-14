#include "help_thread.test.h"

#include <thread>

extern "C" void test_thread_create(test_thread_t      *thread,
                                   test_thread_routine routine,
                                   void               *user_data) {
  std::thread *p = new std::thread([=]() { routine(user_data); });
  *((std::thread **) thread) = p;
}

extern "C" void test_thread_join(test_thread_t thread) {
  std::thread *p = (std::thread *) thread;
  p->join();
  delete p;
}
