# CMake entry point
cmake_minimum_required (VERSION 3.0)
project (Assignments)

find_package(OpenGL REQUIRED)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	libglew_static
)

# Compile external dependencies 
add_subdirectory (external/glfw)
add_subdirectory (external/glew)

include_directories(
	external/glfw/include/
	external/glm/
	external/glew/include/
	.
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

# Assignment 1
add_executable(a1
	a1/a1.cpp
	common/shader.cpp
	common/shader.hpp
	common/objloader.cpp
	common/objloader.hpp
	common/vboindexer.cpp
	common/vboindexer.hpp
	a1/VertexShader.vertexshader
	a1/FragmentShader.fragmentshader
	a1/camera.cpp
	a1/curve.cpp
	a1/parse.cpp
	a1/surf.cpp
	a1/camera.h
	a1/curve.h
	a1/parse.h
	a1/surf.h
	a1/extra.h
	a1/tuple.h
)
target_link_libraries(a1
	${ALL_LIBS}
)
set_target_properties(a1 PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/a1/")
set_target_properties(a1 PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/a1/")


