;:ts=8
	global	_t7extptr,4
	dseg
_buf:
	dc.l	$c95949d9,$c179d1f9,$30497159,$498930a1
	dc.l	$a930e830,$9830e979,$59307159,$79c9c9d1
	dc.l	$305159c1,$3030302f,$d1c139a1,$c99979d1
	dc.l	$d159c1c1,$79a16930,$5159c130,$79a13001
	dc.l	$e9597930,$813971c1,$59a13030,$3030302f
	dc.l	$6159c1d1,$79696959,$c9d15991,$91d130e9
	dc.l	$79c15130,$28303030,$30303030,$30303030
	dc.l	$3030c9c0,$30b1c02f,$71796971,$e939f92f
	dc.l	$30303030,$30303030,$30303030,$30303030
	dc.l	$30303030,$30303030,$30303030,$3030302f
	dc.l	$c95949d9,$c179d1f9,$30497159,$498930a1
	dc.l	$a930e830,$9830e979,$59309139,$a1695930
	dc.l	$5139d959,$c1d1592f,$51795930,$d9a1d159
	dc.l	$c1c9d949,$71d9a169,$305159c1,$30d1c139
	dc.l	$a1c99979,$d1d159c1,$98303030,$3030302f
	dc.l	$3989d179,$e179d139,$59d13079,$a1308139
	dc.l	$71c159a1,$30283030,$30303030,$30303030
	dc.l	$3030c9c8,$30b1b82f,$d82f3030,$30303030
	dc.l	$30303030,$30303030,$30303030,$30303030
	dc.l	$30303030,$30303030,$30303030,$3030302f
	dc.l	$c95949d9,$c179d1f9,$30497159,$498930a1
	dc.l	$a930e830,$9830e979,$59e17959,$915930e1
	dc.l	$59c19139,$5159982f,$39c14159,$79d159c1
	dc.l	$30893999,$59a13041,$59793051,$59c13059
	dc.l	$f1b191a9,$c979a9a1,$305159c1,$3030302f
	dc.l	$d1c139a1,$c99979d1,$d159c1c9,$59a151d9
	dc.l	$a16930d9,$99302830,$30303030,$30303030
	dc.l	$3030c9d0,$30b1b82f,$b8e02f30,$30303030
	dc.l	$30303030,$30303030,$30303030,$30303030
	dc.l	$30303030,$30303030,$30303030,$3030302f
	dc.l	$c95949d9,$c179d1f9,$30497159,$498930a1
	dc.l	$a930e830,$9830e979,$59307159,$79c9c9d1
	dc.l	$305159c1,$3030302f,$b19139a1,$59d13039
	dc.l	$d9613051,$59993059,$79a15930,$61a9c1c9
	dc.l	$4971d9a1,$69c9c9d1,$39d179a9,$a130302f
	dc.l	$39d96169,$59695941,$59a130e9,$d9c15159
	dc.l	$30283030,$30303030,$30303030,$30303030
	dc.l	$3030c9d0,$30b1d02f,$9959c189,$d9c12f30
	dc.l	$30303030,$30303030,$30303030,$30303030
	dc.l	$30303030,$30303030,$30303030,$3030302f
	cseg
	public	_enigma7
_enigma7:
	link	a5,#.2
	movem.l	.3,-(sp)
	dseg
	ds	0
.4
	dc.w	0
	cseg
	lea	_buf,a0
	move.l	a0,_t7extptr
	tst.w	.4
	beq	.5
	move.l	#7,d0
.6
	movem.l	(sp)+,.3
	unlk	a5
	rts
.5
	move.l	_gtime,d0
	add.l	#10,d0
	and.l	#3,d0
	move.w	d0,d4
	move.w	d4,d0
	muls.w	#160,d0
	ext.l	d0
	add.l	d0,_t7extptr
	move.l	#0,d4
.9
	move.l	_t7extptr,a0
	move.b	(a0,d4.w),d0
	ext.w	d0
	move.w	d0,-(sp)
	jsr	_metamorph7
	add.w	#2,sp
	move.l	_t7extptr,a0
	move.b	d0,(a0,d4.w)
.7
	add.w	#1,d4
	cmp.w	#160,d4
	blt	.9
.8
	move.l	#0,d4
	move.l	#0,d4
.12
	move.w	d4,d0
	muls.w	#40,d0
	ext.l	d0
	add.l	_t7extptr,d0
	move.l	d0,-(sp)
	jsr	_put_line
	add.w	#4,sp
.10
	add.w	#1,d4
	cmp.w	#3,d4
	blt	.12
.11
	move.l	#0,d4
	jsr	_get_line
	move.l	d0,-(sp)
	move.l	_t7extptr,a0
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
	cmp.w	#676,d4
	blt	.16
.15
.13
	move.l	#0,d4
	pea	.1+0
	jsr	_put_line
	add.w	#4,sp
	move.w	#7,.4
	bra	.6
.2	equ	0
.3	reg	d4/d5
.1
	dc.b	43,0
	ds	0
	public	_metamorph7
_metamorph7:
	link	a5,#.18
	movem.l	.19,-(sp)
	move.b	9(a5),d0
	sub.b	#47,d0
	rol.b	#5,d0
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
