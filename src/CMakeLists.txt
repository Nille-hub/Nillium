
add_definitions(-DSDL_MAIN_HANDLED)

add_executable(
	Nillium
	"main.cpp"
)

 target_include_directories(Nillium PRIVATE 
	${CMAKE_CURRENT_BINARY_DIR} # Would be relevant for headers generated with configure_file
	${CMAKE_CURRENT_SOURCE_DIR} # Relevant for including from src as root when having subfolders in src
 )

target_compile_definitions(Nillium PRIVATE -D_USE_MATH_DEFINES)