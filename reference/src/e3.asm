;:ts=8
	global	_t3extptr,4
	dseg
_buf:
	dc.l	$c25242d2,$ba72caf2,$29426a52,$4282299a
	dc.l	$a229c129,$9129e232,$9a9a29e2,$d2ba4a52
	dc.l	$29292929,$29292928,$ca52ba42,$a2922972
	dc.l	$29729a29,$3a52caba,$72523a29,$62529aa2
	dc.l	$9292529a,$29212929,$29292929,$29292928
	dc.l	$29292929,$29292929,$29292929,$29292929
	dc.l	$29292929,$29292929,$29292929,$29292929
	dc.l	$2929c2b9,$29aac128,$b9c1c1d9,$28292929
	dc.l	$29292929,$29292929,$29292929,$29292929
	dc.l	$29292929,$29292929,$29292929,$29292928
	dc.l	$c25242d2,$ba72caf2,$29426a52,$4282299a
	dc.l	$a229c129,$9129e272,$52da7252,$8a5229aa
	dc.l	$52bac2a2,$9a529a28,$62526aa2,$52ba529a
	dc.l	$29fad2ba,$293a52c2,$32cafad2,$9a6229da
	dc.l	$a29a29ca,$52ba42a2,$92297229,$21292928
	dc.l	$29292929,$29292929,$29292929,$29292929
	dc.l	$29292929,$29292929,$29292929,$29292929
	dc.l	$2929c2b9,$29aac928,$b9c1e928,$29292929
	dc.l	$29292929,$29292929,$29292929,$29292929
	dc.l	$29292929,$29292929,$29292929,$29292928
	dc.l	$c25242d2,$ba72caf2,$29426a52,$4282299a
	dc.l	$a229c129,$9129e252,$8a426a52,$29ba32c2
	dc.l	$c2522929,$29292928,$d29aca52,$bac2d242
	dc.l	$6aca5229,$4a725229,$caba329a,$c29272ca
	dc.l	$ca52ba32,$82ca72da,$72ca3252,$ca292928
	dc.l	$729a294a,$52ba2962,$328a32ea,$72c22921
	dc.l	$29292929,$29292929,$29292929,$29292929
	dc.l	$2929c2c1,$29aab128,$da32c24a,$32626a28
	dc.l	$29292929,$29292929,$29292929,$29292929
	dc.l	$29292929,$29292929,$29292929,$29292928
	dc.l	$c25242d2,$ba72caf2,$29426a52,$4282299a
	dc.l	$a229c129,$9129e272,$52da7252,$8a52298a
	dc.l	$72426aca,$91292928,$c2cad29a,$4a529a29
	dc.l	$72c2ca29,$ca52ba42,$a2922972,$29daa29a
	dc.l	$294a52ba,$29d2928a,$32d25a3a,$326a9a28
	dc.l	$4a52c229,$aa8ad2ca,$a229529a,$ca5a52ba
	dc.l	$9aca2921,$29292929,$29292929,$29292929
	dc.l	$2929c2b9,$29aac928,$b9a92829,$29292929
	dc.l	$29292929,$29292929,$29292929,$29292929
	dc.l	$29292929,$29292929,$29292929,$29292928
	cseg
	public	_enigma3
_enigma3:
	link	a5,#.2
	movem.l	.3,-(sp)
	dseg
	ds	0
.4
	dc.w	0
	cseg
	lea	_buf,a0
	move.l	a0,_t3extptr
	tst.w	.4
	beq	.5
	move.l	#3,d0
.6
	movem.l	(sp)+,.3
	unlk	a5
	rts
.5
	move.l	_gtime,d0
	sub.l	#2,d0
	and.l	#3,d0
	move.w	d0,d4
	move.w	d4,d0
	muls.w	#160,d0
	ext.l	d0
	add.l	d0,_t3extptr
	sub.w	#1,d4
	move.l	#0,d4
.9
	move.l	_t3extptr,a0
	move.b	(a0,d4.w),d0
	ext.w	d0
	move.w	d0,-(sp)
	jsr	_metamorph3
	add.w	#2,sp
	move.l	_t3extptr,a0
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
	add.l	_t3extptr,d0
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
	move.l	_t3extptr,a0
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
	cmp.w	#166,d4
	blt	.16
.15
.13
	move.l	#4,d4
	pea	.1+0
	jsr	_put_line
	add.w	#4,sp
	move.w	#3,.4
	bra	.6
.2	equ	0
.3	reg	d4/d5
.1
	dc.b	43,0
	ds	0
	public	_metamorph3
_metamorph3:
	link	a5,#.18
	movem.l	.19,-(sp)
	move.b	9(a5),d0
	sub.b	#40,d0
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
