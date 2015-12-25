#include <limits.h>
#include "vm.h"

#define CSTACK_SIZE 16384

#define NEEDS_DEPTH(n) if (si < (n)-1) return VM_EDATAUF;
#define PUSHES(n) if (si > VM_STACK_SIZE-1-(n)) return VM_EDATAOF;

#if LITTLE_ENDIAN
#define IMMED(x) \
    if (opslen - (pc - ops) < sizeof(vm_cell_t)) { \
        return VM_EIMMED; \
    } \
    x = *(vm_cell_t *) pc; \
    pc += sizeof(vm_cell_t);
#else
#define IMMED(x) \
    if (opslen - (pc - ops) < sizeof(vm_cell_t)) { \
        return VM_EIMMED; \
    } \
    x =pc[0] | (((uint32_t) pc[1]) << 8) \
             | (((uint32_t) pc[2]) << 16) \
             | (((int32_t) pc[3]) << 24); \
    pc += sizeof(vm_cell_t);
#endif /* #if LITTLE_ENDIAN */

vm_status_t run(const unsigned char *ops, size_t opslen, vm_native_t **natives, size_t nlen) {
    const unsigned char *pc = ops;
    const unsigned char *cstack[CSTACK_SIZE];
    int csi = -1;
    vm_cell_t stack[VM_STACK_SIZE];
    int si = -1;

    vm_cell_t cell;
    vm_status_t status;

#ifdef __GNUC__
    #pragma GCC diagnostic ignored "-Wgnu-label-as-value"
    static void *labels[UCHAR_MAX+1];
    if (labels[0] == 0) {
        int i;
        labels[OP_HALT]   = &&op_halt;
        labels[OP_PUSH]   = &&op_push;
        labels[OP_POP]    = &&op_pop;
        labels[OP_DUP]    = &&op_dup;
        labels[OP_SWAP]   = &&op_swap;
        labels[OP_ROT]    = &&op_rot;
        labels[OP_NATIVE] = &&op_native;
        labels[OP_ADD]    = &&op_add;
        labels[OP_SUB]    = &&op_sub;
        labels[OP_MUL]    = &&op_mul;
        labels[OP_DIV]    = &&op_div;
        labels[OP_MOD]    = &&op_mod;
        labels[OP_ANDB]   = &&op_andb;
        labels[OP_ORB]    = &&op_orb;
        labels[OP_NOTB]   = &&op_notb;
        labels[OP_XORB]   = &&op_xorb;
        labels[OP_ANDL]   = &&op_andl;
        labels[OP_ORL]    = &&op_orl;
        labels[OP_NOTL]   = &&op_notl;
        labels[OP_XORL]   = &&op_xorl;
        labels[OP_JMP]    = &&op_jmp;
        labels[OP_BR]     = &&op_br;
        labels[OP_CALL]   = &&op_call;
        labels[OP_RET]    = &&op_ret;
        for (i = OP_UNDEFMIN; i <= UCHAR_MAX; i++) {
            labels[i] = &&undefined;
        }
    }
#else
    #define op_push   case OP_PUSH
    #define op_pop    case OP_POP
    #define op_dup    case OP_DUP
    #define op_swap   case OP_SWAP
    #define op_rot    case OP_ROT
    #define op_native case OP_NATIVE
    #define op_add    case OP_ADD
    #define op_sub    case OP_SUB
    #define op_mul    case OP_MUL
    #define op_div    case OP_DIV
    #define op_mod    case OP_MOD
    #define op_andb   case OP_ANDB
    #define op_orb    case OP_ORB
    #define op_notb   case OP_NOTB
    #define op_xorb   case OP_XORB
    #define op_andl   case OP_ANDL
    #define op_orl    case OP_ORL
    #define op_notl   case OP_NOTL
    #define op_xorl   case OP_XORL
    #define op_jmp    case OP_JMP
    #define op_br     case OP_BR
    #define op_call   case OP_CALL
    #define op_ret    case OP_RET
    #define undefined default
#endif

    if (opslen == 0) {
        return VM_OK;
    }

    /* requiring a HALT at the end of the program frees us from bounds-checking
       pc at the beginning of every cycle. */
    if (ops[opslen-1] != OP_HALT) {
        return VM_ENOHALT;
    } else if (opslen > 1 && ops[opslen-2] == OP_NATIVE) {
        return VM_ENOHALT;
    } else if (opslen > 4 && (ops[opslen-5] == OP_PUSH
                           || ops[opslen-5] == OP_JMP
                           || ops[opslen-5] == OP_BR
                           || ops[opslen-5] == OP_CALL)) {
        return VM_ENOHALT;
    }

    for (;;) {
#ifdef __GNUC__
        goto *labels[*pc++];
#else
        switch (*pc++) {
#endif
        op_halt:
            return VM_OK;
        op_push:
            PUSHES(1)
            IMMED(stack[++si])
            continue;
        op_pop:
            NEEDS_DEPTH(1)
            si--;
            continue;
        op_dup:
            NEEDS_DEPTH(1)
            PUSHES(1)
            stack[si+1] = stack[si];
            si++;
            continue;
        op_swap:
            NEEDS_DEPTH(2)
            cell = stack[si];
            stack[si] = stack[si-1];
            stack[si-1] = cell;
            continue;
        op_rot:
            NEEDS_DEPTH(3)
            cell = stack[si-2];
            stack[si-2] = stack[si-1];
            stack[si-1] = stack[si];
            stack[si] = cell;
            continue;
        op_native:
            if (*pc >= nlen || !natives) {
                return VM_ENATIVE;
            }
            status = natives[*pc++](stack, &si);
            if (status != VM_OK) {
                return status;
            }
            continue;
        op_add:
            NEEDS_DEPTH(2)
            stack[si-1] += stack[si];
            si--;
            continue;
        op_sub:
            NEEDS_DEPTH(2)
            stack[si-1] -= stack[si];
            si--;
            continue;
        op_mul:
            NEEDS_DEPTH(2)
            stack[si-1] *= stack[si];
            si--;
            continue;
        op_div:
            NEEDS_DEPTH(2)
            stack[si-1] /= stack[si];
            si--;
            continue;
        op_mod:
            NEEDS_DEPTH(2)
            stack[si-1] %= stack[si];
            si--;
            continue;
        op_andb:
            NEEDS_DEPTH(2)
            stack[si-1] &= stack[si];
            si--;
            continue;
        op_orb:
            NEEDS_DEPTH(2)
            stack[si-1] |= stack[si];
            si--;
            continue;
        op_notb:
            NEEDS_DEPTH(1)
            stack[si] = ~stack[si];
            continue;
        op_xorb:
            NEEDS_DEPTH(2)
            stack[si-1] ^= stack[si];
            si--;
            continue;
        op_andl:
            NEEDS_DEPTH(2)
            stack[si-1] = stack[si-1] && stack[si];
            si--;
            continue;
        op_orl:
            NEEDS_DEPTH(2)
            stack[si-1] = stack[si-1] || stack[si];
            si--;
            continue;
        op_notl:
            NEEDS_DEPTH(1)
            stack[si] = !stack[si];
            continue;
        op_xorl:
            NEEDS_DEPTH(2)
            stack[si-1] = (stack[si-1] || stack[si]) && !(stack[si-1] && stack[si]);
            si--;
            continue;
        op_br:
            NEEDS_DEPTH(1)
            if (!stack[si--]) {
                pc += 4;
                continue;
            }
            /* fall through */
        op_jmp:
            IMMED(cell)
            pc += cell;
            if (pc < ops || pc >= ops+opslen) {
                return VM_EOFFSET;
            }
            continue;
        op_call:
            if (csi == CSTACK_SIZE-1) return VM_ECALLOF;
            IMMED(cell)
            cstack[++csi] = pc;
            pc += cell;
            if (pc < ops || pc >= ops+opslen) {
                return VM_EOFFSET;
            }
            continue;
        op_ret:
            if (csi == -1) return VM_ECALLUF;
            pc = cstack[csi--];
            continue;
        undefined:
            return VM_EUNDEF;
#ifndef __GNUC__
        }
#endif
    }
}
