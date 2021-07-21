# Attack Lab

## Ctarget

當進入 `getbuf` 函式時
At the beginning of function `getbuf` the %rsp address and the content in it are

```shell
$rsp = 0x556dca0
($rsp) = 0x00401976 // return address of getbuf
```

After declare local variables, the %rsp points to

```shell
$rsp = 0x5561dc78
```

and the content of the stack is

```shell
0x5561dc78:     0x10    0x83    0x06    0x00    0x00    0x00    0x00    0x00
0x5561dc80:     0xf4    0x00    0x00    0x00    0x00    0x00    0x00    0x00
0x5561dc88:     0xc0    0xdc    0x61    0x55    0x00    0x00    0x00    0x00
0x5561dc90:     0x00    0x00    0x00    0x00    0x00    0x00    0x00    0x00
0x5561dc98:     0x00    0x00    0x00    0x00    0x00    0x00    0x00    0x00
0x5561dca0:     0x76    0x19    0x40    0x00    0x00    0x00    0x00    0x00
```

Here we can see the memory for the input string is probably 16 bytes located at
address (0x5561dc78, 0x5561dc88). Therefore, if the input string is longer than
15 characters, it will cause buffer overflow, and it is what we need for our
attack. If we make the string overflow to the memory in which the return
address should be at, the function will return to the different place from the
one it should go.

### Level 1

In this level the goal is to redirect the return function to `touch1` without
any argument. It is easy to do by make the string overflows to the return
address and reset the address to the start address of `touch1`, and done.

### Level 2

In this level the goal is to redirect the return function to `touch2` with an
argument. To do this trick, I first redirect the return address to part of 
the stack, then let it move a value into %rdi (the first argument is placed).
Then set the %rsp to the other position where the address of `touch2` is put.

### Level 3

In this level the goal is to redirect the return function to `touch3` with an
argument. To do this trick, I first redirect the return address to part of 
the stack, then let it move the string pointer address into %rdi. Then set the
%rsp to the other position where the address of `touch3` is put.

## Return-Oriented Programming

### Usable Gadgets

4019ab
pop %rax
<addval_219>

4019c5
; movq %rax,%rdi
<setval_426>

4019dd
; popq %rsp; movl %eax,%edx
<getval_481>

401a06
; movq %rsp,%rax
<addval_190>

401a13
; movl %ecx,%esi
<addval_436>

401a34
; movl %edx,%ecx
<getval_159>

4019d6
<add_xy>:
