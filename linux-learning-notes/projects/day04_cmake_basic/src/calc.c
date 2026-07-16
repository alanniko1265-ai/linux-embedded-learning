#include "calc.h"

int add(int left, int right)
{
    return left + right;
}

int sub(int left, int right)
{
    return left - right;
}

int mul(int left, int right)
{
    return left * right;
}

int divide(int left, int right)
{
    if (right == 0) {
        return 0;
    }

    return left / right;
}
