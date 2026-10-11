
# SDL.h renames main() to SDL_main() unless SDL_MAIN_HANDLED is defined.
# Without it, linking fails with an unresolved main/SDL_main.
# With it, call SDL_SetMainReady() before SDL_Init().
add_definitions(-DSDL_MAIN_HANDLED)

add_executable(
	Nillium
	"main.cpp"
)

 target_include_directories(Nillium PRIVATE 
	${CMAKE_CURRENT_BINARY_DIR} # Needed later for headers generated with configure_file
	${CMAKE_CURRENT_SOURCE_DIR} # Relevant for including from src as root when having subfolders in src
 )