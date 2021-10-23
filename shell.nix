# NOTE: We need to specify a commit version in which cling is available but
# valgrind also builds properly. A quick search on nixpkgs reveals that, at the
# time of writing, this is impossible (as the earliest version of cling appears
# as part of the packages after the valgrind package begins failing on
# mac/darwin. Three things are possible:
#
# 1. Specify a (stable) verison of nix which has a working version of valgrind
# and then build cling. There are build issues when running the build script.
# Specifically, we are unable to find a "_mach_msg_destroy" within the
# libcoregrind library (requires some help from upstream developers).
#
# 2. Specify a version with working cling, and rebuilt valgrind. Installing
# cling with the working version of valgrind (on BigSur) results in us not being
# able to find the *.dylib libraries (as they are no longer searchable in the
# system). An idea: Modify cling script such that it can find dylib files using
# dlopen rather than finding file directly within system.
#
# 3. Install valgrind and forget about cling. We could argue that optimizing
# performance (through cachegrind) is more important than being able to have a
# cpp interpreter for quickly testing things. This is not really a fix as we are
# side stepping having the ability to use both on a BigSur machine, especially
# because they are both are important for a proper development experience.
# Gameplan: Specify differences between old working cling and new one and what
# changed. We may need to (re)build an old version of llvm with dev (among other
# pkgs) as cling requies this.

{ nixpkgs ? import (
  fetchTarball {
    name = "darwin-21.08";
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
  name = "cpp-shell";
  buildInputs = [
    # cpu/memory profiling
    pprof
    graphviz
    gperftools
    cling # c/cpp shell
    llvmPackages_12.llvm # llvm-symbolizer
    llvmPackages_12.clang # core header libs
    # apple specific libs
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
  '';
}
