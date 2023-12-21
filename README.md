## 词法扫描器

**Lexical Scanning**

>  the job of the scanner is to identify the lexical elements, or *tokens*, in the input language.

核心关键：

1. 需要生成对应的token结构体，来存储每个字符的token情况
2. 编写scan方法实现文档扫描，并根据扫描到的每个字符，对其记录对应的token

### 步骤：

1. 构建token表示结构，用来存储读取到的字符的类型。

    ```c
    =======defs.h=======
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

    ```

2.  拥有token结构之后需要从file中不断的读入字符，通常先创建全局变量Infile来表示文件

    ```c
    // -------data.c-------
    
    #include <stdio.h>
    #ifndef extern_
     #define extern_ extern
    #endif
    
    extern_ int Line;  // 表示当前读取到文件的哪一行
    extern_ int Putback;  // 表示预先返回的字符
    extern_ FILE *Infile;
    ```

    紧接着需要实现读取每一个字符的操作next()。

    ```c
    // -------scan.c-------
    
    /*
    scan.c 文件包含我们词法扫描器的函数。我们将逐个从输入文件中读取一个字符。
    然而，如果我们在输入流中读取得太远，就需要“放回”一个字符。
    我们还想追踪我们当前所在的行，以便在调试消息中打印行号。所有这些都由 next() 函数完成：
    */
    static int next(void) {
        int c;
    
        if (Putback) {
            c = Putback;  // 将一个字符提前返回
            Putback = 0;
            return c;
        }
    
        c = fgetc(Infile);  // 从输入文件读入一个字符
        if ('\n' == c) {
            Line ++;  // 如果读到换行符则说明行号+1
        }
    
        return c;
    }
    ```

    由于Putback表示上一次预先返回的值，所以同样需要一个静态函数来存储该值

    ```c
    static void putback(int c) {
        Putback = c;
    }
    ```

    词法扫描器需要的字符是有效的字符，而不是空格等无意义字符，所以需要编写一个忽略空白的函数skip

    ```c
    // 跳过空白字符, 直到非空白，返回非空白的字符
    static int skip(void) {
        int c;
        c = next();
        while (' ' == c || '\t' == c || '\n' == c || '\r' == c || '\f' == c) {
            c = next();
        }
        return (c);
    }
    ```

3. 扫描每个字符，并对每个有意义的词生成对应的token

    ```c
    // 扫描字符并将token加入到对应的结构中
    int scan(struct token *t) {
        
        int c;
        c = skip();
    
        switch (c) {
            case EOF:  // 文件终止符（实际就是-1）
                return (0);
            case '+':
                t->token = T_PLUS;
                break;
            case '-':
                t->token = T_MINUS;
                break;
            case '*':
                t->token = T_STAR;
                break;
            case '/':
                t->token = T_SLASH;
                break;
            default:
                // 如果是整数数字，需要扫描所有的字面值
                if (isdigit(c)) {
                    t->intvalue = scanint(c);
                    t->token = T_INTLIT;
                    break;
                }
                printf("Error: exits no recognize character %c at %d Line.\n", c, Line);
                exit(1);
        }
    
        return (1);
    }
    ```

    如果是数字的化，可能有多位，所以需要scanint函数来完成后续可能多位字符的扫描。

    ```c
    // 返回字符c在字符串s中的位置
    static int chrpos(char *s, int c) {
        char *p;
        p = strchr(s, c);  // strchr函数用于查找字符串中的一个字符，并返回该字符在字符串中出第一次出现的位置, 返回值是指向这个位置的指针，否则即为null
        return (p ? p - s : -1);
    }
    
    static int scanint(int c) {
        int k, val = 0;
        // 将每个字符转成对应的整数值
        while ((k = chrpos("0123456789", c)) >= 0) {
            val = val * 10 + k;
            c = next();
        }
        
        // 碰到非整数字符就将其返回
        putback(c);
        return val;
    }
    ```

4. 最后在主函数中打印一下scan扫描到的词。

    ```c
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
    
    char *tokstr[] = {"+", "-", "*", "/", "intlit"};
    
    static void scanfile() {
        struct token T;
    
        while (scan(&T)) {
            printf("Token %s", tokstr[T.token]);
            if (T.token == T_INTLIT) {
                printf(", value %d", T.intvalue);
            }
            printf("\n");
        }
    }
    
    void main(int argc, char *argv[]) {
        if (argc != 2) {
            usage(argv[0]);
        }
    
        init();
    
        if((Infile = fopen(argv[1], "r")) == NULL) {
            fprintf(stderr, "Error: Unable to open %s: %s.\n", argv[1], strerror(errno));
            exit(1);
        }
    
        scanfile();
        exit(0);
    }
    ```

    

