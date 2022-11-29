from conans import ConanFile, CMake, tools

Options = [
    # Cafe
    ("CAFE_INCLUDE_TESTS", [True, False], False),
]


class CafeCoreConan(ConanFile):
    name = "Cafe.Core"
    version = "0.1"
    license = "MIT"
    author = "akemimadoka <chino@hotococoa.moe>"
    url = "https://github.com/akemimadoka/Cafe.Core"
    description = "A general purpose C++ library"
    topics = "C++"
    settings = "os", "compiler", "build_type", "arch"
    options = {opt[0]: opt[1] for opt in Options}
    default_options = {opt[0]: opt[2] for opt in Options}

    python_requires = "CafeCommon/0.1"

    generators = "cmake"

    exports_sources = "CMakeLists.txt", "CafeCommon*", "src*", "Test*"

    def requirements(self):
        if self.options.CAFE_INCLUDE_TESTS:
            self.requires("catch2/3.2.0", private=True)

    def configure_cmake(self):
        cmake = CMake(self)
        for opt in Options:
            cmake.definitions[opt[0]] = getattr(self.options, opt[0])
        cmake.configure()
        return cmake

    def package(self):
        with tools.vcvars(self.settings, filter_known_paths=False) if self.settings.compiler == "Visual Studio" else tools.no_op():
            cmake = self.configure_cmake()
            cmake.install()

    def package_info(self):
        self.python_requires["CafeCommon"].module.addCafeSharedCompileOptions(self)
        # 见 Cafe::Core::Misc::MakeSingleParamTemplate 注释
        if self.settings.compiler == "clang" or self.settings.compiler == "apple-clang":
            self.cpp_info.cxxflags = [ "-frelaxed-template-template-args" ]

    def package_id(self):
        self.info.header_only()
