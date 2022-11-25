target_sources(
  ${LAPLACE_TEST}
    PRIVATE
      execution.test.c main.test.c state.test.c impact.test.c
      generator.test.c buffer.test.c layout.test.c controller.test.c)