## 解析器

> the job of the parser is to recognise the syntax and structural elements of the input and ensure that they conform to the *grammar* of the language.

这里需要用到Backus-Naur(巴科斯范式)

> ~~~c
> ```
> expression: number
>           | expression '*' expression
>           | expression '/' expression
>           | expression '+' expression
>           | expression '-' expression
>           ;
> 
> number:  T_INTLIT
>          ;
> ```
> ~~~
> 
> The vertical bars separate options in the grammar, so the above says:
> 
>   + An expression could be just a number, or
>   + An expression is two expressions separated by a '*' token, or
>   + An expression is two expressions separated by a '/' token, or
>   + An expression is two expressions separated by a '+' token, or
>   + An expression is two expressions separated by a '-' token
>   + A number is always a T_INTLIT token
> 

### AST抽象语法树

我们需要用AST树来存储每一个表达式，以至于后续可以通过中序遍历来实现求得表达式的最后值。

> https://zhuanlan.zhihu.com/p/599326926?utm_id=0

通过扫描器我们可以获取每个词符，现在则需要根据运算符来构建AST树。

1. 编写AST树的结构

    ```c
    // ASTree 结构
    struct ASTnode {
        int op;	// 如果op是操作符token，那么有左右孩子，如果是INTLIT那么就没有左右孩子，只有intvalue值
        struct ASTnode *left;			
        struct ASTnode *right;
        int intvalue;	
    };
    ```

    生成AST树的节点函数

    ```c
    struct ASTnode *mkastnode(int op, struct ASTnode *left, struct ASTnode *right, int intvalue) {
        struct ASTnode *n;
        // 动态创建一个空节点
        n = (struct ASTnode *) malloc (sizeof(struct ASTnode));
        if (n == NULL) {
            fprintf(stderr, "Error: Unable to malloc in mkastnode().\n");
            exit(1);
        }

        n->op = op;
        n->left = left;
        n->right = right;
        n->intvalue = intvalue;
        return (n);
    }

    // 创建AST的叶子节点
    struct ASTnode *mkastleaf(int op, int intvalue) {
        return (mkastnode(op, NULL, NULL, intvalue));
    }

    // 创捷AST的只有一个子节点的node
    struct ASTnode *mkastunary(int op, struct ASTnode *left, int intvalue) {
        return (mkastnode(op, left, NULL, intvalue));
    }

    ```

2. 由于现在针对的是运算符操作，所以可以重写构建一个枚举，来记录运算符标识，使得可以和词符的token区分开

    ```c
    // AST node types
    enum {
    A_ADD, A_SUBTRACT, A_MULTIPLY, A_DIVIDE, A_INTLIT
    };
    ```

3. 在读取每一个字符之后我们都可以进行运算符的判断，并搭建AST树

    ```c
    // 返回根为二进制运算符的AST树, 就是构建一个AST树，先左后递归右最后中。（后序遍历的思想）
    struct ASTnode *binexpr(void) {
        struct ASTnode *n, *left, *right;
        int nodetype;

        // 获取节点左侧的文字
        left = primary();
        // 如果没有下一个token了，就直接返回左节点
        if (Token.token == T_EOF)
            return (left);

        // nodetype变成token映射成ASTnode的标签
        nodetype = arithop(Token.token);
        // 继续获取下一个token
        scan(&Token);

        // 递归获取右子树
        right = binexpr();

        n = mkastnode(nodetype, left, right ,0);
        return (n);
    }
    ```
    构建AST树我们采用递归的方式，其中还需要设计一个函数primary，帮助我们获取整数文字。

    ```c
    // 检查下一个标记是否是整数文字，并构建一个对应的ASTnode来保存文字值
    static struct ASTnode *primary(void) {
        struct ASTnode *n;
        // Token是一个全局的token，用于保存读取到的字符
        switch (Token.token) {
            // 只有是整数文字才创建AST叶子节点，否则都是不合法的语义
            case T_INTLIT:
                n = mkastleaf(A_INTLIT, Token.intvalue);
                scan(&Token);
                return (n);
            default:
                fprintf(stderr, "Error: syntax error on line %d.\n", Line);
                exit(1);
        }
    }
    ```

