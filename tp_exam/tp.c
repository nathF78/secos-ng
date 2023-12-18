/* GPLv2 (c) Airbus */
#include <debug.h>
#include <pagemem.h>
#include <info.h>
#include <string.h>
#include <segmem.h>
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

void userland2() {
   int i = 0;
   while (1) {
      if (i++ % 10000000 == 0) {
         debug("userland2 says hello!\n");
         asm volatile ("mov %eax, %cr0");
      }
      else {
         asm volatile ("nop");
      }
   }
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

   // ********** Segmentation **********
   init_gdt();

   // ********** Gestion des interruptions **********
   init_idt();
   enable_hardware_interrupts();

   // ********** Pagination **********
   debug("\nPaging configuration... \n");
   uint32_t cr3 = get_cr3();
	debug("\tCurrent CR3 = 0x%x\n", (unsigned int) cr3);

   // PGD du noyau
   debug("\tKernel PGD initialization... ");
   //Pagination -> Je pense faudrait une fonction pour définir la pagination de chaque tâche puis une fonction qui nous permette de switch -> truc du swtich faudra regarder pour les tables TLB 
   pde32_t *pgd = (pde32_t*)0x600000; //Définition d'une première pgd à voir où on place les suivantes 

   // On définit la pgd courante
   set_cr3((uint32_t)pgd); 
   cr3 = get_cr3();
   debug(" Success !\n");
	debug("\t\tNew CR3 = 0x%x\n", (unsigned int) cr3);

   // On définit la ptb associé à la pgd courante
   debug("\tKernel PTB1 initialization... ");
   // plus logique à 1000 entrées de 4 octets -> 4Ko ?
   pte32_t *ptb = (pte32_t*)0x601000; //Définition de la ptb associé -> à voir où on les stockes car si limite 0x400000 ça va être juste 
   for(int i=0;i<1024;i++) {          // Avec ça on id map tout jusqu'à 0x400000
	 	pg_set_entry(&ptb[i], PG_KRN|PG_RW, i); // Ici faut voir les drois qu'on met à chaque tâche et c'est là où ça se complique
	}
	memset((void*)pgd, 0, PAGE_SIZE);
	pg_set_entry(&pgd[0], PG_KRN|PG_RW, page_nr(ptb)); // De même ici faut voir comment on met les droits
   debug(" Success !\n");
   //Attention, bien id map tout ce qu'on fait avant la pagination !! 

   // On définit la ptb2 associé à la pgd courante //comprend pas trop ce qu'on fait ici ni pourquoi ???
   debug("\tKernel PTB2 initialization... ");
   pte32_t *ptb2 = (pte32_t*)0x602000;
	for(int i=0;i<1024;i++) {
		pg_set_entry(&ptb2[i], PG_KRN|PG_RW, i+1024);
	}
	pg_set_entry(&pgd[1], PG_KRN|PG_RW, page_nr(ptb2));
   debug(" Success !\n");

   //activation de la pagination -> fait planter les GP exception -> à voir pourquoi
   // debug("\tEnabling paging (set CR0)... ");
   // uint32_t cr0 = get_cr0();
	// set_cr0(cr0|CR0_PG);
   // debug(" Success !\n");

   // debug("PTB[1] = %d\n", ptb[1].raw);

   //enable_GP_intercept();
   

   //pb passage en mode user -> voir si on peut pas faire un truc du genre
   //debug("PTB[1] = %d\n", ptb[1].raw);

   go_to_ring3(&test_ring0);

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

