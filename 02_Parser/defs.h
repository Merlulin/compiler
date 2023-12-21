#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// token 用枚举类实现
enum {
  T_EOF, T_PLUS, T_MINUS, T_STAR, T_SLASH, T_INTLIT
};

struct token
{
    int token;  
    int intvalue;
};

// AST node types
enum {
  A_ADD, A_SUBTRACT, A_MULTIPLY, A_DIVIDE, A_INTLIT
};

// ASTree 结构
struct ASTnode {
    int op;	// 如果op是操作符token，那么有左右孩子，如果是INTLIT那么就没有左右孩子，只有intvalue值
    struct ASTnode *left;			
    struct ASTnode *right;
    int intvalue;	
};
