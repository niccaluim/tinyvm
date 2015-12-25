#ifndef __VM_H__
#define __VM_H__

#include <stddef.h>

#include "config.h"
#if HAVE_STDINT_H
#include <stdint.h>
#endif

#include "ops.h"

typedef enum {
    VM_OK,
    VM_ENOHALT,
    VM_EUNDEF,
    VM_ENATIVE,
    VM_EDATAOF,
    VM_EDATAUF,
    VM_ECALLOF,
    VM_ECALLUF,
    VM_EIMMED,
    VM_EOFFSET
} vm_status_t;

#define VM_STACK_SIZE 16384
typedef int32_t vm_cell_t;

typedef vm_status_t vm_native_t(vm_cell_t *, int *);

vm_status_t run(const unsigned char *, size_t, vm_native_t **, size_t);

#endif /* #ifndef __VM_H__ */
