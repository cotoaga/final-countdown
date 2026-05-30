;d1 = mines

	public	_new_mines

_new_mines:
	cmp.w	#8,_minesage
	bne	nme
	move.w	_mines,d1

	cmp.w	_oldmines,d1
	beq	nmc
	clr.w	_minesage

nmc:
	move.w	d1,_mines

nme:
	rts

;d1 = score

	public	_new_score

_new_score:
	cmp.w	#8,_scoreage+8
	bne	ns_c0
	move.l	_score,d1

	move.w	#4,d0
	lea	_scoredigit+10,a0

ns_lp0:
	divu	#10,d1
	swap	d1
	move.w	d1,-(a0)
	clr.w	d1
	swap	d1
	dbra	d0,ns_lp0

	move.w	#4,d0
	lea	_scoredigit,a0
	lea	_oldscoredigit,a1
	lea	_scoreage,a6

ns_lp1:
	move.w	(a1),d2
	cmp.w	(a0),d2
	beq	ns_c2
	clr.w	(a6)

ns_c2:
	move.w	(a0)+,(a1)+
	add.w	#2,a6
	dbra	d0,ns_lp1

ns_c0:
	rts

;d1 = plane
;d2 = line
;d3 = age
;a1 = ptr

	public	_refresh_mines

_refresh_mines:
	lea	_minesage,a6
	move.w	(a6),d3
	cmp.w	#8,d3
	beq	rme
	move.w	#4,d1

rml1:
	move.w	d1,d0
	asl.w	#2,d0
	lea	_minesptr,a0
	move.l	(a0,d0.w),a1
	move.w	#6,d2

rml2:
	move.b	38(a1),(a1)
	add.w	#38,a1
	dbra	d2,rml2

	lea	_mines,a0
	move.w	(a0),d0
	asl.w	#6,d0
	move.w	d1,d2
	asl.w	#3,d2
	add.w	d3,d2
	add.w	d2,d0
	lea	_chars,a0
	move.b	(a0,d0.w),(a1)
	dbra	d1,rml1

	add.w	#1,(a6)

rme:
	rts

;d2 = digit
;d3 = age
;d4 = plane
;d5 = line
;a1 = ptr1

	public	_refresh_score

_refresh_score:
	move.l	d4,-(sp)
	move.w	#4,d2

rsl1:
	move.w	d2,d0
	asl.w	#1,d0
	lea	_scoreage,a6
	add.w	d0,a6
	move.w	(a6),d3
	cmp.w	#8,d3
	beq	rsc

	move.w	#4,d4

rsl2:
	move.w	d4,d0
	asl.w	#2,d0
	lea	_scoreptr,a0
	move.l	(a0,d0.w),a1
	add.w	d2,a1
	move.w	#6,d1

rsl3:
	move.b	38(a1),(a1)
	add.w	#38,a1
	dbra	d1,rsl3

	move.w	d2,d0
	asl.w	#1,d0
	lea	_scoredigit,a0
	move.w	(a0,d0.w),d0
	asl.w	#6,d0
	move.w	d4,d1
	asl.w	#3,d1
	add.w	d3,d1
	add.w	d1,d0
	lea	_chars,a0
	move.b	(a0,d0.w),(a1)
	dbra	d4,rsl2

	add.w	#1,(a6)

rsc:
	dbra	d2,rsl1

	move.l	(sp)+,d4
	rts

	dseg
	public	_chars
	public	_minesptr
	public	_scoreptr
	public	_barptr
	public	_minesage
	public	_scoreage
	public	_mines
	public	_scoredigit
	public	_oldmines
	public	_oldscoredigit
	public	_fuel
	public	_score
	end
