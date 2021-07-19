# Bomb Lab Solution

## Register Usage

| Register | Usage |
|:---:| :--- |
|%rax|basically for return value|
|%rbx|callee-save reg|
|%rcx|function arg 3|
|%rdx|function arg 2|
|%rdi|function arg 0|
|%rsi|function arg 1|
|%rbp|callee-save reg|
|%rsp|Stack pointer|
|%r8 |function arg 4|
|%r9 |function arg 5|
|%r10|caller-save reg|
|%r11|caller-save reg|
|%r12|callee-save reg|
|%r13|callee-save reg|
|%r14|callee-save reg|
|%r15|callee-save reg|

## Instructions

mov 0x20($rsp),$rbx -> rbx is the content of ($rsp + 0x20)
lea 0x20($rsp),$rbx -> rbx = $rsp + 0x20

## Phase 1

The assembly codes of Phase 1 bomb is shown below

``` assembly
0000000000400ee0 <phase_1>:
  400ee0: 48 83 ec 08           sub    $0x8,%rsp
  400ee4: be 00 24 40 00        mov    $0x402400,%esi
  400ee9: e8 4a 04 00 00        callq  401338 <strings_not_equal>
  400eee: 85 c0                 test   %eax,%eax
  400ef0: 74 05                 je     400ef7 <phase_1+0x17>
  400ef2: e8 43 05 00 00        callq  40143a <explode_bomb>
  400ef7: 48 83 c4 08           add    $0x8,%rsp
  400efb: c3                    retq   
```

By the function names at `400ee9` and `400ef2`, it's obvious that this phase 1
function only do a comparison between the input string (already saved in %rdi)
and a string saved in memory address `0x402400`.

Therefore, using gdb we can print out the string in `0x402400` and write that
into the first line of the input file.

```shell
(gdb) x/s 0x402400
0x402400:       "Border relations with Canada have never been better."
```

Phase 1 solved!!

## Phase 2

The assmbly codes of Phase 2 are

```assembly
0000000000400efc <phase_2>:
  400efc: 55                    push   %rbp
  400efd: 53                    push   %rbx
  400efe: 48 83 ec 28           sub    $0x28,%rsp
  400f02: 48 89 e6              mov    %rsp,%rsi
  400f05: e8 52 05 00 00        callq  40145c <read_six_numbers>
  400f0a: 83 3c 24 01           cmpl   $0x1,(%rsp) ; num[0] == 1?
  400f0e: 74 20                 je     400f30 <phase_2+0x34> ; if true jump
  400f10: e8 25 05 00 00        callq  40143a <explode_bomb>
  400f15: eb 19                 jmp    400f30 <phase_2+0x34>
  400f17: 8b 43 fc              mov    -0x4(%rbx),%eax ; tmp = nums[i-1]
  400f1a: 01 c0                 add    %eax,%eax ; tmp = 2 * tmp
  400f1c: 39 03                 cmp    %eax,(%rbx) ; nums[i] == tmp?
  400f1e: 74 05                 je     400f25 <phase_2+0x29>
  400f20: e8 15 05 00 00        callq  40143a <explode_bomb>
  400f25: 48 83 c3 04           add    $0x4,%rbx ; i++
  400f29: 48 39 eb              cmp    %rbp,%rbx ; i < 6?
  400f2c: 75 e9                 jne    400f17 <phase_2+0x1b>
  400f2e: eb 0c                 jmp    400f3c <phase_2+0x40>
  400f30: 48 8d 5c 24 04        lea    0x4(%rsp),%rbx ; rbx = &(num + 1)
  400f35: 48 8d 6c 24 18        lea    0x18(%rsp),%rbp ; rbx =  &(num + 6)
  400f3a: eb db                 jmp    400f17 <phase_2+0x1b>
  400f3c: 48 83 c4 28           add    $0x28,%rsp
  400f40: 5b                    pop    %rbx
  400f41: 5d                    pop    %rbp
  400f42: c3                    retq   
```

First, it calls a function `read_six_numbers`, so I guess that in this
phase, the password is a string composed of 6 numbers.
Next, I figure out the main process flow.

- start function
- read 6 number to rsp (&nums = $rsp)
- check if nums[0] == 1, if not then explode
- initialize the loop (0x400f30)
- run the loop to check if any nums[i] == 2 * nums[i-1] for i > 0

Thus, the only string fit this pattern is `1 2 4 8 16 32`

Phase 2 solved!!
