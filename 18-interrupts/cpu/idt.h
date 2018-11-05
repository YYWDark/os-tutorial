#ifndef IDT_H
#define IDT_H

#include "types.h"

/* Segment selectors */
#define KERNEL_CS 0x08

/* How every interrupt gate (handler) is defined */
typedef struct {
    u16 low_offset; /* Lower 16 bits of handler function address */
    u16 sel; /* Kernel segment selector */
    u8 always0;
    /* First byte
     * Bit 7: "Interrupt is present"
     * Bits 6-5: Privilege level of caller (0=kernel..3=user)
     * Bit 4: Set to 0 for interrupt gates
     * Bits 3-0: bits 1110 = decimal 14 = "32 bit interrupt gate" */
     /*
      Bits 6-5 gate 的权限设置为 3 级：这样可以给用户代码有足够的权限去访问 gate
      gate 的权限设置为 0 级：只允许内核代码访问，用户无权通过这个 gate 去访问 interrupt handler
      Bits 3-0 来标明该descriptor的类型，还有常见1111==32-bit trap gate，
      0101 = task gate
      
      OX8E = 1000 1110
     */
    u8 flags; 
    u16 high_offset; /* Higher 16 bits of handler function address */
} __attribute__((packed)) idt_gate_t ;

/* A pointer to the array of interrupt handlers.
 * Assembly instruction 'lidt' will read it */
typedef struct {
    u16 limit;
    u32 base;
} __attribute__((packed)) idt_register_t;

#define IDT_ENTRIES 256
idt_gate_t idt[IDT_ENTRIES];
idt_register_t idt_reg;


/* Functions implemented in idt.c */
void set_idt_gate(int n, u32 handler);
void set_idt();

#endif
