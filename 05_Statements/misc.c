#include "defs.h"
#include "data.h"
#include "decl.h"

// 杂项功能

// 匹配分号，并捕捉下一个
void semi(void) {
    match(T_SEMI, ";");
}

// token和字符串的类型是否匹配
void match(int t, char *what) {
    if (Token.token == t) {
        scan(&Token);
    } else {
        printf("Error: %s expected on line %d\n", what, Line);
        exit(1);
    }
}