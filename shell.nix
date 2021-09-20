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

# let
#   cling = ( let
#     unwrapped = stdenv.mkDerivation rec {
#       pname = "cling-unwrapped";
#       version = "0.7";
# 
#       src = fetchgit {
#         url = "http://root.cern/git/clang.git";
#         # This commit has the tag cling-0.7 so we use it, even though cpt.py
#         # tries to use refs/tags/cling-patches-rrelease_50
#         rev = "354b25b5d915ff3b1946479ad07f3f2768ea1621";
#         branchName = "cling-patches";
#         sha256 = "0q8q2nnvjx3v59ng0q3qqqhzmzf4pmfqqiy3rz1f3drx5w3lgyjg";
#       };
# 
#       clingSrc = fetchgit {
#         url = "http://root.cern/git/cling.git";
#         rev = "70163975eee5a76b45a1ca4016bfafebc9b57e07";
#         sha256 = "1mv2fhk857kp5rq714bq49iv7gy9fgdwibydj5wy1kq2m3sf3ysi";
#       };
# 
#       preConfigure = ''
#         echo "add_llvm_external_project(cling)" >> tools/CMakeLists.txt
#         cp -r $clingSrc ./tools/cling
#         chmod -R a+w ./tools/cling
#       '';
# 
#       nativeBuildInputs = [ python3 git cmake ];
#       buildInputs = [ libffi llvmPackages_10.llvm zlib ];
# 
#       cmakeFlags = [
#         "-DLLVM_TARGETS_TO_BUILD=host;NVPTX"
#         "-DLLVM_ENABLE_RTTI=ON"
# 
#         # Setting -DCLING_INCLUDE_TESTS=ON causes the cling/tools targets to be
#         # built; see cling/tools/CMakeLists.txt
#         "-DCLING_INCLUDE_TESTS=ON"
#       ];
# 
#       meta = with lib; {
#         description = "The Interactive C++ Interpreter";
#         homepage = "https://root.cern/cling/";
#         license = with licenses; [ lgpl21 ncsa ];
#         maintainers = with maintainers; [ thomasjm ];
#         platforms = platforms.unix;
#       };
#     };
# 
#     # The flags passed to the wrapped cling should
#     # a) prevent it from searching for system include files and libs, and
#     # b) provide it with the include files and libs it needs (C and C++ standard
#     #     library)
# 
#     # These are also exposed as cling.flags/cling.compilerIncludeFlags because
#     # it's handy to be able to pass them to tools that wrap Cling, particularly
#     # Jupyter kernels such as xeus-cling and the built-in jupyter-cling-kernel.
#     # Both of these use Cling as a library by linking against
#     # libclingJupyter.so, so the makeWrapper approach to wrapping the binary
#     # doesn't work. Thus, if you're packaging a Jupyter kernel, you either need
#     # to pass these flags as extra args to xcpp (for xeus-cling) or put them in
#     # the environment variable CLING_OPTS (for jupyter-cling-kernel)
#     flags = [
#       "-nostdinc"
#       "-nostdinc++"
#       "-isystem" "${lib.getDev stdenv.cc.libc}/include"
#       "-I" "${unwrapped}/include"
#       "-I" "${unwrapped}/lib/clang/10.0.1/include"
#       # "-I" "${unwrapped}/lib/clang/5.0.2/include"
#     ];
# 
#     # Autodetect the include paths for the compiler used to build Cling, in the
#     # same way Cling does at
#     # https://github.com/root-project/cling/blob/v0.7/lib/Interpreter/CIFactory.cpp#L107:L111
#     # Note: it would be nice to just put the compiler in Cling's PATH and let it
#     # do this by itself, but unfortunately passing -nostdinc/-nostdinc++
#     # disables Cling's autodetection logic.
#     compilerIncludeFlags = runCommandNoCC "compiler-include-flags.txt" {} ''
#       export LC_ALL=C
#       ${stdenv.cc}/bin/c++ -xc++ -E -v /dev/null 2>&1 | sed -n -e '/^.include/,''${' -e '/^ \/.*++/p' -e '}' > tmp
#       sed -e 's/^/-isystem /' -i tmp
#       tr '\n' ' ' < tmp > $out
#     '';
#   in
#     runCommand "cling-${unwrapped.version}" {
#       buildInputs = [ makeWrapper ];
#       inherit unwrapped flags compilerIncludeFlags;
#       inherit (unwrapped) meta;
#     } ''
#       makeWrapper $unwrapped/bin/cling $out/bin/cling \
#         --add-flags "$(cat "$compilerIncludeFlags")" \
#         --add-flags "$flags"
#     ''
#   );
# in

