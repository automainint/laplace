target_sources(
  ${LAPLACE_OBJ}
    PRIVATE
      ui_rect.cpp ui_layout.cpp ui_widget.cpp
    PUBLIC
      widget.h helper.h rect.h context.h layout.h
)
add_subdirectory(elem)
