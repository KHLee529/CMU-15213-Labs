# Bomb Lab Solution

## Register Usage

The common usage of each register are

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

```s
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

```s
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

```s
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

```s
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

```s
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

The assembly codes of phase 5 are

```s
0000000000401062 <phase_5>:
  401062: 53                    push   %rbx
  401063: 48 83 ec 20           sub    $0x20,%rsp
  401067: 48 89 fb              mov    %rdi,%rbx
  40106a: 64 48 8b 04 25 28 00  mov    %fs:0x28,%rax
  401071: 00 00
  401073: 48 89 44 24 18        mov    %rax,0x18(%rsp)
  401078: 31 c0                 xor    %eax,%eax
  40107a: e8 9c 02 00 00        callq  40131b <string_length>
  40107f: 83 f8 06              cmp    $0x6,%eax
  401082: 74 4e                 je     4010d2 <phase_5+0x70>
  401084: e8 b1 03 00 00        callq  40143a <explode_bomb>
  401089: eb 47                 jmp    4010d2 <phase_5+0x70>
  40108b: 0f b6 0c 03           movzbl (%rbx,%rax,1),%ecx
  40108f: 88 0c 24              mov    %cl,(%rsp)
  401092: 48 8b 14 24           mov    (%rsp),%rdx
  401096: 83 e2 0f              and    $0xf,%edx
  401099: 0f b6 92 b0 24 40 00  movzbl 0x4024b0(%rdx),%edx
  4010a0: 88 54 04 10           mov    %dl,0x10(%rsp,%rax,1)
  4010a4: 48 83 c0 01           add    $0x1,%rax
  4010a8: 48 83 f8 06           cmp    $0x6,%rax
  4010ac: 75 dd                 jne    40108b <phase_5+0x29>
  4010ae: c6 44 24 16 00        movb   $0x0,0x16(%rsp)
  4010b3: be 5e 24 40 00        mov    $0x40245e,%esi
  4010b8: 48 8d 7c 24 10        lea    0x10(%rsp),%rdi
  4010bd: e8 76 02 00 00        callq  401338 <strings_not_equal>
  4010c2: 85 c0                 test   %eax,%eax
  4010c4: 74 13                 je     4010d9 <phase_5+0x77>
  4010c6: e8 6f 03 00 00        callq  40143a <explode_bomb>
  4010cb: 0f 1f 44 00 00        nopl   0x0(%rax,%rax,1)
  4010d0: eb 07                 jmp    4010d9 <phase_5+0x77>
  4010d2: b8 00 00 00 00        mov    $0x0,%eax
  4010d7: eb b2                 jmp    40108b <phase_5+0x29>
  4010d9: 48 8b 44 24 18        mov    0x18(%rsp),%rax
  4010de: 64 48 33 04 25 28 00  xor    %fs:0x28,%rax
  4010e5: 00 00
  4010e7: 74 05                 je     4010ee <phase_5+0x8c>
  4010e9: e8 42 fa ff ff        callq  400b30 <__stack_chk_fail@plt>
  4010ee: 48 83 c4 20           add    $0x20,%rsp
  4010f2: 5b                    pop    %rbx
  4010f3: c3                    retq
```

We can know that the input should be a 6 characters string by the code from the
beginning to line `0x401082`. \
After checking the string length, it will start the main process of this phase,
which are lines between line `0x40108b` and line `0x4010d7`.

By analyzing the main program flow of the main process, I got the pseudo code

```c
/* ... preprocess ... */
char s[7]; // store in 0x10(%rsp)

/*
 * the "code" array is store in memory address 0x4024b0
 * 0x4024b0: 0x6d 0x61 0x64 0x75 0x69 0x65 0x72 0x73
 * 0x4024b8: 0x6e 0x66 0x6f 0x74 0x76 0x62 0x79 0x6c
 */
char code[] = {0x6d, 0x61, 0x64, 0x75, 0x69, 0x65, 0x72, 0x73,
                0x6e, 0x66, 0x6f, 0x74, 0x76, 0x62, 0x79, 0x6c};

