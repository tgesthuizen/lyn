# The lyn language

Lyn is a small language targetting resource constrained environments.
Currently the compiler targets the ARM architecture and generates
Thumb instructions.

# Building the project

You will need a binutils crosscompiled for the target architecture
(arm-none-eabi) in order to compile liblyn.
Otherwise you will need a C++17 compatible compiler and GNU Bison to
build the compiler itself.
Then you can create a build directory and configure the part of the
project you want to build:
```
cmake .. # For the whole project
cmake ../lync # If you just want to build the compiler
```
