
add_definitions(-DSDL_MAIN_HANDLED)

include(FetchContent)
FetchContent_Declare(
	glfw
	GIT_REPOSITORY https://github.com/glfw/glfw.git
	GIT_TAG        7b6aead9fb88b3623e3b3725ebb42670cbe4c579  # 3.4 release
	GIT_SHALLOW    TRUE
)

add_executable(
	Nillium
	"main.cpp"
)

 target_include_directories(Nillium PRIVATE 
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
 )

target_compile_definitions(Nillium PRIVATE -D_USE_MATH_DEFINES)