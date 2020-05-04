	opt	o+,d+,y+,l1,p=68030,w+
;	output	"g:\total.sup\objects\skel.o"
	output	"e:\my_games\total.sup\objects\skel.o"
;	output	"f:\tmp\skel.o"

	export	INIT_resolution,INIT_vecteur,logout,vsync
	export	EXIT_resolution,EXIT_vecteur
	export	SPRITE_afficher,SPRITE_encoder,SPRITE_sauver
	export	zone_NOT
	export	SOURIS_get320240
	export	SWAP_reso
	export	FONT_carac,print
	export	ksouris,xsouris,ysouris
	export	user,super
	export	ae1,ae2,ecr1,ecr2

;==========================================================================
vvrap	equ	$ffff820e	;permet le changement de rezo
vwidth	equ	$ffff8210
spshift	equ	$ffff8266
hht	equ	$ffff8282
hbb	equ	$ffff8284
hbe	equ	$ffff8286
hdb	equ	$ffff8288
hde	equ	$ffff828a
hss	equ	$ffff828c
vft	equ	$ffff82a2
vbb	equ	$ffff82a4
vbe	equ	$ffff82a6
vdb	equ	$ffff82a8
vde	equ	$ffff82aa
vss	equ	$ffff82ac
rco	equ	$ffff82c0
vco	equ	$ffff82c2
;==========================================================================
	section text

;	jsr	super

;	jsr	INIT_resolution

;	move.l	ae1,a0
;	lea	spr+640*90+2*70,a1
;	move.w	#32-1,d0
;.t:
;	move.w	#100-1,d1
;.y:
;	move.w	(a1)+,(a0)+
;	dbra	d1,.y
;	adda.w	#320+120,a0
;	adda.w	#320+120,a1
;	dbra	d0,.t

;	jsr	INIT_vecteur
;==========================================================================
;	move.l	ae1,a0
;	move.l	a0,a4
;	adda.l	#640*20,a4
;	lea	sspri,a2
;	bsr	SPRITE_encoder

;	move.l	ae1,a0
;	adda.l	#640*50+50*2,a0
;	lea	sspri,a1
;	bsr	SPRITE_afficher

;	bra	logout

;	move.l	ae1,a0
;principal:
;	clr.l	$ffff9800.w
;	bsr	vsync
;	move.l	#$ff,$ffff9800.w

;	bsr	SOURIS_get320240


;	move.w	#10000,d0
;.w:
;	nop
;	nop
;	dbra	d0,.w

;	clr.w	(a0)
;	move.w	xsouris,d0
;	add.w	d0,d0
;	clr.l	d1
;	move.w	ysouris,d1
;;	mulu.w	#640,d1
;	move.l	ae1,a0
;	adda.l	d1,a0
;	adda.w	d0,a0


;	clr.w	d0
;	btst.b	#0,ksouris
;	beq.s	.b
;	or.w	#$1f,d0
;.b:
;	btst.b	#1,ksouris
;	beq.s	.c
;	or.w	#$f800,d0
;.c:
;	not.w	d0
;	move.w	d0,(a0)

;www:
;	cmp.b	#$3c,$fffffc02.w
;	bne.s	principal

;	bra	logout

super:
	clr.l	-(sp)
	move.w	#$20,-(sp)	;passe en superviseur
	trap	#1
	addq.l	#6,sp
	move.l	d0,asp
	rts

;==========================================================================
SWAP_reso:
	move.l	ae1,d0
	move.l	ae2,ae1
	move.l	d0,ae2
	move.b	ae1+1,$ffff8201.w
	move.b	ae1+2,$ffff8203.w
	move.b	ae1+3,$ffff820d.w
	rts

