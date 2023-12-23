#include "defs.h"
#include "data.h"
#include "decl.h"


// 解析变量的声明
void var_declaration(void) {
    match(T_INT, "int");
    ident();
    addglob(Text);
    genglobsym(Text);
    semi();
}