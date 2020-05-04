#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tos.h>
#include "complet.h"
extern S_FICHIER *tab_file;
extern int nb_fichier;

char *unshift;

#define BYTE char
#define WORD short int
#define LONG long
#define UBYTE unsigned char
#define UWORD unsigned short int
#define ULONG unsigned long

typedef struct {
	char     NOM[64];
	UWORD    *DONNEES;
	LONG     TAILLE;
	WORD     X1,Y1,X2,Y2;
} S_SPRITE;



void user(void);
void super(void);

void INIT_resolution(void);void EXIT_resolution(void);
void INIT_vecteur(void);
void EXIT_vecteur(void);

void logout(void);
void vsync(void);

void SPRITE_afficher(void *ecran, void *sprite);
void SPRITE_sauver(void *ecran, void *sprite);
LONG SPRITE_encoder(WORD nbl, void *ecran, void *sprite);

void FONT_carac(void *ecran, WORD carac, UWORD coul);
void print(UWORD *ecran, char *chaine, UWORD coul);

void SOURIS_get320240(void);

void zone_NOT(WORD h, WORD w, WORD *scr);

void tga24(UBYTE *source, UWORD *dest);

extern BYTE ksouris;
extern WORD xsouris,ysouris;
extern WORD *ae1,*ae2;
extern WORD ecr1,ecr2;

char name[64]="", path[64]="";
int r;

char msg[64];

#define SWAPW(a,b) {WORD r; r=a; a=b; b=r;}

#define MAX_SPRITE 16
S_SPRITE sprite[MAX_SPRITE], *sprite_courant = sprite, *last_sprite = sprite;

char *mymalloc(size_t t) {
	char *r;
	
	r = malloc(t);
	if (r == 0)
		logout();
	return r;
}

int charger(char *name, void *where) {
	register int r;

	r = Fopen(name, FO_READ);
	if (r < 0)
		return -1;
	Fread(r, -1L, where);
	Fclose(r);
	return 0;
}

int handle;
WORD fond_souris;

/* attend que l'utilisateur relache le bouton gauche de la souris */
void mouse_release(void) {
	do {
		vsync();
		ae1[xsouris+ysouris*320L] = fond_souris;
		SOURIS_get320240();
		fond_souris = ae1[xsouris+ysouris*320L];
		ae1[xsouris+ysouris*320L] = 0xffff;

		sprintf(msg, "X:%3d Y:%3d", xsouris, ysouris);
		print(ae1+320*30L, msg, 0xffff);

	} while (ksouris & 2);
	ae1[xsouris+ysouris*320L] = fond_souris;
}

/* attend que l'utilisateur appuye sur le bouton gauche de la souris */
void mouse_push(void) {
	do {
		vsync();
		ae1[xsouris+ysouris*320L] = fond_souris;
		SOURIS_get320240();
		fond_souris = ae1[xsouris+ysouris*320L];
		ae1[xsouris+ysouris*320L] = 0xffff;

		sprintf(msg, "X:%3d Y:%3d", xsouris, ysouris);
		print(ae1+320*30L, msg, 0xffff);
	} while (!(ksouris & 2));

	ae1[xsouris+ysouris*320L] = fond_souris;
}

