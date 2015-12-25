#include <stdio.h>
#include "vm.h"

vm_status_t tell(vm_cell_t *stack, int *si) {
    if (*si == -1) {
        return VM_EDATAUF;
    }
    printf("%ld\n", (long) stack[*si--]);
    return VM_OK;
}

static vm_native_t *natives[] = { tell };

int main(int argc, char **argv) {
    unsigned char ops[] = {
        OP_PUSH, 1, 0, 0, 0,
        OP_PUSH, 5, 0, 0, 0,
        OP_PUSH, 9, 0, 0, 0,
        OP_ROT,
        OP_SUB,
        OP_MUL,
        OP_NATIVE, 0,
        OP_HALT
    };
    return run(ops, sizeof(ops), natives, sizeof(natives)/sizeof(natives[0]));
}
