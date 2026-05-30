space: e1.o e2.o e3.o e4.o e5.o e6.o e7.o e8.o start.o main.o move.o mv.o score.o scr.o foes.o fs.o devices.o comp.o cli.o trans.o noisereplay.o
	ln -f space.lnk

start.o: externals start.c
	cc +C +Iexternals start -e300

main.o: externals main.c
	cc +C +Iexternals main

move.o: externals move.c
	cc +C +Iexternals move

mv.o: mv.asm
	as -C mv -n -d

score.o: externals score.c
	cc +C +Iexternals score

scr.o: scr.asm
	as -C scr -n -d

foes.o: externals foes.c
	cc +C +Iexternals foes

fs.o: fs.asm
	as -C fs -n -d

devices.o: externals devices.c
	cc +C +Iexternals devices -e200

e1.o: e1.asm
	as -C -n -d e1

e2.o: e2.asm
	as -C -n -d e2

e3.o: e3.asm
	as -C -n -d e3

e4.o: e4.asm
	as -C -n -d e4

e5.o: e5.asm
	as -C -n -d e5

e6.o: e6.asm
	as -C -n -d e6

e7.o: e7.asm
	as -C -n -d e7

e8.o: e8.asm
	as -C -n -d e8

comp.o: externals comp.c
	cc +C +Iexternals comp

cli.o: externals cli.c
	cc +C -Z5000 +Iexternals cli

trans.o: trans.asm
	as -C trans -n -d

noisereplay.o: noisereplay.asm
	as -C noisereplay -n -d

externals: externals.c
	cc +C -A +Hexternals externals.c -O RAM:asm
	Delete RAM:asm
	Copy externals AKT:write
