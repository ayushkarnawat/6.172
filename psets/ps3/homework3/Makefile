CC := clang

CFLAGS := -Wall -std=gnu99
EXTRA_CFLAGS ?=
LDFLAGS := # -lrt

ifeq ($(DEBUG),1)
  CFLAGS += -O0
else
  CFLAGS += -O3 -DNDEBUG
endif

ifeq ($(ASSEMBLE),1)
  CFLAGS += -S
endif

ifeq ($(AVX2),1)
  CFLAGS += -mavx2
endif

ifeq ($(RELEASE),0)
  CFLAGS += -g -gdwarf-3      # easier debugging
  CFLAGS += -fno-unroll-loops # turn-off loop unrolling
endif

ifeq ($(VECTORIZE),1)
  CFLAGS += -Rpass=loop-vectorize -Rpass-missed=loop-vectorize \
            -Rpass-analysis=loop-vectorize -ffast-math
else
  CFLAGS += -fno-vectorize
endif

CFLAGS += $(EXTRA_CFLAGS)

# You shouldn't need to touch this.  This keeps track of whether or
# not you've changed CFLAGS.
OLD_CFLAGS := $(shell cat .cflags 2> /dev/null)
ifneq ($(CFLAGS),$(OLD_CFLAGS))
.cflags::
	@echo "$(CFLAGS)" > .cflags
endif


all: loop

%.o: %.c .cflags
	$(CC) $(CFLAGS) -c $<

loop: loop.o
	$(CC) -o loop $^ $(LDFLAGS)

clean:
	rm -f loop *.o *.s .cflags perf.data */perf.data
