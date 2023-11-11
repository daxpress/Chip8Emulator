from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMake

class Chip8EmulatorRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"
    build_policy = "missing"

    def requirements(self):
        self.requires("sdl/2.28.5")

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()