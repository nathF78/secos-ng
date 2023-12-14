/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <string.h>
#include <segmem.h>
#include <pagemem.h>
#include <cr.h>
#include "irq.c"
#include "segmentation.c"


extern info_t   *info;
extern uint32_t __kernel_start__;
extern uint32_t __kernel_end__;

char* inttypetochar(int t) {
   switch(t) {

      case 1:
         return("MULTIBOOT_MEMORY_AVAILABLE");
      case 2:
         return("MULTIBOOT_MEMORY_RESERVED");
      case 3:
         return("MULTIBOOT_MEMORY_ACPI_RECLAIMABLE");
      case 4:
         return("MULTIBOOT_MEMORY_NVS");
      default:
         return("Unknown type");
   }
}

__attribute__((section(".user")))
void user1() {
   debug("user1\n");
   while(1);
}

__attribute__((section(".user")))
void user2() {
   debug("user2\n");
   while(1);
}

void tp() {
   printf("\n");

   // show memory map
   printf("Memory map:\n");
   debug("kernel mem [0x%p - 0x%p]\n", &__kernel_start__, &__kernel_end__);
   debug("MBI flags 0x%x\n", info->mbi->flags);

   multiboot_memory_map_t* entry = (multiboot_memory_map_t*)info->mbi->mmap_addr;
   while((uint32_t)entry < (info->mbi->mmap_addr + info->mbi->mmap_length)) {
      // Q2 
      debug("[0x%x - ", (unsigned int)entry->addr);
      debug("0x%x]", (unsigned int) (entry->len + entry->addr - 1));
      debug(" %s\n", inttypetochar(entry->type));
      // end Q2
      entry++;
   }

   // Init GDT
   initGDT();

   //Gestion des interruptions 
   init_idt();
   enable_hardware_interrupts();


   //Pagination -> Je pense faudrait une fonction pour définir la pagination de chaque tâche puis une fonction qui nous permette de switch -> truc du swtich faudra regarder pour les tables TLB 
   pde32_t *pgd = (pde32_t*)0x302000; //Définition d'une première pgd à voir où on place les suivantes 
   pte32_t *ptb = (pte32_t*)0x601000; //Définition de la ptb associé -> à voir où on les stockes car si limite 0x400000 ça va être juste 
   for(int i=0;i<1024;i++) {          // Avec ça on id map tout jusqu'à 0x400000
	 	pg_set_entry(&ptb[i], PG_KRN|PG_RW, i); // Ici faut voir les drois qu'on met à chaque tâche et c'est là où ça se complique
	}
	memset((void*)pgd, 0, PAGE_SIZE);
	pg_set_entry(&pgd[0], PG_KRN|PG_RW, page_nr(ptb)); // De même ici faut voir comment on met les droits
   //Attention, bien id map tout ce qu'on fait avant la pagination !! 

   int i = 0;
   while (1) {
      if (i++ % 10000000 == 0) {
         debug("kernel says hello!\n");
      }
      else {
         asm volatile ("nop");
      }
   }


   



 

}

