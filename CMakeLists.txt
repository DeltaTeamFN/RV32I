cmake_minimum_required(VERSION 3.16)
project(RV32I C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compile flags for strict safety and optimizations
if(MSVC)
    add_compile_options(/W4 /O2)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -O2)
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/shared
    ${CMAKE_CURRENT_SOURCE_DIR}/engine
    ${CMAKE_CURRENT_SOURCE_DIR}/disassemble
)

# Source files
set(SOURCES
    main.c
    engine/memory.c
    engine/cpu.c
    engine/elf_loader.c
    disassemble/disasm.c
)

add_executable(rv32i ${SOURCES})
