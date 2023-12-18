#include <debug.h>
#include <segmem.h>
#include <string.h>

//Index des segments que l'on va selectionner 
#define c0_idx  1
#define d0_idx  2
#define c3_idx  3
#define d3_idx  4
#define ts_idx  5

//Ici on définit plusieurs sélecteur de segment, le get_usr/krn_seg_sel permet de créer un selecteur (le chiffre associé plutôt) qui pointe vers le descripteur passer en argument et avec des privilège (pour le sélecteur) soit usr soit krn
#define c0_sel  gdt_krn_seg_sel(c0_idx)
#define d0_sel  gdt_krn_seg_sel(d0_idx)
#define c3_sel  gdt_usr_seg_sel(c3_idx) //ce qui va changer entre c3 et d3 eest le type du descripteur un en data et un en code (celui la)
#define d3_sel  gdt_usr_seg_sel(d3_idx) //ce qui va changer entre c3 et d3 eest le type du descripteur un en code et un en data (celui la)
#define ts_sel  gdt_krn_seg_sel(ts_idx)

seg_desc_t GDT[6]; //Les descripteur de segments 
tss_t      TSS; //TSSS bien de le voir mais pas important pour ce TP 

//Défini un descripteur de segment en mode flat avec le _dSc_ pour l'adresse du descripteur, _pVl_ pour les privilèges et _tYp_ pour le type 

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

//Definit le TSS mais pas besoin pour ce tp 
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

//Ici on définit les différent descripteur avec les différent droits 
#define c0_dsc(_d) gdt_flat_dsc(_d,0,SEG_DESC_CODE_XR)
#define d0_dsc(_d) gdt_flat_dsc(_d,0,SEG_DESC_DATA_RW)
#define c3_dsc(_d) gdt_flat_dsc(_d,3,SEG_DESC_CODE_XR)
#define d3_dsc(_d) gdt_flat_dsc(_d,3,SEG_DESC_DATA_RW)

//Initialise la gdt 
void init_gdt() {
   gdt_reg_t gdtr;

   GDT[0].raw = 0ULL;

   //Initialisaton des descripteur 
   c0_dsc( &GDT[c0_idx] );
   d0_dsc( &GDT[d0_idx] );
   c3_dsc( &GDT[c3_idx] );
   d3_dsc( &GDT[d3_idx] );

   //Creation de la table (cf tp1)
   gdtr.desc  = GDT;
   gdtr.limit = sizeof(GDT) - 1;
   set_gdtr(gdtr);

   //Initialisation des registres, ces deux là sont particulièrement important car ce sont eux qu'on doit changer dans le tp mais ce n'est pas si simples car il sont protégé et on n'a pas le droit d'écraser leur valeur sauf avec un IRET (manip fait ensuite pour pouvoir passer en ring3)
   set_cs(c0_sel); //ici le selecteur est en kernel ainsi que son descripteur 
   set_ss(d0_sel); //pareil

   //Initialisation des autres registres en ring 0 
   set_ds(d0_sel);
   set_es(d0_sel);
   set_fs(d0_sel);
   set_gs(d0_sel);
}

void userland() {
   asm volatile ("mov %eax, %cr0");
}

void tp() {
   // Q1

   //On init la gdt et les registres en ring 0 puis on va passer en contexte ring 3 
   init_gdt();

   //La on passe les registres simple en ring 3 (mode usr). Ici on peut le faire directement
   set_ds(d3_sel);
   set_es(d3_sel);
   set_fs(d3_sel);
   set_gs(d3_sel);
   TSS.s0.esp = get_ebp();
   TSS.s0.ss  = d0_sel;
   tss_dsc(&GDT[ts_idx], (offset_t)&TSS);
   set_tr(ts_sel);
   //Maintenant pour finir le changement de contexte il nous reste les deux selecteurs SS et CS à passer en ring 3 ce qu'on va faire à la prochaines questions 
   // end Q1

   // Q2

   //Ici on prépare la pile pour pouvoir mettre les bonnes valeurs à un retour d'intéruption (cf cours) et surtout on met les valeurs des selceteurs que l'on veut pour le changement de contexte en ring 3 pour SS et CS
   asm volatile (
   "push %0    \n" // ss
   "push %%ebp \n" // esp
   "pushf      \n" // eflags
   "push %1    \n" // cs
   "push %2    \n" // eip  -> adresse de retour !
   // end Q2

   // Q3

   //On efectue le changement de contexte grâce à l'instruction IRET qui fait beaucoup de chose dont écraser les valeurs de CS et SS
   "iret"
   ::
    "i"(d3_sel),
    "i"(c3_sel),
    "r"(&userland)
   );
   // end Q3
}

/*
Question à poser :

En vrai même si je capte la logique dans la q2 je comprends pas comment on a l'idée d'utiliser ça ! Du coup la question est comment on est censé penser à ça ? -> coco l'assembleur + lire et comprendre le code et avoir bien compris les questions ! 
Et, est-ce qu'il est possible de faire autrement avec un push ss par exemple car c'est ce que j'ai fait quand je me suis retrouvé seul devant l'exercice. -> non ici push ss n'as pas de sens car cela voudrait dire qu'on veut remplacer SS par la valeur actuelle de SS ce qui n'est pas le but de l'exercice. Si on fait push ss on ne fait pas de changement de contexte. 
Typiquement le d3_sel est-ce qu'on aurait pu le faire comme on avait fait dans le tp1 avec le type seg_sel_t ? -> Oui mais ici on nous montre une manière certes plus complexe de déclarer des segments mais aussi plus standard et plus utilisé ! 
*/
