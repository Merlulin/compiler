#include "defs.h"
#include "data.h"
#include "decl.h"

// 杂项功能

void lbrace(void) {
  match(T_LBRACE, "{");
}

void rbrace(void) {
  match(T_RBRACE, "}");
}

void lparen(void) {
  match(T_LPAREN, "(");
}

void rparen(void) {
  match(T_RPAREN, ")");
}

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

// 匹配是否是标识符
void ident(void) {
    match(T_IDENT, "identifier");
}


// fatal 报错信息函数
void fatal(char *s) {
    fprintf(stderr, "Error: %s on line %d\n", s, Line); exit(1);
}

void fatals(char *s1, char *s2) {
  fprintf(stderr, "Error: %s:%s on line %d\n", s1, s2, Line); exit(1);
}

void fatald(char *s, int d) {
  fprintf(stderr, "Error: %s:%d on line %d\n", s, d, Line); exit(1);
}

void fatalc(char *s, int c) {
  fprintf(stderr, "Error: %s:%c on line %d\n", s, c, Line); exit(1);
}