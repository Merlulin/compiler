#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define TEXTLEN 512 // 输入的符号的长度上限
#define NSYMBOLS 1024 // 符号表的条目数


// token 用枚举类实现
enum {
  // op
  T_EOF, T_PLUS, T_MINUS, T_STAR, T_SLASH, T_INTLIT, T_SEMI, T_EQUALS, 
  T_IDENT,
  // Keywords
  T_PRINT, T_INT
};

struct token
{
    int token;  
    int intvalue;
};

// AST node types
enum {
  A_ADD, A_SUBTRACT, A_MULTIPLY, A_DIVIDE, A_INTLIT,
  A_IDENT, 
  A_LVIDENT,  // LVIDENT表示与特定位置相关联的值，即保存变量值的地址。
  A_ASSIGN
};

// ASTree 结构
struct ASTnode {
    int op;	// 如果op是操作符token，那么有左右孩子，如果是INTLIT那么就没有左右孩子，只有intvalue值
    struct ASTnode *left;			
    struct ASTnode *right;
    union {
      int intvalue; // 对于A_INTLIT，是整数值
      int id; // 对于A_IDENT,是指符号槽
    }	v;
};


// 符号表
struct symtable {
  char *name; // 符号的名称
};