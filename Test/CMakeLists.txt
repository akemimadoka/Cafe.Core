set(SOURCE_FILES
    BitField.Test.cpp
    Functions.Test.cpp
    Main.cpp
    TypeTraits.Test.cpp
    Utility.Test.cpp
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    list(REMOVE_ITEM SOURCE_FILES Functions.Test.cpp)
endif()

add_executable(Cafe.Core.Test ${SOURCE_FILES})

target_link_libraries(Cafe.Core.Test PRIVATE
    CONAN_PKG::catch2 Cafe.Core)

ParseAndAddCatchTests(Cafe.Core.Test)
