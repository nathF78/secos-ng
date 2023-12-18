/* GPLv2 (c) Airbus */
#include <debug.h>
#include <segmem.h>
#include <string.h>

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


void tp() {
	// Q2)
    gdt_reg_t gdtr; 
    get_gdtr(gdtr); 
    print_gdt_content(gdtr);

    // Q3)
    printf("ss : %x \ncs : %x \nes : %x \nfs : %x \nds : %x \ngs : %x \n", get_ss(), get_seg_sel(cs), get_es(), get_fs(), get_ds(), get_gs());

    // Q5)
    seg_desc_t my_gdt[7];
    my_gdt[0].raw = 0ULL;
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

    //Q6) + Q7)
    gdt_reg_t new_gdt; 
    new_gdt.desc = my_gdt;
    new_gdt.limit = sizeof(my_gdt)-1;
    set_gdtr(new_gdt);
    get_gdtr(gdtr);
    print_gdt_content(gdtr); 

    //Q8)
    seg_sel_t my_ds; 
    my_ds.index = 2; 
    my_ds.ti = 0; 
    my_ds.rpl = 0;
    set_ds(my_ds);
    printf("ds : %x \n", get_ds());

    /*
    seg_sel_t my_cs; 
    my_cs.index = 1; 
    my_cs.ti = 0; 
    my_cs.rpl = 0;
    set_cs(my_cs);
    printf("cs : %x \n", get_seg_sel(cs));
    */

    /*
    set_ds(0x10);
    set_cs(0x8); 
    printf("ds : %x \n", get_ds()); 
    printf("cs : %x \n", get_seg_sel(cs));  
    */

    //Q9)
    printf("\nQ9)\n"); 
    my_gdt[3].limit_1 = 0x20;   //:16;     /* bits 00-15 of the segment limit */
    my_gdt[3].base_1 = 0x0000;    //:16;     /* bits 00-15 of the base address */
    my_gdt[3].base_2 = 0x60;      //:8;      /* bits 16-23 of the base address */
    my_gdt[3].type = 3; //data,RW //:4;      /* segment type */
    my_gdt[3].s = 1;              //:1;      /* descriptor type */
    my_gdt[3].dpl = 0; //ring0    //:2;      /* descriptor privilege level */
    my_gdt[3].p = 1;              //:1;      /* segment present flag */
    my_gdt[3].limit_2 = 0x0;      //:4;      /* bits 16-19 of the segment limit */
    my_gdt[3].avl = 1;            //:1;      /* available for fun and profit */
    my_gdt[3].l = 0; // 32 bits   //:1;      /* longmode */
    my_gdt[3].d = 1;              //:1;      /* default length, depend on seg type */
    my_gdt[3].g = 0;              //:1;      /* granularity */
    my_gdt[3].base_3 = 0x00;      //:8;      /* bits 24-31 of the base address */

    char  src[64];
    char *dst = 0;

    memset(src, 0xff, 64);
    _memcpy8(dst, src, 32);
    printf("dst : %p \n", dst);
    get_gdtr(gdtr);
    print_gdt_content(gdtr);
    printf("\n");

    //Q10)
    printf("Q10)\n");
    seg_sel_t my_es; 
    my_es.index = 3;
    my_es.ti = 0;
    my_es.rpl = 0;
    set_es(my_es); 
    _memcpy8(dst, src, 32);
    printf("es : %x \n", get_es()); 
    printf("dst : %p \n", dst);
    get_gdtr(gdtr);
    print_gdt_content(gdtr);
    printf("\n");

    //Q11)
    printf("Q11)\n"); 
    _memcpy8(dst, src, 64);
    get_gdtr(gdtr);
    print_gdt_content(gdtr);
    printf("\n"); 

    //Q12)
    printf("Q12)\n"); 
    my_gdt[5].type = 3;
    my_gdt[5].d = 1; 
    my_gdt[5].dpl = 3; 
    my_gdt[5].base_1 = 0x0000;
    my_gdt[5].base_2 = 0x00;
    my_gdt[5].base_3 = 0x00; 
    my_gdt[5].limit_1 = 0xffff;
    my_gdt[5].limit_2 = 0xf;
    my_gdt[5].s = 1;
    my_gdt[5].p = 1; 
    my_gdt[5].avl = 1;
    my_gdt[5].l = 0;
    my_gdt[5].g = 1;
    my_gdt[4].limit_1 = 0xffff;   //:16;     /* bits 00-15 of the segment limit */
    my_gdt[4].base_1 = 0x0000;    //:16;     /* bits 00-15 of the base address */
    my_gdt[4].base_2 = 0x00;      //:8;      /* bits 16-23 of the base address */
    my_gdt[4].type = 11;//Code,RX //:4;      /* segment type */
    my_gdt[4].s = 1;              //:1;      /* descriptor type */
    my_gdt[4].dpl = 0; //ring0    //:2;      /* descriptor privilege level */
    my_gdt[4].p = 1;              //:1;      /* segment present flag */
    my_gdt[4].limit_2 = 0xf;      //:4;      /* bits 16-19 of the segment limit */
    my_gdt[4].avl = 1;            //:1;      /* available for fun and profit */
    my_gdt[4].l = 0; //32bits     //:1;      /* longmode */
    my_gdt[4].d = 1;              //:1;      /* default length, depend on seg type */
    my_gdt[4].g = 1;              //:1;      /* granularity */
    my_gdt[4].base_3 = 0x00;      //:8;      /* bits 24-31 of the base address */
    get_gdtr(gdtr);
    print_gdt_content(gdtr);
    printf("\n");

    // Q13
    printf("Q13)\n"); 
    // DS/ES/FS/GS
    /*
    set_ds(gdt_usr_seg_sel(5));
    set_es(gdt_usr_seg_sel(5));
    set_fs(gdt_usr_seg_sel(5));
    set_gs(gdt_usr_seg_sel(5));
    */ 
   seg_sel_t load_ds; 
    load_ds.index = 4;
    load_ds.ti = 0;
    load_ds.rpl = 3;
    set_ds(load_ds);
    seg_sel_t load_es; 
    load_es.index = 5;
    load_es.ti = 0;
    load_es.rpl = 3;
    set_es(load_es);
    seg_sel_t load_fs; 
    load_fs.index = 5;
    load_fs.ti = 0;
    load_fs.rpl = 3;
    set_fs(load_fs);
    seg_sel_t load_gs; 
    load_gs.index = 5;
    load_gs.ti = 0;
    load_gs.rpl = 3;
    set_gs(load_gs);
    printf("es : %x \nfs : %x \nds : %x \ngs : %x \n",get_es(), get_fs(), get_ds(), get_gs());
    // SS
    // set_ss(gdt_usr_seg_sel(5)); // plante, #GP
    /*
    seg_sel_t load_ss; 
    load_ss.index = 5;
    load_ss.ti = 0;
    load_ss.rpl = 0;
    set_ss(load_ss);
    */
    // CS via farjump
    // fptr32_t fptr = {.segment = gdt_usr_seg_sel(5), .offset = (uint32_t)userland}; 
    // farjump(fptr);  // plante, #GP
    // interdit, un moyen de démarrer une tâche ring 3 depuis le ring 0 est 
    // de détourner l'usage principal de iret pour profiter du changement 
    // de contexte que le CPU sait effectuer à ce moment-là... cf. TP3 pour l'implem.
}

