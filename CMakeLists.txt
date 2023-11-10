cmake_minimum_required(VERSION 3.8)

project(Chip8Emulator LANGUAGES CXX)

# Download automatically, you can also just copy the conan.cmake file
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://raw.githubusercontent.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
                 "${CMAKE_BINARY_DIR}/conan.cmake" 
                 TLS_VERIFY ON)
endif()
include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_run(CONANFILE conanfile.txt
                BASIC_SETUP CMAKE_TARGETS
                BUILD missing)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})


# MAIN EXECUTABLE

set(PROJ_CPPS src/emulator.cpp)
set(PROJ_HS include/emulator.h include/irandom_generator.h include/iinput_command.h include/gamefile.h
include/keys.h include/input_type.h)

set(PROJ_HS_SDL include/sdl/emulator_random_generator.h include/sdl/sdl_input.h include/sdl/loader.h)
set(PROJ_CPPS_SDL src/sdl/emulator_random_generator.cpp src/sdl/sdl_input.cpp src/sdl/loader.cpp)

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

set(TEST_SRCS tests/main.cpp tests/test_emulator.cpp tests/mocks.h) 

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


add_test(NAME Chip8Tests COMMAND Chip8Tests -x)

