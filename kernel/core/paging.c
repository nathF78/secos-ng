#include <paging.h>
#include <debug.h>
#include <info.h>
#include <string.h>
#include <segmem.h>
#include <pagemem.h>
#include <cr.h>

extern info_t *info;
extern uint32_t __kernel_start__;
extern uint32_t __kernel_end__;

pde32_t *pgd_kernel = (pde32_t *)0x400000;

pde32_t *pgd_user1 = (pde32_t *)0x700000;
pte32_t *ptb1_user1 = (pte32_t *)0x701000; 
pte32_t *ptb2_user1 = (pte32_t *)0x702000;

pde32_t *pgd_user2 = (pde32_t *)0xa00000;
pte32_t *ptb1_user2 = (pte32_t *)0xa01000;
pte32_t *ptb2_user2 = (pte32_t *)0xa02000;
pte32_t *ptb3_user2 = (pte32_t *)0xa03000;

void init_kernel_pgd()
{
    debug("\nKernel paging configuration (identity mapped)... \n");
    uint32_t cr3 = get_cr3();
    debug("\tCurrent CR3 = 0x%x\n", (unsigned int)cr3);

    debug("\tKernel PGD initialization at physical addr : %p... ", pgd_kernel);
    set_cr3((uint32_t)pgd_kernel);
    cr3 = get_cr3();
    memset((void *)pgd_kernel, 0, PAGE_SIZE);
    debug(" Success !\n");
    debug("\t\tNew CR3 = 0x%x\n", (unsigned int)cr3);
}

void init_user1_pgd()
{
    debug("\tUser1 PGD initialization at physical addr : %p... ", pgd_user1);
    set_cr3((uint32_t)pgd_user1);
    uint32_t cr3 = get_cr3();
    memset((void *)pgd_user1, 0, PAGE_SIZE);
    debug(" Success !\n");
    debug("\t\tNew CR3 = 0x%x\n", (unsigned int)cr3);
    debug("\t\tResetting CR3 to kernel pgd... ");
    set_cr3((uint32_t)pgd_kernel);
    debug(" Success !\n");
}

void init_user2_pgd()
{
    debug("\tUser2 PGD initialization at physical addr : %p... ", pgd_user2);
    set_cr3((uint32_t)pgd_user2);
    uint32_t cr3 = get_cr3();
    memset((void *)pgd_user2, 0, PAGE_SIZE);
    debug(" Success !\n");
    debug("\t\tNew CR3 = 0x%x\n", (unsigned int)cr3);
    debug("\t\tResetting CR3 to kernel pgd... ");
    set_cr3((uint32_t)pgd_kernel);
    debug(" Success !\n");
}

void init_user1_ptb()
{
    pte32_t *ptb = ptb1_user1;
    debug("\tUser1 PTB initialization at physical addr : %p... ", ptb);

    //Initialisation 1ere ptb 
    for (int i = 0; i < 1024; i++)
    {
        pg_set_entry(&ptb[i], PG_USR | PG_RW, i + 0 * 1024); // ATTENTION je pense qu'on devrait faire autrement mais je n'ai pas la réponse pour le moment 
    }
    pg_set_entry(&pgd_user1[0], PG_USR | PG_RW, page_nr(ptb));
    debug(" Success !\n");

    //Initialisation 2eme ptb 
    ptb = ptb2_user1;
    uint32_t curseur = 0x400000;
    uint32_t offset = 4096; 
    debug("\tUser1 PTB initialization at physical addr : %p... ", ptb); 
    for (int i = 0; i < 1024; i++)
    {
        if ((curseur + (i+1)*offset) <= 0x500000)
        {
            pg_set_entry(&ptb[i], PG_KRN | PG_RW, i + 1 * 1024);
        }
        else 
        {
            pg_set_entry(&ptb[i], PG_USR | PG_RW, i + 1 * 1024);
        }
    }
    pg_set_entry(&pgd_user1[1], PG_USR | PG_RW, page_nr(ptb));    
    debug(" Success !\n");
}

void init_user2_ptb()
{
    pte32_t *ptb = ptb1_user2;
    debug("\tUser2 PTB initialization at physical addr : %p... ", ptb);

    //Initialisation 1ere ptb 
    for (int i = 0; i < 1024; i++)
    {
        pg_set_entry(&ptb[i], PG_KRN | PG_RW, i + 0 * 1024);
    }
    pg_set_entry(&pgd_user2[0], PG_USR | PG_RW, page_nr(ptb));
    debug(" Success !\n");

    //Initialisation 2eme ptb 
    ptb = ptb2_user2;
    uint32_t curseur = 0x400000;
    uint32_t offset = 4096; 
    debug("\tUser2 PTB initialization at physical addr : %p... ", ptb); 
    for (int i = 0; i < 1024; i++)
    {
        if ((curseur + (i+1)*offset) <= 0x500000)
        {
            pg_set_entry(&ptb[i], PG_KRN | PG_RW, i + 1 * 1024);
        }
    }
    pg_set_entry(&pgd_user2[1], PG_USR | PG_RW, page_nr(ptb));    
    debug(" Success !\n");

    //Initialisation 3eme ptb 
    ptb = ptb3_user2;
    debug("\tUser2 PTB initialization at physical addr : %p... ", ptb);
    curseur = 0x800000;
    for (int i = 0; i < 1024; i++)
    {
        pg_set_entry(&ptb[i], PG_USR | PG_RW, i + 2 * 1024);
    }
    pg_set_entry(&pgd_user2[2], PG_USR | PG_RW, page_nr(ptb));
    debug(" Success !\n");
}

void init_kernel_ptb(pte32_t *addr, int idx)
{
    debug("\tKernel PTB initialization at physical addr : %p... ", addr);
    pte32_t *ptb = addr;
    for (int i = 0; i < 1024; i++)
    {
        pg_set_entry(&ptb[i], PG_KRN | PG_RW, i + idx * 1024);
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