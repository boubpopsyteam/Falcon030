#include <stdio.h>
#include <stdlib.h>

#include <tos.h>
#include "complet.h"

static DTA mydta;
static char prev_chemin[128];

S_FICHIER *tab_file=0;
int nb_fichier;

void init_file(void) {
	Fsetdta(&mydta);
}


static int first_file(char *path, S_FICHIER *fichier) {
	char buf[128];
	
	sprintf(buf, "%s*.*", path);
	if (Fsfirst(buf, 0x3f) != 0)
		return -1;
	strcpy(fichier->NOM, mydta.d_fname);
	fichier->FLAGS = 0;
	if (mydta.d_attrib & 0x10)
		fichier->FLAGS |= F_REPERTOIRE;
	return 0;
}

static int next_file(S_FICHIER *fichier) {
	if (Fsnext() != 0)
		return -1;
	strcpy(fichier->NOM, mydta.d_fname);
	fichier->FLAGS = 0;
	if (mydta.d_attrib & 0x10)
		fichier->FLAGS |= F_REPERTOIRE;
	return 0;
}

static int COMP_remplir_repertoire(char *path, S_FICHIER **tab) {
	int nb_fichier, nb_alloue;
	S_FICHIER *curs;

	nb_alloue = 50;
	curs = *tab = (S_FICHIER *)malloc(nb_alloue*sizeof(S_FICHIER));
	if (curs == 0)
		return -1;	/* pb de memoire */

	nb_fichier=0;

	/* initialise la fonction de recherche des fichiers */
	init_file();
	
	/* on essaye de trouver le 1er fichier */
	if (first_file(path, curs++) != 0) {
		free(*tab);
		return -2;
	}
 	
 	/* et enfin, on recupere tous les autres */
	do {
	 	nb_fichier++;
 		if (nb_fichier >= nb_alloue) {
 			nb_alloue += 50;
 			*tab = realloc(*tab, nb_alloue*sizeof(S_FICHIER));
 			curs = *tab+nb_fichier;
 		}
 	} while (next_file(curs++) == 0);

	return nb_fichier;
}

static void extraire_repertoire(char *d, char *f, char *s) {
	char *dcurs, *curs;
	
	dcurs = s;
	curs = s+strlen(s);
	/* on remonte de la fin vers le debut pour trouver le
	 * dernier repertoire.
	 */
	while (curs >= dcurs) {
		if (*curs == '\\')
			break;
		curs --;
	}
	/* 2 cas, s'il y a pas de repertoire c'est que l'on commence a
	 * la racine. sinon on a trouve.
	 */
	if (curs < dcurs) {
		strcpy(d, "\\");
		strcpy(f, s);
		return;
	}
	strncpy(d, s, curs-s+1);
	d[curs-s+1] = 0;
	strcpy(f, curs+1);
}

void COMP_debuter(void) {
	prev_chemin[0] = 0;
}

static int compare(char *s1, char *s2, int l) {
	for (; l>0 ; l--,s1++,s2++)
		if (*s1 != *s2)
			return -1;
	return 0;
}

int COMP_completer(char *file) {
	char chemin[128], fichier[64];
	int nb_trouve;
	S_FICHIER *curs, *fcurs;
	
	extraire_repertoire(chemin, fichier, file);
	
	if (strcmp(chemin, prev_chemin) != 0) {
		strcpy(prev_chemin, chemin);
		if (tab_file != 0)
			free(tab_file);
		nb_fichier = COMP_remplir_repertoire(chemin, &tab_file);
	}

	nb_trouve = 0;
	{
	int len;

	curs = tab_file;
	fcurs = tab_file+nb_fichier;
	len = strlen(fichier);
	for (; curs<fcurs; curs++) {
		if (compare(fichier, curs->NOM, len) == 0) {
			curs->FLAGS |= F_AFFICHER;
			nb_trouve++;
		} else
			curs->FLAGS &= ~F_AFFICHER;
	}
	}

	if (nb_trouve == 0)
		return 0;
	/* ici, on connait tous les fichiers ayant le bon debut.
	 * on va se debrouiller pour completer le plus de caracteres
	 * possibles => vrai completion de la mort ki tu
	 */
	{
	S_FICHIER *first;
	int i;
	i = strlen(fichier);
	
	curs = tab_file;
	fcurs = tab_file+nb_fichier;
	/* cherche le 1er qui matche. il va servir de reference pour les
	 * autres fichiers.
	 */
	for (; curs<fcurs; curs++)
		if (curs->FLAGS & F_AFFICHER)
			break;
	if (nb_trouve == 1) {
	      	sprintf(file, "%s%s", chemin, curs->NOM);
	      	if (curs->FLAGS & F_REPERTOIRE)
	      		strcat(file, "\\");
	      	return 1;
	}

	first = curs;
	
	for (;;i++) {
		for (curs=first+1; curs<fcurs; curs++) {
			if (curs->FLAGS & F_AFFICHER)
				if (curs->NOM[i] != first->NOM[i])
					goto fin;
		}
	}
fin: /* ici, on a le i donnant le nombre de caracteres max pour
      * la completion.
      */
      	strncpy(fichier, first->NOM, i);
      	fichier[i] = 0;
      	sprintf(file, "%s%s", chemin, fichier);
	}

	return nb_trouve;
}

void COMP_finir(void) {
	free(tab_file);
}

/*
void aff(void) {
	S_FICHIER *curs, *fcurs;
	
	curs = tab_file;
	fcurs = tab_file+nb_fichier;
	for (; curs<fcurs; curs++) {
		if (curs->FLAGS & F_AFFICHER) {
			if (curs->FLAGS & F_REPERTOIRE)
				printf("* ");
			printf(" '%s'\n", curs->NOM);
		}
	}
}

void main(void) {

	COMP_debuter();

	COMP_completer("");
	aff();

	COMP_completer("f:\\mfc2\\");
	aff();

	COMP_finir();
	exit(0);
}
*/