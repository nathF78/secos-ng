#ifndef __SEGMENTATION_H__
#define __SEGMENTATION_H__

#include <debug.h>
#include <info.h>
#include <string.h>
#include <segmem.h>
#include <cr.h>
#include <pagemem.h>

void print_gdt_content(gdt_reg_t gdtr_ptr);
void test_ring0(); 
void go_to_ring3(void* ptr); 
void init_gdt();  


#endif