4. 构建好AST树之后，需要解释器来解释AST树

    ```c
    static char *ASTop[] = { "+", "-", "*", "/" };

    // 给定AST树，解释其中的运算符并返回最终的结果（本质上是一个树的后序遍历）。
    int interpretAST(struct ASTnode *n) {
        int leftval, rightval;

        if (n->left) 
            leftval = interpretAST(n->left);
        if (n->right) 
            rightval = interpretAST(n->right);

        // 显示解释情况
        if (n->op == A_INTLIT) {
            printf("int %d\n", n->intvalue);
        } else {
            printf("%d %s %d\n", leftval, ASTop[n->op], rightval);
        }
        
        switch (n->op) {
            case A_ADD:
                return (leftval + rightval);
            case A_SUBTRACT:
                return (leftval - rightval);
            case A_MULTIPLY:
                return (leftval * rightval);
            case A_DIVIDE:
                return (leftval / rightval);
            case A_INTLIT:
                return (n->intvalue);
            default:
                fprintf(stderr, "Error: Unknown AST operator %d.\n", n->op);
                exit(1);
        }
    }
    ```

**需要注意的是现在构造好的AST树并没有按照运算符的优先级顺序实现，而是直接按照输入顺序生成，所以得到的结果是错误的AST。所以下一步我们需要重写我们的binexpr函数，使得可以按照优先级关系生成AST树**

## 解析器（改进运算优先级）

在上一节里，已经实现了AST树的搭建和解释，但是暂时是一个错误的AST树，该节主要是将AST树调整正确。

之前错误的主要原因是我们并没有设定运算符的优先级，所以我们需要完成以下两点：

1. 在语言的语法中显式地指定运算符优先级

2. 使用运算符优先级表影响现有解析器

所以我们回到expr.c中，增加静态的符号优先级定义

```c
// 为每一个符号生成对应的优先级(EOF, +, -, *, /, INTLIT)
static int OpPrec[] = { 0, 10, 10, 20, 20, 0}; 
```

> 优先级为0用于标记标识符而不是运算符


下面修改一下巴科斯范式：
```markdown
expression: additive_expression
    ;

additive_expression:
      multiplicative_expression
    | additive_expression '+' multiplicative_expression
    | additive_expression '-' multiplicative_expression
    ;

multiplicative_expression:
      number
    | number '*' multiplicative_expression
    | number '/' multiplicative_expression
    ;

number:  T_INTLIT
         ;
```
表达式是一个加法表达式，加法表达式可以由加法表达式和乘法表达式组成（减法和除法本质上可以由上述两个实现）

根据巴科斯范式重新修改binexpr函数
```c
// 为什么再加一个函数，为了错误提示
static int op_precedence(int tokentype) {
    int prec = OpPrec[tokentype];
    if (prec == 0) {
        fprintf(stderr, "Error: syntax error on line %d, token %d\n", Line, tokentype);
        exit(1);
    }
    // 返回字符的优先级
    return (prec);
}


// 返回一个AST树，ptp是上一个运算符的优先级
struct ASTnode *binexpr(int ptp) {
    struct ASTnode *left, *right;
    int tokentype;

    left = primary();
    tokentype = Token.token;
    if (tokentype == T_EOF) 
        return (left);
    
    // 如果当前符号的优先级大于上一个符号的优先级，那么就先构建当前子树
    while (op_precedence(tokentype) > ptp) {
        scan(&Token);

        // 递归构建右侧子树
        right = binexpr(OpPrec[tokentype]);

        left = mkastnode(arithop(tokentype), left, right, 0);

        tokentype = Token.token;
        if (tokentype == T_EOF) 
            return (left);
    }

    // 如果当前符号的优先级小，那么直接返回当前以及构建好的子树
    return (left);
}
```

> 通俗的了讲就是如果当前优先级比上一个小，则说明我是孩子节点，如果当前优先级比上一个大，则说明我应该是上一个的父节点，所以先去递归我的右子树再整合成一个完整的树。<br> 该算法可以叫做：Pratt解析

## 解释器（真实的编译器）

之前的工作只是生成了一个表达式的AST树，然后通过后序遍历解释器进行解析表达式结果，但是真实的编译器，我们需要根据AST树生成对应的汇编指令（本项目采用X86指令集）。

所以我们需要新建一个gen.c，用于编写我们根据AST树生成汇编指令的代码。

