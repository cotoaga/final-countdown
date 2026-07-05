;:ts=8
	global	_fh,4
	global	_mem,4
	dseg
	public	_fname
_fname:
	dc.b	83
	dc.b	101
	dc.b	99
	dc.b	47
	dc.b	83
	dc.b	101
	dc.b	99
	dc.b	67
	dc.b	104
	dc.b	101
	dc.b	99
	dc.b	107
	dc.b	48
	dc.b	0
	cseg
	dseg
	public	_sname
_sname:
	dc.b	83
	dc.b	101
	dc.b	99
	dc.b	47
	dc.b	77
	dc.b	101
	dc.b	108
	dc.b	101
	dc.b	110
	dc.b	99
	dc.b	117
	dc.b	114
	dc.b	105
	dc.b	111
	dc.b	110
	dc.b	48
	dc.b	0
	cseg
	public	_main
_main:
	link	a5,#.2
	movem.l	.3,-(sp)
	clr.l	-(sp)
	pea	1000
	jsr	_AllocMem
	add.w	#8,sp
	move.l	d0,_mem
	move.l	#0,d4
.6
	pea	_fname
	pea	.1+0
	jsr	_printf
	add.w	#8,sp
	pea	1005
	pea	_fname
	jsr	_Open
	add.w	#8,sp
	move.l	d0,_fh
	pea	1000
	move.l	_mem,-(sp)
	move.l	_fh,-(sp)
	jsr	_Read
	lea	12(sp),sp
	move.l	d0,d6
	move.l	_fh,-(sp)
	jsr	_Close
	add.w	#4,sp
	move.l	d6,-(sp)
	pea	.1+4
	jsr	_printf
	add.w	#8,sp
	move.l	#0,d5
	bra	.10
.9
	move.l	_mem,a0
	move.b	(a0,d5.w),d0
	ext.w	d0
	move.w	d0,-(sp)
	move.w	d4,-(sp)
	jsr	_enigma
	add.w	#4,sp
	move.l	_mem,a0
	move.b	d0,(a0,d5.w)
.7
	add.w	#1,d5
.10
	move.w	d5,d0
	ext.l	d0
	cmp.l	d6,d0
	blt	.9
.8
	pea	_sname
	pea	.1+9
	jsr	_printf
	add.w	#8,sp
	pea	1006
	pea	_sname
	jsr	_Open
	add.w	#8,sp
	move.l	d0,_fh
	pea	640
	move.l	_mem,-(sp)
	move.l	_fh,-(sp)
	jsr	_Write
	lea	12(sp),sp
	move.l	_fh,-(sp)
	jsr	_Close
	add.w	#4,sp
	add.b	#1,_fname+12
	add.b	#1,_sname+15
.4
	add.w	#1,d4
	cmp.w	#8,d4
	blt	.6
.5
	pea	1000
	move.l	_mem,-(sp)
	jsr	_FreeMem
	add.w	#8,sp
.11
	movem.l	(sp)+,.3
	unlk	a5
	rts
.2	equ	0
.3	reg	d4/d5/d6
.1
	dc.b	37,115,32,0,37,108,100,10,0,37,115,10,0
	ds	0
	public	_enigma
_enigma:
	link	a5,#.13
	movem.l	.14,-(sp)
	cmp.b	#10,11(a5)
	bne	.15
	clr.b	11(a5)
.15
	move.w	8(a5),d0
	ext.l	d0
	bra	.16
.18
	move.b	11(a5),d0
	ext.w	d0
	move.w	d0,-(sp)
	jsr	_enigma0
	add.w	#2,sp
	move.b	d0,11(a5)
	bra	.17
.19
	move.b	11(a5),d0
	ext.w	d0
	move.w	d0,-(sp)
	jsr	_enigma1
	add.w	#2,sp
	move.b	d0,11(a5)
	bra	.17
.20
	move.b	11(a5),d0
	ext.w	d0
	move.w	d0,-(sp)
	jsr	_enigma2
	add.w	#2,sp
	move.b	d0,11(a5)
	bra	.17
.21
	move.b	11(a5),d0
	ext.w	d0
	move.w	d0,-(sp)
	jsr	_enigma3
	add.w	#2,sp
	move.b	d0,11(a5)
	bra	.17
