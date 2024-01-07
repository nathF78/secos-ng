#include <irq.h>
#include <debug.h>
#include <info.h>
#include <string.h>
#include <segmem.h>
#include <io.h>
#include <gpr.h>
#include <segmentation.h>

extern info_t   *info;
extern uint32_t __kernel_start__;
extern uint32_t __kernel_end__;

int clock = 0;
int user = 0; 
uint64_t old_GP_handler;

void interrupt_clock_handler(){
    asm volatile ("pusha"); 
    debug("Clock interrupt : %d \n", clock); 
    clock++;

    //Scheduler 
    tss_t tr;
    get_tr(tr);
    tr.s0.esp = get_esp(); 
    user++; user = user % 2; 
    debug("User : %d", user); 
    change_context(user); 
    get_tr(tr); 
    set_esp(tr.s0.esp);

    //tell the hardware that we have handled the interrupt
    outb(0x20,0x20);
    
    asm volatile ("popa");
    asm volatile ("leave; iret"); 
   
}

void interrupt_test_GP_handler(){
    asm volatile ("pusha"); 
    printf("GP interruption ! \n");
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
    debug("\nInterrupt configuration... \n");
    debug("\tGetting idtr adress... ");
    idt_reg_t idtr; 
    get_idtr(idtr);
    debug(" Success !\n");
    debug("\t\tidtr addr : %lx \n", idtr.addr);

    //Interruption de l'horloge
    debug("\tConfiguring clock interrupt... ");
    idtr.desc[32].offset_1 = (int) &interrupt_clock_handler;
    debug(" Success !\n");
}

void enable_GP_intercept() {
    debug("\tEnabling GP intercept... ");
    idt_reg_t idtr; 
    get_idtr(idtr);
    old_GP_handler = idtr.desc[13].offset_1;
    idtr.desc[13].offset_1 = (int) &interrupt_test_GP_handler;
    debug(" Success !\n");
}

void disable_GP_intercept() {
    debug("\tDisabling GP intercept... ");
    idt_reg_t idtr; 
    get_idtr(idtr);
    idtr.desc[13].offset_1 = old_GP_handler;
    debug(" Success !\n");
}

void enable_hardware_interrupts() {
    debug("\tEnabling hardware interrupts... ");
    asm volatile ("sti");
    debug(" Success !\n");
}