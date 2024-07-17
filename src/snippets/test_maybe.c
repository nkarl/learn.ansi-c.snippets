#include "test_maybe.h"
#include <stdio.h>

MAKE_NEWTYPE(int)

void test_make_maybe(void) {

    Maybe(int) a = Unit(Maybe(int))(2);
    __show_obj(a);
    Maybe(int) b = Unit(Maybe(int))(0);
    __show_obj(b);
}

int add(int x, int y) { return x + y; }

void test_fmap(void) {
    Maybe(int) mx = Unit(Maybe(int))(2);
    Maybe(int) my = fmap(int, (*add), 2, mx);
    __show_obj(my);
    int z = fmap2((*add), mx.__data, 2);
    printf("%d\n", z);
}

void test_maybe(void) {
    test_make_maybe();
    test_fmap();
}
