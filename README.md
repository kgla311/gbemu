Game Boy Emulator (C++ / SDL2)

Status: Work in Progress

This is a Game Boy emulator written entirely in modern C++ with SDL2.
It is a learning project focused on low-level system emulation and demonstrates strong understanding of CPU architecture, memory management, and timing.

Current Features

CPU emulation: All documented Game Boy instructions implemented.

Instruction decoder: Handles hundreds of opcodes.

Memory subsystem (MMU): Correct mapping of Game Boy memory regions.

Interrupts: Basic interrupt handling (V-Blank, timer, etc.).

Graphics (PPU): Initial rendering pipeline with SDL2 output. Planned improvements include sprites, scrolling, and polishing the pipeline.

RAII resource management: SDL2 wrapped in safe, exception-resistant classes.

Status

Emulator runs most test ROMs successfully.

Currently halts on Blaarg’s timer tests — issue identified in timer implementation.

All debug output is logged in debug.txt.

Fix in progress: refining timer increment/overflow cycle accuracy.

What I Learned

Designing modular software architecture (CPU, MMU, PPU as separate subsystems).

Implementing timers, interrupts, and low-level hardware registers.

Writing clean, maintainable code in a large C++ project.

Debugging hardware behavior by cross-referencing with test ROMs.
