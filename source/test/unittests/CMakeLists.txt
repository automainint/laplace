target_sources(
  laplace_test_suite
    PRIVATE
      execution.test.c main.test.c state.test.c impact.test.c
      generator.test.c buffer.test.c layout.test.c controller.test.c)
