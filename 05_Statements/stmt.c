#include "defs.h"
#include "data.h"
#include "decl.h"

// 解析语句
void statements(void) {
    struct ASTnode *tree;
    int reg;

    while (1) {
        // 将print作为第一个匹配的标识符
        match(T_PRINT, "print");

        // 解析表达式并生成汇编
        tree = binexpr(0);
        reg = genAST(tree);
        genprintint(reg);
        genfreeregs();

        // 匹配最后的分号
        semi(); 
    
        // 直到匹配到文本结束符，否则持续循环解析表达式
        if (Token.token == T_EOF) {
            return;
        }
    }
}