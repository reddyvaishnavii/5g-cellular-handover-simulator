cmake_minimum_required(VERSION 3.20)

project(CellularHandoverSimulator
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(cellular_simulator
    src/main.cpp
    src/Cell.cpp
    src/UserEquipment.cpp
    src/HandoverManager.cpp
)

target_include_directories(cellular_simulator
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# -------------------------
# Tests
# -------------------------

enable_testing()

add_executable(test_handover
    ../tests/test_handover.cpp
    src/Cell.cpp
    src/UserEquipment.cpp
    src/HandoverManager.cpp
)

target_include_directories(test_handover
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_test(
    NAME HandoverTests
    COMMAND test_handover
)