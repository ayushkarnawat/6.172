## Collision Detection
Optimized collision detector. Computes the number of line-wall and line-line
collisions that occur during a certain number of frames in a screensaver.

### Notes

#### Graphics
As the main objective of the program is to test the performance of different
collion detection algorithms, the graphics is mainly used for visual testing
(e.g. checking whether the positions of the lines after a collision is
plausible/correct).

To that end, and to ensure maximum portability, the windowing library (e.g. X11,
XQuartz, Wayland) is not a required dependency. Instead, if the user wishes to
run the graphics demo, they will need to recompile the following code by
specifying the `GRAPHICS=1` option when using make.

#### Parallel
If available, the code will automatically use OpenMP to speed up the collision
detecton algorithms. Otherwise, we fall back to the serial implementation.

Alternatively, the user has the option to compile just the serial code for
performance metrics purposes (e.g. comparing performance between different
algos, showcasing the speedup achieved by the parallel implementation when
compared to the serial one, and determining which algos benefit most from a
parallel implementation).

### Usage
For detailed information on how to use the program, please run the `screensaver`
executable.

If the user specifies the `-g` flag when running the screensaver (compiled
without graphics enabled), a warning message will be printed to stderr stating
that the executable was not compiled with graphics enabled and the non-GUI demo
will be run instead. To show the graphics interface, please recompile with X11
and rerun the program.

### Tests
Currently, we do not have an extensive test suite to ensure the correctness and
performance of the algorithms. We plan on adding this ASAP.

### TODOs
- [x] Make graphics code optional (can compile/run code without X11)
- [x] Remove parallel Cilk implementation
- [ ] Compile with CMake
- [ ] Discuss ideas/methods/approaches which need to be implemented. In
      particular, highlighty the tradeoffs between performance, runtime and
      space complexity.
- [ ] Add unit + integration + performance test suite
