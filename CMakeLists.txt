cmake_minimum_required(VERSION 3.16)
project(AlgoLab VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release)
endif()

# --- The algorithm core: plain C++17, no dependencies, builds with any compiler ---------------
add_library(algolab_core STATIC
  core/src/Graph.cpp
  core/src/Trace.cpp
  core/src/FlowNetwork.cpp
  core/src/Registry.cpp
  core/src/algorithms/Bfs.cpp
  core/src/algorithms/Dfs.cpp
  core/src/algorithms/CycleDetection.cpp
  core/src/algorithms/BestFirst.cpp
  core/src/algorithms/MaxFlow.cpp
  core/src/algorithms/VertexDisjointPaths.cpp)
target_include_directories(algolab_core PUBLIC core/include)
if(MSVC)
  target_compile_options(algolab_core PRIVATE /W4 /utf-8)
else()
  target_compile_options(algolab_core PRIVATE -Wall -Wextra)
endif()

# --- Tests (native, or run through node when built with Emscripten) ----------------------------
enable_testing()
add_executable(algolab_tests tests/tests.cpp)
target_link_libraries(algolab_tests PRIVATE algolab_core)
add_test(NAME algolab_tests COMMAND algolab_tests)

# --- The WebAssembly module used by the web app ------------------------------------------------
if(EMSCRIPTEN)
  add_executable(algolab wasm/api.cpp)
  target_link_libraries(algolab PRIVATE algolab_core)
  target_link_options(algolab PRIVATE
    -sMODULARIZE=1
    -sEXPORT_NAME=createAlgoLab
    -sENVIRONMENT=web,node
    -sSINGLE_FILE=1
    -sALLOW_MEMORY_GROWTH=1
    # The searches recurse once per vertex and ccall puts the request text on the stack;
    # the 64KB default is too small for the largest graphs the core accepts.
    -sSTACK_SIZE=1048576
    -sEXPORTED_RUNTIME_METHODS=ccall
    --no-entry)
  set_target_properties(algolab PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/web/wasm)
endif()