for (int i = 0; i < 6; i++) {
    /*
     * movzbl (%rbx,%rax,1),%ecx
     * mov    %cl,(%rsp)
     * mov    (%rsp),%rdx
     * and    $0xf,%edx
     */
    tmp = 0xf & input[i];

    /*
     * movzbl 0x4024b0(%rdx),%edx
     * mov    %dl,0x10(%rsp,%rax,1)
     */
    s[i] = code[tmp];
}

/* 
 * the key string here is store in memory address 0x40245e 
 * 0x40245e: "flyers"
 */
if (string_not_equal(s, "flyers")){
    explode_bomb();
}
/* ... postprocess ... */
```

Going through this process, the valid input is a sequence with their least
important four bits are the index of `"flyers"` in array `code`. \
The corresponding ascii representation of `"flyers"` are

```c
0x66, 0x6c, 0x79, 0x65, 0x72, 0x73 
```

Therefore the index should be

```c
0x9, 0xf, 0xe, 0x5, 0x6, 0x7
```

Finally, I find a string with their last four bits match the indices, `"ionefg"`.

Phase 5 solved!!

## Phase 6

The assembly codes of this phase are too many and hard to understand so I
separated the codes into several parts.

### Part 1

First, from the beginning to line `0x401123`, it check if the inputs are six
integers, and then get into the first loop.

```s
40110e: 41 bc 00 00 00 00     mov    $0x0,%r12d ; counter start from 0
401114: 4c 89 ed              mov    %r13,%rbp 
401117: 41 8b 45 00           mov    0x0(%r13),%eax ; nums[i]
40111b: 83 e8 01              sub    $0x1,%eax
40111e: 83 f8 05              cmp    $0x5,%eax ; if the nums[0] - 1 <= 5
401121: 76 05                 jbe    401128 <phase_6+0x34>
401123: e8 12 03 00 00        callq  40143a <explode_bomb>
401128: 41 83 c4 01           add    $0x1,%r12d 
40112c: 41 83 fc 06           cmp    $0x6,%r12d ; leave loop if counter == 6
401130: 74 21                 je     401153 <phase_6+0x5f>
401132: 44 89 e3              mov    %r12d,%ebx ; the other counter
401135: 48 63 c3              movslq %ebx,%rax
401138: 8b 04 84              mov    (%rsp,%rax,4),%eax
40113b: 39 45 00              cmp    %eax,0x0(%rbp)
40113e: 75 05                 jne    401145 <phase_6+0x51>
401140: e8 f5 02 00 00        callq  40143a <explode_bomb>
401145: 83 c3 01              add    $0x1,%ebx
401148: 83 fb 05              cmp    $0x5,%ebx
40114b: 7e e8                 jle    401135 <phase_6+0x41>
40114d: 49 83 c5 04           add    $0x4,%r13
401151: eb c1                 jmp    401114 <phase_6+0x20>
401153: 48 8d 74 24 18        lea    0x18(%rsp),%rsi
401158: 4c 89 f0              mov    %r14,%rax
40115b: b9 07 00 00 00        mov    $0x7,%ecx
401160: 89 ca                 mov    %ecx,%edx
401162: 2b 10                 sub    (%rax),%edx
401164: 89 10                 mov    %edx,(%rax)
401166: 48 83 c0 04           add    $0x4,%rax
40116a: 48 39 f0              cmp    %rsi,%rax
40116d: 75 f1                 jne    401160 <phase_6+0x6c>
```

In this loop, some registers has their own particular usages shown below

|register|usage|
|:-:|:-|
|`%r12d`|counter `i`|
|`%r13`|address of the currunt number `&nums[i]`|
|`%rbp`|`&nums[i]` also|
|`%ebx`|counter `j`|
|`%rsp`|the start of the array of input number `&nums[]`|

and there is a sub-loop, shown below, nested inside this loop in which check
if the numbers after current number `nums[i]` is different from itself.

```s
401132: 44 89 e3              mov    %r12d,%ebx ; j = i
401135: 48 63 c3              movslq %ebx,%rax
401138: 8b 04 84              mov    (%rsp,%rax,4),%eax ; get nums[j]
40113b: 39 45 00              cmp    %eax,0x0(%rbp)
40113e: 75 05                 jne    401145 <phase_6+0x51>
401140: e8 f5 02 00 00        callq  40143a <explode_bomb>
401145: 83 c3 01              add    $0x1,%ebx ; j++
401148: 83 fb 05              cmp    $0x5,%ebx
40114b: 7e e8                 jle    401135 <phase_6+0x41>
```

Accordingly, the assumed corresponding c codes are

```c
for (int i = 0, *p = nums; i < 6; i++, p++) {
    if (nums[i] - 1 > 5) 
        explode_bomb();
    for (int j = i + 1; j < 6; j++) {
        if (nums[i] == nums[j])
            explode_bomb();
    }
}
```

However, in this part we can see two different forms of looping an array.

- get `(%rsp,%rax,4)` and add %rax by 1 every step which is more like the form \
  `for (int i = 0; i < 6; i++)`
- get `%r13` and add it by 4 every step which is similar to the form \
  `for (int *p = nums;; p++)`

### Part 2

Later, there is a simple part doing small tricks on each number

```s
  401153: 48 8d 74 24 18        lea    0x18(%rsp),%rsi ; the end of the iterator
  401158: 4c 89 f0              mov    %r14,%rax
  40115b: b9 07 00 00 00        mov    $0x7,%ecx
  401160: 89 ca                 mov    %ecx,%edx
  401162: 2b 10                 sub    (%rax),%edx
  401164: 89 10                 mov    %edx,(%rax)
  401166: 48 83 c0 04           add    $0x4,%rax
  40116a: 48 39 f0              cmp    %rsi,%rax
  40116d: 75 f1                 jne    401160 <phase_6+0x6c>
