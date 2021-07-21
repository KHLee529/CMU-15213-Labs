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

## Phase 3

The assmbly codes of Phase 3 are

```assembly
0000000000400f43 <phase_3>:
  400f43: 48 83 ec 18           sub    $0x18,%rsp
  400f47: 48 8d 4c 24 0c        lea    0xc(%rsp),%rcx
  400f4c: 48 8d 54 24 08        lea    0x8(%rsp),%rdx
  400f51: be cf 25 40 00        mov    $0x4025cf,%esi
  400f56: b8 00 00 00 00        mov    $0x0,%eax
  400f5b: e8 90 fc ff ff        callq  400bf0 <__isoc99_sscanf@plt>
  400f60: 83 f8 01              cmp    $0x1,%eax
  400f63: 7f 05                 jg     400f6a <phase_3+0x27>
  400f65: e8 d0 04 00 00        callq  40143a <explode_bomb>
  400f6a: 83 7c 24 08 07        cmpl   $0x7,0x8(%rsp)
  400f6f: 77 3c                 ja     400fad <phase_3+0x6a>
  400f71: 8b 44 24 08           mov    0x8(%rsp),%eax
  400f75: ff 24 c5 70 24 40 00  jmpq   *0x402470(,%rax,8)
  400f7c: b8 cf 00 00 00        mov    $0xcf,%eax
  400f81: eb 3b                 jmp    400fbe <phase_3+0x7b>
  400f83: b8 c3 02 00 00        mov    $0x2c3,%eax
  400f88: eb 34                 jmp    400fbe <phase_3+0x7b>
  400f8a: b8 00 01 00 00        mov    $0x100,%eax
  400f8f: eb 2d                 jmp    400fbe <phase_3+0x7b>
  400f91: b8 85 01 00 00        mov    $0x185,%eax
  400f96: eb 26                 jmp    400fbe <phase_3+0x7b>
  400f98: b8 ce 00 00 00        mov    $0xce,%eax
  400f9d: eb 1f                 jmp    400fbe <phase_3+0x7b>
  400f9f: b8 aa 02 00 00        mov    $0x2aa,%eax
  400fa4: eb 18                 jmp    400fbe <phase_3+0x7b>
  400fa6: b8 47 01 00 00        mov    $0x147,%eax
  400fab: eb 11                 jmp    400fbe <phase_3+0x7b>
  400fad: e8 88 04 00 00        callq  40143a <explode_bomb>
  400fb2: b8 00 00 00 00        mov    $0x0,%eax
  400fb7: eb 05                 jmp    400fbe <phase_3+0x7b>
  400fb9: b8 37 01 00 00        mov    $0x137,%eax
  400fbe: 3b 44 24 0c           cmp    0xc(%rsp),%eax
  400fc2: 74 05                 je     400fc9 <phase_3+0x86>
  400fc4: e8 71 04 00 00        callq  40143a <explode_bomb>
  400fc9: 48 83 c4 18           add    $0x18,%rsp
  400fcd: c3                    retq   
```

In this phase, first I find a `sscanf` at the beginning of the function.
By taking the 2nd parameter of `sscanf`, I found the input should be 2 integers.
And then, through the instruction `0x400f6f`, the bomb will explode if the 1st
number is greater than 7.

Later, observing the instruction `0x400f75`, the instruction jump to the
corresponding instruction depending on the 1st given number.

The relations between given numbers and the jumping destinations are

- 0: 0x0000000000400f7c
- 1: 0x0000000000400fb9
- 2: 0x0000000000400f83
- 3: 0x0000000000400f8a
- 4: 0x0000000000400f91
- 5: 0x0000000000400f98
- 6: 0x0000000000400f9f
- 7: 0x0000000000400fa6

Finally, check if the 2nd given number equals the corresponding key number.

Therefore, there are 8 sets of the available password to phase 3. Choose one of
them to defuse this phase

Phase 3 solved!!

### Code Representation Assumption

In this phase, the insturctions jumps depending on the first number in the
passwords. I assume the original code might be a structure using `switch`.

