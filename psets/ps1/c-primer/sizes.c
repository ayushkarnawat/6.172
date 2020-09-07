// Copyright (c) 2012 MIT License by 6.172 Staff

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


#define PRINT_SIZE(a, b) printf("size of %s: %zu bytes \n", a, sizeof(b));


int main(int argc, char *argv[]) {
  // Please print the sizes of the following types:
  // int, short, long, char, float, double, unsigned int, long long
  // uint8_t, uint16_t, uint32_t, and uint64_t, uint_fast8_t,
  // uint_fast16_t, uintmax_t, uintmax_t, __int128, and student

  // Here's how to show the size of one type. See if you can define a macro
  // to avoid copy pasting this code.
  // printf("size of %s : %zu bytes \n", "int", sizeof(int));
  // e.g. PRINT_SIZE("int", int);
  //      PRINT_SIZE("short", short);
  PRINT_SIZE("int", int);
  PRINT_SIZE("short", short);
  PRINT_SIZE("long", long);
  PRINT_SIZE("char", char);
  PRINT_SIZE("float", float);
  PRINT_SIZE("double", double);
  PRINT_SIZE("unsigned int", unsigned int);
  PRINT_SIZE("long long", long long);
  PRINT_SIZE("uint8_t", uint8_t);
  PRINT_SIZE("uint16_t", uint16_t);
  PRINT_SIZE("uint32_t", uint32_t);
  PRINT_SIZE("uint64_t", uint64_t);
  PRINT_SIZE("uint_fast8_t", uint_fast8_t);
  PRINT_SIZE("uint_fast16_t", uint_fast16_t);
  PRINT_SIZE("uintmax_t", uintmax_t);
  PRINT_SIZE("uintmax_t", uintmax_t);
  PRINT_SIZE("__int128", __int128);

  // Alternatively, you can use stringification
  // (https://gcc.gnu.org/onlinedocs/cpp/Stringification.html) so that
  // you can write
  // e.g. PRINT_SIZE(int);
  //      PRINT_SIZE(short);

  // Composite types have sizes too.
  typedef struct {
    int id;
    int year;
  } student;

  student you;
  you.id = 12345;
  you.year = 4;

  // Array declaration. Use your macro to print the size of this.
  int x[5];
  PRINT_SIZE("x", x);

  // You can just use your macro here instead: PRINT_SIZE("student", you);
  // printf("size of %s : %zu bytes \n", "student", sizeof(you));
  PRINT_SIZE("student", you);

  return 0;
}
