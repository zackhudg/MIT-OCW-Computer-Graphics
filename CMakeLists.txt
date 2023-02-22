# CMake entry point
cmake_minimum_required (VERSION 3.0)
project (Assignments)

find_package(OpenGL REQUIRED))

# Compile external dependencies 
add_subdirectory (external/glew/)

include_directories(
	external/glfw/include/
	external/glm/
	external/glew0/include/
	.
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	glew
)

add_definitions(
    -DTW_STATIC
    -DTW_NO_LIB_PRAGMA
    -DTW_NO_DIRECT3D
    -DGLEW_STATIC
)


# Assignment 0
add_executable(a0
	a0/a0.cpp
	common/shader.cpp
	common/shader.hpp
	common/objloader.cpp
	common/objloader.hpp
	common/vboindexer.cpp
	common/vboindexer.hpp
	a0/VertexShader.vertexshader
	a0/FragmentShader.fragmentshader
)
target_link_libraries(a0
	${ALL_LIBS}
)
#set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/a0/")
set_target_properties(a0 PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/a0/")
set_target_properties(a0 PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/a0/")
#create_target_launcher(a0 WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/a0/")

