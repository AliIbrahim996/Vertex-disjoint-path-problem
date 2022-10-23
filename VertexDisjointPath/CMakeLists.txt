cmake_minimum_required(VERSION 3.15)

include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

### Static Library ###

include("../cmakeHelper/libTemplates.cmake")

FILE(GLOB_RECURSE HEADERS include/*.h include/*.hpp)
FILE(GLOB_RECURSE SOURCES src/*.cpp src/*.h src/*.hpp)

generate_default_lib_target(SOURCES ${HEADERS} ${SOURCES})

# include-directories (for usage with '<>')
target_include_directories(${CONAN_PACKAGE_NAME} PRIVATE src include PUBLIC include)

# settings for IDE-visualisation
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} ${HEADERS})