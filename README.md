# The lyn language

Lyn is a small language targetting resource constrained environments.
Currently the compiler targets the ARM architecture and generates
Thumb instructions.

# Building the project

You will need a binutils crosscompiled for the target architecture
(arm-none-eabi) in order to compile liblyn.
Furthermore you will need a C++17 compatible compiler to build the
compiler itself.
The project is built with CMake.
If you have problems building it: feel free to ask for help.
