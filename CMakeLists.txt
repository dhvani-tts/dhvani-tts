cmake_minimum_required(VERSION 2.8)

project(dhvani-tts-cmake)

add_subdirectory(src)

add_executable(dhvani-tts-cmake src/dhvani.c)
install(TARGETS dhvani-tts-cmake RUNTIME DESTINATION bin)


target_link_libraries(dhvani-tts-cmake dhvani)