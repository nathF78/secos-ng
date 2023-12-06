/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <string.h>
#include <segmem.h>

extern info_t   *info;
extern uint32_t __kernel_start__;
extern uint32_t __kernel_end__;


#define tss_dsc(_dSc_,_tSs_)                                            \
   ({                                                                   \
      raw32_t addr    = {.raw = _tSs_};                                 \
      (_dSc_)->raw    = sizeof(tss_t);                                  \
      (_dSc_)->base_1 = addr.wlow;                                      \
      (_dSc_)->base_2 = addr._whigh.blow;                               \
      (_dSc_)->base_3 = addr._whigh.bhigh;                              \
      (_dSc_)->type   = SEG_DESC_SYS_TSS_AVL_32;                        \
      (_dSc_)->p      = 1;                                              \
   })

void userland() {
   asm volatile ("mov %eax, %cr0");
}

void print_gdt_content(gdt_reg_t gdtr_ptr) {
    seg_desc_t* gdt_ptr;
    gdt_ptr = (seg_desc_t*)(gdtr_ptr.addr);
    int i=0;
    while ((uint32_t)gdt_ptr < ((gdtr_ptr.addr) + gdtr_ptr.limit)) {
        uint32_t start = gdt_ptr->base_3<<24 | gdt_ptr->base_2<<16 | gdt_ptr->base_1;
        uint32_t end;
        if (gdt_ptr->g) {
            end = start + ( (gdt_ptr->limit_2<<16 | gdt_ptr->limit_1) <<12) + 4095;
        } else {
            end = start + (gdt_ptr->limit_2<<16 | gdt_ptr->limit_1);
        }
        debug("%d ", i);
        debug("[0x%x ", start);
        debug("- 0x%x] ", end);
        debug("seg_t: 0x%x ", gdt_ptr->type);
        debug("desc_t: %d ", gdt_ptr->s);
        debug("priv: %d ", gdt_ptr->dpl);
        debug("present: %d ", gdt_ptr->p);
        debug("avl: %d ", gdt_ptr->avl);
        debug("longmode: %d ", gdt_ptr->l);
        debug("default: %d ", gdt_ptr->d);
        debug("gran: %d ", gdt_ptr->g);
        debug("\n");
        gdt_ptr++;
        i++;
    }
}

