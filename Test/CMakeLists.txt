set(SOURCE_FILES
    BitField.Test.cpp
    Functions.Test.cpp
    Main.cpp
    TypeTraits.Test.cpp
    Utility.Test.cpp
)

add_executable(Cafe.Core.Test ${SOURCE_FILES})

target_link_libraries(Cafe.Core.Test PRIVATE
    CONAN_PKG::catch2 Cafe.Core)

catch_discover_tests(Cafe.Core.Test)
