cmake_minimum_required(VERSION 3.12)

project(
        SearchEngine
        LANGUAGES CXX
)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(lib)
add_subdirectory(bin)

enable_testing()
add_subdirectory(tests)