int choisir_fichier(char *intitule, char *s, char *d) {
	UBYTE *clav=0xfffffc02, cla, oclav, cnt;
	UWORD *ofond;
	char *curs;
	LONG key;

	mouse_release();

	ofond = mymalloc(240*640L);
	memcpy(ofond, ae1, 240*640L);

	COMP_debuter();

	memset(ae1+40*320L, 0, 128000L);
	print(ae1+40*320L, intitule, 0x001f);

	curs = d; d[0] = 0;
	cnt = 0;
	oclav = 0;
	do {
		vsync();

		cla = *clav;
		
		if (cla == 0)
			continue;
		/* on fait gaffe a la repetition de touche */
		if (cla >= 128)
			continue;
		if (oclav != cla)
			cnt = 0;
		else
			cnt++;
		if (cnt >= 5)
			cnt = 0;
		if (cnt != 0)
			continue;
		oclav = cla;

		switch (cla) {
		case 0x0f: /* TAB */
			/* on fait un tab => on va completer */
			EXIT_vecteur();
			COMP_completer(d);
			memset(ae1+60*320L, 0, 180*640L);
			{
			S_FICHIER *curs, *fcurs;
			WORD coul, nb;
			curs = tab_file;
			fcurs = tab_file+nb_fichier;
			for (nb=0; (curs<fcurs) && (nb<60); curs++) {
				if (curs->FLAGS & F_AFFICHER) {
					if (curs->FLAGS & F_REPERTOIRE)
						coul = 0xf800;
					else
						coul = 0xfff0;
					print(ae1+(nb%20)*8*320L+(nb/20)*104L+60*320L,
					      curs->NOM, coul);
					print(ae2+(nb%20)*8*320L+(nb/20)*104L+60*320L,
					      curs->NOM, coul);
					nb++;
				}
			}
			}
			INIT_vecteur();
			curs = d+strlen(d); /* on met le curseur a la fin */
			break;
		case 0x53: /* DEL */
			if (*curs != 0) {
				strcpy(curs, curs+1);
				print(ae1+48*320L+strlen(d)*8L, "  ",0);
				print(ae2+48*320L+strlen(d)*8L, "  ",0);
			}
			break;
		case 0x0e: /* BKSP */
			if (curs > d) {
				strcpy(curs-1,curs);
				curs--;
				print(ae1+48*320L+strlen(d)*8L, "  ",0);
				print(ae2+48*320L+strlen(d)*8L, "  ",0);
			}
			break;
		case 0x01: /* ESC */
			memcpy(ae1, ofond, 240*640L);
			memcpy(ae2, ofond, 240*640L);
			free(ofond);
			COMP_finir();
			return 1;
			break;
		case 0x4b: /* left */
			if (curs > d)
				curs--;
			break;
		case 0x4d: /* right*/
			if (*curs != 0)
				curs++;
			break;
		default:
			key = unshift[cla & 0x7f];
			if (key == '/')
				key = '\\';

			if ( ((key & 0xff) != 0) && (strlen(d)<60)) {
			memmove(curs+1, curs, strlen(curs)+1);
			*curs++ = key;
			}
			break;
		}
		print(ae1+48*320L, d, 0xffff);
		print(ae2+48*320L, d, 0xffff);
		zone_NOT(8, 8, ae1 + 48*320L + (curs-d)*8L);
		zone_NOT(8, 8, ae2 + 48*320L + (curs-d)*8L);
		
	/* on valide par enter et return */
	} while ((cla != 0x1c) && (cla != 0x72));

	COMP_finir();

	memcpy(ae1, ofond, 240*640L);
	memcpy(ae2, ofond, 240*640L);
	free(ofond);

	return 0;
}

void menu_load_tga(void) {
	char nom[64];
	UBYTE *tga;

	if (choisir_fichier("Choisir une image TGA a charger", "\\", nom) != 0)
		return;
	/* le type a fait OK => on charge */
	EXIT_vecteur();
	tga = mymalloc(192018L);
	charger(nom, tga);
	tga24(tga+18, ae1+40*320L);
	free(tga);
	INIT_vecteur();
	memcpy(ae2+40*320L, ae1+40*320L,640*200L);
}
void menu_load_raw(void) {
	char nom[64];

	mouse_release();
	if (choisir_fichier("Choisir une image RAW a charger", "\\", nom) != 0)
		return;
	/* le type a fait OK => on charge */
	EXIT_vecteur();
	charger(nom, ae1+40*320L);
	INIT_vecteur();
	memcpy(ae2+40*320L, ae1+40*320L,640*200L);
}

