*Concepts you may want to Google beforehand: C types and structs, include guards, type attributes: packed, extern, volatile, exceptions*

**Goal: Set up the Interrupt Descriptor Table to handle CPU interrupts**

This lesson and the following ones have been heavily inspired
by [JamesM's tutorial](https://web.archive.org/web/20160412174753/http://www.jamesmolloy.co.uk/tutorial_html/index.html)

Data types
----------
First, we will define some special data types in `cpu/types.h`,
which will help us uncouple data structures for raw bytes from chars and ints.
It has been carefully placed on the `cpu/` folder, where we will
put machine-dependent code from now on. Yes, the boot code
is specifically x86 and is still on `boot/`, but let's leave
that alone for now.

Some of the already existing files have been changed to use
the new `u8`, `u16` and `u32` data types.

From now on, our C header files will also have include guards.


Interrupts
----------
/*
expectable and  unexpectable
*/
Interrupts are one of the main things that a kernel needs to 
handle. We will implement it now, as soon as possible, to be able
to receive keyboard input in future lessons.

Another examples of interrupts are: divisions by zero, out of bounds,
invalid opcodes, page faults, etc.

Interrupts are handled on a vector, with entries which are
similar to those of the GDT (lesson 9). However, instead of
programming the IDT in assembly, we'll do it in C.

/*
GDT(Global Descriptor Table),only one , base and limit
*/
`cpu/idt.h` defines how an idt entry is stored `idt_gate` (there need to be
256 of them, even if null, or the CPU may panic) and the actual
idt structure that the BIOS will load, `idt_register` which is 
just a memory address and a size, similar to the GDT register.

Finally, we define a couple variables to access those data structures
from assembler code.

`cpu/idt.c` just fills in every struct with a handler. 
As you can see, it is a matter
of setting the struct values and calling the `lidt` assembler command.


ISRs
----

The Interrupt Service Routines run every time the CPU detects an 
interrupt, which is usually fatal. 

We will write just enough code to handle them, print an error message,
and halt the CPU.

On `cpu/isr.h` we define 32 of them, manually. They are declared as
`extern` because they will be implemented in assembler, in `cpu/interrupt.asm`

Before jumping to the assembler code, check out `cpu/isr.c`. As you can see,
we define a function to install all isrs at once and load the IDT, a list of error
messages, and the high level handler, which kprints some information. You
can customize `isr_handler` to print/do whatever you want.

Now to the low level which glues every `idt_gate` with its low-level and
high-level handler. Open `cpu/interrupt.asm`. Here we define a common
low level ISR code, which basically saves/restores the state and calls
the C code, and then the actual ISR assembler functions which are referenced
on `cpu/isr.h`

Note how the `registers_t` struct is a representation of all the registers
we pushed in `interrupt.asm`

That's basically it. Now we need to reference `cpu/interrupt.asm` from our
Makefile, and make the kernel install the ISRs and launch one of them.
Notice how the CPU doesn't halt even though it would be good practice
to do it after some interrupts.


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//https://zhuanlan.zhihu.com/p/25867829
//https://wiki.osdev.org/GDT_Tutorial

 1.volatile:由于主存访问速度远不及CPU处理速度，为提高机器整体性能因为访问寄存器要比访问主存快的多,所以编译器减少存取主存的优化，直接读取寄存器的值。volatile是告诉编译器直接去访问变量地址，可以解决多线程下值变化的问题。

 2.字节对齐（32位）
 typedef struct {
    char m1;
    short m2;
    int m3;
 }Family; //sizeof(Family) == 8

 typedef struct {
    char m1;
    int m3;
    short m2;
 }Family; //sizeof(Family) == 12

 对齐原则:
 .char 偏移量必须为sizeof(char) 即1的倍数,可以任意地址开始存储
 .short 偏移量必须为sizeof(short) 即2的倍数,只能从0,2,4...等2的倍数的地址开始存储
 .int 偏移量必须为sizeof(int) 即4的倍数,只能从0,4,8...等4的倍数的地址开始存储
 .float 偏移量必须为sizeof(float) 即4的倍数,只能从0,4,8...等4的倍数的地址开始存储
 .double 偏移量必须为sizeof(double)即8的倍数,只能从0,8,16...等地址开始存储

 结构体的总大小,也就是sizeof的结果,必须是其内部最大成员的整数倍.不足的要补齐
 #pragma pack() 是指按照默认的对齐方式，也就是上面按照最大成员size大小对齐

 #pragma pack (1) //告诉编译器以1个字节 sizeof(Family) == 7
 #pragma pack (2) //告诉编译器以1个字节 sizeof(Family) == 8
 typedef struct {
    char m1;
    int m3;
    short m2;
 }Family; 

 //优点:这种对齐方式简化了处理器和储存器之间的硬件设计。假如我要访问一个8字节的浮点型，那我只需要去访问一个8字节的储存块，如果不是默认的对齐方式，可能要访问两个。

 3.pageFault
 https://www.zybuluo.com/theway/note/1330948






中断：中断是执行过程中的强制转移，转移到相应的处理程序。比如说timer的action或者鼠标的插入，还有一些错误的处理。
ISR:构建IDT,响应中断事件




