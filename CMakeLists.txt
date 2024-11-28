# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(mlog)

# Set the C standard (optional, but recommended)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Include directories for headers
include_directories(
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/port
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}
)

# Define source files
set(SOURCES
    src/mlog.c
    port/mlog_port.c
    main.c
)

# Define the executable target
add_executable(mlog_app ${SOURCES})

# Optional: Set C flags (e.g., add debugging flags or warnings)
target_compile_options(mlog_app PRIVATE -Wall -Werror)

