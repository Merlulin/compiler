#include "defs.h"
#include "data.h"
#include "decl.h"

// cg.c用于针对X86的CPU进行实际的汇编代码生成
// 对CPU操作，实际都在操作寄存器，因为我们需要用寄存器存储读入的值，再执行对应的操作，最后返回结果后可以删除该寄存器内的值继续使用
// 必须三个函数：分配寄存器，释放寄存器，释放全部寄存器

// 使用通用寄存器：r0、r1、r2 和 r3。这是一个包含实际寄存器名称的字符串表
static char *reglist[4] = {"%r8", "%r9", "%r10", "%r11"};
static char *breglist[4] = { "%r8b", "%r9b", "%r10b", "%r11b" };

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


/// @brief 生成加法汇编语句
/// @param r1 左数所在的寄存器
/// @param r2 右数所在的寄存器
/// @return 返回加和值的存放位置 *r1 + *r2 -> *r2
int cgadd(int r1, int r2) {
    fprintf(Outfile, "\taddq\t%s, %s\n", reglist[r1], reglist[r2]);
    free_register(r1);
    return(r2);
}

/// @brief 生成乘法汇编语句
/// @param r1 左数所在的寄存器
/// @param r2 右数所在的寄存器
/// @return 返回乘积值的存放位置 *r1 * *r2 -> *r2
int cgmul(int r1, int r2) {
  fprintf(Outfile, "\timulq\t%s, %s\n", reglist[r1], reglist[r2]);
  free_register(r1);
  return(r2);
}

/// @brief 生成减法汇编语句
/// @param r1 左数所在的寄存器
/// @param r2 右数所在的寄存器
/// @return 返回差值的存放位置 *r2 - *r1 -> *r1
int cgsub(int r1, int r2) {
    fprintf(Outfile, "\tsubq\t%s, %s\n", reglist[r2], reglist[r1]);
    free_register(r2);
    return (r1);
}

/// @brief 生成除法汇编语句
/// @param r1 
/// @param r2 
/// @return 并返回具有结果的寄存器的编号
int cgdiv(int r1, int r2) {
  fprintf(Outfile, "\tmovq\t%s,%%rax\n", reglist[r1]);
  fprintf(Outfile, "\tcqo\n");
  fprintf(Outfile, "\tidivq\t%s\n", reglist[r2]);
  fprintf(Outfile, "\tmovq\t%%rax,%s\n", reglist[r1]);
  free_register(r2);
  return(r1);
}

// 汇编前处理，将所有的寄存器释放
void cgpreamble()
{
  freeall_registers();
  fputs(
	"\t.text\n"
	".LC0:\n"
	"\t.string\t\"%d\\n\"\n"
	"printint:\n"
	"\tpushq\t%rbp\n"
	"\tmovq\t%rsp, %rbp\n"
	"\tsubq\t$16, %rsp\n"
	"\tmovl\t%edi, -4(%rbp)\n"
	"\tmovl\t-4(%rbp), %eax\n"
	"\tmovl\t%eax, %esi\n"
	"\tleaq	.LC0(%rip), %rdi\n"
	"\tmovl	$0, %eax\n"
	"\tcall	printf@PLT\n"
	"\tnop\n"
	"\tleave\n"
	"\tret\n"
	"\n"
	"\t.globl\tmain\n"
	"\t.type\tmain, @function\n"
	"main:\n"
	"\tpushq\t%rbp\n"
	"\tmovq	%rsp, %rbp\n",
  Outfile);
}

// 汇编后处理
void cgpostamble()
{
  fputs(
	"\tmovl	$0, %eax\n"
	"\tpopq	%rbp\n"
	"\tret\n",
  Outfile);
}

void cgprintint(int r) {
    // Linux x86-64期望将函数的第一个参数放在 %rdi 寄存器中，因此在 call printint 之前，我们将寄存器移入 %rdi。
    fprintf(Outfile, "\tmovq\t%s, %%rdi\n", reglist[r]);
    fprintf(Outfile, "\tcall\tprintint\n");
    free_register(r);
}

// Load an integer literal value into a register.
// Return the number of the register
int cgloadint(int value) {
  // Get a new register
  int r = alloc_register();

  // Print out the code to initialise it
  fprintf(Outfile, "\tmovq\t$%d, %s\n", value, reglist[r]);
  return (r);
}

// Load a value from a variable into a register.
// Return the number of the register
int cgloadglob(char *identifier) {
  // Get a new register
  int r = alloc_register();

  // Print out the code to initialise it
  fprintf(Outfile, "\tmovq\t%s(\%%rip), %s\n", identifier, reglist[r]);
  return (r);
}

// Store a register's value into a variable
int cgstorglob(int r, char *identifier) {
  fprintf(Outfile, "\tmovq\t%s, %s(\%%rip)\n", reglist[r], identifier);
  return (r);
}

// Generate a global symbol
void cgglobsym(char *sym) {
  fprintf(Outfile, "\t.comm\t%s,8,8\n", sym);
}

// Compare two registers.
static int cgcompare(int r1, int r2, char *how) {
  fprintf(Outfile, "\tcmpq\t%s, %s\n", reglist[r2], reglist[r1]);
  fprintf(Outfile, "\t%s\t%s\n", how, breglist[r2]);
  fprintf(Outfile, "\tandq\t$255,%s\n", reglist[r2]);
  free_register(r1);
  return (r2);
}

int cgequal(int r1, int r2) { return(cgcompare(r1, r2, "sete")); }
int cgnotequal(int r1, int r2) { return(cgcompare(r1, r2, "setne")); }
int cglessthan(int r1, int r2) { return(cgcompare(r1, r2, "setl")); }
int cggreaterthan(int r1, int r2) { return(cgcompare(r1, r2, "setg")); }
int cglessequal(int r1, int r2) { return(cgcompare(r1, r2, "setle")); }
int cggreaterequal(int r1, int r2) { return(cgcompare(r1, r2, "setge")); }