# let
#   valgrind = stdenv.mkDerivation rec {
#     name = "valgrind-3.16.1";
# 
#     src = fetchurl {
#       url = "https://sourceware.org/pub/valgrind/${name}.tar.bz2";
#       sha256 = "1jik19rcd34ip8a5c9nv5wfj8k8maqb8cyclr4xhznq2gcpkl7y9";
#     };
# 
#     outputs = [ "out" "dev" "man" "doc" ];
# 
#     hardeningDisable = [ "stackprotector" ];
# 
#     # GDB is needed to provide a sane default for `--db-command'.
#     # Perl is needed for `callgrind_{annotate,control}'.
#     buildInputs = [ gdb perl ] ++ lib.optionals (stdenv.isDarwin) [
#       darwin.bootstrap_cmds
#       darwin.xnu
#     ];
# 
#     # Perl is also a native build input.
#     nativeBuildInputs = [ perl ];
# 
#     enableParallelBuilding = true;
#     separateDebugInfo = stdenv.isLinux;
# 
#     preConfigure = lib.optionalString stdenv.isDarwin (
#       let OSRELEASE = ''
#         $(awk -F '"' '/#define OSRELEASE/{ print $2 }' \
#         <${darwin.xnu}/Library/Frameworks/Kernel.framework/Headers/libkern/version.h)'';
#       in ''
#         echo "Don't derive our xnu version using uname -r."
#         substituteInPlace configure --replace "uname -r" "echo ${OSRELEASE}"
#         # Apple's GCC doesn't recognize `-arch' (as of version 4.2.1, build 5666).
#         echo "getting rid of the \`-arch' GCC option..."
#         find -name Makefile\* -exec \
#           sed -i {} -e's/DARWIN\(.*\)-arch [^ ]\+/DARWIN\1/g' \;
#         sed -i coregrind/link_tool_exe_darwin.in \
#             -e 's/^my \$archstr = .*/my $archstr = "x86_64";/g'
#         substituteInPlace coregrind/m_debuginfo/readmacho.c \
#           --replace /usr/bin/dsymutil ${stdenv.cc.bintools.bintools}/bin/dsymutil
#         echo "substitute hardcoded /usr/bin/ld with ${darwin.cctools}/bin/ld"
#         substituteInPlace coregrind/link_tool_exe_darwin.in \
#           --replace /usr/bin/ld ${darwin.cctools}/bin/ld
#       '');
# 
#     # To prevent rebuild on linux when moving darwin's postPatch fixes to preConfigure
#     postPatch = "";
# 
#     configureFlags =
#       lib.optional (stdenv.hostPlatform.system == "x86_64-linux" || stdenv.hostPlatform.system == "x86_64-darwin") "--enable-only64bit"
#       ++ lib.optional stdenv.hostPlatform.isDarwin "--with-xcodedir=${darwin.xnu}/include";
# 
#     doCheck = false; # fails
# 
#     postInstall = ''
#       for i in $out/lib/valgrind/*.supp; do
#         substituteInPlace $i \
#           --replace 'obj:/lib' 'obj:*/lib' \
#           --replace 'obj:/usr/X11R6/lib' 'obj:*/lib' \
#           --replace 'obj:/usr/lib' 'obj:*/lib'
#       done
#     '';
# 
#     meta = with lib; {
#       homepage = "http://www.valgrind.org/";
#       description = "Debugging and profiling tool suite";
# 
#       longDescription = ''
#         Valgrind is an award-winning instrumentation framework for
#         building dynamic analysis tools.  There are Valgrind tools that
#         can automatically detect many memory management and threading
#         bugs, and profile your programs in detail.  You can also use
#         Valgrind to build new tools.
#       '';
# 
#       license = licenses.gpl2Plus;
# 
#       maintainers = [ maintainers.eelco ];
#       platforms = platforms.unix;
#       badPlatforms = [
#         "armv5tel-linux" "armv6l-linux" "armv6m-linux"
#         "sparc-linux" "sparc64-linux"
#         "riscv32-linux" "riscv64-linux"
#         "alpha-linux"
#       ];
#     };
#   };
# in

let
  pprof = buildGoPackage rec {
    # NOTE: We are building an older version of pprof instead of the latest HEAD
    # as that version cannot be built (errors with cgo/SWIG not being available
    # when testing compilation of C files). More details: https://git.io/Jzvzn
    pname = "pprof-unstable";
    version = "2020-12-17";
    rev = "b9804c9f04c2d0c7bbd1af5129f853563f990541";
    # version = "2021-08-27";
    # rev = "02619b876842e0d0afb5e5580d3a374dad740edb";

    # nativeBuildInputs = [ swig4 cgo ];
    goPackagePath = "github.com/google/pprof";

    src = fetchgit {
      inherit rev;
      url = "git://github.com/google/pprof";
      sha256 = "0znp0z2450l7cg7k98l7b4klp5ncx03619clxzb7ld8cdqc6l4dc";
      # sha256 = "0j9y1a8hcbmd6rk0lmlqlxyzbnn9a0qnfh3g78cyip3wk9z47p0d";
    };

    goDeps = ./deps.nix;

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
    llvmPackages_11.llvm # llvm-symbolizer
    llvmPackages_11.clang # link core header libs
    darwin.apple_sdk.frameworks.CoreFoundation
    darwin.apple_sdk.frameworks.CoreServices
  ];
  # Automate process of extracting correct paths to binaries. This (somewhat)
  # hacky approach works despite the fact that there might be multiple different
  # versions of the pkg of interest, as each version has its own unique hash.
  shellHook = ''
    clang_dir=$(ls -l /nix/store/ | grep ${llvmPackages_11.clang.pname + "-" +
      llvmPackages_11.clang.version} | grep '^d' | awk '/ / {print $NF}')
    export PATH="/nix/store/$clang_dir/bin:$PATH"

    # Use latest pprof (github HEAD)
    # NOTE: to delete go pkgs: `go get pprof@none`, then delete $HOME/go dir
    # export GOPATH="$HOME/go/"
    # go get -u github.com/google/pprof
    # export PATH="$GOPATH/bin:$PATH"

    pprof_dir=$(ls -l /nix/store/ | grep ${pprof.pname + "-" + pprof.version} \
      | grep '^d' | awk '/ / {print $NF}')
    export PATH="/nix/store/$pprof_dir/bin:$PATH"
  '';
}
