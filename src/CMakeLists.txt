project(snake)

add_executable(${PROJECT_NAME}
  main.cpp
  Core.cpp
)

target_link_libraries(${PROJECT_NAME}
  ncurses
)
