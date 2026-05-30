;:ts=8
	global	_t5extptr,4
	dseg
_buf:
	dc.l	$78403880,$74507c90,$ab384c40,$3858ab64
	dc.l	$68abffab,$dfab8850,$40845040,$5c40ab6c
	dc.l	$40747868,$6440642b,$7850643c,$ab344050
	dc.l	$ab3c4060,$ab885074,$34405c78,$7c807460
	dc.l	$ab844074,$60807c5c,$50384cab,$ababab2b
	dc.l	$80604840,$58686060,$4064ab27,$abababab
	dc.l	$abababab,$abababab,$abababab,$abababab
	dc.l	$abab78ff,$ab6cf72b,$f7fbffeb,$ebeb2bab
	dc.l	$abababab,$abababab,$abababab,$abababab
	dc.l	$abababab,$abababab,$abababab,$ababab2b
	dc.l	$78403880,$74507c90,$ab384c40,$3858ab64
	dc.l	$68abffab,$dfab8840,$5c384c40,$ab648060
	dc.l	$604074ab,$ababab2b,$4c307c7c,$40ab3c40
	dc.l	$74ab6040,$7c406878,$307cab3c,$4074ab3c
	dc.l	$5040ab44,$5c807c88,$405c5c40,$ababab2b
	dc.l	$74404850,$787c7450,$40747c40,$ab27abab
	dc.l	$abababab,$abababab,$abababab,$abababab
	dc.l	$abab7803,$ab6cef2b,$bff2bab,$abababab
	dc.l	$abababab,$abababab,$abababab,$abababab
	dc.l	$abababab,$abababab,$abababab,$ababab2b
	dc.l	$78403880,$74507c90,$ab384c40,$3858ab64
	dc.l	$68abffab,$dfab8850,$40845040,$5c40ab60
	dc.l	$407c4074,$ababab2b,$4c68384c,$ab883074
	dc.l	$ab3c5040,$ab308044,$ab4c3088,$3050ab94
	dc.l	$80743078,$40643c40,$abababab,$ababab2b
	dc.l	$445c807c,$88405c5c,$40ab27ab,$abababab
	dc.l	$abababab,$abababab,$abababab,$abababab
	dc.l	$abab7803,$ab6cef2b,$3fb2bab,$abababab
	dc.l	$abababab,$abababab,$abababab,$abababab
	dc.l	$abababab,$abababab,$abababab,$ababab2b
	dc.l	$78403880,$74507c90,$ab384c40,$3858ab64
	dc.l	$68abffab,$dfab8850,$40ab6440,$64647cab
	dc.l	$7850384c,$ababab2b,$3c5040ab,$305c3074
	dc.l	$60787c80,$4440ab3c,$5040ab30,$60abefe3
	dc.l	$308048e3,$f3fbf707,$abababab,$ababab2b
	dc.l	$30807848,$40748044,$4064ab88,$80743c40
	dc.l	$ab27abab,$abababab,$abababab,$abababab
	dc.l	$abab7803,$ab6cef2b,$305c6c4c,$30f32bab
	dc.l	$abababab,$abababab,$abababab,$abababab
	dc.l	$abababab,$abababab,$abababab,$ababab2b
	cseg
	public	_enigma5
_enigma5:
	link	a5,#.2
	movem.l	.3,-(sp)
	dseg
	ds	0
.4
	dc.w	0
	cseg
	lea	_buf,a0
	move.l	a0,_t5extptr
	tst.w	.4
	beq	.5
	move.l	#5,d0
.6
	movem.l	(sp)+,.3
	unlk	a5
	rts
.5
	move.l	_gtime,d0
	sub.l	#5,d0
	and.l	#3,d0
	move.w	d0,d4
	move.w	d4,d0
	muls.w	#160,d0
	ext.l	d0
	add.l	d0,_t5extptr
	move.l	#0,d4
.9
	move.l	_t5extptr,a0
	move.b	(a0,d4.w),d0
	ext.w	d0
	move.w	d0,-(sp)
	jsr	_metamorph5
	add.w	#2,sp
	move.l	_t5extptr,a0
	move.b	d0,(a0,d4.w)
.7
	add.w	#1,d4
	cmp.w	#160,d4
	blt	.9
.8
	sub.w	#6,d4
	move.l	#0,d4
.12
	move.w	d4,d0
	muls.w	#40,d0
	ext.l	d0
	add.l	_t5extptr,d0
	move.l	d0,-(sp)
	jsr	_put_line
	add.w	#4,sp
.10
	add.w	#1,d4
	cmp.w	#3,d4
	blt	.12
.11
	jsr	_get_line
	move.l	d0,-(sp)
	move.l	_t5extptr,a0
	pea	120(a0)
	jsr	_strcmp
	add.w	#8,sp
	tst.w	d0
	beq	.13
	move.l	#0,d4
.16
	sub.w	#1,d4
.14
	add.w	#1,d4
	cmp.w	#566,d4
	blt	.16
.15
.13
	move.l	#43,d4
	pea	.1+0
	jsr	_put_line
	add.w	#4,sp
	move.w	#5,.4
	bra	.6
.2	equ	0
.3	reg	d4/d5
.1
	dc.b	43,0
	ds	0
	public	_metamorph5
_metamorph5:
	link	a5,#.18
	movem.l	.19,-(sp)
	move.b	9(a5),d0
	sub.b	#43,d0
	rol.b	#6,d0
	ext.w	d0
.20
	movem.l	(sp)+,.19
	unlk	a5
	rts
.18	equ	0
.19	reg	
	public	_strcmp
	public	_put_line
	public	_get_line
	public	.begin
	dseg
	public	_gtime
	end