```c
void phase_3(input_string) {
    int a, b;
    int key;
    if (sscanf(input_string, "%d %d", &a, &b) < 2)
        explode_bomb();
    switch (a) {
    case 0:
        key = key0;
        break;
    case 1:
        key = key1;
        break;
    case 2:
        key = key2;
        break;
    case 3:
        key = key3;
        break;
    case 4:
        key = key4;
        break;
    case 5:
        key = key5;
        break;
    case 6:
        key = key6;
        break;
    case 7:
        key = key7;
        break;
    }
    if (b != key)
        explode_bomb();
    return;
}
```

## Phase 4

The assmbly codes of Phase 3 are

```assembly
000000000040100c <phase_4>:
  40100c: 48 83 ec 18           sub    $0x18,%rsp
  401010: 48 8d 4c 24 0c        lea    0xc(%rsp),%rcx
  401015: 48 8d 54 24 08        lea    0x8(%rsp),%rdx
  40101a: be cf 25 40 00        mov    $0x4025cf,%esi
  40101f: b8 00 00 00 00        mov    $0x0,%eax
  401024: e8 c7 fb ff ff        callq  400bf0 <__isoc99_sscanf@plt>
  401029: 83 f8 02              cmp    $0x2,%eax
  40102c: 75 07                 jne    401035 <phase_4+0x29>
  40102e: 83 7c 24 08 0e        cmpl   $0xe,0x8(%rsp)
  401033: 76 05                 jbe    40103a <phase_4+0x2e>
  401035: e8 00 04 00 00        callq  40143a <explode_bomb>
  40103a: ba 0e 00 00 00        mov    $0xe,%edx
  40103f: be 00 00 00 00        mov    $0x0,%esi
  401044: 8b 7c 24 08           mov    0x8(%rsp),%edi
  401048: e8 81 ff ff ff        callq  400fce <func4>
  40104d: 85 c0                 test   %eax,%eax
  40104f: 75 07                 jne    401058 <phase_4+0x4c>
  401051: 83 7c 24 0c 00        cmpl   $0x0,0xc(%rsp)
  401056: 74 05                 je     40105d <phase_4+0x51>
  401058: e8 dd 03 00 00        callq  40143a <explode_bomb>
  40105d: 48 83 c4 18           add    $0x18,%rsp
  401061: c3                    retq   
```

At the beginning, I knew the input should be 2 integers by the same way in
phase 3.
However, here comes a function call to check if the first given integer is
valid.
The assembly code of `func4` are

```assembly
0000000000400fce <func4>:
  400fce: 48 83 ec 08           sub    $0x8,%rsp
  400fd2: 89 d0                 mov    %edx,%eax
  400fd4: 29 f0                 sub    %esi,%eax
  400fd6: 89 c1                 mov    %eax,%ecx
  400fd8: c1 e9 1f              shr    $0x1f,%ecx
  400fdb: 01 c8                 add    %ecx,%eax
  400fdd: d1 f8                 sar    %eax
  400fdf: 8d 0c 30              lea    (%rax,%rsi,1),%ecx
  400fe2: 39 f9                 cmp    %edi,%ecx
  400fe4: 7e 0c                 jle    400ff2 <func4+0x24>
  400fe6: 8d 51 ff              lea    -0x1(%rcx),%edx
  400fe9: e8 e0 ff ff ff        callq  400fce <func4>
  400fee: 01 c0                 add    %eax,%eax
  400ff0: eb 15                 jmp    401007 <func4+0x39>
  400ff2: b8 00 00 00 00        mov    $0x0,%eax
  400ff7: 39 f9                 cmp    %edi,%ecx
  400ff9: 7d 0c                 jge    401007 <func4+0x39>
  400ffb: 8d 71 01              lea    0x1(%rcx),%esi
  400ffe: e8 cb ff ff ff        callq  400fce <func4>
  401003: 8d 44 00 01           lea    0x1(%rax,%rax,1),%eax
  401007: 48 83 c4 08           add    $0x8,%rsp
  40100b: c3                    retq   
```

In `funct4` it is not difficult to understand what it does by going through the
assembly codes.
The thing we should care about in this phase is the parameter transfer between
caller and callee, so its important to know that the `func4` is called in the
form `func4(given_num1, 0, 14)`, then it's easy to figure out the valid keyword
of this phase.

Phase 4 solved!!

## Phase 5
