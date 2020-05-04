	opt	l1,v+,w+,o+,y-,d-,p=68030
	output	e:\my_games\total.sup\objects\tga24.o
;	output	f:\tmp\tga24.o

	export tga24

;convertie une tga24 320*200 en 16bits
tga24:
;	move.l	4(a7),a1	;ad destination
;	move.l	8(a7),a0	;ad source
	move.l	d7,-(a7)
	move.w	#320*200-1,d7
.conv:
	move.b	(a0)+,d2
	move.b	(a0)+,d1
	move.b	(a0)+,d0
	and.w	#%11111000,d0
	and.w	#%11111100,d1
	and.w	#%11111000,d2
	rol.w	#8,d0
	rol.w	#3,d1
	ror.w	#3,d2
	or.w	d1,d0
	or.w	d2,d0
	move.w	d0,(a1)+
	dbra	d7,.conv

	move.l	(a7)+,d7
	rts