INIT_resolution:
	movem.l	d2-d7/a2-a6,-(a7)

	move.l	#ecr1,d0
	clr.b	d0	;mets des ad ecran multi de 256 pour le falcon
	add.l	#256,d0
	move.l	d0,ae1
	move.l	#ecr2,d0
	clr.b	d0
	add.l	#256,d0
	move.l	d0,ae2

	clr.l	axb2
	move.b	$ffff8201.w,axb2+1
	move.b	$ffff8203.w,axb2+2
	move.b	$ffff820d.w,axb2+3

	lea	pal,a0
	bsr	sauve_pal
	
	lea	videl_reg,a0
	bsr	sauve_reso

	move.b	ae1+1,$ffff8201.w
	move.b	ae1+2,$ffff8203.w
	move.b	ae1+3,$ffff820d.w

	move.w	#89,-(sp)
	trap	#14		;regarde le type d'ecran
	addq.l	#2,sp
		
	cmp.b	#2,d0		;est ce un vga?
	bne.s	.pvga

	move.w	#-1,-(sp)
	move.w	#88,-(sp)	;regarde la rezo
	trap	#14
	addq.l	#4,sp
	move.w	d0,d7
	and.w	#%1111111000110000,d7
	or.w	#%100,d7	;resolution 320*200 tc

	bset	#8,d7		;on met le mode double ligne

	move.w	d7,-(sp)
	move.w	#88,-(sp)	;met le 320*240 tc pour le VGA
	trap	#14
	addq.l	#4,sp
	bra.s	.fin
.pvga:

	lea	h_320_tc,a0	;mode horizontal True Color 320 pixels
	lea	v_240,a1	;mode vertical 240 lignes
	bsr	install_reso
.fin:
	movem.l	(a7)+,d2-d7/a2-a6
	rts
sauve_pal:
	lea	$ffff8240.w,a1
	rept	8
	move.l	(a1)+,(a0)+
	endr
	lea	$ffff9800.w,a1
	move.w	#256/4-1,d0
.c
	rept	4
	move.l	(a1)+,(a0)+
	endr
	dbra	d0,.c
	rts
restore_pal:
	lea	$ffff8240.w,a1
	rept	8
	move.l	(a0)+,(a1)+
	endr
	lea	$ffff9800.w,a1
	move.w	#256/4-1,d0
.c
	rept	4
	move.l	(a0)+,(a1)+
	endr
	dbra	d0,.c
	rts
	
;==========================================================================
INIT_vecteur:
	move.w	#$2700,sr
	lea	avect,a0
	move.l	$8.w,(a0)+	;sauve les exeptions
	move.l	$c.w,(a0)+	;bus,adr,et ill
	move.l	$10.w,(a0)+
	move.l	$70.w,(a0)+	;vbl
	move.l	$118.w,(a0)+	;clavier
	move.b	$fffffa07.w,(a0)+
	move.b	$fffffa09.w,(a0)+
	move.b	$fffffa15.w,(a0)+
	move.b	$fffffa17.w,(a0)+

	move.l	#error,d0
	move.l	d0,$8.w		;et les remplaces
	move.l	d0,$c.w
	move.l	d0,$10.w
	move.l	#IT_vbl,$70.w		;installe une nouvelle gestion du clavier
	move.l	#IT_souris,$118.w
	bclr	#3,$fffffa17.w
	bset	#6,$fffffa09.w
	bset	#6,$fffffa15.w

	move.w	#$2300,sr

.ins:	btst	#1,$fffffc00.w	;**init sou**
	beq.s	.ins
	move.b	#$8,$fffffc02.w

	rts
;==========================================================================
EXIT_vecteur:
	move.w	#$2700,sr
	lea	avect,a0
	move.l	(a0)+,$8.w		;restaure les exeptions
	move.l	(a0)+,$c.w
	move.l	(a0)+,$10.w
	move.l	(a0)+,$70.w
	move.l	(a0)+,$118.w
	move.b	(a0)+,$fffffa07.w	;remet les ITs du MFP
	move.b	(a0)+,$fffffa09.w
	move.b	(a0)+,$fffffa15.w
	move.b	(a0)+,$fffffa17.w
	move.w	#$2300,sr
	rts

EXIT_resolution:		;retour a l'ancienne resolution
	lea	videl_reg,a0
	bsr	restore_reso

	move.b	axb2+1,$ffff8201.w
	move.b	axb2+2,$ffff8203.w
	move.b	axb2+3,$ffff820d.w
	
	lea	pal,a0
	bsr	restore_pal
	rts

