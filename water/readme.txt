Water demo
by Alex Nankervis

An ocean water simulation with FFT-animated heightfields, reflections, and fresnel effect. This began as an attempt to implement a projected grid, where all vertices are equally spaced in screen space, but this resulted in a disturbing swimming effect on the height of vertices, and so it was dropped in favor of a traditional heightfield grid.

This demo requires a graphics card supporting the ARB_fragment_program opengl extension (geforce fx, radeon 9500, and newer).


Controls:
W/S - move camera forward/backward
A/D - move camera side to side
mouse - change camera direction
space/control - move camera up/down
ESC - exit