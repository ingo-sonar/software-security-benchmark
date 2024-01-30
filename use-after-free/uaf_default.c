#include <stdio.h>
#include <stdlib.h>

// clang -emit-llvm -fno-discard-value-names -S -g doublefree.c

void bar(int *f, void (*fn)(int *)) { fn(f); }

void foo(int *f) { bar(f, foo); }

int main() {
    // __attribute__((annotate("psr.source"))) int *f = (int *)malloc(32); // ptr - ptr -int
    // foo(f);

    int *foo = (int *)malloc(32); // ptr - ptr -int
    free(foo);
    int b = *foo;
    return 0;
}