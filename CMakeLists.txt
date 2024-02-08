cmake_minimum_required( VERSION 3.5 )
project(citty)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_C_FLAGS "-Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS_RELEASE "-O3")

set( OpenGL_GL_PREFERENCE "GLVND" )
find_package(OpenGL 3.3 REQUIRED)

set( GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE )
set( GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE )
set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE )

add_subdirectory( ${PROJECT_SOURCE_DIR}/extern/glfw )

add_executable( citty ${PROJECT_SOURCE_DIR}/src/main.c )


find_package(GLEW 2.0 REQUIRED)
target_link_libraries(citty GLEW::GLEW)

target_link_libraries(citty glfw)
target_link_libraries(citty OpenGL::GL)


