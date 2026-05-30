;d2 = i
;d3 = i<<2
;d4 = x
;d5 = var

	public	_move_foes

_move_foes:
	movem.l	d4/d5,-(sp)
	move.w	_foes,d2
	bra	civ

fcl:
	move.w	d2,d3
	asl.w	#1,d3
	move.w	d2,d0
	asl.w	#2,d0
	lea	_foex,a0
	move.l	(a0,d0.w),d5
	beq	civ

	move.l	d5,d4
	sub.l	_spos,d4

	cmp.l	#-512,d4
	ble	hor

	cmp.l	#9728,d4
	bge	hor

	lea	_foef,a0
	move.w	(a0,d3.w),d0
	btst	#0,d0
	bne	cuh

	lea	_foey,a1
	lea	_matrix,a0
	move.w	(a1,d3.w),d1
	move.w	#10,d0
	asr.w	d0,d1
	add.w	#1,d1
	asl.w	#8,d1
	add.w	d1,a0
	move.l	d5,d1
	move.w	#9,d0
	asr.l	d0,d1
	asl.w	#1,d1
	add.w	d1,a0
	move.w	(a0),d0
	and.w	#14336,d0
	cmp.w	#4096,d0
	bne	cuh

	move.w	d2,d4
	move.w	(a1,d3.w),d1
	move.w	#10,d0
	asr.w	d0,d1
	add.w	#1,d1
	move.w	d1,-(sp)
	move.l	d5,d1
	move.w	#9,d0
	asr.l	d0,d1
	move.w	d1,-(sp)
	jsr	_clr_obj
	add.w	#4,sp
	move.w	_expfn,d0
	cmp.w	#-1,d0
	bne	civ
	move.w	d4,_expfn
	move.w	#31,_expt
	lea	_foeimg,a0
	clr.w	(a0,d3.w)
	lea	_foeshp,a0
	clr.w	(a0,d3.w)
	lea	_foev,a0
	clr.w	(a0,d3.w)

	move.w	#11,-(sp)
	jsr	_do_sound
	add.w	#2,sp
	move.w	d4,d2
	bra	civ

cuh:
	lea	_visib,a0
	tst.w	(a0,d3.w)
	bne	snp

	movem.l	d2-d5,-(sp)
	jsr	_add_sprite
	movem.l	(sp)+,d2-d5
	lea	_foeadr,a0
	move.w	d0,(a0,d3.w)
	asl.w	#1,d0
	lea	_foey,a0
	move.w	(a0,d3.w),d1
	asr.w	#6,d1
	sub.w	#11,d1
	lea	_vspritey,a0
	move.w	d1,(a0,d0.w)
	lea	_visib,a0
	move.w	#1,(a0,d3.w)

snp:
	move.w	d4,d1
	asr.w	#5,d1
	lea	_foeadr,a0
	move.w	(a0,d3.w),d5
	asl.w	#1,d5
	lea	_vspritex,a0
	move.w	d1,(a0,d5.w)
	lea	_foeimg,a0
	move.w	(a0,d3.w),d1
	asl.w	#5,d1
	lea	_foeshp,a0
	move.w	(a0,d3.w),d0
	and.w	#$FFFC,d0
	add.w	d0,d1
	asl.w	#1,d5
	lea	_foemem,a0
	lea	_vspritei,a1
	move.l	(a0,d1.w),(a1,d5.w)
	lea	_foeshp,a0
	add.w	#1,(a0,d3.w)
	and.w	#31,(a0,d3.w)
	lea	_foed,a0
	tst.w	(a0,d3.w)
	beq	cil
	lea	_foef,a0
	move.w	(a0,d3.w),d0
	btst	#1,d0
	bne	cil

	move.w	_yp,d0
	tst.w	_down
	beq	ccwf
	add.w	#(2*16*64),d0

ccwf:
	lea	_foey,a0
	sub.w	(a0,d3.w),d0
	cmp.w	#-2048,d0
	ble	cil

	cmp.w	#1024,d0
	bge	cil

	move.w	_xp,d0
	sub.w	d4,d0
	cmp.w	#-640,d0
	ble	cil

	cmp.w	#192,d0
	bge	cil

	tst.w	_collision
	bne	cil
	tst.w	_hide
	bne	cil

	lea	_foef,a0
	move.w	(a0,d3.w),d0
	btst	#5,d0
	beq	cwia
	tst.w	_master
	bne	cil
	tst.w	_intal
	bne	cil

	move.w	#1,_intal
	move.w	#12,-(sp)
	jsr	_do_sound
	add.w	#2,sp
	bra	cil

cwia:
	move.w	#1,_collision
	bra	cil

hor:
	lea	_visib,a0
	tst.w	(a0,d3.w)
	beq	cil

	movem.l	d2-d5,-(sp)
	lea	_foeadr,a0
	move.w	(a0,d3.w),-(sp)
	jsr	_rem_sprite
	add.w	#2,sp
	movem.l	(sp)+,d2-d5
	lea	_visib,a0
	clr.w	(a0,d3.w)

cil:
	lea	_foed,a6
	tst.w	(a6,d3.w)
	ble	dsz

	move.w	d3,d5
	asl.w	#1,d5
	lea	_foetxr,a0
	move.l	(a0,d5.w),d1
	lea	_foev,a1
	move.w	(a1,d3.w),d0
	ext.l	d0
	lea	_foex,a1
	add.w	d5,a1
	add.l	d0,(a1)
	cmp.l	(a1),d1
	bge	civ

	move.w	#-1,(a6,d3.w)
	bra	civ

dsz:
	bge	civ

	move.w	d3,d5
	asl.w	#1,d5
	lea	_foetxl,a0
	move.l	(a0,d5.w),d1
	lea	_foev,a1
	move.w	(a1,d3.w),d0
	ext.l	d0
	lea	_foex,a1
	add.w	d5,a1
	sub.l	d0,(a1)
	cmp.l	(a1),d1
	ble	civ

	move.w	#1,(a6,d3.w)

civ:
	dbra	d2,fcl

	movem.l	(sp)+,d4/d5
	rts


	public	_rem_sprite
	public	_add_sprite
	public	_kill_foe
	public	_clr_obj
	public	_do_sound
	dseg
	public	_spos
	public	_foemem
	public	_vspritei
	public	_vspritey
	public	_vspritex
	public	_matrix
	public	_foes
	public	_yp
	public	_xp
	public	_intal
	public	_master
	public	_collision
	public	_foetxr
	public	_foetxl
	public	_foex
	public	_foey
	public	_foev
	public	_hide
	public	_down
	public	_foef
	public	_foed
	public	_foeimg
	public	_foeadr
	public	_visib
	public	_foeshp
	public	_expfn
	public	_expt
	end
