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