.22
	move.b	11(a5),d0
	ext.w	d0
	move.w	d0,-(sp)
	jsr	_enigma4
	add.w	#2,sp
	move.b	d0,11(a5)
	bra	.17
.23
	move.b	11(a5),d0
	ext.w	d0
	move.w	d0,-(sp)
	jsr	_enigma5
	add.w	#2,sp
	move.b	d0,11(a5)
	bra	.17
.24
	move.b	11(a5),d0
	ext.w	d0
	move.w	d0,-(sp)
	jsr	_enigma6
	add.w	#2,sp
	move.b	d0,11(a5)
	bra	.17
.25
	move.b	11(a5),d0
	ext.w	d0
	move.w	d0,-(sp)
	jsr	_enigma7
	add.w	#2,sp
	move.b	d0,11(a5)
	bra	.17
.26
	dc.w	.18-.27-2
	dc.w	.19-.27-2
	dc.w	.20-.27-2
	dc.w	.21-.27-2
	dc.w	.22-.27-2
	dc.w	.23-.27-2
	dc.w	.24-.27-2
	dc.w	.25-.27-2
.16
	cmp.l	#8,d0
	bcc	.17
	asl.l	#1,d0
	move.w	.26(pc,d0.w),d0
.27
	jmp	(pc,d0.w)
.17
	move.b	11(a5),d0
	ext.w	d0
.28
	movem.l	(sp)+,.14
	unlk	a5
	rts
.13	equ	0
.14	reg	
	public	_enigma0
_enigma0:
	link	a5,#.29
	movem.l	.30,-(sp)
	move.b	9(a5),d0
	ror.b	#3,d0
	add.b	#42,d0
	ext.w	d0
.31
	movem.l	(sp)+,.30
	unlk	a5
	rts
.29	equ	0
.30	reg	
	public	_enigma1
_enigma1:
	link	a5,#.32
	movem.l	.33,-(sp)
	move.b	9(a5),d0
	ror.b	#4,d0
	add.b	#41,d0
	ext.w	d0
.34
	movem.l	(sp)+,.33
	unlk	a5
	rts
.32	equ	0
.33	reg	
	public	_enigma2
_enigma2:
	link	a5,#.35
	movem.l	.36,-(sp)
	move.b	9(a5),d0
	ror.b	#5,d0
	add.b	#40,d0
	ext.w	d0
.37
	movem.l	(sp)+,.36
	unlk	a5
	rts
.35	equ	0
.36	reg	
	public	_enigma3
_enigma3:
	link	a5,#.38
	movem.l	.39,-(sp)
	move.b	9(a5),d0
	ror.b	#6,d0
	add.b	#39,d0
	ext.w	d0
.40
	movem.l	(sp)+,.39
	unlk	a5
	rts
.38	equ	0
.39	reg	
	public	_enigma4
_enigma4:
	link	a5,#.41
	movem.l	.42,-(sp)
	move.b	9(a5),d0
	ror.b	#6,d0
	add.b	#43,d0
	ext.w	d0
.43
	movem.l	(sp)+,.42
	unlk	a5
	rts
.41	equ	0
.42	reg	
	public	_enigma5
_enigma5:
	link	a5,#.44
	movem.l	.45,-(sp)
	move.b	9(a5),d0
	ror.b	#4,d0
	add.b	#45,d0
	ext.w	d0
.46
	movem.l	(sp)+,.45
	unlk	a5
	rts
.44	equ	0
.45	reg	
	public	_enigma6
_enigma6:
	link	a5,#.47
	movem.l	.48,-(sp)
	move.b	9(a5),d0
	ror.b	#5,d0
	add.b	#47,d0
	ext.w	d0
.49
	movem.l	(sp)+,.48
	unlk	a5
	rts
.47	equ	0
.48	reg	
	public	_enigma7
_enigma7:
	link	a5,#.50
	movem.l	.51,-(sp)
	move.b	9(a5),d0
	ror.b	#4,d0
	add.b	#69,d0
	ext.w	d0
.52
	movem.l	(sp)+,.51
	unlk	a5
	rts
.50	equ	0
.51	reg	
	public	_FreeMem
	public	_Write
	public	_Close
	public	_printf
	public	_Read
	public	_AllocMem
	public	_Open
	public	.begin
	dseg
	end
