/* GPLv2 (c) Airbus */
#include <debug.h>
#include <pagemem.h>
#include <info.h>
#include <string.h>
#include <segmem.h>
#include <cr.h>
#include "irq.c"
#include "segmentation.c"
#include "paging.c"

extern info_t *info;
extern uint32_t __kernel_start__;
extern uint32_t __kernel_end__;

char *inttypetochar(int t)
{
   switch (t)
   {

   case 1:
      return ("MULTIBOOT_MEMORY_AVAILABLE");
   case 2:
      return ("MULTIBOOT_MEMORY_RESERVED");
   case 3:
      return ("MULTIBOOT_MEMORY_ACPI_RECLAIMABLE");
   case 4:
      return ("MULTIBOOT_MEMORY_NVS");
   default:
      return ("Unknown type");
   }
}

__attribute__((section(".user"))) void user1()
{  
   int i = 0;
   while (1)
   {
      if (i++ % 10000000 == 0)
      {
         printf("userland1 says hello!\n");
         //asm volatile("mov %eax, %cr0");
      }
   }
}

__attribute__((section(".user"))) void user2()
{
   debug("user2\n");
   while (1)
      ;
}

void tp()
{
   printf("\n");

   // show memory map
   printf("Memory map:\n");
   debug("kernel mem [0x%p - 0x%p]\n", &__kernel_start__, &__kernel_end__);
   debug("MBI flags 0x%x\n", info->mbi->flags);

   multiboot_memory_map_t *entry = (multiboot_memory_map_t *)info->mbi->mmap_addr;
   while ((uint32_t)entry < (info->mbi->mmap_addr + info->mbi->mmap_length))
   {
      // Q2
      debug("[0x%x - ", (unsigned int)entry->addr);
      debug("0x%x]", (unsigned int)(entry->len + entry->addr - 1));
      debug(" %s\n", inttypetochar(entry->type));
      // end Q2
      entry++;
   }

   // ********** Segmentation **********
   init_gdt();

   // ********** Gestion des interruptions **********
   init_idt();
   //enable_hardware_interrupts();

   // ********** Pagination **********
   init_kernel_pgd();
   init_kernel_ptb((pte32_t *)0x601000, 0);
   init_kernel_ptb((pte32_t *)0x602000, 1);
   init_kernel_ptb((pte32_t *)0x603000, 2);
   //init_kernel_ptb((pte32_t *)0x604000, 3);

   init_user1_pgd();
   init_user1_ptb((pte32_t *)0x601000, 0);
   init_user1_ptb((pte32_t *)0x602000, 1);
   init_user1_ptb((pte32_t *)0x603000, 2);
   init_user1_ptb((pte32_t *)0x604000, 3);
   // init_user1_ptb((pte32_t *)0x901000);
   // init_user1_ptb((pte32_t *)0x902000);
   // init_user1_ptb((pte32_t *)0x903000);
   // init_user1_ptb((pte32_t *)0x904000);
   // init_user1_ptb((pte32_t *)0x905000);
   // init_user1_ptb((pte32_t *)0x906000);
   // init_user1_ptb((pte32_t *)0x907000);
   // init_user1_ptb((pte32_t *)0x908000);
   // init_user1_ptb((pte32_t *)0x909000);


   enable_paging();

   //user1();

   go_to_ring3(&user1);
   //userland2();

   int i = 0;
   while (1)
   {
      if (i++ % 10000000 == 0)
      {
         debug("kernel says hello!\n");
      }
   }
}