```c
// 给定一个AST树，以递归的方式生成汇编代码
static int genAST(struct ASTnode *n) {
    int leftreg, rightreg;

    // 和解释器类似，以后序遍历的方式去扫描AST树,只是现在不是直接返回值，而是返回生成的汇编代码
    if (n->left) leftreg = genAST(n->left);
    if (n->right) rightreg = genAST(n->right);

    switch (n->op) {
        case A_ADD: return (cgadd(leftreg, rightreg));
        case A_SUBTRACT: return (cgsub(leftreg, rightreg));
        case A_MULTIPLY: return (cgmul(leftreg, rightreg));
        case A_DIVIDE: return (cgdiv(leftreg, rightreg));
        case A_INTLIT: return (cgload(n->intvalue));

        default:
            fprintf(stderr, "Error: Unknown AST operator %d.\n", n->op);
            exit(1);
    }
}

// 调用生成汇编的函数
void generatecode(struct ASTnode *n) {
    int reg;

    cgpreamble(); // 汇编前处理
    reg = genAST(n);  // 执行编译
    cgprintint(reg); // 打印最后的输出结果，int类型
    cgpostamble(); // 汇编后处理
}
```
> 可以发现本质上和我们之前的interp解释器一致，只是之前是之间返回计算结果，现在是需要调用cg.c中的函数（cg.c中存储的是所有有关系统操作的函数，所以单独封装，保持隔离性）

展示一下cg.c中的函数，其实都是基本的汇编指令的输出。

```c
// 使用通用寄存器：r0、r1、r2 和 r3。这是一个包含实际寄存器名称的字符串表
static char *reglist[4] = {"%r8", "%r9", "%r10", "%r11"};
// 判断寄存器是否已使用
static int freereg[4];

// 分配一个尚未空的寄存器
static int alloc_register(void) {
    for (int i = 0 ; i < 4; i ++) {
        if (freereg[i]) {
            freereg[i] = 0;
            return (i);
        }
    }
    fprintf(stderr, "Error: Out of registers!\n");
    exit(1);
}

// 将所有的寄存器置为空
void freeall_registers(void) {
    for(int i = 0 ; i < 4; i ++) 
        freereg[i] = 1;
}

// 释放编号为reg的寄存器
static void free_register(int reg) {
    if (freereg[reg] != 0) {
        fprintf(stderr, "Error: trying to free register %d\n", reg);
        exit(1);
    }
    freereg[reg] = 1;
}

// 将一个值存入寄存器中，并返回存入的值
int cgload(int value) {
    int r = alloc_register();
    fprintf(Outfile, "\tmovq\t$%d, %s\n", value, reglist[r]);
    return (r);
}

void cgprintint(int r) {
    // Linux x86-64期望将函数的第一个参数放在 %rdi 寄存器中，因此在 call printint 之前，我们将寄存器移入 %rdi。
    fprintf(Outfile, "\tmovq\t%s, %%rdi\n", reglist[r]);
    fprintf(Outfile, "\tcall\tprintint\n");
    free_register(r);
}
```
> 汇编的实际都是基于对各种寄存器和堆栈指针的操作，所以分配和释放的函数是不可缺少的。

然后稍微修改一下main.c
```c
void main(int argc, char *argv[]) {
    struct ASTnode *n;

    if (argc != 2) {
        usage(argv[0]);
    }

    init();

    if ((Infile = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "Error: Unable to open %s: %s.\n", argv[1], strerror(errno));
        exit(1);
    }

    if ((Outfile = fopen("out.s", "w")) == NULL) {
        fprintf(stderr, "Error: Unable to open out.s: %s.\n",strerror(errno));
        exit(1);
    }
    // scanfile();
    scan(&Token);
    n = binexpr(0);
    printf("%d\n", interpretAST(n));
    generatecode(n);
    fclose(Outfile);
    exit(0);
}
```
> 此时需要注意的是，由于我们输出的汇编指令需要存到一个.s文件当中，所以注意在data.h中创建好全局变量。

## 表达式

之前的文件内容都只是简单的加减乘除运算，但是现在我们需要实现一个完整的表达式，表达式的内容就是print (加减乘除运算); 以print开头，以分号结尾的表达式。

首先简单的分析一下多出来的两个字符：

- print是关键字

- ；是一个符号

本质上和加减乘除一样，都是需要被系统所识别的符号或关键字，因此自然需要在defs.h中定义好对应的变量id

```c
// token 用枚举类实现
enum {
  // 符号
  T_EOF, T_PLUS, T_MINUS, T_STAR, T_SLASH, T_INTLIT, T_SEMI, 
  // 关键字
  T_PRINT 
};
```

紧接着需要注意就是之前在扫描的过程中都是用单个int类型的c来存储字符，如果是多位数字就用scanint连续扫描，但是如果遇到的是连续的字符串关键字呢，比如print。一种策略就是可以定义一个全局的字符缓冲区，如果读入的print，就可以使用scanident函数连续将print的每个字符读入缓冲区当中。

