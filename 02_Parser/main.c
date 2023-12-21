#include "defs.h"
#define extern_
#include "data.h"
#undef extern_
#include "decl.h"
#include <errno.h>

// 全局初始化
static void init() {
    Line = 1;
    Putback = '\n';
}

// 如果出现报错，指出错误的文件
static void usage(char *prog) {
    fprintf(stderr, "Usage: %s infile.\n", prog);
    exit(1);
}


void main(int argc, char *argv[]) {
    struct ASTnode *n;

    if (argc != 2) {
        usage(argv[0]);
    }

    init();

    if((Infile = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "Error: Unable to open %s: %s.\n", argv[1], strerror(errno));
        exit(1);
    }

    // scanfile();
    scan(&Token);
    n = binexpr();
    printf("%d\n", interpretAST(n));
    exit(0);
}