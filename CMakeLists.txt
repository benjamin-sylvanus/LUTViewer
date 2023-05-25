cmake_minimum_required(VERSION 3.5)
project(LUTViewer VERSION 1.0)
set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} ${SOURCES} main.cpp)

target_include_directories(LUTViewer PRIVATE "ViewLib")  # Path to the library headers
target_link_directories(LUTViewer PRIVATE "ViewLib/lib")  # Path to the library binaries

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(glm REQUIRED)

if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL" "-framework GLUT" "-framework Cocoa" View)
else ()
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} View)
endif ()