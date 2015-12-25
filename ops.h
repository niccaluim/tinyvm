#ifndef __OPS_H__
#define __OPS_H__

enum {
    OP_HALT,
    OP_PUSH,
    OP_POP,
    OP_DUP,
    OP_SWAP,
    OP_ROT,
    OP_NATIVE,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_ANDB,
    OP_ORB,
    OP_NOTB,
    OP_XORB,
    OP_ANDL,
    OP_ORL,
    OP_NOTL,
    OP_XORL,
    OP_JMP,
    OP_BR,
    OP_CALL,
    OP_RET,
    /* lower bound of undefined operations */
    OP_UNDEFMIN
};

#endif /* #ifndef __OPS_H__ */
