set(CMAKE_PLATFORM ARM)
set(CMAKE_SYSTEM Generic)
set(target_triple arm-none-eabi)

set(CMAKE_ASM_COMPILER ${target_triple}-as CACHE FILEPATH
    "Assembler for the target platform")
set(CMAKE_LINKER ${target_triple}-ld CACHE FILEPATH
    "Linker for the target platform")
set(CMAKE_AR ${target_triple}-ar CACHE FILEPATH
    "AR for the target platform")
set(CMAKE_RANLIB ${target_triple}-ranlib CACHE FILEPATH
    "ranlib for the target platform")
