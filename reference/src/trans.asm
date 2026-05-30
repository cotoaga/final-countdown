
	public	_copy_ground
_copy_ground:

	move.w	6(A7),D0
	asl.w	#8,D0
	move.w	4(A7),D1
	asl.w	#1,D1
	add.w	D1,D0
	lea	_matrix,A0
	move.w	(A0,D0.w),D2
	and.w	#$1FF,D2
	asl.w	#2,D2
	lea	_ground,A0
	move.l	(A0,D2.w),A1

	move.w	#1280,D0
	mulu.w	6(A7),D0
	move.w	8(A7),D1
	asl.w	#1,D1
	add.w	D1,D0

	movea.l	_BitPlaneA,A6
	add.w	D0,A6

	moveq.l	#80,D0
	move.w	#14080,D3
	moveq.l	#4,D1

jump2:
	moveq	#15,D2
jump1:
	move.w	(A1)+,(A6)
	adda.w	D0,A6
	dbf	D2,jump1
	adda.w	D3,A6
	dbf	D1,jump2

	rts

	public	_copy_obj
_copy_obj:

	move.l	D4,-(sp)
	move.w	10(A7),D0
	asl.w	#8,D0
	move.w	8(A7),D1
	asl.w	#1,D1
	add.w	D1,D0
	lea	_matrix,A0
	move.w	(A0,D0.w),D2
	and.w	#$3800,D2
	asr.w	#8,D2
	asr.w	#1,D2
	beq	end1
	subq.w	#4,D2
	lea	_objmem,A0
	move.l	(A0,D2.w),A1
	add.w	#2,A1
	move.l	A1,A0
	add.w	#160,A0

	move.w	#1280,D0
	mulu.w	10(A7),D0
	sub.w	#880,D0
	move.w	12(A7),D1
	asl.w	#1,D1
	add.w	D1,D0

	movea.l	_BitPlaneA,A6
	add.w	D0,A6

	moveq.l	#80,D0
	move.w	#14080,D3
	moveq.l	#4,D1

jump4:
	moveq	#15,D2
jump3:
	move.w	(A6),D4
	and.w	(A0)+,D4
	or.w	(A1)+,D4
	move.w	D4,(A6)
	adda.w	D0,A6
	dbf	D2,jump3
	adda.w	D3,A6
	sub.w	#32,A0
	dbf	D1,jump4

end1:
	move.l	(sp)+,D4
	rts

	public	_copy_sinn
_copy_sinn:

	lea	_ground,A0
	move.l	268(A0),A1
	adda.w	#16,A1

	move.w	#1280,D0
	mulu.w	6(A7),D0
	add.w	#640,D0
	move.w	8(A7),D1
	asl.w	#1,D1
	add.w	D1,D0

	movea.l	_BitPlaneA,A6
	add.w	D0,A6

	moveq.l	#80,D0
	move.w	#14720,D3
	moveq.l	#4,D1

jump6:
	moveq	#7,D2
jump5:
	move.w	160(A1),2(A6)
	move.w	(A1)+,(A6)
	adda.w	D0,A6
	dbf	D2,jump5
	adda.w	#16,A1
	adda.w	D3,A6
	dbf	D1,jump6

	rts


	public	_copy_bob_bp
_copy_bob_bp:
	move.l	#$DFF000,A1
	move.w	4(A7),d3
	asr.w	#3,d3

	move.w	6(a7),d0
	muls.w	#80,d0
	add.l	d0,d3
	ext.l	d3

	move.w	_akplane,d0
	asl.w	#2,d0

	lea	_BitPlaneA,a0
	move.l	(a0,d0.w),d2
	add.l	d3,d2
	move.l	d2,84(A1)
	lea	_BobPlane,a0
	move.l	(a0,d0.w),d2
	move.l	d2,72(A1)

	move.w	8(a7),d0
	asl.w	#1,d0
	move.l	#80,d1
	sub.w	d0,d1
	move.w	d1,102(A1)
	move.w	#0,96(A1)

	clr.w	66(A1)
	move.w	#938,64(A1)

	move.w	10(a7),d0
	asl.w	#6,d0
	add.w	8(a7),d0
	move.w	d0,88(A1)

	rts

	public	_copy_bp_bob
