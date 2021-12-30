# 6.172 Performance Engineering of Software Systems
Introduction to building scalable and high-performance software systems. Topics
include performance analysis, algorithmic techniques for high performance,
instruction-level optimizations, caching optimizations, parallel programming,
and building scalable systems.

## Installation

### NixOS (recommended)
To ensure a reproducible environment (e.g. all packages work without build
errors), we recommend you use the Nix environment setup.

1. Install [nix](https://nixos.org/guides/install-nix.html)
2. Activate the environment by running `nix-shell` (or more explicitly
   `nix-shell shell.nix`) from the project root directory. This will create a
   virtual environment with all the necessary pkgs and system information
   (defined in the `shell.nix` file) required to properly build and run each
   executable.

<!-- TODO: Compete -->
### Manual
If Nix is unavailable on your system or you wish to not use it, you have to
download the following packages globally or locally (in a virtual env). Ensure
the path to the binaries are added to your PATH.

```console
cmake
clang

# optional: profiling
llvm (for symbolization)
pprof
gperftools
graphviz

# optional: graphics
x11 (or XQuartz for macos)

# optional: parallel
openmp
```

## Building
To build with default flags, follow the canonical cmake build process,

1. Install dependencies and activate shell env (see Installation)
2. Create build directory: `mkdir build/`
3. Generate OS-specific build files: `cmake -Bbuild -GNinja`
4. Build executables: Run `ninja` for each project/submodule inside `build/`
5. Run each executable

Alternatively, you may wish to (re)compile each project with other flags (e.g.
debug, profiling). To do this globally across all submodules, add the
`-DCMAKE_BUILD_TYPE=Debug` and `-DPROFILE_BUILD=On` flags when generating the
build files, respectively.

For enabling project-specific compilation flags (e.g. X11 graphics), you may
define the flags on the command line as well (see options in each individual
project's CMakeFiles.txt)

## Linting
To automatically make sure if a C file follows Google's C style guide, download
the following [file](https://github.com/sillycross/Leiserchess---MIT-6.172-Fall16-Final-Project/blob/master/clint.py).

The linting tool takes a list of files as input. For full usage instructions,
please see the output of:

> ./clint.py --help

## Notes
Although CilkPlus is now deprecated in favor of OpenCilk, we choose to use
OpenMP for shared-memory parallel programming. The primary reason is
cross-compatabilty (as OpenCilk doesn't work on Windows) and requires
downloading the Tapir/LLVM compiler. Additionally, rather than deal with
low-level instructions (e.g. `pthreads`), OpenMP provides a very simple
interface to parallelize serial code without much overhead.

The code in this repository is (likely) not optimally written or of production
quality. It is currently also not well tested (no extensive unit, integration, or
performance tests).

## Acknowledgements
We would like to thank Dr. Charles Leiserson, Dr. Julian Shun, and MIT
opencourseware for providing access to the videos, slides, assignments, and
projects from the
[F2018](https://ocw.mit.edu/courses/electrical-engineering-and-computer-science/6-172-performance-engineering-of-software-systems-fall-2018/index.htm)
version of the class.
