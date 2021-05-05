from conans import ConanFile, CMake, tools

Options = [
    # Cafe
    ("CAFE_INCLUDE_TESTS", [True, False], False),
]


class CafeConan(ConanFile):
    name = "Cafe"
    version = "0.1"
    license = "MIT"
    author = "akemimadoka <chino@hotococoa.moe>"
    url = "https://github.com/akemimadoka/Cafe.Core"
    description = "A general purpose C++ library"
    topics = "C++"
    settings = "os", "compiler", "build_type", "arch"
    options = {opt[0]: opt[1] for opt in Options}
    default_options = {opt[0]: opt[2] for opt in Options}

    generators = "cmake"

    exports_sources = "CMakeLists.txt", "src", "Test"

    def requirements(self):
        if self.options.CAFE_INCLUDE_TESTS:
            self.requires("catch2/3.0.0@catchorg/stable", private=True)

    def configure_cmake(self):
        cmake = CMake(self)
        for opt in Options:
            cmake.definitions[opt[0]] = getattr(self.options, opt[0])
        cmake.configure()
        return cmake

    def build(self):
        with tools.vcvars(self.settings, filter_known_paths=False) if self.settings.compiler == 'Visual Studio' else tools.no_op():
            cmake = self.configure_cmake()
            cmake.build()

    def package(self):
        with tools.vcvars(self.settings, filter_known_paths=False) if self.settings.compiler == 'Visual Studio' else tools.no_op():
            cmake = self.configure_cmake()
            cmake.install()

    def package_id(self):
        self.info.header_only()
