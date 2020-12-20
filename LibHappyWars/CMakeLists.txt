add_subdirectory(zlib)

add_library(LibHappyWars STATIC
    BFBR.cpp
    Blowfish/blowfish.cpp
    )

target_include_directories(LibHappyWars PUBLIC SYSTEM . ${CMAKE_SOURCE_DIR} zlib)
target_link_libraries(LibHappyWars PUBLIC LibFruit zlibstatic)
