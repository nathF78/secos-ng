/* GPLv2 (c) Airbus */

#include <debug.h>
#include <info.h>
#include <string.h>
#include <segmem.h>

extern info_t   *info;
extern uint32_t __kernel_start__;
extern uint32_t __kernel_end__;

#define c0_idx  1
#define d0_idx  2
#define c3_idx  3
#define d3_idx  4
#define ts_idx  5

#define c0_sel  gdt_krn_seg_sel(c0_idx)
#define d0_sel  gdt_krn_seg_sel(d0_idx)
#define c3_sel  gdt_usr_seg_sel(c3_idx)
#define d3_sel  gdt_usr_seg_sel(d3_idx)
#define ts_sel  gdt_krn_seg_sel(ts_idx)

seg_desc_t *GDT = (seg_desc_t*) 0x310000;
tss_t      TSS;

#define gdt_flat_dsc(_dSc_,_pVl_,_tYp_)                                 \
   ({                                                                   \
      (_dSc_)->raw     = 0;                                             \
      (_dSc_)->limit_1 = 0xffff;                                        \
      (_dSc_)->limit_2 = 0xf;                                           \
      (_dSc_)->type    = _tYp_;                                         \
      (_dSc_)->dpl     = _pVl_;                                         \
      (_dSc_)->d       = 1;                                             \
      (_dSc_)->g       = 1;                                             \
      (_dSc_)->s       = 1;                                             \
      (_dSc_)->p       = 1;                                             \
   })

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

#define c0_dsc(_d) gdt_flat_dsc(_d,0,SEG_DESC_CODE_XR)
#define d0_dsc(_d) gdt_flat_dsc(_d,0,SEG_DESC_DATA_RW)
#define c3_dsc(_d) gdt_flat_dsc(_d,3,SEG_DESC_CODE_XR)
#define d3_dsc(_d) gdt_flat_dsc(_d,3,SEG_DESC_DATA_RW)


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
        debug("\t\t");
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

void test_ring0() {
    debug("Trying a kernel instruction...\n");
    asm volatile ("mov %eax, %cr0");
    debug("We are in ring 0!\n");
    asm volatile ("leave"); 
}

void go_to_ring3(void* ptr) {
    debug("Going to userland...\n");

    set_ds(d3_sel);
    set_es(d3_sel);
    set_fs(d3_sel);
    set_gs(d3_sel);
    //set_cs(c3_sel);

    TSS.s0.esp = get_ebp();
    TSS.s0.ss  = d0_sel;

    tss_dsc(&GDT[ts_idx], (offset_t)&TSS);
    set_tr(ts_sel);

    set_cr3((uint32_t)(pde32_t *)0x700000); // userland1 pgd à changer 

    debug("Entering now...\n");
    asm volatile (
    "push %0    \n" // ss
    "push %1 \n" // esp
    "pushf      \n" // eflags
    "push %1    \n" // cs
    "push %2    \n" // eip
    "iret"
    ::
    "i"(d3_sel),
    "i"(c3_sel),
    "r"(ptr)
    );
}

void init_gdt() {
    gdt_reg_t gdtr;

    debug("\n");
    debug("Segmentation configuration... \n");
    // GDTR and GDT configured by GRUB
    debug("\tGDTR and GDT configured by GRUB : \n");
    gdt_reg_t gdtr_ptr;
    get_gdtr(gdtr_ptr);
    debug("\tGDT addr:  0x%x ", (unsigned int) gdtr_ptr.addr);
    debug("\tlimit: %d\n", gdtr_ptr.limit);
    print_gdt_content(gdtr_ptr);

    //Setting up new GDT
   GDT[0].raw = 0ULL;

   c0_dsc( &GDT[c0_idx] );
   d0_dsc( &GDT[d0_idx] );
   c3_dsc( &GDT[c3_idx] );
   d3_dsc( &GDT[d3_idx] );

   gdtr.desc  = GDT;
   gdtr.limit = sizeof(GDT[0])*6 - 1;
   set_gdtr(gdtr);

    debug("\tNew GDTR and GDT : \n");
   debug("\tGDT addr:  0x%x ", (unsigned int) gdtr.addr);
   debug("\tlimit: %d\n", gdtr.limit);
   print_gdt_content(gdtr);


    debug("\tSetting up segment selectors... ");
   set_cs(c0_sel);

   set_ss(d0_sel);
   set_ds(d0_sel);
   set_es(d0_sel);
   set_fs(d0_sel);
   set_gs(d0_sel);
   debug(" Success !\n");
}
