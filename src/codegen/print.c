#include "codegen.h"

extern int label_count;
extern int str_count;
extern reg scratch[];
extern data_entry* data;

/**********************************************************************
 *                          PRINT FUNCTIONS                           *
 **********************************************************************/

void print_bool(int reg) {
    printf(
        "CMP %s, $0\n",
        scratch_name(reg)
    );
    int true_label = create_label();
    int done_label = create_label();
    printf(
        "JNE %s\n",
        label_name(true_label)
    );
    printf("MOVQ $0x30, %s\n", scratch_name(reg));
    printf("JMP %s\n", label_name(done_label));
    printf("%s:\n", label_name(true_label));
    printf("MOVQ $0x31, %s\n", scratch_name(reg));
    printf("%s:\n", label_name(done_label));
    print_char(reg);
}

void print_char(int reg) {
    printf( /* set length to 1 */
        "MOVQ $1, %%rdx\n"
    );
    printf( /* move char to input buffer */
        "MOVQ %s, %%rsi\n",
        scratch_name(reg)
    );
    printf( /* set fd to stdout */
        "MOVQ $1, %%rdi\n"
    );
    printf( /* set syscall to write */
        "MOVQ $4, %%rax\n"
    );
    printf("SYSCALL\n");
}

void print_str_codegen(int reg) {
    int count = scratch_alloc();
    int pointer = scratch_alloc();
    printf(
        "MOVQ %s, %s\n",
        scratch_name(reg),
        scratch_name(pointer)
    );
    int loop = create_label();
    int done = create_label();
    printf("%s:\n", label_name(loop));
    printf("CMP %s, $0\n", scratch_name(pointer));
    printf("JE %s\n", label_name(done));
    printf("INCQ %s\n", scratch_name(count));
    printf("INCQ %s\n", scratch_name(pointer));
    printf("JMP %s\n", label_name(loop));
    printf("%s:\n", label_name(done));
    scratch_free(pointer);
    printf("MOVQ %s, %%rdx\n", scratch_name(count));
    scratch_free(count);
    printf("MOVQ %s, %%rsi\n", scratch_name(reg));
    printf("MOVQ $1, %%rdi\n");
    printf("MOVQ $4, %%rax\n");
    printf("SYSCALL\n");
}

void print_str_lit_codegen(const char* s) {
    int orig_size = strlen(s);
    int prev_size = 0;
    char* str = strtok(s, "\n");
    bool newline = false;

    while (str != NULL) {
        newline = strlen(str) < (orig_size - prev_size);

        int str_lit = add_str(str, newline);
        printf( /* move string length to third arg */
            "MOVQ %s_len, %%rdx\n",
            str_label(str_lit)
        );
        printf( /* move string to second arg */
            "MOVQ %s ,%%rsi\n",
            str_label(str_lit)
        );
        printf( /* move "1" (stdout) to first arg */
            "MOVQ $1, %%rdi\n"
        );
        printf( /* move "4" (write) to %rax */
            "MOVQ $4, %%rax\n"
        );
        printf( /* invoke the system call*/
            "SYSCALL\n"
        );

        prev_size += strlen(str);
        str = strtok(NULL, "\n");
    }
}

void print_i_to_a(int reg) {
    /* store number in %rax */
    printf(
        "MOVQ %s, %%rax\n",
        scratch_name(reg)
    );
    /* count # of converted digits */
    int num_digits = scratch_alloc();
    printf("MOVQ $0, %s\n", scratch_name(num_digits));
    /* create loop label */
    int convert_loop = create_label();
    printf("%s:\n", label_name(convert_loop));
    printf( /* divide %rax by 10 */
        "IDIV $10\n"
    );
    printf( /* convert remainder to ASCII */
        "ADD $0x30, %%rdx\n"
    );
    printf( /* push character to stack: */
        "PUSH %%rdx\n"
    );
    printf("INC %s\n", scratch_name(num_digits));
    printf("CMP %%rax, $0\n"); 
    printf("JE %s\n", label_name(convert_loop));

    /* check negative */
    printf("CMP %s, $0\n", scratch_name(reg));
    int print_loop = create_label();
    printf("JGE %s\n", label_name(print_loop));
    printf("PUSH $0x2D\n");
    printf("INC %s\n", scratch_name(num_digits));

    /* create print loop label */
    printf("%s:\n", label_name(print_loop));
    printf( /* prepare string length arg */
        "MOVQ $1, %%rdx\n"
    );
    printf( /* prepare stdout arg */
        "MOVQ $1, %%rdi\n"
    );
    printf(/* pop character */
        "POP %%rsi\n"
    );
    printf( /* prepare syscall arg */
        "MOVQ $4, %%rax\n"
    );
    printf("SYSCALL\n");
    printf("DEC %s\n", scratch_name(num_digits));
    printf("CMP %s, $0\n", scratch_name(num_digits));
    printf("JNE %s\n", label_name(print_loop));
}