/*
Q1) L'instruction SGDT permet d'entreposer le sélecteur de segment dans le registre GDTR (registre de table global de descripteur) dans l'opérande de destination.
La macro qui la représente est : SEG_SEL_GDT ou plutôt get_gdtr

Q3) On peut voir quel registre utilise quel segment à l'aide de la définition du selecteur de segment : lorsqu'on les affiches on a ss = 10, cs = 8, es = 10, fs = 1O
ds = 10 et gs = 10 (on est en hexa). Lorsqu'on regarde la déninition d'un selecteur de segment on voit qu'il faut diviser par 8 pour avoir le numéro du descripteur utilisé 
donc ici cs utilise le 1 et le reste utilise le 2 (puisque 10 hexa = 16 en décimal). 

Q4) Cf slide 55 du cours mais en gros on traduit le nombre obtenu dans le print en binaire et on voit que les premiers bits sont à 0. Soit le bit de 2³ ou 2⁴ sont à 1 ce qui correspond au segment
selectionner. Ici c'est soit le 1 soit le 2 et on voit donc que ces segment prennent toute la mémoire donc grub est en mode flat. 

Q5) Regarder comment fonctionne les bits d'un descripteur de segment dans le cours pour comprendre par rapport à l'énoncé. Pour le type = 11 c'est mieux avec accessed comme on sait si on a déjà acccéder 
à ce segment ou pas
On met le met le dpl à O donc on est bien en niveau de privilège le plus élevé. 

Q8) On met 0x8 car on reprend la description d'un selecteur de segment : Ce qui nous permet de récupérer l'élément à l'index 1 de my_gdt. Sinon le résultat à l'air conforme avec la doc puisqu'on
est en flat model (chapitre 3.2) et que du coup on a aucune restriction !

Q9) Attention le bit 22 (ici .d) qui correspond à D/B à une signification différente suivant si on est en 32 bits ou en 64 bits ! De plus la limite (qui est codée sur 16 bits) est donnée en octet !

Q10) Si on suppose que même si dest est null, on est dans le segment chargé dans es, du coup la première adresse du segment (qui est 0x0) correspond à l'adresse du début de segment 0x6000000
c'est donc pour ça qu'on a pas d'erreur (je pense). A moins que ce qu'on copie dans dst c'est l'adresse 0xffffff... et c'est pour ça que ça plante pas (bizarre si c'est la dernière adresse de la
mémoire)
Non je ne pense pas que ça soit ça. Peut être que comme on reste en ring 0 (qui le privilège le plus élevé : le privilège kernel) beh on s'en ballec d'être dans un segment ou non, comme on a tous les droits 
genre même si on veut ecrire en dehors du segment beh on a les droits donc y a pas de raison qu'on ne puisse pas écrire à l'adresse 0 cat dst est null. 
Donc pour cette question c'était bien ma première réponse : quand le cpu est un mode protégé avec la segmentation activé les adresses sont linéaires càd addr_physique = addr_base_segment_courant + adresse_linéaire
donc ici on copie de l'adresse physique [ES_base] + [adresse linéaire] = 0x600000 + 0 = 0x600000 à l'adresse physique : [ES_base] + [adresse_linéaire] = 0x600000 + 31 = 0x60001f et donc on est pas censé avoir de 
fautes puisqu'on reste bien dans les limites du segment.
Ensuite pour le chargement de sélecteur de segment dans ES : bon je suis pas sûr d'avoir encore bien compris mais s'il est chargé dans ES c'est parce que de 1 c'est un segment data et de 2 c'est parce que 
l'instruction liée à _memcpy8 (MOVSB peut être à aller voir) est lié au registre ES et donc c'est pour ça qu'on se trouve dans ce segment particulier. 

Q11) Ici je suppose que c'est censé cracher (ça le fait pas chez moi) ou plutôt lever une exception car on fait la copie sur 64 bits alors que notre segment en fait que 32 (peut être plutot
mémoire ne fait que des adresse codet sur 32 bits)
En fait pareil que pour celle d'avant ce n'est peut être réellement pas censé cracher comme on est en privilège kernel on s'en ballec de la taille du segment on peut écrire derrière si on en a envie
puisqu'on en a les droits. On  gros on est pas contraints à rester dans le segment puisqu'on a les droits d'écrire partout
Bon ici c'est bien censé cracher car on copie de l'adresse physqique : [ES_base] + addr_linéaire = 0x600000 + 0 = 0x600000 à l'adresse physique [ES_base] + addr_linéaire = 0x600000 + 63 = 0x60003F ce qui 
est en dehors du segment et donc on devrait avoir une GP ! Je sais pas pourquoi ça marche pas chez moi. Surement car il y a un bug kvm au debut : https://github.com/foxlet/macOS-Simple-KVM/issues/252 pour resolution ... 

Q13) Pour le chargement de ES, DS, FS, GS tout marche bien et pour le chargement de SS ça plante on a un GP fault car on a pas les accès au registre SS qui est pour la stack et donc critique. 
Pour es, fs, gs et ds on obtient 2b car l'indice est à 5 et le rpl à  3 donc on a un binaire de 101011 ce qui donne 43 soit 2b en hexa
Attention par contre si au lieu de charger le descripteur d'indice 5 on met le 4 on se prend bien un GP car on a notre CPL à 0 (je suppose mais c'est pas important), notre RPL à 3 donc le max vaut 3 et notre DPL 
à 0 et 3 n'est pas <= (inf ou eg) à 0. Ca ne passe pas !!!!!! Logique ! 
QUESTION A POSER. 

Q9) -> Q13) pas tout capter, je sais pas si parfois ça devais pas planter et ça ne le fait pas mais j'avoue je sais pas ce qu'il faut observer à chaque fois. 
*/
/*
Dans le TP1 :
- Questions 9, 10 et 11 : si je ne me trompe pas on crée un descripteur de segment sur 32 octets de l'adresse 0x600000 à 0x600020. La première question que je me pose est comment on sélectionne ce segment ensuite. Car dans la question 10 on est censé écrire dans ce segment (enfin c'est ce que je suppose). Donc est-ce que le simple fait de le charger dans le registre "es" permet de dire que ce qu'on va faire dans la suite du code se trouve en mémoire dans ce segment ? Ensuite, si on suppose qu'on est bien dans ce segment pour les questions 10 et 11 alors je ne suis pas sûr de comprendre pourquoi je n'ai pas d'erreur à l'exécution sur ces deux questions. Est-ce parce que le segment dans lequel on est à les droits 0 qui correspondent au droit kernel et par conséquent on n'est pas réellement contraint par la taille et l'adresse du segment, c'est-à-dire que l'on peut écrire à l'adresse 0 et que l'on peut dépasser le segment car en soit on a les droits du noyau ?
- Question 13 : Pour faire le lien avec les questions précédentes, je ne suis d'avoir compris la différence entre les privilèges spécifiés dans le descripteur de segment et ceux spécifiés dans le sélecteur de segment et à quel moment les uns ou les autres sont utilisés. Et donc je ne comprends pas pourquoi on a une erreur lors du chargement de SS ou du chargement de CS car il ne me semble pas avoir vu de différence de privilèges pour utiliser SS ou ES, GS, DS, FS .
*/