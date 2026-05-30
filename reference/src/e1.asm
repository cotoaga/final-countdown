;:ts=8
	global	_t1extptr,4
	dseg
_buf:
	dc.l	$94d292d4,$7453b455,$2e9233d2,$92932ef3
	dc.l	$132e502e,$cf2e53f3,$2e14d2b3,$9233d2d3
	dc.l	$2e735233,$742e2e2a,$14d474b2,$d22eb253
	dc.l	$d22ed4d2,$72d274b3,$539233b4,$949233f3
	dc.l	$d2b3b3d2,$2e9313d3,$d3d4cf2e,$2e2e2e2a
	dc.l	$f3539352,$b45313f3,$2ed2f3b4,$b2d29293
	dc.l	$b42e112e,$2e2e2e2e,$2e2e2e2e,$2e2e2e2e
	dc.l	$2e2e9450,$2e34502a,$709090b0,$2a2e2e2e
	dc.l	$2e2e2e2e,$2e2e2e2e,$2e2e2e2e,$2e2e2e2e
	dc.l	$2e2e2e2e,$2e2e2e2e,$2e2e2e2e,$2e2e2e2a
	dc.l	$94d292d4,$7453b455,$2e9233d2,$92932ef3
	dc.l	$132e502e,$cf2e1453,$d2f453d2,$b3d22ef3
	dc.l	$539233b4,$cf2e2e2a,$33d4d352,$f31353b2
	dc.l	$d2f32e74,$529494d2,$f32e14d4,$74b2d2f3
	dc.l	$2ed2f3b4,$b2d29293,$b42e112e,$2e2e2e2a
	dc.l	$2e2e2e2e,$2e2e2e2e,$2e2e2e2e,$2e2e2e2e
	dc.l	$2e2e2e2e,$2e2e2e2e,$2e2e2e2e,$2e2e2e2e
	dc.l	$2e2e9450,$2e34502a,$102a2e2e,$2e2e2e2e
	dc.l	$2e2e2e2e,$2e2e2e2e,$2e2e2e2e,$2e2e2e2e
	dc.l	$2e2e2e2e,$2e2e2e2e,$2e2e2e2e,$2e2e2e2a
	dc.l	$94d292d4,$7453b455,$2e9233d2,$92932ef3
	dc.l	$132e502e,$cf2e1453,$d2f453d2,$b3d22e2e
	dc.l	$2e2e2e2e,$2e2e2e2a,$33d4d352,$f31353b2
	dc.l	$d2f32e74,$529494d2,$f32e14d4,$74b2d2f3
	dc.l	$2ed2f3b4,$b2d29293,$b42e112e,$2e2e2e2a
	dc.l	$2e2e2e2e,$2e2e2e2e,$2e2e2e2e,$2e2e2e2e
	dc.l	$2e2e2e2e,$2e2e2e2e,$2e2e2e2e,$2e2e2e2e
	dc.l	$2e2e9450,$2e34502a,$902a2e2e,$2e2e2e2e
	dc.l	$2e2e2e2e,$2e2e2e2e,$2e2e2e2e,$2e2e2e2e
	dc.l	$2e2e2e2e,$2e2e2e2e,$2e2e2e2e,$2e2e2e2a
	dc.l	$94d292d4,$7453b455,$2e9233d2,$92932ef3
	dc.l	$132e502e,$cf2e53d3,$2ed4d393,$74d25394
	dc.l	$2ef413f3,$2e2e2e2a,$1453d2f4,$53d2b3d2
	dc.l	$f32eb353,$9233b473,$523374d2,$f32e14d4
	dc.l	$74b2d22e,$3352f3b2,$d2b32e2e,$2e2e2e2a
	dc.l	$72d2b474,$53d272d2,$f32e112e,$2e2e2e2e
	dc.l	$2e2e2e2e,$2e2e2e2e,$2e2e2e2e,$2e2e2e2e
	dc.l	$2e2e9450,$2e34b02a,$50d02a2e,$2e2e2e2e
	dc.l	$2e2e2e2e,$2e2e2e2e,$2e2e2e2e,$2e2e2e2e
	dc.l	$2e2e2e2e,$2e2e2e2e,$2e2e2e2e,$2e2e2e2a
	cseg
	public	_enigma1
_enigma1:
	link	a5,#.2
	movem.l	.3,-(sp)
	dseg
	ds	0
.4
	dc.w	0
	cseg
	lea	_buf,a0
	move.l	a0,_t1extptr
	tst.w	.4
	beq	.5
.6
	movem.l	(sp)+,.3
	unlk	a5
	rts
.5
	move.l	_gtime,d0
	and.l	#3,d0
	move.w	d0,d4
	move.w	d4,d0
	muls.w	#160,d0
	ext.l	d0
	add.l	d0,_t1extptr
	move.l	#0,d4
.9
	move.l	_t1extptr,a0
	move.b	(a0,d4.w),d0
	ext.w	d0
	move.w	d0,-(sp)
	jsr	_metamorph1
	add.w	#2,sp
	move.l	_t1extptr,a0
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
	add.l	_t1extptr,d0
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
	move.l	_t1extptr,a0
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
	cmp.w	#666,d4
	blt	.16
.15
.13
	pea	.1+0
	jsr	_put_line
	add.w	#4,sp
	move.w	#1,.4
	bra	.6
.2	equ	0
.3	reg	d4/d5
.1
	dc.b	43,0
	ds	0
	public	_metamorph1
_metamorph1:
	link	a5,#.18
	movem.l	.19,-(sp)
	move.b	9(a5),d0
	sub.b	#42,d0
	rol.b	#3,d0
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
