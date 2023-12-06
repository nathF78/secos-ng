/* GPLv2 (c) Airbus */
#include <debug.h>
#include <pagemem.h>
#include <cr.h>

void tp() {
	//Q1
	cr3_reg_t *cr3; 
	cr3 = (cr3_reg_t*) get_cr3();   
	printf("cr3 addr : %x \n", cr3->addr); 
	printf("Après CR3 \n"); 

	//Q2
	pde32_t *pde =(pde32_t *) 0x600000; //Atention tu donnes une adresse mais tu n'alloue rien ... cd Note au début du readme
	cr3_reg_t newcr3; 
	newcr3.addr = (uint32_t)pde >> 12;  //On fait ça car avec le champs 20 il prend que les bits de poids faible, on décalle alors notre adresse de 12 zéros. 
	// newcr3.pcd = 0;
	// newcr3.pwt = 0;
	// newcr3.r1 = 0; 
	// newcr3.r2 = 0; 
	printf ("newcr3 addr : %x \n", newcr3.addr); 
	printf("Adresse pgd : %x \n", newcr3.addr<<12); 
	printf("cr3 adrr avant changement : %x \n", cr3->addr);
	set_cr3(&newcr3); //ATTENTION le set cr3 marche avec cr3_reg_t* et pas juste un cr3_reg_t ...
	cr3 = (cr3_reg_t*) get_cr3();   
	printf("cr3 adrr: %x \n", cr3->addr);

	//Q3
	cr0_reg_t cr0;
	cr0.raw = get_cr0();
	cr0_reg_t newcr0; 
	newcr0.am = cr0.am;
	newcr0.cd = cr0.cd;
	newcr0.em = cr0.em;
	newcr0.et = cr0.et;
	newcr0.mp = cr0.mp;
	newcr0.ne = cr0.ne;
	newcr0.nw = cr0.nw;
	newcr0.pe = cr0.pe;
	newcr0.pg = 1; 
	newcr0.r1 = cr0.r1;
	newcr0.r2 = cr0.r2;
	newcr0.r3 = cr0.r3;
	newcr0.ts = cr0.ts;
	newcr0.wp = cr0.wp;

	//Test activation pagination 
	int i = 2;
	int *ptr = &i; 
	printf("bit de pagination : %d \n", cr0.pg); 
	printf("i avant pagination : %d et ptr : %p \n", *ptr, ptr);
	// set_cr0(newcr0);
	printf("Je me sers de newcr0 car le compilo me casse la tete : %d \n", newcr0.pe); 
	cr0.raw =  get_cr0();
	printf("bit de pagination : %d \n", cr0.pg);
	printf("i apres pagination : %d et ptr : %p \n", *ptr, ptr);

	//Q4
	pte32_t *ptb = (pte32_t *) 0x601000; 

	//Q5
	pde32_t pde_entry_4_ptb; 
	pde_entry_4_ptb.addr = (uint32_t) ptb >> 12; 
	printf("adrr ptb : %x \n", pde_entry_4_ptb.addr); //Validé !
	pde[0] = pde_entry_4_ptb; 

	// pte32_t entry1; 


}

/*
Q3) Bon beh j'avoue je sais pas ce qu'il est censé se passer mais je pense que comme on a des adresses physiques avant et maintenant que des adresses virtuelles 
il faut faire la correspondance entre les anciennes adresses et les nouvelles 
Un peu chelou quand même j'ai le doute de savoir si la pagination est vraiment bien activé 

Q5) Ok si je suis ma logique ici, toutes les adresses sont virtuelles donc il faut regarder les adresses physiques que l'on trouve dans le noyau à l'aide de la
commande readelf -e kernel.elf (dans la section En-têtes de programme) et faire correspondre une adresse virtuelle l'adresse virtuelle qui s'y trouve. Exemple : si
on a l'adresse 0x0032ff44 l'adresse en virtuelle ne sera pas lu pareil et donc il faut la décomposer en 10 bits d'indexe dans la pgd puis 10 bits d'index dans la ptb 
puis 12 bits et écrire à cette endroit l'adresse 0x0032ff44. 
Adresses phyisques utilisées par le noyaux : 0x00300000, 0x00300010 et 0X00302010
Ox00300000 = 0000 0000 0011 0000 0000 0000 0000 0000 -> 0000000000 (pgd) 1100000000 (ptb) 000000000000 -> entry 0 pgd, entry 2⁸+2⁹ = 768 ptb, entry 0
0X00300010 = 0000 0000 0011 0000 0000 0000 0001 0000 -> 0000000000 (pgd) 1100000000 (ptb) 000000010000 -> entry 0 pgd, entry 2⁸+2⁹ = 768 ptb, entry 2⁴ = 16 
0x00302010 = 0000 0000 0011 0000 0010 0000 0001 0000 -> 0000000000 (pgd) 1100000010 (ptb) 000000010000 -> entry 0 pgd, entry 2¹+2⁸+2⁹ = 770 ptb, entry 2⁴ = 16 

Notes correction : 

- Lorsqu'on fait de l'identity maper une bonne technique est de regarder l'adresse max physique qu'on utilise puis d'identity maper tout jusqu'à cette adresse pour pas perdre trop de place
*/
