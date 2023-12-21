#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// token 用枚举类实现
enum {
    T_PLUS, // +
    T_MINUS,  // -
    T_STAR,  // *
    T_SLASH, // /
    T_INTLIT  // int
};

struct token
{
    int token;  
    int intvalue;
};
