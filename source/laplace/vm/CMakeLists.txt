target_sources(
  ${LAPLACE_EXE}
    PRIVATE
      vm_agent.cpp vm_environment.cpp vm_execute.cpp
      vm_heap.cpp vm_instance.cpp
    PUBLIC
      agent.h agent.predef.h bitcode.h environment.h
      environment.predef.h execute.h heap.h instance.h
)
