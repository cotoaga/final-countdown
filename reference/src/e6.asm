;:ts=8
	global	_t6extptr,4
	dseg
_buf:
	dc.l	$62816182,$52c172c2,$2f61b181,$61e12f11
	dc.l	$212f902f,$ff2fa2c1,$8192c181,$f1812f2f
	dc.l	$2f2f2f2f,$2f2f2f2d,$1c1f1f1,$c1211181
	dc.l	$112f0181,$116261b1,$81112f62,$c111712f
	dc.l	$41112f71,$81112fe1,$f1c10141,$ff2f2f2d
	dc.l	$e1417241,$62725221,$32b18111,$2fa18162
	dc.l	$72215251,$81112f20,$2f2f2f2f,$2f2f2f2f
	dc.l	$2f2f6290,$2f32602d,$b0602d2f,$2f2f2f2f
	dc.l	$2f2f2f2f,$2f2f2f2f,$2f2f2f2f,$2f2f2f2f
	dc.l	$2f2f2f2f,$2f2f2f2f,$2f2f2f2f,$2f2f2f2d
	dc.l	$62816182,$52c172c2,$2f61b181,$61e12f11
	dc.l	$212f902f,$ff2fa2c1,$8192c181,$f1812f72
	dc.l	$41a1812f,$2f2f2f2d,$a2c15271,$2f718152
	dc.l	$2f624182,$81526272,$2191912f,$91828152
	dc.l	$2f728152,$6121012f,$c12f2f2f,$2f2f2f2d
	dc.l	$112161b1,$2f5281c1,$61b18111,$2f202f2f
	dc.l	$2f2f2f2f,$2f2f2f2f,$2f2f2f2f,$2f2f2f2f
	dc.l	$2f2f62c0,$2f32402d,$40502d2f,$2f2f2f2f
	dc.l	$2f2f2f2f,$2f2f2f2f,$2f2f2f2f,$2f2f2f2f
	dc.l	$2f2f2f2f,$2f2f2f2f,$2f2f2f2f,$2f2f2f2d
	dc.l	$62816182,$52c172c2,$2f61b181,$61e12f11
	dc.l	$212f902f,$ff2fa2c1,$812fb181,$c1626272
	dc.l	$2f718152,$2f2f2f2d,$41627281,$5221c171
	dc.l	$2f418291,$2f718101,$2f62c161,$b12f7181
	dc.l	$522f5221,$51217281,$522f2f2f,$2f2f2f2d
	dc.l	$518191c1,$11718172,$2f202f2f,$2f2f2f2f
	dc.l	$2f2f2f2f,$2f2f2f51,$81118272,$d28152b1
	dc.l	$f2f6250,$2f32402d,$61815281,$622d2f2f
	dc.l	$2f2f2f2f,$2f2f2f2f,$2f2f2f2f,$2f2f2f2f
	dc.l	$2f2f2f2f,$2f2f2f2f,$2f2f2f2f,$2f2f2f2d
	dc.l	$62816182,$52c172c2,$2f61b181,$61e12f11
	dc.l	$212f902f,$ff2fa281,$f161b181,$2f2f2f2f
	dc.l	$2f2f2f2f,$2f2f2f2d,$5181d281,$c161b111
	dc.l	$8211a12f,$72524181,$a1722f71,$81522f52
	dc.l	$21512172,$81522f41,$82912f71,$81012f2d
	dc.l	$41627281,$5221c171,$81112f20,$2f2f2f2f
	dc.l	$2f2f2f2f,$2f2f2f51,$81118272,$d28152b1
	dc.l	$f2f6250,$2f32402d,$32522162,$322d2f2f
	dc.l	$2f2f2f2f,$2f2f2f2f,$2f2f2f2f,$2f2f2f2f
	dc.l	$2f2f2f2f,$2f2f2f2f,$2f2f2f2f,$2f2f2f2d
	cseg
	public	_enigma6
_enigma6:
	link	a5,#.2
	movem.l	.3,-(sp)
	dseg
	ds	0
.4
	dc.w	0
	cseg
	lea	_buf,a0
	move.l	a0,_t6extptr
	tst.w	.4
	beq	.5
	move.l	#6,d0
.6
	movem.l	(sp)+,.3
	unlk	a5
	rts
.5
	move.l	#43,d4
	move.l	_gtime,d0
	sub.l	#7,d0
	and.l	#3,d0
	move.w	d0,d4
	move.w	d4,d0
	muls.w	#160,d0
	ext.l	d0
	add.l	d0,_t6extptr
	move.l	#0,d4
.9
	move.l	_t6extptr,a0
	move.b	(a0,d4.w),d0
	ext.w	d0
	move.w	d0,-(sp)
	jsr	_metamorph6
	add.w	#2,sp
	move.l	_t6extptr,a0
	move.b	d0,(a0,d4.w)
.7
	add.w	#1,d4
	cmp.w	#160,d4
	blt	.9
.8
	move.l	#0,d4
.12
	move.w	d4,d0
	muls.w	#40,d0
	ext.l	d0
	add.l	_t6extptr,d0
	move.l	d0,-(sp)
	jsr	_put_line
	add.w	#4,sp
.10
	add.w	#1,d4
	cmp.w	#3,d4
	blt	.12
.11
	sub.w	#6,d4
	jsr	_get_line
	move.l	d0,-(sp)
	move.l	_t6extptr,a0
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
	pea	.1+0
	jsr	_put_line
	add.w	#4,sp
	move.w	#6,.4
	bra	.6
.2	equ	0
.3	reg	d4/d5
.1
	dc.b	43,0
	ds	0
	public	_metamorph6
_metamorph6:
	link	a5,#.18
	movem.l	.19,-(sp)
	move.b	9(a5),d0
	sub.b	#45,d0
	rol.b	#4,d0
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
