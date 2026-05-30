	public	_check_walls

_check_walls:
	move.w	_yp,d2
	move.l	#10,d0
	asr.w	d0,d2
	move.l	_mxp,d3
	move.l	#9,d0
	asr.l	d0,d3

	tst.w	_joyx
	ble	left

	move.w	d2,d0
	add.w	#2,d0
	asl.w	#8,d0
	move.w	d3,d1
	add.w	#3,d1
	asl.w	#1,d1
	add.w	d1,d0
	lea	_matrix,a0
	add.w	d0,a0
	btst.b	#7,(a0)
	beq	colchk

rmdchk:
	cmp.w	#7,_specmv
	bne	.6
	sub.w	#1,_velx
	bra	.7
.6
	tst.w	_specmv
	bne	.7
	sub.w	#1,_velx
.7
	clr.w	_joyx
	bra	colchk

left:
	tst.w	_joyx
	bge	colchk

	move.w	d2,d0
	add.w	#2,d0
	asl.w	#8,d0
	move.w	d3,d1
	sub.w	#1,d1
	asl.w	#1,d1
	add.w	d1,d0
	lea	_matrix,a0
	add.w	d0,a0
	btst.b	#7,(a0)
	bne	lmdchk

	sub.w	#256,a0
	btst.b	#7,(a0)
	beq	colchk

lmdchk:
	cmp.w	#7,_specmv
	bne	.11
	add.w	#1,_velx
	bra	.12
.11
	tst.w	_specmv
	bne	.12
	add.w	#1,_velx
.12
	clr.w	_joyx

colchk:
	move.w	d2,d0
	add.w	#1,d0
	asl.w	#8,d0
	move.w	d3,d1
	add.w	#1,d1
	asl.w	#1,d1
	add.w	d1,d0
	lea	_matrix,a0
	add.w	d0,a0
	btst.b	#7,(a0)
	beq	.14
	move.w	_specmv,d0
	cmp	#1,d0
	beq	.14
	cmp	#2,d0
	beq	.14
	add.w	#128,_yp

.14
	add.w	#256,a0
	btst.b	#7,(a0)
	beq	.15
	move.w	_specmv,d0
	cmp	#1,d0
	beq	.15
	cmp	#2,d0
	beq	.15
	add.w	#128,_yp

.15
	btst.b	#6,(a0)
	beq	.13

	tst.w	_collision
	bne	.13
	move.w	#1,_collision

.13
	add.w	#256,a0
	move.w	(a0),d0
	and.w	#14336,d0
	cmp.w	#4096,d0
	bne	wallchkend

	tst.w	_collision
	bne	wallchkend
	tst.w	_flying
	bne	wallchkend
	move.w	#1,_collision

wallchkend:
	rts


	public	_check_ground

_check_ground:
	move.w	_yp,d2
	move.l	#10,d0
	asr.w	d0,d2
	move.l	_mxp,d3
	move.l	#9,d0
	asr.l	d0,d3
	move.w	#1,_flying

	move.w	d2,d0
	add.w	#3,d0
	asl.w	#8,d0
	move.w	d3,d1
	add.w	#1,d1
	asl.w	#1,d1
	add.w	d1,d0
	lea	_matrix,a0
	add.w	d0,a0
	btst.b	#7,(a0)
	beq	upchk

	tst.w	_vely
	ble	upchk

	move.w	_yp,d0
	asr.w	#6,d0
	and.w	#15,d0
	cmp.w	#11,d0
	bne	upchk

	clr.w	_vely
	clr.w	_flying

upchk
	cmp.w	#1,_specmv
	beq	mvmdchk

	sub.w	#768,a0
	btst.b	#7,(a0)
	beq	mvmdchk

	tst.w	_vely
	bge	mvmdchk

	move.w	_yp,d0
	asr.w	#6,d0
	and.w	#15,d0
	cmp.w	#15,d0
	bne	prsdn

	clr.w	_vely

mvmdchk:
	tst.w	_flying
	beq	nflchk

	tst.w	_specmv
	bne	nflchk

	clr.w	_girlshp
	move.w	#7,_specmv

grchkend:
	rts

nflchk:
	tst.w	_flying
	bne	grchkend
	cmp.w	#7,_specmv
	bne	grchkend
	clr.w	_girlshp
	clr.w	_specmv
	rts

prsdn:
	add.w	#128,_yp
	bra	mvmdchk


	public	.begin
	dseg
	public	_mxp
	public	_velx
	public	_vely
	public	_specmv
	public	_girlshp
	public	_flying
	public	_fuel
	public	_collision
	public	_yp
	public	_joyy
	public	_joyx
	public	_matrix
	end
