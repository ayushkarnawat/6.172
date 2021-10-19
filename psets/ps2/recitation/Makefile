CC := clang
CXX := clang++
LDFLAGS := # -lrt

ifeq ($(DEBUG),1)
CFLAGS := -Wall -g -Og -fno-omit-frame-pointer -fsanitize=address
LDFLAGS += -fsanitize=address -lprofiler
else
CFLAGS := -Wall -O3 -DNDEBUG
endif

all: isort sum

isort.o: isort.c
	$(CC) $(CFLAGS) -c isort.c

qsort.o: qsort.c
	$(CC) $(CFLAGS) -c qsort.c

isort: isort.o qsort.o
	$(CC) -o isort isort.o qsort.o $(LDFLAGS)
ifeq ($(DEBUG),1)
	dsymutil isort
	CPUPROFILE=isort.prof ./isort 10000 10
	PPROF_BINARY_PATH=./
	pprof -list '.*' isort.prof
endif

sum.o: sum.c
	$(CC) $(CFLAGS) -c sum.c

sum: sum.o
	$(CC) -o sum sum.o $(LDFLAGS)
ifeq ($(DEBUG),1)
	dsymutil sum
	CPUPROFILE=sum.prof ./sum
	PPROF_BINARY_PATH=./
	pprof -list '.*' sum.prof
endif

clean:
	rm -rf isort sum *.o *.prof *.dSYM *.dylib perf.data */perf.data cachegrind.out.*