void menu_sauve_raw(void) {
	char nom[64];
	LONG h;

	if (choisir_fichier("Choisir une image RAW a sauver", "\\", nom) != 0)
		return;
	/* le type a fait OK => on charge */
	EXIT_vecteur();
	h = Fcreate(nom,0);
	if (h < 0)
		goto fin;
	Fwrite(h, 128000L, ae1+40*320L);
	Fclose(h);
fin:
	INIT_vecteur();
}
void menu_load_sprite(void) {
}
void menu_center_sprite(void) {
	UWORD *fond, *b;
	
	mouse_release();

	if (sprite_courant->DONNEES == 0)
		return;

	b = mymalloc(sprite_courant->TAILLE);
	memcpy(b, sprite_courant->DONNEES, sprite_courant->TAILLE);

	/* fait un backup du fond de l'ecran avant d'y mettre
	la croix de centrage */
	fond = mymalloc(153600L);
	memcpy(fond, ae1, 153600L);
	memset(ae1, 0, 153600L);

	/* dessine la croix de centrage */
	{
		UWORD *curs;
		int i;
		curs = ae1+160;
		for (i=0; i<24; i++, curs+=3200) {
			curs[0] = curs[320] = curs[640] = curs[960] =
			          curs[1280] = curs[1600] = curs[1920] =
			          curs[2240] = curs[2560] =
			          curs[2880] = curs[1] = curs[2] = 0xffff;
		}
		curs = ae1+320*120L;
		for (i=0; i<32; i++) {
			curs[320] = curs[640] = 0xffff;
			*curs++ = 0xffff;
			*curs++ = 0xffff;
			*curs++ = 0xffff;
			*curs++ = 0xffff;
			*curs++ = 0xffff;
			*curs++ = 0xffff;
			*curs++ = 0xffff;
			*curs++ = 0xffff;
			*curs++ = 0xffff;
			*curs++ = 0xffff;
		}
	}
	memcpy(ae2, ae1, 153600L);
	
	SPRITE_sauver(ae1+160+120*320L,b);
	do {
		SOURIS_get320240();
		vsync();

		SPRITE_afficher(ae1+160+120*320L,b);

		{
		static WORD dec[6] = {-2,2,-80,80,-640,640};
		UBYTE *clav=0xfffffc02L, c;
		c = *clav;
		if ((c >= 0x3d) && (c <= 0x42)) {
			sprite_courant->DONNEES[0] += dec[c-0x3d];
			b[0] += dec[c-0x3d];
		}
		}

		SPRITE_sauver(ae1+160+120*320L,b);
		SPRITE_afficher(ae1+160+120*320L,sprite_courant->DONNEES);

	} while (!(ksouris & 2));

	memcpy(ae1, fond, 153600L);
	memcpy(ae2, fond, 153600L);
	free(fond);
}
void menu_get_sprite(void) {
	int x1,y1;
	int *b;
	int b1[4], b2[4];
	UWORD *wclear;

	mouse_release();

	/* on attend que l'utilisateur choisisse le premier coin */
	mouse_push();

	b = b1;
	x1 = xsouris; y1 = ysouris;
	b1[0] = b1[1] = b1[2] = b1[3] = 0;
	b2[0] = b2[1] = b2[2] = b2[3] = 0;
	
	/* maintenant, on affiche le rectangle de selection */
	do {
		/* on efface d'abord l'ancienne selection */
		zone_NOT(b[3]-b[1], b[2]-b[0], ae2 + b[1]*320L + b[0]);

		SOURIS_get320240();
		
		/* pour afficher la nouvelle */
		b[0] = x1; b[1] = y1;
		b[2] = xsouris; b[3] = ysouris;
		if (b[0] > b[2])
			SWAPW(b[0],b[2]);
		if (b[1] > b[3])
			SWAPW(b[1],b[3]);
		zone_NOT(b[3]-b[1], b[2]-b[0], ae2 + b[1]*320L + b[0]);

		sprintf(msg, "W:%3d H:%3d", b[2]-b[0], b[3]-b[1]);
		print(ae1+320*30L, msg, 0xfff0);

		/*
		 * enfin, quand l'affichage est termine, on peut
		 * premuter les ecrans (ce qui evite le clignotement)
		 */
		vsync();
		if (b == b1)
			b = b2;
		else
			b = b1;
		SWAP_reso();
	} while (ksouris & 2);

	if (ysouris < 40) {
		zone_NOT(b[3]-b[1], b[2]-b[0], ae1 + b[1]*320L + b[0]);
		zone_NOT(b[3]-b[1], b[2]-b[0], ae2 + b[1]*320L + b[0]);
		return;
	}

	/* fini l'effacement */
	zone_NOT(b[3]-b[1], b[2]-b[0], ae2 + b[1]*320L + b[0]);
	SWAP_reso();
	zone_NOT(b[3]-b[1], b[2]-b[0], ae2 + b[1]*320L + b[0]);

	sprite_courant->X1 = b[0];
	sprite_courant->Y1 = b[1];
	sprite_courant->X2 = b[2];
	sprite_courant->Y2 = b[3];

	/* reinitialise le sprite */
	if (sprite_courant->DONNEES != 0)
		free(sprite_courant->DONNEES);

	/* on prepare un buffer de taille maximale, au cas ou */
/*	sprite_courant->DONNEES = (UWORD *)mymalloc((b[3]-b[1])*(b[2]-b[0]+4));*/
	sprite_courant->DONNEES = (UWORD *)mymalloc((b[3]-b[1])*640L);

	/* init de la memoire de travail, ca doit etre un ecran de 320
	 * pixels de large, contenant uniquement le sprite
	 */
	wclear = (UWORD *)mymalloc((b[3]-b[1]+2)*640L);
	memset(wclear, 0, (b[3]-b[1]+2)*640L);

	{
		UWORD *s, *d, *fs;
		fs = s = ae1+b[1]*320L+b[0];
		fs += (b[3]-b[1])*320L;
		d = wclear;
		for (; s<fs; s+=320, d+=320)
			memcpy(d, s, (b[2]-b[0])*2);
	}

	/* on a plus qu'a encoder */
	sprite_courant->TAILLE = SPRITE_encoder(b[3]-b[1],
			wclear, sprite_courant->DONNEES);
	sprite_courant->DONNEES = realloc(sprite_courant->DONNEES,
	                                  sprite_courant->TAILLE);
	free(wclear);

}
void menu_prec_sprite(void) {
	mouse_release();
	
	if (sprite_courant > sprite)
		sprite_courant--;
}
void menu_suiv_sprite(void) {
	mouse_release();
	
	if (sprite_courant < last_sprite)
		sprite_courant++;
}
void menu_new_sprite(void) {
	mouse_release();
	
	if (sprite+MAX_SPRITE-1 > last_sprite)
		last_sprite++;
}
void menu_sauve_sprite(void) {
	char nom[64];
	LONG h;

	if (choisir_fichier("Choisir un sprite a sauver", "\\", nom) != 0)
		return;
	/* le type a fait OK => on charge */
	EXIT_vecteur();
	h = Fcreate(nom,0);
	if (h < 0)
		goto fin;
	/* un sprite est constitue de sa taille suivie de ses donnees */
	Fwrite(h, 4, &sprite->TAILLE);
	Fwrite(h, sprite_courant->TAILLE, sprite->DONNEES);
	Fclose(h);
fin:
	INIT_vecteur();
}

