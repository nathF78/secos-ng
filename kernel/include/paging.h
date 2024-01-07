#ifndef __PAGING_H__
#define __PAGING_H__

#include <debug.h>
#include <info.h>
#include <string.h>
#include <segmem.h>
#include <pagemem.h>
#include <cr.h>

void init_kernel_pgd(); 
void init_user1_pgd(); 
void init_user2_pgd(); 
// void init_user1_ptb(pte32_t *addr, int idx); 
void init_user1_ptb(); 
void init_user2_ptb(); 
void init_kernel_ptb(pte32_t *addr, int idx); 
void enable_paging(); 


#endif