cmake_minimum_required(VERSION 3.23)

project(Chip8Emulator LANGUAGES CXX)

# MAIN EXECUTABLE

set(PROJ_CPPS src/emulator.cpp)
set(PROJ_HS include/emulator.h include/irandom_generator.h include/iinput_command.h include/gamefile.h
include/keys.h include/input_type.h include/renderer.h include/export.h)

set(PROJ_HS_SDL include/sdl/emulator_random_generator.h include/sdl/sdl_input.h include/sdl/loader.h
include/sdl/sdl_emu_renderer.h)
set(PROJ_CPPS_SDL src/sdl/emulator_random_generator.cpp src/sdl/sdl_input.cpp src/sdl/loader.cpp
src/sdl/sdl_emu_renderer.cpp)

set(PROJ_SRCS ${PROJ_CPPS} ${PROJ_HS} ${PROJ_HS_SDL} ${PROJ_CPPS_SDL})

add_executable(Chip8Emulator src/main.cpp ${PROJ_SRCS})
target_compile_definitions(Chip8Emulator PRIVATE $<$<CONFIG:Debug>:DEBUG_BUILD>)

set_property(TARGET Chip8Emulator PROPERTY CXX_STANDARD 20)

target_include_directories(Chip8Emulator PUBLIC src include)

find_package(SDL2 CONFIG REQUIRED)
if(TARGET SDL2::SDL2)
	target_link_libraries(Chip8Emulator SDL2::SDL2)
	message(STATUS "SDL2 successfully linked")
else()
	message(WARNING "SDL2 not found")
endif()

set (RESOURCES_DIR ${PROJECT_SOURCE_DIR}/resources)
add_custom_command(
    TARGET Chip8Emulator POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${RESOURCES_DIR} $<TARGET_FILE_DIR:Chip8Emulator>/resources
    )

# BUILD TESTS

enable_testing()

project(Chip8Tests LANGUAGES CXX)

set(TEST_SRCS tests/main.cpp tests/test_emulator.cpp) 

add_executable(Chip8Tests ${PROJ_SRCS} ${TEST_SRCS})

# exposes test methods and shuts off prints while testing
target_compile_definitions(Chip8Tests PUBLIC EMU_TEST)

set_property(TARGET Chip8Tests PROPERTY CXX_STANDARD 20)

target_include_directories(Chip8Tests PUBLIC include src tests)

if(TARGET SDL2::SDL2)
	target_link_libraries(Chip8Tests SDL2::SDL2)
	message(STATUS "SDL2 successfully linked")
else()
	message(WARNING "SDL2 not found")
endif()

find_package(clove-unit REQUIRED)
if(TARGET clove-unit::clove-unit)
	target_link_libraries(Chip8Tests clove-unit::clove-unit)
	message(STATUS "clove unit successfully linked")
else()
	message(WARNING "clove unit not found")
endif()

add_test(NAME Chip8Tests COMMAND Chip8Tests -x)

# BUILD clean libs and export header files

set(LIB_SRC ${PROJ_CPPS} ${PROJ_HS})

add_library(Chip8LibDLL SHARED ${LIB_SRC})
add_library(Chip8LibLIB STATIC ${LIB_SRC})

set_property(TARGET Chip8LibDLL PROPERTY CXX_STANDARD 20)
set_property(TARGET Chip8LibLIB PROPERTY CXX_STANDARD 20)

target_include_directories(Chip8LibDLL PUBLIC include)
target_include_directories(Chip8LibLIB PUBLIC include)