#define IN(_x1,_y1,_x2,_y2) ((_x1<=xsouris)&&(xsouris<=_x2)&&(_y1<=ysouris)&&(ysouris<=_y2))

void main(void) {

	/* recupere le mapping des touches selon le clavier
	 * => marche aussi bien sur un azerty qu'un qwerty...*/
	{
	KEYTAB *tab;
	tab = Keytbl(-1L,-1L,-1L);
/*	unshift = tab->unshift;*/
	unshift = tab->capslock;
	}

	/* maintenant on peut passer en TC */
	super();
	INIT_resolution();
	/* on met notre menu dans les 40 premieres lignes de l'ecran */
	charger("menu.dat", ae1);
	memcpy(ae2, ae1, 640*40);
/*
	charger("f:\\tmp\\t.raw", ae1+12800L);
	memcpy(ae2+12800L, ae1+12800L, 128000L);
*/
	INIT_vecteur();

	do {
		vsync();
		ae1[xsouris+ysouris*320L] = fond_souris;
		SOURIS_get320240();
		fond_souris = ae1[xsouris+ysouris*320L];
		ae1[xsouris+ysouris*320L] = 0xffff;

		sprintf(msg, "X:%3d Y:%3d", xsouris, ysouris);
		print(ae1+320*30L, msg, 0xffff);
		sprintf(msg, "Spr %d/%d", (UWORD)(sprite_courant-sprite),
		                             (UWORD)(last_sprite-sprite));
		print(ae1+320-10*8, msg, 0xf800);
		print(ae2+320-10*8, msg, 0xf800);
		/* choisit on un bouton du menu ??? */
		if ( (ksouris & 2) && (ysouris<40) ) {
		/* voui => on agit en consequence */
		if IN(1,0,37,17)
			menu_load_tga();
		else if IN(41,0,78,17)
			menu_load_raw();
		else if IN(83,0,121,17)
			menu_sauve_raw();
		else if IN(132,0,164,17)
			menu_load_sprite();
		else if IN(169,0,226,17)
			menu_center_sprite();
		else if IN(135,20,162,37)
			menu_get_sprite();
		else if IN(165,21,183,38)
			menu_prec_sprite();
		else if IN(185,21,203,38)
			menu_suiv_sprite();
		else if IN(205,21,230,38)
			menu_new_sprite();
		else if IN(234,21,252,38)
			menu_sauve_sprite();
		else if IN(0,0,0,0)
			logout();
		}

	} while ((ksouris & 1) == 0);
	
	logout();
}
