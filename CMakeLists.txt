cmake_minimum_required(VERSION 3.0.0)
project(assignment VERSION 0.1.0)
add_compile_options(-std=c++11)
add_compile_definitions(_GNU_SOURCE)
aux_source_directory(./src SRCS)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

find_package(Eigen3 REQUIRED)

include_directories(
    include
    ${Eigen3_INCLUDE_DIR}
)

add_library (hungarian SHARED 
    src/HungarianAlg.cpp
)

add_executable(${PROJECT_NAME} 
    src/main.cc
)
target_link_libraries(${PROJECT_NAME} 
    hungarian
)