user:
	move.l	asp,-(sp)
	move.w	#$20,-(sp)	;retour en mode utilisateur
	trap	#1
	addq.l	#6,sp
	rts
logout:
.w:	cmp.b	#$3b,$fffffc02.w
	bne.s	.w
	
	bsr	EXIT_vecteur
	bsr	EXIT_resolution
	bsr	user
fin:
	clr.w	-(sp)
	trap	#1

Tvsync:
	movem.l	d0/a0,-(a7)
	move.w	#$25,-(a7)
	trap	#14
	addq.l	#2,a7
	movem.l	(a7)+,d0/a0
	rts
;==========================================================================
IT_vbl:
	addq.l	#1,$466.w
	addq.w	#1,nb_vbl
	rte
nb_vbl:	ds.w	1	; nombre de vbl depuis le dernier vsync

vsync:
	clr.w	nb_vbl
.w:	tst.w	nb_vbl
	beq.s	.w
	rts
;==========================================================================

;==========================================================================
error:
	move.l	#$ff,$ffff9800.w
	bra	logout
;==========================================================================
install_reso:
	move.w	(a0)+,d0
	or	(a1)+,d0
	move.w	d0,vco.w	; D‚finitions horizontales, entrelacement, double ligne
	move	(a0)+,spshift.w	; Registre d‚finissant le nombre de couleurs
	move	(a0)+,d1
	btst	#1,d0		; Mode entrelac‚ ?
	beq.s	.pas_entrelace
	bset	#3,d1		; alors on trafique le bit 3 de RCO
.pas_entrelace:
	move	d1,rco.w	; Bits de config vid‚o divers et vari‚s...
	move	(a0)+,vwidth.w	; Largeur de l'‚cran en mots
	movem.l	(a0)+,d0-d2
	movem.l	d0-d2,hht.w	; Les 6 registres de contr“le horizontaux
	movem.l	(a1)+,d0-d2
	movem.l	d0-d2,vft.w	; Les 6 registres de contr“le verticaux
	rts			; Et c'est fini !!!

sauve_reso:
	bsr	Tvsync
;	lea	$ffff8240.w,a1
;	move.l	(a1)+,(a0)+
;	move.l	(a1)+,(a0)+
;	move.l	(a1)+,(a0)+
;	move.l	(a1)+,(a0)+
	move.w	vvrap.w,(a0)+
	move.w	vwidth.w,(a0)+
	move.w	spshift.w,(a0)+
	move.w	hht.w,(a0)+
	move.w	hbb.w,(a0)+
	move.w	hbe.w,(a0)+
	move.w	hdb.w,(a0)+
	move.w	hde.w,(a0)+
	move.w	hss.w,(a0)+
	move.w	vft.w,(a0)+
	move.w	vbb.w,(a0)+
	move.w	vbe.w,(a0)+
	move.w	vdb.w,(a0)+
	move.w	vde.w,(a0)+
	move.w	vss.w,(a0)+
	move.w	rco.w,(a0)+
	move.w	vco.w,(a0)+
	rts
restore_reso:
	bsr	Tvsync
;	lea	$ffff8240.w,a1
;	move.l	(a0)+,(a1)+
;	move.l	(a0)+,(a1)+
;	move.l	(a0)+,(a1)+
;	move.l	(a0)+,(a1)+
	move.w	(a0)+,vvrap.w
	move.w	(a0)+,vwidth.w
	move.w	(a0)+,spshift.w
	move.w	(a0)+,hht.w
	move.w	(a0)+,hbb.w
	move.w	(a0)+,hbe.w
	move.w	(a0)+,hdb.w
	move.w	(a0)+,hde.w
	move.w	(a0)+,hss.w
	move.w	(a0)+,vft.w
	move.w	(a0)+,vbb.w
	move.w	(a0)+,vbe.w
	move.w	(a0)+,vdb.w
	move.w	(a0)+,vde.w
	move.w	(a0)+,vss.w
	move.w	(a0)+,rco.w
	move.w	(a0)+,vco.w
	rts
