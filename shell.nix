{ nixpkgs ? import (
  fetchTarball {
    name = "nixpkgs-21.11";
    url = "https://github.com/nixos/nixpkgs/archive/" +
      "0c0f340c22b504ceba6e646c68b9abae5e716eca.tar.gz";
    sha256 = "0ciz08wrjdgg6g2gnzjwc67p86fi7dz1qi1hgsl3np9ikr9h65vh";
  }) {}
}:

with nixpkgs;

nixpkgs.mkShell {
  buildInputs = [
    # cpu/memory profiling
    pprof
    graphviz
    gperftools

    # c/c++ tooling, libs, pkgs
    cling                   # c/cpp repl
    cmake                   # meta buildsystem
    llvmPackages_12.clang   # core header libs
    llvmPackages_12.llvm    # llvm-symbolizer
    llvmPackages_12.openmp  # openmp

    # graphics
    xorg.libX11
    xorg.libXext
    xorg.xorgserver
    xquartz # darwin

    # apple specific
    darwin.apple_sdk.frameworks.CoreFoundation
    darwin.apple_sdk.frameworks.CoreServices
  ];
  # Automate process of extracting correct paths to binaries. This (somewhat)
  # hacky approach works despite the fact that there might be multiple different
  # versions of the pkg of interest, as each version has its own unique hash.
  shellHook = ''
    clang_dir=$(ls -l /nix/store/ | grep ${llvmPackages_12.clang.pname + "-" +
      llvmPackages_12.clang.version} | grep '^d' | awk '/ / {print $NF}')
    export PATH="/nix/store/$clang_dir/bin:$PATH"

    # HACK: Nix (intentionally) adds the `-no-uuid` flag to skip generation of
    # the load command LC_UUID from the final binary (for reproducible builds).
    # We re-enable the flag (by deleting the `libc-ldflags-before` file) in
    # order to properly symbolize our code (for debugging purposes).
    binutils_dir=$(ls -l /nix/store/ | grep ${stdenv.cc.bintools.pname + "-" +
      stdenv.cc.bintools.version} | grep '^d' | awk '/ / {print $NF}')
    if [ -e /nix/store/$binutils_dir/nix-support/libc-ldflags-before ]; then
      sudo rm /nix/store/$binutils_dir/nix-support/libc-ldflags-before
    fi;

    pprof_dir=$(ls -l /nix/store/ | grep ${pprof.pname + "-" + pprof.version} \
      | grep '^d' | awk '/ / {print $NF}')
    export PATH="/nix/store/$pprof_dir/bin:$PATH"

    xquartz_dir=$(ls -l /nix/store/ | grep ${xquartz.pname + "-" +
      xquartz.version} | grep '^d' | awk '/ / {print $NF}')
    if [ "$(uname)" = "Darwin" -a -f /nix/store/$xquartz_dir/etc/X11/fonts.conf ]; then
      export FONTCONFIG_FILE=/nix/store/$xquartz_dir/etc/X11/fonts.conf
    fi
  '';
}
