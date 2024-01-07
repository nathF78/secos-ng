/* GPLv2 (c) Airbus */

#include <segmentation.h>
#include <debug.h>
#include <info.h>
#include <string.h>
#include <segmem.h>
#include <cr.h>
#include <pagemem.h>
#include <stack.h>
#include <gpr.h>

extern info_t   *info;
extern uint32_t __kernel_start__;
extern uint32_t __kernel_end__;
extern uint32_t __user1_start__; 
extern uint32_t __user2_start__; 


#define c0_idx  1
#define d0_idx  2
#define c3_idx  3
#define d3_idx  4
#define ts_idxa 5
#define ts_idxb 6 


#define c0_sel  gdt_krn_seg_sel(c0_idx)
#define d0_sel  gdt_krn_seg_sel(d0_idx)
#define c3_sel  gdt_usr_seg_sel(c3_idx)
#define d3_sel  gdt_usr_seg_sel(d3_idx)
#define ts_sela gdt_krn_seg_sel(ts_idxa)
#define ts_selb gdt_krn_seg_sel(ts_idxb)

seg_desc_t *GDT = (seg_desc_t*) 0x310000;
tss_t      TSSA;
tss_t      TSSB; 

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

    
    set_tr(ts_sela);
    tss_t tr;
    get_tr(tr); 
    debug("Esp dans le tr du cpu : %x " ,tr.s0.esp); 


    set_cr3((uint32_t)(pde32_t *)0x700000); // userland1 pgd à changer 

    set_esp(TSSA.s0.esp);
    uint32_t esp = get_esp(); 
    debug("esp : %x \n", esp);  

    debug("ptr : %x \n", (uint32_t) ptr);

    debug("Entering now...\n");

    // uint32_t   ustack = 0x900000;
    // asm volatile (
    //   "push %0 \n" // ss
    //   "push %1 \n" // esp pour du ring 3 !
    //   "pushf   \n" // eflags
    //   "push %2 \n" // cs
    //   "push %3 \n" // eip
    //   "iret"
    //   ::
    //    "i"(d3_sel),
    //    "m"(ustack),
    //    "i"(c3_sel),
    //    "r"(ptr)
    //   );
    asm volatile (
        "iret"
    ); 
}

void change_context(int user){
    if (user == 0) {
        set_cr3((uint32_t)(pde32_t *)0x700000);
        set_tr(ts_sela);
    }
    else if (user == 1){
        set_cr3((uint32_t)(pde32_t *)0xa00000);
        set_tr(ts_selb);
    }
}

void init_stack(){
    uint32_t *stack1 = (uint32_t *) stack_nu1;
    uint32_t *stack2 = (uint32_t *) stack_nu2;
    uint32_t stacku1 = stack_u1;
    uint32_t stacku2 = stack_u2;
    uint32_t flags = get_flags();

    //Initialisation du contexte de la tâche 1 pour le premier lancement 
    stack1[0] = d3_sel;                         //ss3
    stack1[-1] = stacku1;                       //esp3
    stack1[-2] = flags;                         //FLAGS
    stack1[-3] = c3_sel;                        //cs 
    stack1[-4] = (uint32_t) &__user1_start__;   //eip

    //Initialisation du contexte de la tâche 1 pour le premier lancement 
    stack2[0] = d3_sel;                         //ss3
    stack2[-1] = stacku2;                       //esp3
    stack2[-2] = flags;                         //FLAGS
    stack2[-3] = c3_sel;                        //cs 
    stack2[-4] = (uint32_t) &__user2_start__;   //eip
    stack2[-5] = (uint32_t) 0;                  //ebx
    stack2[-6] = (uint32_t) 0;                  //eax
    stack2[-7] = (uint32_t) 0;                  //ecx
    stack2[-8] = (uint32_t) 0;                  //edx
    stack2[-9] = (uint32_t) 0;                  //esi
    stack2[-10] = (uint32_t) 0;                 //edi

    //Initialisation des TSS correspondantes 
    TSSA.s0.esp = (uint32_t) &stack1[-4]; 
    TSSA.s0.ss = d0_sel; 
    TSSB.s0.esp = (uint32_t) &stack2[-10]; 
    TSSB.s0.ss = d0_sel; 
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

    tss_dsc(&GDT[ts_idxa], (offset_t)&TSSA); 
    tss_dsc(&GDT[ts_idxb], (offset_t)&TSSB);

    gdtr.desc  = GDT;
    gdtr.limit = sizeof(GDT[0])*7 - 1;
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
