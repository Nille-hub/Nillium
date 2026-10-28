# CMakeList.txt : CMake project for Nillium

cmake_minimum_required (VERSION 3.20)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Nillium LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

set(GLFW_BUILD_DOCS     OFF)
set(GLFW_BUILD_TESTS    OFF)
set(GLFW_BUILD_EXAMPLES OFF)

FetchContent_Declare(
	glfw
	GIT_REPOSITORY https://github.com/glfw/glfw.git
	GIT_TAG        7b6aead9fb88b3623e3b3725ebb42670cbe4c579 # 3.4
)
FetchContent_MakeAvailable(glfw)

set(GLM_BUILD_LIBRARY OFF)
set(GLM_BUILD_TESTS   OFF)
set(GLM_BUILD_INSTALL OFF)

FetchContent_Declare(
	glm
	GIT_REPOSITORY https://github.com/g-truc/glm.git
	GIT_TAG        8d1fd52e5ab5590e2c81768ace50c72bae28f2ed # 1.0.3
)
FetchContent_MakeAvailable(glm)

add_subdirectory("external/glad")
add_subdirectory("src")