```c
=======defs.h=======
#define TEXTLEN 512 // 输入的符号的长度上限

=======data.h=======
extern_ char Text[TEXTLEN + 1];  // 标识符缓冲区

=======scan.c=======
// 从输入文件中扫描一个标识符，并存储到buf缓存种，返回标识符的长度，直到扫描到非字母数字的字符
static int scanident(int c, char *buf, int lim) {
    int i = 0;

    // 当输入的c是一个字母、数字、或者下划线，继续取读取
    while (isalpha(c) || isdigit(c) || '_' == c) {
        if (lim - 1 == i) {
            printf("Error: identifier too long on long %d\n", Line);
            exit(1);
        } else if (i < lim - 1) {
            buf[i ++] = c;
        }
        // 读入下一个字符
        c = next();
    }
    // 退出时说明遇到非字母数字下划线，所以将读入的c推出
    putback(c);
    // 加入结束标识符
    buf[i] = '\0';
    return (i);
}
```

那么现在能读入了，但是我们在构建AST树的时候还是依据token来的，所以如何给我们读入的print设定token？那么久需要在scan的过程中，一旦遇到了字母字符，那么我们就去识别一下scanident读入到缓冲中的是否是print关键字，如果是则为token设置对应的tokentype，那么print不是单个字符了，所以他的tokentype就需要使用一个map或者函数来映射。

```c
=======scan.c=======
// 关键词匹配，匹配buf中读入的字符,返回关键字标记号，如果不是关键字则返回0
static int keyword(char *s) {
    switch (*s) {
        case 'p':
            if (!strcmp(s, "print"))
                return (T_PRINT);
            break;
    }
    return (0);
}


int scan(struct token *t) {
    
    ...
    switch(c) {  

        ...
        case ';':
            t->token = T_SEMI;
            break;
        default:
                // 如果是整数数字，需要扫描所有的字面值
                if (isdigit(c)) {
                    t->intvalue = scanint(c);
                    t->token = T_INTLIT;
                    break;
                } else if (isalpha(c) || '_' == c) {
                    scanident(c, Text, TEXTLEN);

                    // 如果是keyword中已知的关键字，返回标记
                    if (tokentype = keyword(Text)) {
                        t->token = tokentype;
                        break;
                    }

                    printf("Error: Unrecognised character %c on line %d\n", c, Line);
                    exit(1);
                }
                printf("Error: exits no recognize character %c at %d Line.\n", c, Line);
                exit(1);
    }
```

现在我们可以正常的扫描一个print表达式了。下一步是思考如何根据扫描的到的字符修改对应的构建AST树的函数呢？

其实只需要修改一下原本的退出规则即可，因为对于print关键字，实际上我们可以先去识别，如果第一个单词不是print直接报错就行了，如果是那么就正常的构建表达式的AST树，知道遇到";”退出AST树的构建就行了。

```c
=======expr.c=======
// 返回一个AST树，ptp是上一个运算符的优先级
struct ASTnode *binexpr(int ptp) {
    struct ASTnode *left, *right;
    int tokentype;

    left = primary();
    tokentype = Token.token;
    if (tokentype == T_SEMI) 
        return (left);
    
    // 如果当前符号的优先级大于上一个符号的优先级，那么就先构建当前子树
    while (op_precedence(tokentype) > ptp) {
        scan(&Token);

        // 递归构建右侧子树
        right = binexpr(OpPrec[tokentype]);

        left = mkastnode(arithop(tokentype), left, right, 0);

        tokentype = Token.token;
        if (tokentype == T_SEMI) 
            return (left);
    }

    // 如果当前符号的优先级小，那么直接返回当前以及构建好的子树
    return (left);
}

=======stmt.c=======
// 解析语句
void statements(void) {
    struct ASTnode *tree;
    int reg;

    while (1) {
        // 将print作为第一个匹配的标识符
        match(T_PRINT, "print");

        // 解析表达式并生成汇编
        tree = binexpr(0);
        // genAST返回的是存放表达式结果的寄存器的id
        reg = genAST(tree); 
        genprintint(reg);
        genfreeregs();

        // 匹配最后的分号，本质就是调用的match函数
        semi(); 
    
        // 直到匹配到文本结束符，否则持续循环解析表达式
        if (Token.token == T_EOF) {
            return;
        }
    }
}
```

可能第一次看会稍微疑惑，其实stmt.c中新增的gen*操作都是有关生成X86代码的打印操作，存放在cg.c文件当中，如果你熟悉了X86指令集，那么你可以自己去编写对应的输出内容。同时match函数用于匹配当前读取的关键字是否是与传入的token_id一致，如果一致则说明匹配正确，否则则说明这不是一个合法的以print开头的表达式。