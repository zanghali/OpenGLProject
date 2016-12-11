
cmake_minimum_required(VERSION 2.6)

project(executable)


file(
GLOB
source_files
src/*.[ch]pp src/shader*
src/format/*.[ch]pp
)

SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_CXX_FLAGS "-O2 -Wall -Wextra" )
SET(CMAKE_CXX_COMPILER "g++")



add_executable(	
   executable
   ${source_files}
   )	

target_link_libraries(executable "-L/usr/lib/nvidia-331 -lm -lglut -lGLEW -lGLU -lGL -pthread")


