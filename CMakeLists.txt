cmake_minimum_required(VERSION 3.13)

project(NET LANGUAGES CXX)

add_library(NET INTERFACE)
add_subdirectory(TAT)
target_include_directories(NET INTERFACE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(NET INTERFACE TAT)
target_compile_definitions(NET INTERFACE TAT_USE_VALID_DEFAULT_TENSOR TAT_USE_COPY_WITHOUT_WARNING)
target_compile_definitions(NET INTERFACE TAT_USE_SIMPLE_NAME TAT_USE_SINGULAR_MATRIX TAT_USE_SIMPLE_NOSYMMETRY)
target_link_libraries(NET INTERFACE gvc cgraph)

add_executable(test.out example/prog.cpp example/timer.cpp)
target_link_libraries(test.out NET)
target_compile_definitions(test.out PRIVATE NET_GRAPH_VIZ NET_SHOW_FIG)
