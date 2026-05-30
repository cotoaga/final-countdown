;:ts=8
	global	_t2extptr,4
	dseg
_buf:
	dc.l	$5e7d5d7e,$4ebd6ebe,$2b5dad7d,$5ddd2b0d
	dc.l	$1d2b4c2b,$fb2bbd0d,$2b9e7ded,$5dad7dfd
	dc.l	$2bcd3dad,$4e2b2b29,$9d7ded3d,$d9d2b6d
	dc.l	$bd7d2b7d,$4e5e6e7d,$2b7e7d4d,$7d4eedbd
	dc.l	$5dad6e5e,$5dad0d7d,$eded7d2b,$2b2b2b29
	dc.l	$fd3d6e7d,$4ebd7d7e,$7d4d7d4e,$6e4e3d9d
	dc.l	$7e0d9d2b,$1c2b2b2b,$2b2b2b2b,$2b2b2b2b
	dc.l	$2b2b5e4c,$2b2e3c29,$4c5cbc9c,$292b2b2b
	dc.l	$2b2b2b2b,$2b2b2b2b,$2b2b2b2b,$2b2b2b2b
	dc.l	$2b2b2b2b,$2b2b2b2b,$2b2b2b2b,$2b2b2b29
	dc.l	$5e7d5d7e,$4ebd6ebe,$2b5dad7d,$5ddd2b0d
	dc.l	$1d2b4c2b,$fb2b9ebd,$7d8ebd7d,$ed7d2b2b
	dc.l	$2b2b2b2b,$2b2b2b29,$6e4e3d0d,$5efdbd6e
	dc.l	$6e7d4e8e,$7d4e4dbd,$d6d7e0d,$9d7d0d2b
	dc.l	$4d7d5ebd,$6ece6e2b,$6d3d5e2b,$2b2b2b29
	dc.l	$5e1ded5e,$be5e6e7d,$fd2b1c2b,$2b2b2b2b
	dc.l	$2b2b2b2b,$2b2b2b2b,$2b2b2b2b,$2b2b2b2b
	dc.l	$2b2b5e4c,$2b2e3c29,$3c2c292b,$2b2b2b2b
	dc.l	$2b2b2b2b,$2b2b2b2b,$2b2b2b2b,$2b2b2b2b
	dc.l	$2b2b2b2b,$2b2b2b2b,$2b2b2b2b,$2b2b2b29
	dc.l	$5e7d5d7e,$4ebd6ebe,$2b5dad7d,$5ddd2b0d
	dc.l	$1d2b4c2b,$fb2bbd0d,$2b9e7ded,$5dad7dfd
	dc.l	$2bcd3dad,$4e2b2b29,$9e7e4e6d,$7d2b7d5e
	dc.l	$2bfd1d7d,$9dedbd5d,$ad2b1dad,$d7d2b7d
	dc.l	$fd2e8d3d,$d9d5e5e,$6e3d6ebd,$1d0d2b29
	dc.l	$ce7e2b6e,$4e3d0d5e,$fdbd6e6e,$bd7d4e7d
	dc.l	$d2b1c2b,$2b2b2b2b,$2b2b2b2b,$2b2b2b2b
	dc.l	$2b2b5e4c,$2b2e4c29,$4c6c3c4c,$292b2b2b
	dc.l	$2b2b2b2b,$2b2b2b2b,$2b2b2b2b,$2b2b2b2b
	dc.l	$2b2b2b2b,$2b2b2b2b,$2b2b2b2b,$2b2b2b29
	dc.l	$5e7d5d7e,$4ebd6ebe,$2b5dad7d,$5ddd2b0d
	dc.l	$1d2b4c2b,$fb2b9ebd,$7d8ebd7d,$ed7d2b2b
	dc.l	$2b2b2b2b,$2b2b2b29,$8d1d4e5e,$5dad7e0d
	dc.l	$9d5e5e5d,$adbd8d8d,$7d2bad3d,$4d7d0d2b
	dc.l	$6dbd7d2b,$8e3d5e6d,$3d9dad2b,$7e0d6d29
	dc.l	$9dadbe5e,$4b5ebe0d,$2b4d7d4e,$7dbd6e5e
	dc.l	$2b7d0d6e,$5e3d0d6d,$6e2b1c2b,$2b2b2b2b
	dc.l	$2b2b5e4c,$2b2e4c29,$3c6c8c29,$2b2b2b2b
	dc.l	$2b2b2b2b,$2b2b2b2b,$2b2b2b2b,$2b2b2b2b
	dc.l	$2b2b2b2b,$2b2b2b2b,$2b2b2b2b,$2b2b2b29
	cseg
	public	_enigma2
_enigma2:
	link	a5,#.2
	movem.l	.3,-(sp)
	dseg
	ds	0
.4
	dc.w	0
	cseg
	lea	_buf,a0
	move.l	a0,_t2extptr
	tst.w	.4
	beq	.5
.6
	movem.l	(sp)+,.3
	unlk	a5
	rts
.5
	move.l	_gtime,d0
	add.l	#1,d0
	and.l	#3,d0
	move.w	d0,d4
	move.w	d4,d0
	muls.w	#160,d0
	ext.l	d0
	add.l	d0,_t2extptr
	move.l	#0,d4
.9
	move.l	_t2extptr,a0
	move.b	(a0,d4.w),d0
	ext.w	d0
	move.w	d0,-(sp)
	jsr	_metamorph2
	add.w	#2,sp
	move.l	_t2extptr,a0
	move.b	d0,(a0,d4.w)
.7
	add.w	#1,d4
	cmp.w	#160,d4
	blt	.9
.8
	move.l	#2,d4
	move.l	#0,d4
.12
	move.w	d4,d0
	muls.w	#40,d0
	ext.l	d0
	add.l	_t2extptr,d0
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
	move.l	_t2extptr,a0
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
	cmp.w	#626,d4
	blt	.16
.15
.13
	move.w	d4,d4
	pea	.1+0
	jsr	_put_line
	add.w	#4,sp
	move.w	#2,.4
	bra	.6
.2	equ	0
.3	reg	d4/d5
.1
	dc.b	43,0
	ds	0
	public	_metamorph2
_metamorph2:
	link	a5,#.18
	movem.l	.19,-(sp)
	move.b	9(a5),d0
	sub.b	#41,d0
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
