target_sources(
  ${EXE_NAME}
    PRIVATE
       ui_context.cpp ui_frame.cpp ui_rect.cpp ui_widget.cpp
    PUBLIC
       context.h frame.h rect.h widget.h
)
add_subdirectory(elem)
add_subdirectory(text)