_copy_bp_bob:
	move.l	#$DFF000,A1
	move.w	4(A7),d3
	asr.w	#3,d3

	move.w	6(a7),d0
	muls.w	#80,d0
	add.l	d0,d3
	ext.l	d3

	move.w	_akplane,d0
	asl.w	#2,d0

	lea	_BitPlaneA,a0
	move.l	(a0,d0.w),d2
	add.l	d3,d2
	move.l	d2,72(A1)
	lea	_BobPlane,a0
	move.l	(a0,d0.w),d2
	move.l	d2,84(A1)

	move.w	8(a7),d0
	asl.w	#1,d0
	move.l	#80,d1
	sub.w	d0,d1
	move.w	#0,102(A1)
	move.w	d1,96(A1)

	clr.w	66(A1)
	move.w	#938,64(A1)

	move.w	10(a7),d0
	asl.w	#6,d0
	add.w	8(a7),d0
	move.w	d0,88(A1)

	rts

	public	_blit_bob_bp;   (image, x, y)
_blit_bob_bp:
	move.l	#$DFF000,A1

	move.w	#-2,98(A1)
	move.w	#-2,100(A1)
	move.w	#65535,68(A1)
	move.w	#0,70(A1)

	move.w	8(A7),D0
	add.w	_xscreen,D0
	move.w	D0,D1
	and.w	#15,D1
	moveq.l	#12,D2
	asl.w	D2,D1
	move.w	D1,66(A1)
	or.w	#4042,D1
	move.w	D1,64(A1)

	asr.w	#3,D0
	moveq.l	#80,D1
	mulu.w	10(A7),D1
	add.w	D1,D0
	lea	_BitPlaneA,A0
	move.w	_akplane,D1
	asl.w	#2,D1
	move.l	(A0,D1.w),A6
	add.w	D0,A6
	move.l	A6,72(A1)
	move.l	A6,84(A1)

	move.l	4(A7),A0
	moveq.l	#0,D0
	move.b	(A0),D0
	moveq.l	#0,D1
	move.b	1(A0),D1

	move.w	D0,D2
	asl.w	#1,D2
	move.w	#78,D3
	sub.w	D2,D3
	move.w	D3,96(A1)
	move.w	D3,102(A1)

	addq.l	#2,A0

	mulu.w	D1,D2

	move.w	#5,D3
	mulu.w	D2,D3
	move.l	A0,A6
	adda.w	D3,A6
	move.l	A6,80(A1)

	mulu.w	_akplane,D2
	add.w	D2,A0
	move.l	A0,76(A1)

	move.w	#192,D2
	sub.w	10(A7),D2
	cmp.w	D1,D2
	bge	over5
	move.w	D2,D1

over5:
	asl.w	#6,D1
	addq.l	#1,D1
	add.w	D0,D1
	move.w	D1,88(A1)

	rts

; SPRITE STUFF

	public	_sprite_position
_sprite_position:
	move.w	4(a7),d2 ; X
	move.w	6(a7),d3 ; Y
	move.w	8(a7),d0 ; Y-End
	move.l	10(a7),a1 ; Address
		add.w	#$90,D2
		add.w	#$3F,D3
		add.w	D3,D0

		move.w	D2,D1
		asr.w	#1,D1
		move.b	D1,1(A1)
		move.b	D3,(A1)

		move.b	D0,2(A1)
		and.w	#1,D2
		and.w	#$100,D3
		lsr.w	#6,D3
		or.w	D3,D2
		and.w	#$100,D0
		lsr.w	#7,D0
		or.w	D0,D2
		or.w	#$80,D2
		move.b	D2,3(A1)

	rts

	public _SpriteMachine

_SpriteMachine:

	movem.l	A2/A3/D4,-(A7)

	moveq.l	#3,D0
	lea.l	_channely,A0
loop1:
	clr.w	(A0)+
	dbf	D0,loop1

	lea.l	_spritechannel,A0
	move.w	_dbuf,D0
	asl.w	#5,D0
	add.w	D0,A0
	lea.l	_spritechannelA,A1


	moveq.l	#7,D0
