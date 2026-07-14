#include <stdio.h>

#include "calc.h"

int main(void)
{
    int a = 12;
    int b = 4;

    printf("calc tool\n");
    printf("%d + %d = %d\n", a, b, add(a, b));
    printf("%d - %d = %d\n", a, b, sub(a, b));
    printf("%d * %d = %d\n", a, b, mul(a, b));
    printf("程序已完成");
    return 0;
}
