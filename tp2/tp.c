/* GPLv2 (c) Airbus */
#include <debug.h>
#include <segmem.h>

void bp_handler() {
	//Q8)
	asm volatile ("pusha"); 
	//Q2)
	printf("BP interruption ! \n");
	//Q7)
	uint32_t val;
   	asm volatile ("mov 4(%%ebp), %0":"=r"(val)); 
	printf("val  blabla : %x \n", val);
	//Q9) 
	asm volatile ("popa"); 
	asm volatile ("leave; iret");  
}

void bp_trigger() {
	//Q4)
	__asm__("int $3"); 
	//Q10) 
	printf("Retour apres avoir gerer BP\n"); 
}

void tp() {
	//Q1)
	idt_reg_t idtr; 
	get_idtr(idtr);
	printf("addr : %lx \n", idtr.addr); 

	//Q3)
	idtr.desc[3].offset_1 = (int) &bp_handler; 

	// TODO call bp_trigger
	bp_trigger();
}

/*
Pour la Q3) on sait que la table IDT contient des descripteur d'interruption et on met un 3 car le code opérande de BP est 3 et donc on suppose (justement) que l'exception BP se trouve au 3eme descripteur. 

Q5) La dernière instruction de la fonction bp_handler est ret. Elle vide la pile (en faisant des pop). Le problème est qu'il manque l'adresse de retour 

Q7) l'asm volatile affiche la valeur de ebp-4 qui est là où est stocké l'adresse de retour. Donc la signification de val est l'adresse de retour.
Donc pour qu'on puisse y voir plus clair addr vaut 304d60 et val vaut 303f9b
Pour retrouver cette valeur dans le code kernel et voit ce à quoi elle correspond on fait : objdump -D kernel.elf > q7 puis cat q7 | grep 303f9b -B 5 -A 5 (B pour before et A pour after) : 
00303f97 <bp_trigger>:
  303f97:       55                      push   %ebp
  303f98:       89 e5                   mov    %esp,%ebp
  303f9a:       cc                      int3
  303f9b:       90                      nop  <------------------------ ebp-4 = saved EIP! Le nop sauve un peu le contexte je crois et c'est comme ça qu'on est censé savoir
  303f9c:       5d                      pop    %ebp
  303f9d:       c3                      ret

Q8) La valeur de retour de la fonction bp_handler() ainsi que le contexte. Pour la q8 ne pas oublier le leave -> revoir cours asm

Q11) Je dirais que ça se fait en C mais comme il faut gérer la pile et tout et que cela se fait directement avec des instructions assembleur il vaut mieux le faire directement 
en assembleur. 

QUESTION A POSER 
 
Chelou car j'ai pas les mêmes adresses que pendant que je faisait le tp sur les machines de l'insa. Q1) et Q7) -> peut être parce que j'avais pas remis les config après le TP1 ...

Si je met l'asm avant le printf dans la fonction handler ça fait un truc bizarre. 
*/