```

|register|usage|
|:-:|:-|
|`%rax`|a iterator for the number looping each `&nums[i]`|

Here comes the other ways to iterate an array, which is finding the end address
and check if the iterator equal to that address and look like \
`for (int *p = nums; p != (end_addr); p++)`

In this part, it substitute all numbers `nums[i]` by `7 - nums[i]`.

### Part 3

In this part, after processing on the numbers, it start doing something really
obscure. Actually the way I found what this codes did is accidentally look into
the address `0x20(%rsp)` after these codes and found the result.

```s
40116f: be 00 00 00 00        mov    $0x0,%esi
401174: eb 21                 jmp    401197 <phase_6+0xa3>
401176: 48 8b 52 08           mov    0x8(%rdx),%rdx ; node = node->next
40117a: 83 c0 01              add    $0x1,%eax
40117d: 39 c8                 cmp    %ecx,%eax
40117f: 75 f5                 jne    401176 <phase_6+0x82>
401181: eb 05                 jmp    401188 <phase_6+0x94>
401183: ba d0 32 60 00        mov    $0x6032d0,%edx 
401188: 48 89 54 74 20        mov    %rdx,0x20(%rsp,%rsi,2) ; nodes[i]->next = node
40118d: 48 83 c6 04           add    $0x4,%rsi
401191: 48 83 fe 18           cmp    $0x18,%rsi
401195: 74 14                 je     4011ab <phase_6+0xb7>
401197: 8b 0c 34              mov    (%rsp,%rsi,1),%ecx ; ecx = nums[i]
40119a: 83 f9 01              cmp    $0x1,%ecx
40119d: 7e e4                 jle    401183 <phase_6+0x8f>
40119f: b8 01 00 00 00        mov    $0x1,%eax
4011a4: ba d0 32 60 00        mov    $0x6032d0,%edx
4011a9: eb cb                 jmp    401176 <phase_6+0x82>
```

|register|usage|
|:-:|:-|
|`%rsi`|a iterator with step 4|
|`%rsp`|`&nums[]`|
|`0x20(%rsp)`|an address stores array of rearranged nodes `&nodes[]`|

In line `0x401183` and `0x4011a4`, there is a special address `0x6032d0`
contains contents below.

```s
0x6032d0 <node1>:  0x000000010000014c  0x00000000006032e0
0x6032e0 <node2>:  0x00000002000000a8  0x00000000006032f0
0x6032f0 <node3>:  0x000000030000039c  0x0000000000603300
0x603300 <node4>:  0x00000004000002b3  0x0000000000603310
0x603310 <node5>:  0x00000005000001dd  0x0000000000603320
0x603320 <node6>:  0x00000006000001bb  0x0000000000000000
```

In these nodes, I found the second half of each memory is the address of
next node, so I assume that these are nodes of a linked list.

Assumed structure is

```c
typedef struct ListNode {
    int idx;
    int val;
    struct ListNode *next;
} Node;
```

Assumed c codes of this part are

```c
for (int i = 0; i < 6; i++) {
    if (nums[i] == 1) {
        nodes[i] = head;
    } else {
        ptr = head;
        while (ptr->idx != nums[i])
            ptr = ptr->next;
        nodes[i] = ptr;
    }
}
```

### Part 4

This part is to link the nodes by the sequence of nodes.

```s
4011ab: 48 8b 5c 24 20        mov    0x20(%rsp),%rbx
4011b0: 48 8d 44 24 28        lea    0x28(%rsp),%rax
4011b5: 48 8d 74 24 50        lea    0x50(%rsp),%rsi
4011ba: 48 89 d9              mov    %rbx,%rcx
4011bd: 48 8b 10              mov    (%rax),%rdx
4011c0: 48 89 51 08           mov    %rdx,0x8(%rcx)
4011c4: 48 83 c0 08           add    $0x8,%rax
4011c8: 48 39 f0              cmp    %rsi,%rax
4011cb: 74 05                 je     4011d2 <phase_6+0xde>
4011cd: 48 89 d1              mov    %rdx,%rcx
4011d0: eb eb                 jmp    4011bd <phase_6+0xc9>
4011d2: 48 c7 42 08 00 00 00  movq   $0x0,0x8(%rdx)
4011d9: 00 
```

|register|usage|
|:-:|:-|
|`%rbx`|`nodes[0]`|
|`%rax`|an iterator to address of node array `&nodes[]`|
|`%rsi`|the end of the iterator `&nodes[]`|
|`%rcx`|an iterator to nodes `ptr`|

### Part 5

Finally in this part, it check if the sequence are the key of this phase. \
Going through the assembly codes shown below, I found that the rule for this
phase is that in the final list the value of a node shouldn't greater then the
value of its previous node.

```s
4011da: bd 05 00 00 00        mov    $0x5,%ebp
4011df: 48 8b 43 08           mov    0x8(%rbx),%rax
4011e3: 8b 00                 mov    (%rax),%eax
4011e5: 39 03                 cmp    %eax,(%rbx)
4011e7: 7d 05                 jge    4011ee <phase_6+0xfa>
4011e9: e8 4c 02 00 00        callq  40143a <explode_bomb>
4011ee: 48 8b 5b 08           mov    0x8(%rbx),%rbx
4011f2: 83 ed 01              sub    $0x1,%ebp
4011f5: 75 e8                 jne    4011df <phase_6+0xeb>
```

|register|usage|
|:-:|:-|
|`%rbp`|counter `i` from 5 to 0|
|`%rbx`|an iterator to nodes `ptr`|
|`%rax`|a variable for `ptr->next`|

review the nodes,

```s
0x6032d0 <node1>:  0x000000010000014c  0x00000000006032e0
0x6032e0 <node2>:  0x00000002000000a8  0x00000000006032f0
0x6032f0 <node3>:  0x000000030000039c  0x0000000000603300
0x603300 <node4>:  0x00000004000002b3  0x0000000000603310
0x603310 <node5>:  0x00000005000001dd  0x0000000000603320
0x603320 <node6>:  0x00000006000001bb  0x0000000000000000
```

to arrange the nodes by descending order of their values, the valid sequence is

```txt
3 -> 4 -> 5 -> 6 -> 1 -> 2
```

In part 2, it modify the origin input numbers by `nums[i] = 7 - nums[i]`,
so the final key of this phase is `432165`.

## Secret Phase

It seems there is a secret phase after phase 6 but I still didn't figure out
how to trigger it.
