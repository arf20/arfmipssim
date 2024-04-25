# arfmipssim

Simulator (emulator and debugger) for UM ETC base MIPS-based CPU

## Build

Dependencies: SDL2

```
mkdir build && cd build
cmake ..
make
```

## Design

```
main.c:     Entry point, options, calling
loader.c:   Loads initial machine state from segment files
emulator.c: Attach segments (memory) to emulator,
            machine state (memory, registers),
            instruction interpreter functions
debugger.c  CLI for running programs, watching state, setting breakpoints, etc
graphic.c   Graphically visualizes machine state and key commands for debugger
```
