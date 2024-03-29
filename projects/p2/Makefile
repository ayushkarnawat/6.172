# Makefile for screensaver
# Copyright (c) 2012 the Massachusetts Institute of Technology
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.


# This Makefile is set up to build screensaver in one of two modes.
# In debug mode, the C compiler does virtually no optimizations
# (-O0) and embeds a bunch of debugging symbols to make work with GDB easier.
# In release mode, the compiler will do every optimization it can (-O3) and keep
# debugging symbols out; this should decrease the size of the final binary and
# make it perform better.
#
# To compile in debug mode, type "make DEBUG=1".  To to compile in release
# mode, type "make DEBUG=0" or simply "make".
#
# If you type "make prof", Make will instrument the output for profiling with
# gprof.  Be sure you run "make clean" first!
#
# If everything gets wacky and you need a sane place to start from, you can
# type "make clean", which will remove all compiled code.
#
# If you want to do something wacky with your compiler flags--like enabling
# debug symbols but keeping optimizations on--you can specify CXXFLAGS or
# LDFLAGS on the command line.  If you want to use a predefined mode but augment
# the predefined CXXFLAGS or LDFLAGS, you can specify EXTRA_CXXFLAGS or
# EXTRA_LDFLAGS on the command line.


# The sources we're building
HEADERS = $(wildcard include/*.h src/*.h)
PRODUCT_SOURCES = $(wildcard src/*.c)

# What we're building
PRODUCT_OBJECTS = $(PRODUCT_SOURCES:.c=.o)
PRODUCT = screensaver
PROFILE_PRODUCT = $(PRODUCT:%=%.prof) # the product, instrumented for gprof

# What we're building with
CXX = clang
CXXFLAGS = -std=gnu99 -Wall -m64 -I include/
LDFLAGS = -lm #-lrt

# Determine which profile--debug or release--we should build against, and set
# CFLAGS appropriately.
ifeq ($(DEBUG),1)
CXXFLAGS += -g -O0 -gdwarf-3
else
CXXFLAGS += -O3 -DNDEBUG
endif

# Make graphics optional; users might not have X11 installed and/or they don't
# want to increase size of the binary if it not going to be used.
ifeq ($(GRAPHICS),1)
HEADERS += $(filter-out extras/cilktool.h, $(wildcard extras/*.h))
PRODUCT_OBJECTS += extras/graphic_stuff.o
CXXFLAGS += -I extras/
LDFLAGS += -lXext -lX11
else
CXXFLAGS += -DPROFILE_BUILD
endif

# By default, make the product.
all: $(PRODUCT)

# How to build for profiling
prof: $(PROFILE_PRODUCT)

lint:
	python ../../clint.py *.h *.c

# How to clean up
clean:
	$(RM) -rf $(PRODUCT) $(PROFILE_PRODUCT) src/*.o extras/*.o *.out *.dSYM *.prof

# How to compile a C file
%.o: %.c $(HEADERS)
	$(CXX) $(CXXFLAGS) $(EXTRA_CXXFLAGS) -o $@ -c $<

# How to link the product
$(PRODUCT):	$(PRODUCT_OBJECTS)
	$(CXX) $(LDFLAGS) $(EXTRA_LDFLAGS) -o $@ $(PRODUCT_OBJECTS)

# How to build the product, instrumented for profiling
$(PROFILE_PRODUCT): CXXFLAGS += -DPROFILE_BUILD -g -fno-omit-frame-pointer
$(PROFILE_PRODUCT): LDFLAGS += -lprofiler
$(PROFILE_PRODUCT): $(PRODUCT_OBJECTS)
	$(CXX)  $(PRODUCT_OBJECTS) $(LDFLAGS) $(EXTRA_LDFLAGS) -o $(PROFILE_PRODUCT)
	dsymutil screensaver.prof
	CPUPROFILE=collisions.prof ./screensaver.prof 1000
	PPROF_BINARY_PATH=./
	pprof -lines '.*' collisions.prof
