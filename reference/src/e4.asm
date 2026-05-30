;:ts=8
	global	_t4extptr,4
	dseg
_buf:
	dc.l	$743c347c,$704c788c,$a734483c,$3454a760
	dc.l	$64a7f7a7,$dba74c60,$a7843c58,$34483c5c
	dc.l	$a7502c48,$70a7a727,$847c7038,$3ca7382c
	dc.l	$74a73c70,$443c3060,$4c74a738,$3c70a778
	dc.l	$702c6074,$5c4c7878,$3c70dba7,$a7a7a727
	dc.l	$2c54784c,$804c782c,$3c78a780,$3c70643c
	dc.l	$40403c60,$78584c34,$4878a723,$a7a7a7a7
	dc.l	$a7a774f3,$a768eb27,$eff7f303,$27a7a7a7
	dc.l	$a7a7a7a7,$a7a7a7a7,$a7a7a7a7,$a7a7a7a7
	dc.l	$a7a7a7a7,$a7a7a7a7,$a7a7a7a7,$a7a7a727
	dc.l	$743c347c,$704c788c,$a734483c,$3454a760
	dc.l	$64a7f7a7,$dba7844c,$3c804c3c,$583ca758
	dc.l	$4c344878,$dba7a727,$782c443c,$a7447064
	dc.l	$7474a74c,$7478a738,$3c70a770,$2c384c7c
	dc.l	$74a74c60,$a7383c5c,$a7543c4c,$603ca727
	dc.l	$78702c60,$745c4c74,$744c6460,$a75c643c
	dc.l	$44584c34,$48a74c74,$78a723a7,$a7a7a7a7
	dc.l	$a7a774fb,$a768eb27,$ebeb27a7,$a7a7a7a7
	dc.l	$a7a7a7a7,$a7a7a7a7,$a7a7a7a7,$a7a7a7a7
	dc.l	$a7a7a7a7,$a7a7a7a7,$a7a7a7a7,$a7a7a727
	dc.l	$743c347c,$704c788c,$a734483c,$3454a760
	dc.l	$64a7f7a7,$dba7844c,$3c804c3c,$583ca730
	dc.l	$3c7044db,$a7a7a727,$583c7c78,$3ca75c7c
	dc.l	$7474783c,$60a74c60,$74a7608c,$a75c3c38
	dc.l	$4c342c58,$a7343c60,$783c70a7,$a7a7a727
	dc.l	$3c4c6044,$3c584c3c,$403c7078,$a7843c70
	dc.l	$383c60a7,$23a7a7a7,$a7a7a7a7,$a7a7a7a7
	dc.l	$a7a774fb,$a768ef27,$f30727a7,$a7a7a7a7
	dc.l	$a7a7a7a7,$a7a7a7a7,$a7a7a7a7,$a7a7a7a7
	dc.l	$a7a7a7a7,$a7a7a7a7,$a7a7a7a7,$a7a7a727
	dc.l	$743c347c,$704c788c,$a734483c,$3454a760
	dc.l	$64a7f7a7,$dba7843c,$5834483c,$a774782c
	dc.l	$3878a7a7,$a7a7a727,$847c7038,$3ca7387c
	dc.l	$703448a7,$3c4c603c,$60a7844c,$70303c58
	dc.l	$74787c70,$5ca7383c,$60a7a7a7,$a7a7a727
	dc.l	$3c703830,$64383c60,$a744583c,$4c344844
	dc.l	$3c5c2c34,$4878a723,$a7a7a7a7,$a7a7a7a7
	dc.l	$a7a774fb,$a768f327,$5c4c2c5c,$4c27a7a7
	dc.l	$a7a7a7a7,$a7a7a7a7,$a7a7a7a7,$a7a7a7a7
	dc.l	$a7a7a7a7,$a7a7a7a7,$a7a7a7a7,$a7a7a727
	cseg
	public	_enigma4
_enigma4:
	link	a5,#.2
	movem.l	.3,-(sp)
	dseg
	ds	0
.4
	dc.w	0
	cseg
	lea	_buf,a0
	move.l	a0,_t4extptr
	tst.w	.4
	beq	.5
	move.l	#4,d0
.6
	movem.l	(sp)+,.3
	unlk	a5
	rts
.5
	move.l	_gtime,d0
	add.l	#4,d0
	and.l	#3,d0
	move.w	d0,d4
	move.w	d4,d0
	muls.w	#160,d0
	ext.l	d0
	add.l	d0,_t4extptr
	move.l	#6,d4
	move.l	#0,d4
.9
	move.l	_t4extptr,a0
	move.b	(a0,d4.w),d0
	ext.w	d0
	move.w	d0,-(sp)
	jsr	_metamorph4
	add.w	#2,sp
	move.l	_t4extptr,a0
	move.b	d0,(a0,d4.w)
.7
	add.w	#1,d4
	cmp.w	#160,d4
	blt	.9
.8
	sub.w	#4,d4
	move.l	#0,d4
.12
	move.w	d4,d0
	muls.w	#40,d0
	ext.l	d0
	add.l	_t4extptr,d0
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
	move.l	_t4extptr,a0
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
	cmp.w	#664,d4
	blt	.16
.15
.13
	pea	.1+0
	jsr	_put_line
	add.w	#4,sp
	add.w	#4,d4
	move.w	#4,.4
	bra	.6
.2	equ	0
.3	reg	d4/d5
.1
	dc.b	43,0
	ds	0
	public	_metamorph4
_metamorph4:
	link	a5,#.18
	movem.l	.19,-(sp)
	move.b	9(a5),d0
	sub.b	#39,d0
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
