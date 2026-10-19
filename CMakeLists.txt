cmake_minimum_required(VERSION 3.14)

project(delta-crypt CXX)


set(CMAKA_CXX_STANDARD 17)
set(CMAKE_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    sodium
    GIT_REPOSITORY https://github.com/jedisct1/libsodium.git
    GIT_TAG 1.0.22-RELEASE
)

FetchContent_Declare(
    cli11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG  v2.7.2
)

FetchContent_MakeAvailable(sodium cli11)

add_executable(delta-crypt src/main.cpp)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SODIUM REQUIRED IMPORTED_TARGET libsodium)

target_link_libraries(delta-crypt PRIVATE sodium CLI11::CLI11)

include(GNUInstallDirs)

install(TARGETS delta-crypt
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
