/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <string.h>
#include <segmem.h>
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


 

}

