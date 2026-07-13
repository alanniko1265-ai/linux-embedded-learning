#include <stdio.h>

#include "calc.h"

int main(void)
{
    int a = 24;
    int b = 6;

    printf("CMake calc tool\n");
    printf("%d + %d = %d\n", a, b, add(a, b));
    printf("%d - %d = %d\n", a, b, sub(a, b));
    printf("%d * %d = %d\n", a, b, mul(a, b));
    printf("%d / %d = %d\n", a, b, divide(a, b));

    return 0;
}