;==========================================================================
;gestion de la souris.
; o lecture des paquets envoyes par le clavier
; o si paquet de souris => recupere les boutons
;                          recupere X puis Y
IT_souris:
	move.w	d0,-(sp)
	move.b	$fffffc02.w,d0
	move.b	d0,ksouris0
	cmp.b	#$f8,d0	;regarde si c'est un message souris
	blo.s	.pas_souris
	move.l	#IT_sourisX,$118.w	;oui=>on passe a la recuperation
.pas_souris:
	move.w	(sp)+,d0
	rte
IT_sourisX:
	move.l	d0,-(sp)
	move.b	$fffffc02.w,d0	;prend le decalage en x
	ext.w	d0
	add.w	d0,xsouris0
	move.l	#IT_sourisY,$118.w

	move.l	(sp)+,d0
	rte
IT_sourisY:
	move.l	d0,-(sp)
	move.b	$fffffc02.w,d0	;prend le decalage en y
	ext.w	d0
	add.w	d0,ysouris0
	move.l	(sp)+,d0
	move.l	#IT_souris,$118.w
	rte
ksouris0:	ds.b	2
xsouris0:	ds.w	1
ysouris0:	ds.w	1

SOURIS_get320240:
	cmp.b	#$f8,ksouris0	;regarde si c'est un message souris
	blo.s	.pas_ksouris
	move.b	ksouris0,ksouris
	and.b	#$7,ksouris
.pas_ksouris
	move.w	xsouris0,xsouris
	bpl.s	.xsp
	clr.w	xsouris0	;bloque a gauche
	clr.w	xsouris
.xsp:
	cmp.w	#319,xsouris
	blo.s	.xsp1	
	move.w	#319,xsouris0
	move.w	#319,xsouris	;bloque a droite
.xsp1:

	move.w	ysouris0,ysouris
	bpl.s	.ysp
	clr.w	ysouris	;bloque en haut
	clr.w	ysouris0
.ysp:
	cmp.w	#239,ysouris
	blo.s	.ysp1	
	move.w	#239,ysouris	;bloque en bas
	move.w	#239,ysouris0
.ysp1:
	rts
;==========================================================================
;d0 = hauteur d1=largeur, a0=adresse ecran
zone_NOT:
	move.l	a2,-(a7)
	move.l	a0,a2
	add.w	d1,d1
	lea	.nnot,a1
	suba.w	d1,a1
	bra.w	.nnot
	rept	320
	not.w	(a0)+
	endr
.nnot:
	move.l	a2,a0
	adda.l	#640,a2
	dbra	d0,.next
.fin
	move.l	(a7)+,a2
	rts
.next
	jmp	(a1)
;==========================================================================

;
; ROUTINES POUR L'AFFICHAGE DE SPRITES EN TC16bits
;
; ½Bod ½mOMo

;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; Structure d'un sprite :
;  1 WORD : nb d'octet sur l'ecran entre 2 series de pixels du sprite
;           (generalement le passage a la ligne suivante)
;  1 WORD : nb d'octet de la serie de pixel a afficher
;  n WORD : les octets a afficher.
; Pour la fin du sprite, le nb de pixel est afficher est -8 de sorte
; que le jmp saute directement sur le RTS
;
; Proto pour PureC : void SPRITE_afficher(WORD *ae, WORD *sprite);
;  'ae' pointe sur la zone ou le sprite sera affiche
;  'sprite' contient les donnees du sprite
SPRITE_afficher:
	move.l	a2,-(sp)
	move.l	#.move,d0
	bra.w	.move

	; on ne peut afficher que 115 pixels d'un seul coup
	rept	115
	move.w	(a1)+,(a0)+
	endr
.move:
	adda.w	(a1)+,a0	; calcul de l'adresse ecran suivante
	move.l	d0,a2
	suba.w	(a1)+,a2	; trouve le bon move
	jmp	(a2)
	move.l	(sp)+,a2
	rts

; s'utilise en faisant la copie en memoire s'un sprite.
; Puis, avant d'afficher le 1er sprite, on appelle cette fonction
; sur la copie. Elle va sauvegarder le fond, que l'on pourra
; restaurer avec un SPRITE_afficher.
SPRITE_sauver:
	move.l	a2,-(sp)
	move.l	#.move,d0
	bra.w	.move

	; on ne peut afficher que 115 pixels d'un seul coup
	rept	115
	move.w	(a0)+,(a1)+
	endr
