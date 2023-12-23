#include <debug.h>
#include <info.h>
#include <string.h>
#include <segmem.h>
#include <pagemem.h>

extern info_t *info;
extern uint32_t __kernel_start__;
extern uint32_t __kernel_end__;

/* kernel is at 0x30000000 and user at 0x90000000*/

pde32_t *pgd_kernel = (pde32_t *)0x600000;

pde32_t *pgd_user1 = (pde32_t *)0x700000;

pde32_t *pgd_user2 = (pde32_t *)0x800000;

void init_kernel_pgd()
{
    debug("\nKernel paging configuration (identity mapped)... \n");
    uint32_t cr3 = get_cr3();
    debug("\tCurrent CR3 = 0x%x\n", (unsigned int)cr3);

    // PGD du noyau
    debug("\tKernel PGD initialization at physical addr : %p... ", pgd_kernel);
    // On définit la pgd courante
    set_cr3((uint32_t)pgd_kernel);
    cr3 = get_cr3();
    memset((void *)pgd_kernel, 0, PAGE_SIZE);
    debug(" Success !\n");
    debug("\t\tNew CR3 = 0x%x\n", (unsigned int)cr3);
}

void init_user1_pgd()
{
    debug("\tUser1 PGD initialization at physical addr : %p... ", pgd_user1);
    // On définit la pgd courante
    set_cr3((uint32_t)pgd_user1);
    uint32_t cr3 = get_cr3();
    memset((void *)pgd_user1, 0, PAGE_SIZE);
    debug(" Success !\n");
    debug("\t\tNew CR3 = 0x%x\n", (unsigned int)cr3);
    debug("\t\tResetting CR3 to kernel pgd... ");
    set_cr3((uint32_t)pgd_kernel);
    debug(" Success !\n");
}

void init_user1_ptb(pte32_t *addr, int idx)
{
    debug("\tUser1 PTB initialization at physical addr : %p... ", addr);
    pte32_t *ptb = addr;
    for (int i = 0; i < 1024; i++)
    {
        pg_set_entry(&ptb[i], PG_USR | PG_RW, i);
    }
    pg_set_entry(&pgd_user1[idx], PG_USR | PG_RW, page_nr(ptb));
    debug(" Success !\n");
}

void init_kernel_ptb(pte32_t *addr, int idx)
{
    debug("\tKernel PTB initialization at physical addr : %p... ", addr);
    pte32_t *ptb = addr;
    for (int i = 0; i < 1024; i++)
    {
        pg_set_entry(&ptb[i], PG_KRN | PG_RW, i);
    }
    pg_set_entry(&pgd_kernel[idx], PG_KRN | PG_RW, page_nr(ptb));
    debug(" Success !\n");
}

void enable_paging()
{
    debug("\tEnabling paging (set CR0)... ");
    uint32_t cr0 = get_cr0();
    set_cr0(cr0 | CR0_PG );
    debug(" Success !\n");
}