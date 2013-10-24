cmake_minimum_required(VERSION 2.8)

project(dhvani)

add_subdirectory(src)

add_executable(dhvani src/dhvani.c)
install(TARGETS dhvani RUNTIME DESTINATION bin)


target_link_libraries(dhvani dhvanilib)