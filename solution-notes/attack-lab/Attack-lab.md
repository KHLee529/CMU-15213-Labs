# Attack Lab

## Code Injection

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

### CI Level 1

In this level the goal is to redirect the return function to `touch1` without
any argument. It is easy to do by make the string overflows to the return
address and reset the address to the start address of `touch1`, and done.

### CI Level 2

In this level the goal is to redirect the return function to `touch2` with an
argument. To do this trick, I first redirect the return address to part of
the stack, then let it move a value into %rdi (the first argument is placed).
Then set the %rsp to the other position where the address of `touch2` is put.

### CI Level 3

In this level the goal is to redirect the return function to `touch3` with an
argument. To do this trick, I first redirect the return address to part of
the stack, then let it move the string pointer address into %rdi. Then set the
%rsp to the other position where the address of `touch3` is put.

## Return-Oriented Programming

### Usable Gadgets

The usable gadgets inside the farm is listed below.

- `pop %rax`
  - addr: 4019ab
  - <addval_219>
- `movq %rax,%rdi`
  - addr: 4019c5
  - <setval_426>
- `movl %eax,%edx`
  - addr: 4019dd
  - <getval_481>
- `movq %rsp,%rax`
  - addr: 401a06
  - <addval_190>
- `movl %ecx,%esi`
  - addr: 401a13
  - <addval_436>
- `movl %edx,%ecx`
  - addr: 401a34
  - <getval_159>
- `x+y`
  - addr: 4019d6
  - <add_xy>:

### ROP Level 2

In this level, the goal is to repeat the same attack with CI Level 2 but only
use the return-oriented programming. We have to move the cookie into register
%rax and then call `touch2` function. Using the gadgets listed above, I found
my way setting the stack to the form shown below.

```markdown
the address of `popq %rax` <- the location of original return address
the heximal representation of the cookie
the address of `mov %rax,%rdi`
the address of `touch2`
```

### ROP Level 3

In this level, the goal is to repeat the same attack with CI Level 3 but only
use the return-oriented programming. We have to move the string into a memory
which we are able to get its memory address by go through some instructions in
the usable gadgets. \
The key point is that there is an `x + y` function in the gadget list.
Therefore, we can access some places with address greater than the original
%rsp by taking the stack pointer with `movq %rsp %rax` and adding a positive
constant to it to get the address of the input string. \
I decided to set the stack into the form shown below to achive this attack.

```markdown
the address of `movq %rsp %rax` <- the location of original return address
the address of `movq %rax %rdi` (get the 1st argument of `x + y`) 
^ here is the address moved into `%rax` by `mpvq %rsp %rax` <-------+
the address of `popq %rax`                                          |
the heximal representation of constant `0x48` to be popped <--------|---+
the address of `mov %eax %edx`                                      |   |
the address of `mov %edx %ecx`                                      |   |
the address of `mov %ecx %esi` (get the 2nd argument of `x + y`)    |   |
the address of `x + y` (the result will be saved in %rax)           |   |
the address of `movq %rax %rdi` (get the argument of `touch3`)      |   |
the address of `touch3`                                             |   |
the string representation of the cookie                             |   |
^ here is the location of the string                                |   |
  which is 0x48 byte away from the original address we got ---------+   |
  that's why I set the constant to 0x48 --------------------------------+
```

Attack Lab Finished!
