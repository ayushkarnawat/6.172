# 6.172 Performance Engineering of Software Systems
Introduction to building scalable and high-performance software systems. Topics
include performance analysis, algorithmic techniques for high performance,
instruction-level optimizations, caching optimizations, parallel programming,
and building scalable systems.


## Building
<!-- TODO: Fix this; needs to be more accurate. -->
Each folder contains more detailed info on how to properly build (compile and
link) the source files for each respective assignment and project.
Unfortunately, the make file(s) will only work with *nix-based systems
currently.

## Linting
To automatically make sure if a C file follows Google's C style guide, download
the following [file](https://github.com/sillycross/Leiserchess---MIT-6.172-Fall16-Final-Project/blob/master/clint.py).

The linting tool takes a list of files as input. For full usage instructions,
please see the output of:

    ./clint.py --help

<!-- ## Profiling
We use [valgrind](https://www.valgrind.org) to automatically check for memory
management and threading bugs (aka race conditions) as well as to profile cache misses in order to
improve performance of our programs. For usage information, see the output of:

    valgrind --tool=memcheck --leak-check=yes --show-reachable=yes program_name
    valgrind --help -->

## Acknowledgements
We would like to thank Dr. Charles Leiserson, Dr. Julian Shun, and MIT
opencourseware for providing access to the videos, slides, assignments, and
projects from the
[F2018](https://ocw.mit.edu/courses/electrical-engineering-and-computer-science/6-172-performance-engineering-of-software-systems-fall-2018/index.htm)
version of the class.
