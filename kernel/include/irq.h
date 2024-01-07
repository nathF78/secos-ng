#ifndef __IRQ_H__
#define __IRQ_H__

#include <debug.h>
#include <info.h>
#include <string.h>
#include <segmem.h>
#include <io.h>
#include <gpr.h>
#include <segmentation.h>

void interrupt_clock_handler();
void interrupt_test_GP_handler(); 
void interrupt_test_handler(); 
void interrupt_test_trigger(); 
void init_idt(); 
void enable_GP_intercept(); 
void disable_GP_intercept(); 
void enable_hardware_interrupts(); 

#endif 