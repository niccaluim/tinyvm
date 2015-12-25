This is a simple bytecode virtual machine written mainly as an exercise on a
sleepless night. It uses indirect gotos if the compiler supports them,
otherwise it uses a slightly slower switch statement. Aside from this the code
is strictly ANSI C.

## Architecture ##

The VM has a data stack with 16,384 cells of 32-bit signed integers and a call
stack with 16,384 cells of return addresses. There are no program-accessible
registers. A special instruction, `native`, permits calls to native functions
provided by the host.

### Instruction Set ###

Instructions are one byte. Some instructions, like `push`, are followed by
immediate values. Immediate values larger than one byte are represented in
bytecode in little endian. (The VM runs on hosts with big-endian byte order
with a slight speed penalty when loading immediate values.)

0x00 `halt` ( -- )
Stops program execution and returns control to the host.

0x01 `push` ( -- v )
Loads the 32-bit value immediately after this instruction and puts it on the
stack.

0x02 `pop` ( v -- )
Removes an item from the stack.

0x03 `dup` ( v -- v v )
Duplicates the item on top of the stack.

0x04 `swap` ( v1 v2 -- v2 v1 )
Swaps the top two items on the stack.

0x05 `rot` ( v1 v2 v3 -- v2 v3 v1 )
Rotates the top three items on the stack.

0x06 `native` ( varies )
Passes control to a native function identified by a one-byte immediate value.
The native function may alter the stack or trigger a trap that halts
execution.

0x07 `add` ( v1 v2 -- v3 )
Adds two values.

0x08 `sub` ( v1 v2 -- v3 )
Subtracts v2 from v1.

0x09 `mul` ( v1 v2 -- v3 )
Multiplies two values.

0x0a `div` ( v1 v2 -- v3 )
Divides v1 by v2 and pushes the quotient.

0x0b `mod` ( v1 v2 -- v3 )
Divides v1 by v2 and pushes the remainder.

0x0c `andb` ( v1 v2 -- v3 )
Performs a bitwise AND.

0x0d `orb` ( v1 v2 -- v3 )
Performs a bitwise OR.

0x0e `notb` ( v1 -- v2 )
Performs a bitwise negation.

0x0f `xorb` ( v1 v2 -- v3 )
Performs a bitwise exclusive OR.

0x10 `andl` ( v1 v2 -- v3 )
Performs a logical AND, where zero is considered false and any other value is
true.

0x11 `orl` ( v1 v2 -- v3 )
Performs a logical OR, where zero is considered false and any other value is
true.

0x12 `notl` ( v1 -- v2 )
Performs a logical NOT, where zero is considered false and any other value is
true.

0x13 `xorl` ( v1 v2 -- v3 )
Performs a logical exclusive OR, where zero is considered false and any other
value is true.

0x14 `jmp` ( -- )
Continues execution at an address computed by adding a 32-bit immediate value
to the address of the next instruction.

0x15 `br` ( v -- )
If the value on the stack is zero, execution continues with the next
instruction. Otherwise, execution continues at an address computed by adding a
32-bit immediate value to the address of the next instruction.

0x16 `call` (call stack: -- a )
The address of the next instruction is pushed onto the call stack. Execution
continues at an address computed by adding a 32-bit immediate value to the
address of the next instruction.

0x17 `ret` (call stack: a -- )
Continues execution at the address on top of the call stack.
