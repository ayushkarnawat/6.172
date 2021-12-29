{ nixpkgs ? import (
  fetchTarball {
    name = "nixpkgs-21.08";
    url = "https://github.com/nixos/nixpkgs/archive/" +
      "fbafeb7ad5dd6196fcc5d84264e1706653a62f81.tar.gz";
    sha256 = "1kvx8j1z0xwfqjh33l3plhjc6dkjdg5md6zg667abdi06sg9w8pk";
  }) {}
}:

with nixpkgs;

let
  pprof = buildGoModule rec {
    pname = "pprof";
    version = "unstable-2021-09-30";

    src = fetchFromGitHub {
      owner = "google";
      repo = "pprof";
      rev = "7fe48b4c820be13151ae35ce5a5e3f54f1b53eef";
      sha256 = "05nr3igdigs1586qplwfm17hfw0v81jy745g6vayq7cbplljfjb1";
    };

    vendorSha256 = "0yl8y3m2ia3cwxhmg1km8358a0225khimv6hcvras8r2glm69h3f";
    runVend = false; # no (current) dependencies contain C files

    meta = with lib; {
      description = "A tool for visualization and analysis of profiling data";
      homepage = "https://github.com/google/pprof";
      license = licenses.asl20;
      longDescription = ''
        pprof reads a collection of profiling samples in profile.proto format
        and generates reports to visualize and help analyze the data. It can
        generate both text and graphical reports (through the use of the dot
        visualization package).

        profile.proto is a protocol buffer that describes a set of callstacks
        and symbolization information. A common usage is to represent a set of
        sampled callstacks from statistical profiling. The format is described
        on the proto/profile.proto file. For details on protocol buffers, see
        https://developers.google.com/protocol-buffers

        Profiles can be read from a local file, or over http. Multiple profiles
        of the same type can be aggregated or compared.

        If the profile samples contain machine addresses, pprof can symbolize
        them through the use of the native binutils tools (addr2line and nm).

        This is not an official Google product.
      '';
    };
  };
in
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
