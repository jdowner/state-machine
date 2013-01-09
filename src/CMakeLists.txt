set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

set(SRC main.cpp State.h Context.h)

add_definitions("-std=c++0x")

add_executable(state-machine ${SRC})
