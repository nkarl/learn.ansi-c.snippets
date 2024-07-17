#ifndef MAYBE_H
#define MAYBE_H

#include "macros.h"

#define Maybe(a) CONCAT(__maybe_, a)
#define Unit(a)  CONCAT(__constructor_, a)

#define STRING_VAR(var) #var

#define NOTHING 0
#define JUST    1

#define MAKE_NEWTYPE(a)                  \
    typedef struct {                     \
        char *__typename;                \
        char  __is;                      \
        a     __data;                    \
    } Maybe(a);                          \
    /* define unit constructor */        \
    Maybe(a) Unit(Maybe(a))(a v) {       \
        return (Maybe(a)){               \
            .__typename = "__maybe_" #a, \
            .__is       = JUST,          \
            .__data     = v,             \
        };                               \
    }

/*
 * NOTE: References
 *  - a trampoline on the stack
 *    - https://nullprogram.com/blog/2019/11/15/
 *  - GNU C referencing types using `typeof`
 *    - https://gcc.gnu.org/onlinedocs/gcc/Typeof.html
 *  - Variadic macros
 *    - https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
 *  - C macros tips, tricks and idioms
 *    - https://github.com/pfultz2/Cloak/wiki/C-Preprocessor-tricks,-tips,-and-idioms
 *  - metalang99
 *    - https://github.com/Hirrolot/metalang99/blob/master/include/metalang99/util.h
 *  - libpp
 *    - https://github.com/mcinglis/libpp?tab=readme-ov-file
 *  - Best C macros tutorials
 *    - https://www.reddit.com/r/C_Programming/comments/s4hbvv/best_c_marco_preprocessor_tutorial_book_or_website/
 *  - Asorted sources on C macros
 *    - https://github.com/Hirrolot/awesome-c-preprocessor
 */

/*
 * TODO: https://stackoverflow.com/a/66865994
 *  - implement currying in C, using ASM
 */

// not type safe
#define fmap2(func, ...) (*func)(__VA_ARGS__)

/* fmap :: { (m) (*mf) (m) } <- { b (*f) (a) } */
#define fmap(type, f, b, m) \
    (Unit(Maybe(type))(f(m.__data, b)))

#define __show_obj(a)                                          \
    { fprintf(stderr,                                          \
              "{ .typename=%s, .is=%s, .data=%ld }\n",         \
              a.__typename,                                    \
              a.__is ? STRING_VAR(JUST) : STRING_VAR(NOTHING), \
              (long)a.__data); }

#endif
