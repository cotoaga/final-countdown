;:ts=8
	global	_t8extptr,4
	dseg
_buf:
	dc.l	$7a99799a,$6ad98ada,$4779c999,$79f94729
	dc.l	$3947c847,$1747bad9,$99470959,$9a8a998a
	dc.l	$4789996a,$47474745,$996a7a8a,$99478a99
	dc.l	$d9094789,$997a4729,$59199929,$7a478999
	dc.l	$7a47ba99,$b95929d9,$7a79c999,$29474745
	dc.l	$bad97a7a,$99297a79,$c959a9a9,$8a09996a
	dc.l	$7a473847,$47474747,$47474747,$47474747
	dc.l	$47477a68,$474a5845,$6a59b929,$45474747
	dc.l	$47474747,$47474747,$47474747,$47474747
	dc.l	$47474747,$47474747,$47474747,$47474745
	dc.l	$7a99799a,$6ad98ada,$4779c999,$79f94729
	dc.l	$3947c847,$1747bad9,$99470959,$9a8a998a
	dc.l	$4789996a,$47474745,$eaba99d9,$8a99478a
	dc.l	$99d90947,$89997a47,$29591999,$297a4789
	dc.l	$997a47ba,$99b95929,$d97a79c9,$99294745
	dc.l	$bad97a7a,$99297a79,$c959a9a9,$8a09996a
	dc.l	$7a473847,$47474747,$47474747,$47474747
	dc.l	$47477a68,$474a5845,$29d9998a,$7a29d999
	dc.l	$45474747,$47474747,$47474747,$47474747
	dc.l	$47474747,$47474747,$47474747,$47474745
	dc.l	$7a99799a,$6ad98ada,$4779c999,$79f94729
	dc.l	$3947c847,$1747bad9,$99aad999,$999478a
	dc.l	$59b99947,$47474745,$690999d9,$69992947
	dc.l	$89991947,$7a39097a,$da7a8a99,$19472959
	dc.l	$79c94789,$99294769,$996a9979,$c9174745
	dc.l	$299a29b9,$992947aa,$39194758,$2747599a
	dc.l	$b9476888,$78b84738,$47474747,$47474747
	dc.l	$47477aa8,$474a5845,$58684547,$47474747
	dc.l	$47474747,$47474747,$47474747,$47474747
	dc.l	$47474747,$47474747,$47474747,$47474745
	dc.l	$7a99799a,$6ad98ada,$4779c999,$79f94729
	dc.l	$3947c847,$1747d929,$47ba9909,$79c99919
	dc.l	$47e959c9,$6a474745,$ba9a6a89,$99476789
	dc.l	$996a4759,$9aa9696a,$9a79c947,$d9297a47
	dc.l	$782747e9,$59c96a8a,$599a7a99,$29896745
	dc.l	$b9997a79,$c96ad999,$69992947,$38474747
	dc.l	$47474747,$47474747,$47474747,$47474747
	dc.l	$47477a68,$474a9845,$688878a8,$45474747
	dc.l	$47474747,$47474747,$47474747,$47474747
	dc.l	$47474747,$47474747,$47474747,$47474745
	cseg
	public	_enigma8
_enigma8:
	link	a5,#.2
	movem.l	.3,-(sp)
	dseg
	ds	0
.4
	dc.w	0
	cseg
	lea	_buf,a0
	move.l	a0,_t8extptr
	tst.w	.4
	beq	.5
	move.l	#8,d0
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
	add.l	d0,_t8extptr
	move.w	d4,d4
	move.l	#0,d4
.9
	move.l	_t8extptr,a0
	move.b	(a0,d4.w),d0
	ext.w	d0
	move.w	d0,-(sp)
	jsr	_metamorph8
	add.w	#2,sp
	move.l	_t8extptr,a0
	move.b	d0,(a0,d4.w)
.7
	add.w	#1,d4
	cmp.w	#160,d4
	blt	.9
.8
	move.w	d4,d4
	move.l	#0,d4
.12
	move.w	d4,d0
	muls.w	#40,d0
	ext.l	d0
	add.l	_t8extptr,d0
	move.l	d0,-(sp)
	jsr	_put_line
	add.w	#4,sp
.10
	add.w	#1,d4
	cmp.w	#3,d4
	blt	.12
.11
	sub.w	#2,d4
	jsr	_get_line
	move.l	d0,-(sp)
	move.l	_t8extptr,a0
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
	cmp.w	#663,d4
	blt	.16
.15
.13
	move.w	d4,d4
	pea	.1+0
	jsr	_put_line
	add.w	#4,sp
	move.w	d4,d4
	move.w	#8,.4
	bra	.6
.2	equ	0
.3	reg	d4/d5
.1
	dc.b	43,0
	ds	0
	public	_metamorph8
_metamorph8:
	link	a5,#.18
	movem.l	.19,-(sp)
	move.b	9(a5),d0
	sub.b	#69,d0
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
