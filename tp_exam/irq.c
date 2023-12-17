/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <string.h>
#include <segmem.h>
#include <io.h>

extern info_t   *info;
extern uint32_t __kernel_start__;
extern uint32_t __kernel_end__;

int clock = 0;

void interrupt_clock_handler(){
    asm volatile ("pusha"); 
    debug("Clock interrupt : %d \n", clock); 
    clock++;

    //tell the hardware that we have handled the interrupt
    outb(0x20,0x20);
    
    asm volatile ("popa");
    asm volatile ("leave; iret"); 
   
}

void interrupt_test_handler(){
	asm volatile ("pusha"); 
	printf("BP interruption ! \n");
	uint32_t val;
   	asm volatile ("mov 4(%%ebp), %0":"=r"(val)); 
	//printf("val  blabla : %x \n", val);
	asm volatile ("popa"); 
	asm volatile ("leave; iret"); 
}

void interrupt_test_trigger() { 
	__asm__("int $32"); 
	printf("Retour apres avoir gerer interupt\n"); 
}

void init_idt() {
    debug("Interrupt configuration... \n");
    idt_reg_t idtr; 
    get_idtr(idtr);
    debug("\tidtr addr : %lx \n", idtr.addr);
    debug("\t\tSuccess !\n");

    //Interruption de l'horloge
    debug("\tConfiguring clock interrupt... \n");
    idtr.desc[32].offset_1 = (int) &interrupt_clock_handler;
    debug("\t\tSuccess !\n");
}

void enable_hardware_interrupts() {
    debug("\tEnabling hardware interrupts... \n");
    asm volatile ("sti");
    debug("\t\tSuccess !\n");
}