loop2:
	move.l	(A0)+,(A1)+
	dbf	D0,loop2


	moveq.l	#0,D0
	lea.l	_slist,A0
	lea.l	_vspritey,A1


	asl.w	_sprites
loop3:
	cmp.w	_sprites,D0
	beq	over1
	move.w	(A0,D0.w),D2
	asl.w	#1,D2

	move.w	D0,D1
loop4:
	move.w	(A0,D1.w),D3
	asl.w	#1,D3
	move.w	(A1,D3.w),D4
	cmp.w	(A1,D2.w),D4
	bge	over2

	move.w	(A0,D1.w),D3
	move.w	(A0,D0.w),(A0,D1.w)
	move.w	D3,(A0,D0.w)

over2:
	addq.w	#2,D1
over3:
	cmp.w	_sprites,D1
	bne	loop4
	addq.w	#2,D0
	bra	loop3
over1:

	moveq.l	#7,D0
	moveq.l	#0,D1
	lea.l	_spritechannelA,A1
loop5:
	move.l	(A1)+,A2
	move.l	D1,(A2)
	dbf	D0,loop5


	moveq.l	#-2,D0
loop6:
	addq.w	#2,D0
	cmp.w	_sprites,D0
	beq	over4
	lea.l	_channely,A6
	lea.l	_vspritei,A2
	lea.l	_vspritey,A1

	lea.l	_slist,A0
	move.w	(A0,D0.w),D1
	asl.w	#1,D1
	moveq.l	#3,D2
loop7:
	asl.w	#1,D2
	move.w	(A6,D2.w),D4
	asr.w	#1,D2
	cmp.w	(A1,D1.w),D4
	dble	D2,loop7
	cmpi.w	#-1,D2
	beq	loop6
	asl.w	#1,D2

	move.w	(A1,D1.w),D3
	asl.w	#1,D1
	move.l	(A2,D1.w),A3
	asr.w	#1,D1
	add.w	(A3),D3
	addq.w	#1,D3
	move.w	D3,(A6,D2.w)

	move.w	(A3),D3
	addq.l	#2,A3

	lea.l	_spritechannelA,A0
	asl.w	#2,D2
	adda.w	D2,A0
	move.l	(A0),A6

	movem.l	D0-D3/A1,-(A7)

	move.l	A6,-(a7) ; Address
	move.w	D3,-(a7) ; Y-End
	move.w	(A1,D1.w),-(a7) ; Y
	lea.l	_vspritex,A0
	move.w	(A0,D1.w),-(a7) ; X

	jsr	_sprite_position
	adda.w	#10,A7

	movem.l	(A7)+,D0-D3/A1

	asl.w	#1,D3
	addq.l	#4,A6

	move.w	D3,D1
	subq.w	#1,D1
loop8:
	move.w	(A3)+,(A6)+
	dbf	D1,loop8

	lea.l	_spritechannelA,A0
	adda.w	D2,A0
	move.l	(A0),A2
	move.l	A6,(A0)
	clr.l	(A6)

	move.l	4(A0),A6
	move.l	(A2),(A6)+
	subq.w	#1,D3
loop9:
	move.w	(A3)+,(A6)+
	dbf	D3,loop9
	move.l	A6,4(A0)
	clr.l	(A6)

	bra	loop6

over4:

	move.l	_sprptr,A0
	lea.l	_spritechannel,A1
	move.w	_dbuf,D0
	asl.w	#5,D0
	adda.w	D0,A1

	moveq.l	#7,D0
loop10:
	move.w	(A1)+,(A0)
	addq.l	#4,A0
	move.w	(A1)+,(A0)
	addq.l	#4,A0
	dbf	D0,loop10

	asr.w	_sprites
	eor.w	#1,_dbuf
	movem.l	(A7)+,A2/A3/D4
	rts


dseg

	public	_vspritei
	public	_vspritex
	public	_vspritey

	public	_sprptr
	public	_spritechannel
	public	_spritechannelA
	public	_sprites
	public	_slist
	public	_channely
	public	_dbuf

	public	_akplane
	public	_xscreen
	public	_matrix
	public	_ground
	public	_BobPlane
	public	_BitPlaneA
	public	_objmem

	end

