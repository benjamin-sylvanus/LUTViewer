cmake_minimum_required(VERSION 3.5)
project(View)

set(CMAKE_CXX_STANDARD 17)

# Set the output directory for library binaries
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")

# Add library source files
set(SOURCES
        datatemplate.cpp
        datatemplate.h
        GLMethods.cpp
        GLMethods.h
        Vectors.h
        )

# Create the dynamic library
add_library(View SHARED ${SOURCES})

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(glm REQUIRED)

if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL" "-framework GLUT" "-framework Cocoa")
else ()
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
endif ()

# Specify include directories
target_include_directories(View PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})