.move:
	adda.w	(a1)+,a0	; calcul de l'adresse ecran suivante
	move.l	d0,a2
	suba.w	(a1)+,a2	; trouve le bon move
	jmp	(a2)
	move.l	(sp)+,a2
	rts

;A0 adresse de la partie de l'ecran ou se trouve le sprite
;A2 buffer de sauvegarde du sprite
;retourne le nombre d'octet du sprite
SPRITE_encoder:
	movem.l	a2-a5,-(sp)
	move.l	a1,a2
	move.l	a1,a5
	move.l	a0,a4
	mulu	#640,d0
	adda.l	d0,a4

.nlle_serie:
	; compte les pixels qui separent 2 series de pixels
	move.l	a0,a1
.look_noblack:
	tst.w	(a0)+	; ce qui separe 2 series est constituee de pixels
	beq.s	.look_noblack	;noirs.
	subq.l	#2,a0

	cmp.l	a4,a0
	bhs.s	.fin

	move.l	a0,d0
	sub.l	a1,d0	; met cette longueur dans le sprite
;	subq.w	#2,d0
;	addq.w	#2,d0
	move.w	d0,(a2)+

	; stocke tous les pixels d'une serie
	move.l	a2,a3
	clr.w	(a2)+
.look_black:
	move.w	(a0)+,(a2)+
	bne.s	.look_black
	subq.l	#2,a0
	subq.l	#2,a2
	move.l	a2,d0
	sub.l	a3,d0
	subq.w	#2,d0
	move.w	d0,(a3)
	
	cmp.l	a4,a0
	blo.s	.nlle_serie
.fin
	clr.w	(a2)+
	move.w	#-8,(a2)
	
	move.l	a2,d0
	sub.l	a5,d0
	addq.l	#2,d0
	movem.l	(sp)+,a2-a5
	rts

; A0 adresse de l'ecran, A1=le texte D0=la couleur
print:
	movem.l	a2-a3,-(sp)
	move.l	a0,a2
	move.l	a1,a3
	move.w	d0,d1
.deb:
	move.b	(a3)+,d0
	beq.s	.fin
	move.l	a2,a0
	bsr	FONT_carac
	adda.w	#16,a2
	bra.s	.deb
.fin:
	movem.l	(sp)+,a2-a3
	rts
; A0 adresse de l'ecran, D0=le caractere, D1=la couleur
FONT_carac:
	move.l	d3,-(sp)
	and.w	#$ff,d0
	lea	(font,d0.w*8),a1	; pointe sur les donnees du caractere

	move.w	#7,d3
.b0
	move.b	(a1)+,d0
	move.w	#7,d2
.b1
	btst	d2,d0
	beq.s	.noir
	move.w	d1,(a0)+
	bra	.fb1
.noir
	clr.w	(a0)+
.fb1	dbra	d2,.b1
	adda.w	#640-16,a0
	dbra	d3,.b0

	move.l	(a7)+,d3
	rts
;==========================================================================
	section data

h_320_tc:	;valeur des registres videos sur RGB
	dc.w	$0,$100,$181,320
	dc.w	$fe,$cb,$27,$2e,$8f,$d8
v_240:
	dc.w	$0
	dc.w	$271,$23b,$5b,$5b,$23b,$26b

;spr:	incbin	momo1.raw
font:	incbin	font88.dat
;==========================================================================
	section bss
asp:	ds.l	1		;ancienne pile utilisateur

axb2:	ds.l	1		;ancienne base video
videl_reg:	ds.w	60

;amode:	ds.w	1		;ancien mode video

avect:	ds.l	10		;anciens vecteurs...

ksouris:	ds.b	2
xsouris:	ds.w	1	;coordonnees de la souris
ysouris:	ds.w	1

ae1:	ds.l	1		;adresse des 2 ecrans
ae2:	ds.l	1
ecr1:	ds.b	320*256*2	;les 2 ecrans du jeu
ecr2:	ds.b	320*256*2

pal:	ds.l	256+8

;sspri:	ds.b	20000
;==========================================================================
	end