void initGDT() {

    printf("\n");
    printf("Segmentation \n");
    // GDTR and GDT configured by GRUB
    printf("GDTR and GDT configured by GRUB\n");
    gdt_reg_t gdtr_ptr;
    get_gdtr(gdtr_ptr);
    debug("GDT addr:  0x%x ", (unsigned int) gdtr_ptr.addr);
    debug("limit: %d\n", gdtr_ptr.limit);
    print_gdt_content(gdtr_ptr);
    seg_desc_t *my_gdt = (seg_desc_t*) 0x306000;

    my_gdt[0].raw = 0ULL;

    // Ring 0 RX
    my_gdt[1].limit_1 = 0xffff;   //:16;     /* bits 00-15 of the segment limit */
    my_gdt[1].base_1 = 0x0000;    //:16;     /* bits 00-15 of the base address */
    my_gdt[1].base_2 = 0x00;      //:8;      /* bits 16-23 of the base address */
    my_gdt[1].type = 11;//Code,RX //:4;      /* segment type */
    my_gdt[1].s = 1;              //:1;      /* descriptor type */
    my_gdt[1].dpl = 0; //ring0    //:2;      /* descriptor privilege level */
    my_gdt[1].p = 1;              //:1;      /* segment present flag */
    my_gdt[1].limit_2 = 0xf;      //:4;      /* bits 16-19 of the segment limit */
    my_gdt[1].avl = 1;            //:1;      /* available for fun and profit */
    my_gdt[1].l = 0; //32bits     //:1;      /* longmode */
    my_gdt[1].d = 1;              //:1;      /* default length, depend on seg type */
    my_gdt[1].g = 1;              //:1;      /* granularity */
    my_gdt[1].base_3 = 0x00;      //:8;      /* bits 24-31 of the base address */

    // Ring 0 RW
    my_gdt[2].limit_1 = 0xffff;   //:16;     /* bits 00-15 of the segment limit */
    my_gdt[2].base_1 = 0x0000;    //:16;     /* bits 00-15 of the base address */
    my_gdt[2].base_2 = 0x00;      //:8;      /* bits 16-23 of the base address */
    my_gdt[2].type = 3; //data,RW //:4;      /* segment type */
    my_gdt[2].s = 1;              //:1;      /* descriptor type */
    my_gdt[2].dpl = 0; //ring0    //:2;      /* descriptor privilege level */
    my_gdt[2].p = 1;              //:1;      /* segment present flag */
    my_gdt[2].limit_2 = 0xf;      //:4;      /* bits 16-19 of the segment limit */
    my_gdt[2].avl = 1;            //:1;      /* available for fun and profit */
    my_gdt[2].l = 0; // 32 bits   //:1;      /* longmode */
    my_gdt[2].d = 1;              //:1;      /* default length, depend on seg type */
    my_gdt[2].g = 1;              //:1;      /* granularity */
    my_gdt[2].base_3 = 0x00;      //:8;      /* bits 24-31 of the base address */

    // Ring 3 RX
    my_gdt[3].limit_1 = 0xffff;   //:16;     /* bits 00-15 of the segment limit */
    my_gdt[3].base_1 = 0x0000;    //:16;     /* bits 00-15 of the base address */
    my_gdt[3].base_2 = 0x00;      //:8;      /* bits 16-23 of the base address */
    my_gdt[3].type = 11;//Code,RX //:4;      /* segment type */
    my_gdt[3].s = 1;              //:1;      /* descriptor type */
    my_gdt[3].dpl = 3; //ring0    //:2;      /* descriptor privilege level */
    my_gdt[3].p = 1;              //:1;      /* segment present flag */
    my_gdt[3].limit_2 = 0xf;      //:4;      /* bits 16-19 of the segment limit */
    my_gdt[3].avl = 1;            //:1;      /* available for fun and profit */
    my_gdt[3].l = 0; //32bits     //:1;      /* longmode */
    my_gdt[3].d = 1;              //:1;      /* default length, depend on seg type */
    my_gdt[3].g = 1;              //:1;      /* granularity */
    my_gdt[3].base_3 = 0x00;      //:8;      /* bits 24-31 of the base address */

    // Ring 3 RW
    my_gdt[4].limit_1 = 0xffff;   //:16;     /* bits 00-15 of the segment limit */
    my_gdt[4].base_1 = 0x0000;    //:16;     /* bits 00-15 of the base address */
    my_gdt[4].base_2 = 0x00;      //:8;      /* bits 16-23 of the base address */
    my_gdt[4].type = 3; //data,RW //:4;      /* segment type */
    my_gdt[4].s = 1;              //:1;      /* descriptor type */
    my_gdt[4].dpl = 3; //ring3    //:2;      /* descriptor privilege level */
    my_gdt[4].p = 1;              //:1;      /* segment present flag */
    my_gdt[4].limit_2 = 0xf;      //:4;      /* bits 16-19 of the segment limit */
    my_gdt[4].avl = 1;            //:1;      /* available for fun and profit */
    my_gdt[4].l = 0; // 32 bits   //:1;      /* longmode */
    my_gdt[4].d = 1;              //:1;      /* default length, depend on seg type */
    my_gdt[4].g = 1;              //:1;      /* granularity */
    my_gdt[4].base_3 = 0x00;      //:8;      /* bits 24-31 of the base address */

    gdt_reg_t my_gdtr;
    my_gdtr.addr = (long unsigned int)my_gdt;
    my_gdtr.limit = sizeof(my_gdt[1])*6;
    set_gdtr(my_gdtr);

    debug("\nGDTR and GDT");
    get_gdtr(my_gdtr);
    debug("GDT addr:  0x%x ", (unsigned int) my_gdtr.addr);
    debug("limit: %d\n", my_gdtr.limit);
    print_gdt_content(my_gdtr);
    // end Q7

    // // Q9
    // char  src[64];
    // char *dst = 0;
    // memset(src, 0xff, 64);

    // my_gdt[3].limit_1 = 0x20;   //:16;     /* bits 00-15 of the segment limit */
    // my_gdt[3].base_1 = 0x0000;    //:16;     /* bits 00-15 of the base address */
    // my_gdt[3].base_2 = 0x60;      //:8;      /* bits 16-23 of the base address */
    // my_gdt[3].type = 3; //data,RW //:4;      /* segment type */
    // my_gdt[3].s = 1;              //:1;      /* descriptor type */
    // my_gdt[3].dpl = 0; //ring0    //:2;      /* descriptor privilege level */
    // my_gdt[3].p = 1;              //:1;      /* segment present flag */
    // my_gdt[3].limit_2 = 0x0;      //:4;      /* bits 16-19 of the segment limit */
    // my_gdt[3].avl = 1;            //:1;      /* available for fun and profit */
    // my_gdt[3].l = 0; // 32 bits   //:1;      /* longmode */
    // my_gdt[3].d = 1;              //:1;      /* default length, depend on seg type */
    // my_gdt[3].g = 0;              //:1;      /* granularity */
    // my_gdt[3].base_3 = 0x00;      //:8;      /* bits 24-31 of the base address */
    // print_gdt_content(my_gdtr);
    // // end Q9

    // // Q10
    // seg_sel_t my_es;
    // my_es.index = 3;
    // my_es.ti = 0;
    // my_es.rpl = 0;
    // set_es(my_es);
    // _memcpy8(dst, src, 32);
    // // end Q10

    // // Q11
    // // _memcpy8(dst, src, 64);
    // // end Q11

    // // Q12

    // // end Q12

    // // Q13
    // // DS/ES/FS/GS
    // set_ds(gdt_usr_seg_sel(5));
    // set_es(gdt_usr_seg_sel(5));
    // set_fs(gdt_usr_seg_sel(5));
    // set_gs(gdt_usr_seg_sel(5));
    // // SS
    // //set_ss(gdt_usr_seg_sel(5)); // plante, #GP
    // tss_t TSS;
    // TSS.s0.esp = get_ebp();
    // TSS.s0.ss  = gdt_krn_seg_sel(2);
    // tss_dsc(&my_gdt[6], (offset_t)&TSS);
    // set_tr(gdt_krn_seg_sel(6));
    // // CS via farjump
    // // fptr32_t fptr = {.segment = gdt_usr_seg_sel(5), .offset = (uint32_t)userland}; 
    // // farjump(fptr);  // plante, #GP
    // // interdit, un moyen de démarrer une tâche ring 3 depuis le ring 0 est 
    // // de détourner l'usage principal de iret pour profiter du changement 
    // // de contexte que le CPU sait effectuer à ce moment-là... cf. TP3 pour l'implem.
    // // end Q13
}